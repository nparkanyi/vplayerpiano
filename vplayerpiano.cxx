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
#include <iostream>


class Keyboard
{
public:
  Keyboard(SDL_Surface * out);

private:
  void genKeyRects();

  int key_width = 9; //width (including gap) of a white key in pixels
  int key_height; //determined relative to the width
  bool key_states[88];
  SDL_Rect key_rects[88];
};


Keyboard::Keyboard(SDL_Surface * out) : key_height(6 * key_width){
  for (bool &key : key_states){
    key = false;
  }

  genKeyRects();
}

void Keyboard::genKeyRects(){
  int offset = 0;
  int black_width = 2 / 3 * key_width;
  int black_height = 2 / 3 * key_height;

  for (int i = 0; i < 88; i++){
    if (i % 12 == 1 || i % 12 == 6){ //Bb and Eb
      key_rects[i].h = black_height;
      key_rects[i].w = black_width;
      key_rects[i].x = offset - key_width / 6;
      key_rects[i].y = 0;
    } else if (i % 12 == 4 || i % 12 == 9){ //Db and Gb
      key_rects[i].h = black_height;
      key_rects[i].w = black_width;
      key_rects[i].x = offset - 1 / 2 * key_width;
      key_rects[i].y = 0;
    } else if (i % 12 == 11){ //Ab
      key_rects[i].h = black_height;
      key_rects[i].w = black_width;
      key_rects[i].x = offset - 1 / 3 * key_width;
      key_rects[i].y = 0;
    } else { //white notes
      key_rects[i].h = key_height;
      key_rects[i].w = key_width - 2;
      key_rects[i].x = offset + 1;
      key_rects[i].y = 0;
      offset += key_width;
    }
  }
}
