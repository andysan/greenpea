/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <libpdp12/cpu.h>
#include <libpdp12utils/bin.h>
#include <libpdp12utils/log.h>

#include "shell.h"
#include "readln.h"
#include "parser.h"

void vr12_fade();
#ifdef HAVE_SDL
void sdl_update();
#endif

typedef struct {
    cpu_instance_t* cpu;
    int end;
} cmd_data_t;

static void
cmd_exit(int argc, char **argp, void *data) {
    ((cmd_data_t *)data)->end = 1;
}

static void
cmd_state(int argc, char **argp, void *data) {
    cpu_instance_t *cpu = ((cmd_data_t *)data)->cpu;

    printf("IR: %.4o\tMQ: %.4o\n"
           "PC: %.4o\tAC: %.4o\n"
           "LINK: %o\n"
           "IF: %.2o\t\tDF: %.2o\n"
           "FLO: %i\t8MODE: %i\n"
           "SS: %.2o\tLS: %.4o\tRS: %.4o\n",
           cpu->ir, cpu->mq,
           cpu->pc, cpu->ac, cpu->l,
           cpu->ifr, cpu->dfr,
           cpu->flags & CPU_FLAGS_FLO ? 1 : 0,
           cpu->flags & CPU_FLAGS_8MODE ? 1 : 0,
           cpu->ss, cpu->ls, cpu->rs);

}

static void
cmd_load(int argc, char **argp, void *data) {
    cpu_instance_t *cpu = ((cmd_data_t *)data)->cpu;
    int words;
    FILE *f;
    int offset = 0;

    if (argc <= 1 || argc > 3) {
        lprintf(LOG_ERROR,
                "Invalid number of parameters for 'load'.\n");
        return;
    }

    f = fopen(argp[1], "r");
    if (f == NULL) {
        lprintf(LOG_ERROR,
                "Failed to input open file: %s\n",
                strerror(errno));
        return;
    }

    if (argc >= 3)
        offset = atoi(argp[2]);

    words = load_bin(f, offset, cpu->core, CPU_CORE_SIZE);
    if (words == -1) {
        lprintf(LOG_ERROR,
                "Failed to load RIM-file...\n"
                "Core might be dirty.\n");
        fclose(f);
        return;
    }

    lprintf(LOG_NORMAL,
            "Loaded %i words from '%s'.\n",
            words, argp[1]);

    fclose(f);
}

static void
cmd_step(int argc, char **argp, void *data) {
    cpu_step(((cmd_data_t *)data)->cpu);
}

static void
cmd_run(int argc, char **argp, void *data) {
    cpu_instance_t *cpu = ((cmd_data_t *)data)->cpu;
    unsigned int i = 0;
    clock_t start = clock();
    clock_t stop;

    cpu_set_flag(cpu, CPU_FLAGS_RUN);
    lprintf(LOG_VERBOSE, "Running...\n");
    while (cpu->flags & CPU_FLAGS_RUN) {
        i++;
        cpu_step(cpu);
        if (i % 1000 == 0) {
            vr12_fade();
#ifdef HAVE_SDL
            sdl_update();
#endif
        }
    }
    stop = clock();

    lprintf(LOG_NORMAL, "Halted at %.4o.\n", cpu->pc);
    lprintf(LOG_NORMAL,
            "Did %i instructions in %f seconds (%f i/s)\n",
            i,
            ((double)(stop - start))/((double)CLOCKS_PER_SEC),
            ((double)i * (double)CLOCKS_PER_SEC)/((double)(stop - start))
        );
}

static void
cmd_set(int argc, char **argp, void *data) {
    cpu_instance_t *cpu = ((cmd_data_t *)data)->cpu;
    char *reg = argp[1];
    int value;
    int addr;

    if (sscanf(argp[2], "%i", &value) != 1) {
        lprintf(LOG_ERROR, "Invalid value specified!\n");
        return;
    }

    if (sscanf(argp[1], "%i", &addr) == 1) {
        if (addr >= 0 && addr < CPU_CORE_SIZE)
            cpu->core[addr] = value;
        else
            lprintf(LOG_ERROR, "Invalid address.\n");
    } else {
        if (!strcmp(reg, "pc"))
            cpu->pc = value;
        else if (!strcmp(reg, "ac"))
            cpu->ac = value;
        else if (!strcmp(reg, "mq"))
            cpu->mq = value;
        else if (!strcmp(reg, "if"))
            cpu->ifr = value;
        else if (!strcmp(reg, "df"))
            cpu->dfr = value;
        else if (!strcmp(reg, "ss"))
            cpu->ss = value;
        else if (!strcmp(reg, "ls"))
            cpu->ls = value;
        else if (!strcmp(reg, "rs"))
            cpu->rs = value;
        else
            lprintf(LOG_ERROR, "Invalid register specified!\n");
    }
}

static void
cmd_dump(int argc, char **argp, void *data) {
    cpu_instance_t *cpu = ((cmd_data_t *)data)->cpu;
    int i;
    int start, end;

    if (sscanf(argp[1], "%i", &start) != 1 ||
        start >= CPU_CORE_SIZE) {
        lprintf(LOG_ERROR, "Illegal start address received.\n");
        return;
    }

    if (sscanf(argp[2], "%i", &end) != 1 ||
        end >= CPU_CORE_SIZE || end < start) {
        lprintf(LOG_ERROR, "Illegal end address received.\n");
        return;
    }

    for (i = start; i <= end; i++) {
        if (i % 010 == 0)
            printf("\n%.4o: ", i);

        printf(" %.4o", cpu->core[i]);
    }
    printf("\n");
}

static void
cmd_linc(int argc, char **argp, void *data) {
    cpu_instance_t *cpu = ((cmd_data_t *)data)->cpu;
    cpu_clear_flag(cpu, CPU_FLAGS_8MODE);
}

static void
cmd_pdp8(int argc, char **argp, void *data) {
    cpu_instance_t *cpu = ((cmd_data_t *)data)->cpu;
    cpu_set_flag(cpu, CPU_FLAGS_8MODE);
}

static void
cmd_script(int argc, char **argp, void *data) {
    cpu_instance_t *cpu = ((cmd_data_t *)data)->cpu;
    shell_script(cpu, argp[1]);
}

static const parser_command cmds[] = {
    {"exit", 0, &cmd_exit, "Exits the emulator", NULL},
    {"state", 0, &cmd_state, "Prints the CPU state.", NULL},
    {"load", -1, &cmd_load, "Loads a paper tape into memory.",
     "load FILENAME [LOAD OFFSET]\nLoads FILENAME into memory."},
    {"script", 1, &cmd_script, "Executes a script.",
     "script FILENAME\nExecutes the commands in FILENAME."},
    {"set", 2, &cmd_set, "Sets a processor register or memory address.",
     "set REGISTER VALUE\nSets REGISTER to the value VALUE."},
    {"step", 0, &cmd_step, "Executes the next instruction.", NULL},
    {"run", 0, &cmd_run, "Runs the loaded program from the loaded PC.", NULL},
    {"dump", 2, &cmd_dump, "Dumps the core to the console.", NULL},
    {"pdp8", 0, &cmd_pdp8, "Enables PDP-8 mode.", NULL},
    {"linc", 0, &cmd_linc, "Enables LINC mode.", NULL},
    {NULL, 0, NULL, NULL}
};

void
shell_script(cpu_instance_t *cpu, const char *name) {
    FILE *script;
    char *cmd = malloc(MAX_SCRIPT_LINE_LENGTH);
    cmd_data_t cd;

    cd.cpu = cpu;
    cd.end = 0;

    script = fopen(name, "r");
    if (script) {
        do {
            fgets(cmd, MAX_SCRIPT_LINE_LENGTH, script);
            if (!feof(script)) {
                lprintf(LOG_NORMAL, "PDP12>%s", cmd);
                parser_exec(cmds, &cd, cmd);
            } else {
                break;
            }
        } while (!cd.end);
        fclose(script);
    } else {
        lprintf(LOG_ERROR, "The script '%s' can't be opened: %s\n",
                name, strerror(errno));
    }

    free(cmd);
}

void
shell_start(cpu_instance_t *cpu) {
    char *c;
    cmd_data_t cd;

    cd.cpu = cpu;
    cd.end = 0;

    do {
        /*
         * If readline functionallity is emulated by in readln.c and
         * if the "real thing" isn't installed.
         */
        c = readline("PDP12>");
        if (c && *c) {
#ifdef HAVE_READLINE_HISTORY
            add_history(c);
#endif
            parser_exec(cmds, &cd, c);
            free(c);
        }
    } while (!cd.end && c);
    if (!c)
        printf("\n");

}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
