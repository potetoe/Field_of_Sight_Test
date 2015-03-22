#ifndef MAIN_STATE_H_INCLUDED
#define MAIN_STATE_H_INCLUDED

#include "global.h"
#include "option_menu.h"
#include "FOV.h"

class main_state
{
private:
  TextureImage Sprite_Sheet;
  SDL_Rect sheet_clips[4] {
    {0, 0,16,16}, //stone floor
    {16,0,16,16}, //crate
    {32,0,16,16}, //black
    {48,0,16,16}  //player
  };

  //we use this to draw collision points.
  SDL_Rect redundant_rect
  {0,0,32,32};

  TextureImage Option_Background;

  //All the input / rendering for the option menu, when needed.
  Option_Manager option_menu;

  //field of view
  FOVRecurse field_of_view;

  std::vector<std::vector<int>> tileset;
  std::vector<std::pair<int,int>> view_field;

  int xpos = 16, ypos = 16; //player coords
  int xpos2 = 16, ypos2 = 16; //interpolation coords (last position)
  const int vel = 4;
  int last_tile_x = 1, last_tile_y = 1; //if we moved over a tile, so we don't excessively do the field of view.
  int camera_x = 0, camera_y = 0; //camera position
  bool vUp = false, vDown = false, vLeft = false, vRight = false;//keys pressed

  bool options_state = false; //pressing tab

  //main_state(bool _); //delegating constructor to do tileset

public:
  main_state();
  void input();
  void logic();
  void render(double interpolation);
  ~main_state();
};

#endif // MAIN_STATE_H_INCLUDED
