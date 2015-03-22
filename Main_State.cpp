#include <fstream>
#include <stdexcept>
#include <math.h>

#include "Main_State.h"

main_state::main_state() : field_of_view(tileset,view_field,last_tile_x,last_tile_y)
{
  //load map
  std::ifstream our_map( "base/map.txt" );

  //first 2 numbers are the height and width.
  our_map >> MAP_WIDTH;
  our_map >> MAP_HEIGHT;

  //initialize the map with -1's
  tileset = std::vector<std::vector<int>>(MAP_WIDTH,std::vector<int>(MAP_HEIGHT,tile_null));

  for(int i = 0, x = 0, y = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i && ++x)
  {
    int tiletype = tile_null;
    our_map >> tiletype;
    if( x >= MAP_WIDTH ) {
      ++y;
      x = 0;
    }
    if( our_map.fail() ) {
      std::cerr<<"FAILED TO LOAD map.map!\n";
      quit = true;
      return;
    } else if( tiletype == tile_non_solid || tiletype == tile_solid ) {
      try{ //insert tile, with exception for bad dimensions
        if(x < MAP_WIDTH && y < MAP_HEIGHT)
          tileset.at(x).at(y) = tiletype; //top left tile
      } catch(const std::out_of_range& oor){std::cerr<<"MAP SIZE INVALID!\n"; quit = true; return;}
    } else {
      std::cerr<<"Invalid tile type!\n";
      quit = true;
      return;
    }
  }

  //initial field of view
  field_of_view.GetVisibleCells();

  //Load sprite sheet
  if(!Sprite_Sheet.loadFromFile("base/sprite_sheet.png"))
    quit = true;

  Option_Background.FillRect(SCREEN_WIDTH,SCREEN_HEIGHT,{0,0,0,150});
}

void main_state::input()
{
  if(!options_state) {
    //movement
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
      switch(e.key.keysym.sym)
      {
      case SDLK_w: case SDLK_UP:
        vUp = true;
        break;
      case SDLK_s: case SDLK_DOWN:
        vDown = true;
        break;
      case SDLK_a: case SDLK_LEFT:
        vLeft = true;
        break;
      case SDLK_d: case SDLK_RIGHT:
        vRight = true;
        break;
      }
    if( e.type == SDL_KEYUP && e.key.repeat == 0 )
      switch(e.key.keysym.sym)
      {
      case SDLK_w: case SDLK_UP:
        vUp = false;
        break;
      case SDLK_s: case SDLK_DOWN:
        vDown = false;
        break;
      case SDLK_a: case SDLK_LEFT:
        vLeft = false;
        break;
      case SDLK_d: case SDLK_RIGHT:
        vRight = false;
        break;
      case SDLK_TAB:
        options_state = true;
        option_menu.open = true;
        vUp = false; vDown = false; vLeft = false; vRight = false;
        break;
      }
  } else
    option_menu.input();
}
void main_state::logic()
{
  if(!options_state) {

    //refresh.
    xpos2 = xpos; ypos2 = ypos;

    if(vUp || vDown || vLeft || vRight) {

      if(vUp) ypos -= vel;
      if(vDown) ypos += vel;
      if(vLeft) xpos -= vel;
      if(vRight) xpos += vel;

      int real_dir = -1;

      //find real dir by key presses
      if(vRight && !vLeft) {            //if right
        if(vDown)                         //if down
          real_dir = Facing_Down_Right;     //its down right
        else if(vUp)                      //else if up
          real_dir = Facing_Up_Right;       //its up right
        else                              //else
          real_dir = Facing_Right;          //its right
      } else if(vLeft && !vRight) {     //else if left
        if(vDown)                         //if down
          real_dir = Facing_Down_Left;      //its down left
        else if(vUp)                      //else if up
          real_dir = Facing_Up_Left;        //its up left
        else                              //else
          real_dir = Facing_Left;           //its left
      } else if(vUp && !vDown)          //else if down
        real_dir = Facing_Up;             //its
      else if(!vUp && vDown)            //else
        real_dir = Facing_Down;           //its down

      int offset = Collision({xpos, ypos, 16, 16},real_dir,vel,tileset);
        if(offset != -1 && offset != 0) {
          //carry on my wardward son there will be peace when you are done,
          //there will be cake or something :S
          if(vUp) ypos += offset;
          if(vDown) ypos -= offset;
          if(vLeft) xpos += offset;
          if(vRight) xpos -= offset;

          int offset2 = 0;

          //so that you skid on a wall instead of stick on it.
          switch(real_dir) {
          case Facing_Up_Left: xpos -= offset;
            offset2 = Collision({xpos, ypos, 16, 16},Facing_Left,offset,tileset);
            if(offset2 != -1 && offset2 != 0){ xpos += offset2; ypos -= offset;
            offset2 = Collision({xpos, ypos, 16, 16},Facing_Up,offset,tileset);
            if(offset2 != -1 && offset2 != 0) ypos += offset2; }
            break;
          case Facing_Up_Right: xpos += offset;
            offset2 = Collision({xpos, ypos, 16, 16},Facing_Right,offset,tileset);
            if(offset2 != -1 && offset2 != 0){ xpos -= offset2; ypos -= offset;
            offset2 = Collision({xpos, ypos, 16, 16},Facing_Up,offset,tileset);
            if(offset2 != -1 && offset2 != 0) ypos += offset2; }
            break;
          case Facing_Down_Left: xpos -= offset;
            offset2 = Collision({xpos, ypos, 16, 16},Facing_Left,offset,tileset);
            if(offset2 != -1 && offset2 != 0){ xpos += offset2; ypos += offset;
            offset2 = Collision({xpos, ypos, 16, 16},Facing_Down,offset,tileset);
            if(offset2 != -1 && offset2 != 0) ypos -= offset2; }
            break;
          case Facing_Down_Right: xpos += offset;
            offset2 = Collision({xpos, ypos, 16, 16},Facing_Right,offset,tileset);
            if(offset2 != -1 && offset2 != 0){ xpos -= offset2; ypos += offset;
            offset2 = Collision({xpos, ypos, 16, 16},Facing_Down,offset,tileset);
            if(offset2 != -1 && offset2 != 0) ypos -= offset2; }
            break;
          }
        }

      if(last_tile_x != xpos/16 || last_tile_y != ypos/16){
        last_tile_x = xpos/16; last_tile_y = ypos/16;
        //set the field of view
        field_of_view.GetVisibleCells();
      }
    }

  } else {
    option_menu.logic();
    if(option_menu.open == false)
      options_state = false;
  }
}
void main_state::render(double interpolation)
{
  //we have to do it here to make the interpolation smooth.
  //re-center the camera ----------------------------
  camera_x = ( Interpolate(xpos2*2,xpos*2,interpolation) + 16 ) - SCREEN_WIDTH / 2;
  camera_y = ( Interpolate(ypos2*2,ypos*2,interpolation) + 16 ) - SCREEN_HEIGHT / 2;

  //Keep the camera in bounds
  if( camera_x < 0 )
    camera_x = 0;
  else if( camera_x > (MAP_WIDTH*32) - SCREEN_WIDTH )
    camera_x = (MAP_WIDTH*32) - SCREEN_WIDTH;
  if( camera_y < 0 )
    camera_y = 0;
  else if( camera_y > (MAP_HEIGHT*32) - SCREEN_HEIGHT )
    camera_y = (MAP_HEIGHT*32) - SCREEN_HEIGHT;
  //------------------------------------------------

  for(const auto& v: view_field)
    Sprite_Sheet.render(v.first*32 - camera_x, v.second*32 - camera_y, &sheet_clips[tileset[v.first][v.second]], true);

  //render player
  Sprite_Sheet.render(Interpolate(xpos2*2,xpos*2,interpolation) - camera_x,
                      Interpolate(ypos2*2,ypos*2,interpolation) - camera_y, &sheet_clips[3], true);

  for(auto& point: collision_rects){
    SDL_SetRenderDrawColor( Renderer, 255, 0, 0, 0 );
    redundant_rect.x = point.first*32 - camera_x; redundant_rect.y = point.second*32 - camera_y;
    SDL_RenderDrawRect( Renderer, &redundant_rect );
  }

  if(options_state) {
    Option_Background.render(0,0);
    option_menu.render();
  }
}
main_state::~main_state()
{
}

bool MouseOver(SDL_Rect rect)
{
  int x, y;
  SDL_GetMouseState( &x, &y );
  //Check if mouse is in button
  bool inside = true;
  //Mouse is left of the button
  if( x < rect.x )
  {
    inside = false;
  }
  //Mouse is right of the button
  else if( x > rect.x + rect.w )
  {
    inside = false;
  }
  //Mouse above the button
  else if( y < rect.y )
  {
    inside = false;
  }
  //Mouse below the button
  else if( y > rect.y + rect.h )
  {
    inside = false;
  }
  //Mouse is outside button
  if( !inside )
  {
    return false;
  }
  //Mouse is inside button
  else { return true; }
}
