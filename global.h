#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <SDL.h>                //Programming API
#include <SDL_image.h>          //Texture loading
#include <SDL_ttf.h>            //Fonts

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <chrono>

#include "Texture_Wrapper.h"

extern SDL_Window* Window;
extern SDL_Renderer* Renderer;

extern TTF_Font* Font;

extern SDL_Event e;

//Window lengths.
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

//defined later
extern int MAP_WIDTH;
extern int MAP_HEIGHT;

extern int main_length;

extern bool quit;

//debug options
extern bool draw_collision_rects;
extern std::vector<std::pair<int,int>> collision_rects;
extern unsigned int timestep; //sets game's frame rate

//tools
int Collision(const SDL_Rect& a,const int& dir,const int& dist,const std::vector<std::vector<int>>& tiles);

bool MouseOver(SDL_Rect rect);

inline double Interpolate( double y1,double y2,double mu )
{
   return(y1*(1-mu)+y2*mu);
}

enum tiles {
  tile_non_solid,
  tile_solid,
  tile_null=-1
};

enum Sprite_Facing
{
  Facing_Up,
  Facing_Down,
  Facing_Left,
  Facing_Right,
  Facing_Up_Left,
  Facing_Up_Right,
  Facing_Down_Left,
  Facing_Down_Right
};

enum Options
{
  Option_Time_Step,
  Option_Show_Collision_Points
};

#endif // GLOBAL_H_INCLUDED
