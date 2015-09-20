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

#define DEFAULT_KEY_WIDTH 9

typedef struct {
  int key_width; //width (including gap) of a white key in pixels
  int key_height; //determined relative to the width
  int key_states[88];
  SDL_Rect key_rects[88];
} Keyboard;

void keyboard_init(Keyboard * kbd);

void gen_key_rects(Keyboard * kbd);

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
  int black_width = 2 / 3 * kbd->key_width;
  int black_height = 2 / 3 * kbd->key_height;

  for (int i = 0; i < 88; i++){
    if (i % 12 == 1 || i % 12 == 6){ //Bb and Eb
      kbd->key_rects[i].h = black_height;
      kbd->key_rects[i].w = black_width;
      kbd->key_rects[i].x = offset - kbd->key_width / 6;
      kbd->key_rects[i].y = 0;
    } else if (i % 12 == 4 || i % 12 == 9){ //Db and Gb
      kbd->key_rects[i].h = black_height;
      kbd->key_rects[i].w = black_width;
      kbd->key_rects[i].x = offset - 1 / 2 * kbd->key_width;
      kbd->key_rects[i].y = 0;
    } else if (i % 12 == 11){ //Ab
      kbd->key_rects[i].h = black_height;
      kbd->key_rects[i].w = black_width;
      kbd->key_rects[i].x = offset - 1 / 3 * kbd->key_width;
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

int main(){
  Keyboard kbd;
  
  keyboard_init(&kbd);
  
  printf("Hello\n\n");
  return 0;
}
