#include "global.h"

//For moving entities, any size, returns -1 if unknown, otherwise it returns how far to go back.
int Collision(const SDL_Rect& a,const int& dir,const int& dist,const std::vector<std::vector<int>>& tiles) {
  //In short we use collision points, which are 1x1 points scattered on the hitbox, and collision points must be less then 16 apart.

  //std::chrono::steady_clock::time_point benchmark = std::chrono::steady_clock::now(); //benchmark

  if(draw_collision_rects)collision_rects.clear();

  int nearest = 0; //distance how far to go back, -1 = no collision -2 = unknown
  bool out = false; //we gotta use this :/

  //diagonal collision is handled differently than the normal directions.
  if(dir == Facing_Up || dir == Facing_Down || dir == Facing_Left || dir == Facing_Right) {
    /*
    a 16x16 entity that moved left(or right, the points would be at the same place)
    |---------|###
    |1.......2|.3#
    |.........|..#
    |.........|..#
    |4.......5|.6#
    |---------|###
    */

    //I've been thinking of ways to make this more efficient, mainly
    //on trying to make the placing point part not repeat tiles
    //using a set, but it just seems so inefficient... maybe there
    //could be another way... maybe a vector of bools, but that also
    //wastes resources... But I haven't tested and benchmarked
    //these ideas yet, so maybe they are faster... or maybe I'm just lazy...

    //modified player values.
    int m_x = a.x, m_y = a.y, m_w = a.w, m_h = a.h;

    //expanding the box with the moved distance---------------------------
    //note: if you are confused about this, we already moved, so yea...
    switch(dir) {
    case Facing_Up:
      m_h += dist; break;
    case Facing_Down:
      m_y -= dist; m_h += dist; break;
    case Facing_Left:
      m_w += dist; break;
    case Facing_Right:
      m_x -= dist; m_w += dist; break;
    }

    //magic, here we find the nearness(distance) from a collision point
    auto check_collision = [&](const int& tile_x, const int& tile_y) {
      if(draw_collision_rects)collision_rects.emplace_back(std::pair<int,int>(tile_x,tile_y));
      const int x=tile_x*16, y=tile_y*16;
      if(tile_x >= 0 && tile_y >= 0 && tile_x < MAP_WIDTH && tile_y < MAP_HEIGHT) {
        if(tiles[tile_x][tile_y] == tile_solid)
          switch(dir) {
            case Facing_Up: if( y + 16 - m_y > nearest) nearest = y + 16 - m_y; break;
            case Facing_Down: if( m_y + m_h - y > nearest) nearest = m_y + m_h - y; break;
            case Facing_Left: if( x + 16 - m_x > nearest) nearest = x + 16 - m_x; break;
            case Facing_Right: if( m_x + m_w - x > nearest) nearest = m_x + m_w - x; break;
          }
        else if(tiles[tile_x][tile_y] == -1) out = true; //if it is in unseen
      }
    };

    //Some consts----------
    const int l=m_x/16 /*left column*/,
              t=m_y/16 /*top row*/,
              w=(m_x+m_w-1)/16 /*right column*/,
              h=(m_y+m_h-1)/16; /*bottom row*/

    //==========Place the points while checking==========
    //The Top chunk
    for(int y=t; y<=h; ++y) {
      for(int x=l; x<=w && m_w!=1; ++x)
        check_collision(x,y);
      check_collision(w,y);
    }
    //The bottom line
    if(m_h!=1){
      for(int x=l; x<=w && m_w!=1; ++x)
        check_collision(x,h);
      check_collision(w,h);
    }
  }
  else
  {

    /*
    A 6x6 entity that moved downright(or upleft)
    10 units on a map with 5x5 tiles, with the
    points ordered 1-9, A-V

    1----6A\
    |....||C\
    |....|| E\
    |....||  G\
    |....||   I\
    8----2|---|K\
    B-----|   | M\
    \D   |    |  O\
     \F  |    |   Q\
      \H |    |    S\
       \J|----3----|U
        \L    |....|7
         \N   |....||
          \P  |....||
           \R |....||
            \T|----4|
             \V9----5

    (heh, all the diagonal lines are
    conveniently using the alphabet.)

    16J? is the old position if we are going
    downright, and 3GTV is the new, note the 3
    would not be there if 10 wasn't dividable
    by 5.

    Also if the size was 7x7, the box 2??3
    would be complete, and a couple other
    points too.
    */

    //modified player values, later on changed.
    int m_x = a.x, m_y = a.y;

    switch(dir) {
    case Facing_Down_Right:
      m_x-=dist; m_y-=dist; break;
    case Facing_Down_Left:
      m_y-=dist; break;
    case Facing_Up_Right:
      m_x-=dist; break;
    }

    //magic, here we find the nearness(distance) from a collision point
    auto check_collision = [&](const int& tile_x, const int& tile_y) {
      if(draw_collision_rects)collision_rects.emplace_back(std::pair<int,int>(tile_x,tile_y));
      const int x=tile_x*16, y=tile_y*16;
      if(tile_x >= 0 && tile_y >= 0 && tile_x < MAP_WIDTH && tile_y < MAP_HEIGHT) {
        if(tiles[tile_x][tile_y] == tile_solid)
          switch(dir) {
          case Facing_Up_Left:
            if( y + 16 - m_y <= x + 16 - m_x && y + 16 - m_y > nearest ) nearest = y + 16 - m_y;
            else if( y + 16 - m_y > x + 16 - m_x && x + 16 - m_x > nearest ) nearest = x + 16 - m_x;
            break;
          case Facing_Down_Right:
            if( m_y + a.h + dist - y <= m_x + a.w + dist - x && m_y + a.h + dist - y > nearest ) nearest = m_y + a.h + dist - y;
            else if( m_y + a.h + dist - y > m_x + a.w + dist - x && m_x + a.w + dist - x > nearest ) nearest = m_x + a.w + dist - x;
            break;
          case Facing_Up_Right:
            if( y + 16 - m_y <= m_x + a.w + dist - x && y + 16 - m_y > nearest ) nearest = y + 16 - m_y;
            else if( y + 16 - m_y > m_x + a.w + dist - x && m_x + a.w + dist - x > nearest ) nearest = m_x + a.w + dist - x;
            break;
          case Facing_Down_Left:
            if( m_y + a.h + dist - y <= x + 16 - m_x && m_y + a.h + dist - y > nearest ) nearest = m_y + a.h + dist - y;
            else if( m_y + a.h + dist - y > x + 16 - m_x && x + 16 - m_x > nearest ) nearest = x + 16 - m_x;
            break;
          }
        else if(tiles[tile_x][tile_y] == -1) out = true; //if it is in unseen
      }
    };

    //Some consts----------
    const int tiles_wide=(a.w-1)/16,
              tiles_tall=(a.h-1)/16,
              dist_wide=(dist-1)/16,
              dist_tall=(dist-1)/16,
              l=m_x/16,                //left column
              t=m_y/16,                //top row
              w=(m_x+a.w+dist-1)/16,   //right column
              h=(m_y+a.h+dist-1)/16,   //bottom row
              w2=(m_x+dist-1)/16;      //I dont even know man, please dont ask.

    switch(dir) {
    case Facing_Up_Left: case Facing_Down_Right:
      //Filling the middle
      if(a.w!=1 && a.h!=1){
        for(int j=0, x=l; j<=tiles_wide; ++j, ++x){ //top row cascade
          check_collision(x,t);
          for(int i=1, y=t+1; i<=dist_tall; ++i, ++y)
            check_collision(x+i,y);
          check_collision(w,h-j);
        } for(int j=1, y=t+1; j<=tiles_tall; ++j, ++y){ //left column cascade
          check_collision(l,y);
          for(int i=1, x=l+1; i<=dist_wide; ++i, ++x)
            check_collision(x,y+i);
          check_collision(w-j,h);
        }
      }
      //The two lines...
      for(int i=0; i<dist; ++i){
        check_collision((m_x+a.w+i)/16,(m_y+i)/16); //AHH THE INEFFICENCY!
        if(a.h != 1 && a.w != 1){ //2nd line
          check_collision((m_x+i)/16,(m_y+a.h+i)/16);
        }
      }
      break;

    //laaaaaaazy cheat, w2 works with Facing downleft, but w-tiles_wide works with upright...
    //But until I find a bug that forces me to rewrite this to be super elegant like the strait line collisions, this will have to do...

    case Facing_Down_Left:
      //Filling the middle
      if(a.w!=1 && a.h!=1){
        for(int j=0, x=w2; j<=tiles_wide; ++j, --x){ //top row cascade  <--weird w2 here
          check_collision(x,t);
          for(int i=1, y=t+1; i<=dist_tall; ++i, ++y)
            check_collision(x-i,y);
          check_collision(l,h-tiles_tall+j);
        } for(int j=1, y=t+1; j<=tiles_tall; ++j, ++y){ //right column cascade
          check_collision(w,y);
          for(int i=1, x=w2; i<=dist_wide; ++i, ++x)  // <-- the other weird w2
            check_collision(x,y+i);
          check_collision(l+j,h-tiles_tall);
        }
      }
      //The two lines...
      for(int i=0; i<dist; ++i){
        check_collision((m_x+dist-i)/16,(m_y+i)/16);
        if(a.h != 1 && a.w != 1){ //2nd line
          check_collision((m_x+dist+a.w-1-i)/16,(m_y+a.h+i)/16);
        }
      }
      break;

    case Facing_Up_Right:
      //Filling the middle
      if(a.w!=1 && a.h!=1){
        for(int j=0, x=w-tiles_wide; j<=tiles_wide; ++j, --x){ //top row cascade <- weird w-tiles_wide here
          check_collision(x,t);
          for(int i=1, y=t+1; i<=dist_tall; ++i, ++y)
            check_collision(x-i,y);
          check_collision(l,h-tiles_tall+j);
        } for(int j=1, y=t+1; j<=tiles_tall; ++j, ++y){ //right column cascade
          check_collision(w,y);
          for(int i=1, x=w-tiles_wide; i<=dist_wide; ++i, ++x)  //<- weird w-tiles_wide here too
            check_collision(x,y+i);
          check_collision(l+j,h-tiles_tall);
        }
      }
      //The two lines...
      for(int i=0; i<dist; ++i){
        check_collision((m_x+dist-i)/16,(m_y+i)/16);
        if(a.h != 1 && a.w != 1){ //2nd line
          check_collision((m_x+dist+a.w-1-i)/16,(m_y+a.h+i)/16);
        }
      }
      break;
    }
  }

  //HANDLING BOARDER COLLISIONS-------------

  if(!nearest) {
    if(a.x < 0){                            //--left--
      if(a.y < 0 && a.y < a.x)              //top left
        nearest = -a.y;
      else if(a.y+a.h > MAP_HEIGHT*16       //bottom left
              && a.y+a.h-(MAP_HEIGHT*16) > -a.x)
        nearest = a.y+a.h-(MAP_HEIGHT*16);
      else
        nearest = -a.x;
    } else if(a.y < 0){                     //--top-- (left eliminated top left)
      if(a.x+a.w > MAP_WIDTH*16             //top right
         && a.x+a.w-(MAP_WIDTH*16) > -a.y)
        nearest = a.x+a.w-(MAP_WIDTH*16);
      else
        nearest = -a.y;
    } else if(a.x+a.w > MAP_WIDTH*16){      //--right-- (top eliminated top right)
      if(a.y+a.h > MAP_HEIGHT*16            //bottom right
         && a.y+a.h-(MAP_HEIGHT*16) > a.x+a.w-(MAP_WIDTH*16))
        nearest = a.y+a.h-(MAP_HEIGHT*16);
      else
        nearest = a.x+a.w-(MAP_WIDTH*16);
    } else if(a.y+a.h > MAP_HEIGHT*16)      //down (left eliminated bottom left)
      nearest = a.y+a.h-(MAP_HEIGHT*16);
  }

  //std::cout<<"time:"<<std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - benchmark).count()<<"\n";

  if(out && !nearest)return -1;
  return nearest;
}
