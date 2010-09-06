/*
 * Copyright (c) 2006, 2007 Andreas Sandberg
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, arepermitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in
 *          the documentation and/or other materials provided with the
 *          distribution.
 *        * Neither the name of the Greenpea project nor the names of its
 *          contributors may be used to endorse or promote products derived
 *          from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <libpdp12/cpu.h>
#include <libpdp12/iob.h>
#include <libpdp12utils/rim.h>
#include <libpdp12utils/log.h>

#ifdef HAVE_ARGP_H
#include <argp.h>
#else
#include "include/argp.h"
#endif

#ifdef HAVE_SDL
#include <SDL.h>

static int sdl_available = 0;
static SDL_Surface *screen = NULL;
static Uint32 channel_colors[2][0x100];

#endif

#include "readln.h"
#include "shell.h"

unsigned char vr12_surface[2][512][512];

static cpu_instance_t *cpu = NULL;

const char *argp_program_version =
"PDP12 simulator version " VERSION ".\n"
"Copyright (C) 2006, 2007 Andreas Sandberg <sandberg@update.uu.se>";

const char *argp_program_bug_address = "<sandberg@update.uu.se>";

typedef struct {
     char *core;
     char *script;
     int no_vr12;
} args;

const struct argp_option options[] = {
     { "debug", 'd', "level", 0, "Modifies the debug output threshold"},
     { "verbose", 'v', NULL, 0, "Produce verbose output"},
     { "no-vr12", 'n', NULL, 0, "Disable the emulation of the VR12 display."},
     { "script", 's', "FILE", 0, "The script FILE on startup."},
     { NULL, 0, NULL, 0, NULL}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
     args* a = state->input;

     switch (key) {
     case 'd':
          global_log_level = atoi(arg);
          break;

     case 'v':
          if (global_log_level > LOG_VERBOSE)
               global_log_level = LOG_VERBOSE;
          break;

     case 'n':
          a->no_vr12 = 1;
          break;

     case 's':
          a->script = arg;
          break;

     case ARGP_KEY_ARG:
          if (state->arg_num >= 1)
               argp_usage(state);

          a->core = arg;
          break;

     case ARGP_KEY_END:
          if (state->arg_num > 1)
               argp_usage(state);
          break;

     default:
          return ARGP_ERR_UNKNOWN;
     }

     return 0;
}


static const struct argp a_argp = {
     options,
     &parse_opt,
     "[core]",
     "Starts a PDP-12 simulation on the console.",
     NULL,
     NULL,
     NULL
};

static void
sig_int(int signum) {
     if (cpu && cpu->flags & CPU_FLAGS_RUN) {
          cpu_clear_flag(cpu, CPU_FLAGS_RUN);
          lprintf(LOG_NORMAL, "User requested HALT.\n");
     }
}

static void
asr33_print(unsigned char c, void *data) {
     if (global_log_level <= LOG_VERBOSE)
          lprintf(LOG_NORMAL, "asr33_print: %c (0%o)\n", c & 0177, c);
     else
          lprintf(LOG_NORMAL, "%c", c & 0177);

     cpu->asr33->printer_flag = 1;
}

static void
vr12_plot(int x, int y, int channel, void *data) {
     switch (channel) {
     case 0:
     case 1:
          vr12_surface[channel][x][y] = 0xFF;
          break;

     default:
          vr12_surface[0][x][y] = 0xFF;
          vr12_surface[1][x][y] = 0xFF;
          break;
     }
}

void
vr12_fade() {
     int x, y;

     for (x = 0; x < 512; x++) {
          for (y = 0; y < 512; y++) {
               if (vr12_surface[0][x][y] > 10)
                    vr12_surface[0][x][y] -= 10;

               if (vr12_surface[1][x][y] > 10)
                    vr12_surface[1][x][y] -= 10;
          }
     }
}

#ifdef HAVE_SDL
void
sdl_update() {
     int bpp;
     Uint8 *p;
     Uint32 c;
     int x, y;

     if (!screen)
          return;

     if (SDL_LockSurface(screen) == -1)
          return;

     bpp = screen->format->BytesPerPixel;
     for (x = 0; x < 512; x++) {
          for (y = 0; y < 512; y++) {
               c = channel_colors[0][vr12_surface[0][x][y]];
               c |= channel_colors[1][vr12_surface[1][x][y]];

               p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

               switch (bpp) {
               case 1:
                    *p = (Uint8)c;
                    break;

               case 2:
                    *(Uint16 *)p = (Uint16)c;
                    break;

               case 3:
                    p[0] = (c >> 16) & 0xff;
                    p[1] = (c >> 8) & 0xff;
                    p[2] = c & 0xff;
                    break;

               case 4:
                    *(Uint32 *)p = c;
                    break;
               }
          }
     }

     SDL_UnlockSurface(screen);
     SDL_UpdateRect(screen, 0, 0, 0, 0);
}

#endif

static void
start_emulator(args *a) {
     io_device_t *devices[] = { NULL };
     asr33_t asr33;
     vr12_t vr12;
     /* int i; */

     asr33.print = &asr33_print;
     asr33.read = NULL;
     asr33.keyboard_flag = 0;
     asr33.printer_flag = 1;
     asr33.data = NULL;

     memset(vr12_surface, '\0', 2*512*512);

     vr12.dis = &vr12_plot;
     vr12.dsc_half = NULL;
     vr12.dsc_full = NULL;
     vr12.data = NULL;

     signal(SIGINT, &sig_int);

     cpu = cpu_create();
     cpu->devices = devices;
     cpu->asr33 = &asr33;
     cpu->vr12 = &vr12;

     if (a->script)
          shell_script(cpu, a->script);

     shell_start(cpu);

     /*
       for (i = 0; devices[i]; i++)
       devices[i]->destroy(devices[i]->data);
     */

     cpu_destroy(cpu);
}

#ifdef HAVE_SDL
static void
init_sdl() {
     int i;

     sdl_available = (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) == 0);
     if (!sdl_available) {
          lprintf(LOG_WARNING,
                  "Failed to initialize SDL (%s), VR12 emulation disabled.\n",
                  SDL_GetError());
     } else {
          screen = SDL_SetVideoMode(512, 512, 24, SDL_SWSURFACE | SDL_ANYFORMAT);
          if (!screen) {
               lprintf(LOG_WARNING,
                       "Failed to create SDL surface (%s), VR12 emulation disabled.\n",
                       SDL_GetError());
          } else {
               for (i = 0; i < 0x100; i++) {
                    channel_colors[0][i] = SDL_MapRGB(screen->format, i, 0, 0);
                    channel_colors[1][i] = SDL_MapRGB(screen->format, 0, i, 0);
               }
          }
     }
}
#endif

int
main(int argc, char **argp) {
     args a;

     a.core = NULL;
     a.script = NULL;
     a.no_vr12 = 0;

     argp_parse(&a_argp, argc, argp, 0, NULL, &a);

#ifdef HAVE_SDL
     if (!a.no_vr12)
          init_sdl();
#endif

     start_emulator(&a);

#ifdef HAVE_SDL
     if (sdl_available)
          SDL_Quit();
#endif

     return 0;
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
