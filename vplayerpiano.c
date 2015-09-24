/*
 * Copyright 2015 Nicholas Parkanyi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SDL.h>
#include <stdio.h>
#include "libmidi/midi.h"

#define MIDI_ERROR(errstr) printf("libmidi: " errstr "\n")
#define MIDI_ERROR_VARS(errstr, vars) printf("libmidi: " errstr, vars); puts("\n")
#define DEFAULT_KEY_WIDTH 16
#define RES_X 832
#define RES_Y 96

typedef struct {
  int key_width; //width (including gap) of a white key in pixels
  int key_height; //determined relative to the width
  int key_states[88];
  SDL_Rect key_rects[88];
} Keyboard;

void keyboard_init(Keyboard * kbd);
void gen_key_rects(Keyboard * kbd);
void draw_keyboard(Keyboard * kbd, SDL_Renderer * out);
int is_black_key(int i);

int main(int argc, char * argv[]){
  int r;
  MIDIFile midi;
  MIDITrack * tracks;
  MIDIEvent ** ptrs;
  MIDIEventIterator * iters;
  long conversion;
  unsigned long ticks[10] = {0, 0, 0, 0, 0};

  SDL_Window * win;
  SDL_Renderer * screen;
  Keyboard kbd;


  /*******************
   * Load MIDI file
   *******************/
  r = MIDIFile_load(&midi, argv[1]);
  switch (r){
    case FILE_IO_ERROR:
      MIDI_ERROR("Failed to open file!");
      return 1;
    case FILE_INVALID:
      MIDI_ERROR("Invalid MIDI file!");
      return 1;
  }

  tracks = malloc(sizeof(MIDITrack) * midi.header.num_tracks);
  ptrs = malloc(sizeof(MIDIEvent*) * midi.header.num_tracks);
  iters = malloc(sizeof(MIDIEventIterator) * midi.header.num_tracks);
  for (int i = 0; i < midi.header.num_tracks; i++){
    r = MIDITrack_load(&tracks[i], midi.file);
    switch(r){
      case FILE_INVALID:
        MIDI_ERROR_VARS("Track %d: Invalid data!", i);
        break;
      case FILE_IO_ERROR:
        MIDI_ERROR_VARS("Track %d: File IO problem!", i);
        break;
      case MEMORY_ERROR:
        MIDI_ERROR("Failed to allocate memory!");
        break;
    }
  }

  conversion = 60000 / (120 * midi.header.time_div);

  for (int i = 0; i < midi.header.num_tracks; i++){
    iters[i] = MIDIEventList_get_start_iter(tracks[i].list);
    if (!iters[i].node){
      MIDI_ERROR_VARS("Track %d failed to load!", i);
    }
  }


  /**************************
   * Set up graphical output
   **************************/
  keyboard_init(&kbd);
  kbd.key_states[50] = 1;
  kbd.key_states[49] = 1;
  SDL_Init(SDL_INIT_VIDEO);

  win = SDL_CreateWindow("vplayerpiano", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, RES_X, RES_Y, 0);
  if (!win){
    printf("Failed to open window!\n");
  }

  screen = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
  if (!screen){
    printf("Failed to initialize renderer!\n");
  }

  while(1){
    for (int i = 0; i < midi.header.num_tracks; i++){
      ptrs[i] = MIDIEventList_get_event(iters[i]);
      if (ptrs[i]->type == EV_NOTE_ON && ptrs[i]->delta_time * conversion <= SDL_GetTicks() - ticks[i]){
        if (((MIDIChannelEventData*)(ptrs[i]->data))->param2){
          kbd.key_states[((MIDIChannelEventData*)(ptrs[i]->data))->param1 - 33] = 1;
        } else {
          kbd.key_states[((MIDIChannelEventData*)(ptrs[i]->data))->param1 - 33] = 0;
        }
        iters[i] = MIDIEventList_next_event(iters[i]);
        ticks[i] = SDL_GetTicks();
      } else if (ptrs[i]->type == EV_NOTE_OFF && ptrs[i]->delta_time * conversion <= SDL_GetTicks() - ticks[i]){
        kbd.key_states[((MIDIChannelEventData*)(ptrs[i]->data))->param1 - 33] = 0;
        iters[i] = MIDIEventList_next_event(iters[i]);
        ticks[i] = SDL_GetTicks();
      } else if (ptrs[i]->type != EV_NOTE_OFF && ptrs[i]->type != EV_NOTE_ON /*&&
                                                                       ptr->delta_time * conversion <= SDL_GetTicks() - ticks*/){
        iters[i] = MIDIEventList_next_event(iters[i]);
        ticks[i] = SDL_GetTicks();
      }
    }
    draw_keyboard(&kbd, screen);
    SDL_RenderPresent(screen);
  }

  SDL_DestroyRenderer(screen);
  SDL_DestroyWindow(win);
  SDL_Quit();
  for (int i = 0; i < midi.header.num_tracks; i++){
    MIDITrack_delete_events(&tracks[i]);
  }
  fclose(midi.file);
  return 0;
}

void keyboard_init(Keyboard * kbd){
  kbd->key_width = DEFAULT_KEY_WIDTH;
  kbd->key_height = 6 * kbd->key_width;
  for (int i = 0; i < 88; i++){
      kbd->key_states[i] = 0;
  }

  gen_key_rects(kbd);
}

void gen_key_rects(Keyboard * kbd){
  int offset = 0;
  int black_width = (2 * kbd->key_width) / 3;
  int black_height = (2 * kbd->key_height) / 3;

  for (int i = 0; i < 88; i++){
    if (i % 12 == 1 || i % 12 == 6){ //Bb and Eb
      kbd->key_rects[i].h = black_height;
      kbd->key_rects[i].w = black_width;
      kbd->key_rects[i].x = offset - kbd->key_width / 3;
      kbd->key_rects[i].y = 0;
    } else if (i % 12 == 4 || i % 12 == 9){ //Db and Gb
      kbd->key_rects[i].h = black_height;
      kbd->key_rects[i].w = black_width;
      kbd->key_rects[i].x = offset - kbd->key_width * 3 / 8;
      kbd->key_rects[i].y = 0;
    } else if (i % 12 == 11){ //Ab
      kbd->key_rects[i].h = black_height;
      kbd->key_rects[i].w = black_width;
      kbd->key_rects[i].x = offset - kbd->key_width / 3;
      kbd->key_rects[i].y = 0;
    } else { //white notes
      kbd->key_rects[i].h = kbd->key_height;
      kbd->key_rects[i].w = kbd->key_width - 2;
      kbd->key_rects[i].x = offset + 1;
      kbd->key_rects[i].y = 0;
      offset += kbd->key_width;
    }
  }
}

void draw_keyboard(Keyboard * kbd, SDL_Renderer * out)
{
  //draw keyboard
  SDL_SetRenderDrawColor(out, 100, 100, 100, 255);
  SDL_RenderClear(out);
  for (int i = 0; i < 88; i++){
    if (!is_black_key(i)){
      if (kbd->key_states[i]){
        SDL_SetRenderDrawColor(out, 200, 30, 30, 255);
      } else {
        SDL_SetRenderDrawColor(out, 255, 255, 255, 255);
      }
      SDL_RenderFillRect(out, &kbd->key_rects[i]);
    }
  }
  for (int i = 0; i < 88; i++){
    if (is_black_key(i)){
      if (kbd->key_states[i]){
        SDL_SetRenderDrawColor(out, 30, 200, 30, 255);
      } else {
        SDL_SetRenderDrawColor(out, 0, 0, 0, 255);
      }
      SDL_RenderFillRect(out, &kbd->key_rects[i]);
    }
  }
}

int is_black_key(int i){
  return (i % 12 == 1 || i % 12 == 4 || i % 12 == 6 || i % 12 == 9
      || i % 12 == 11);
}
