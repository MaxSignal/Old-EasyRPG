/* font.h, EasyRPG player font class declaration file.
   Copyright (C) 2007 EasyRPG Project <http://easyrpg.sourceforge.net/>.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef FONT_H_
#define FONT_H_

class Font{
protected:
          int size;
          int fR,  fG,  fB,  fU; // fU = unused
          const char * Fname;
public:
      void init_Font();
      SDL_Surface* drawText(char* string);
      SDL_Surface* drawText(char* string,int r, int b,int g, int u);
      SDL_Surface* drawText(const char* string);
      void draw_temp_Text(SDL_Surface* screen,char* string, int x, int y);
      void Quit();

};

#endif
