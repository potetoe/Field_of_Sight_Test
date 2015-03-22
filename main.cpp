#include "global.h"
#include "SDL_INIT.h"
#include "Main_State.h"

SDL_Window* Window = NULL;
SDL_Renderer* Renderer = NULL;

SDL_Event e;

TTF_Font* Font;

//Window lengths.
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 576;

//defined later
int MAP_WIDTH = 25;
int MAP_HEIGHT = 18;

//tile width and height
int main_length = 32;

unsigned int timestep = 33; //sets game's frame rate
bool draw_collision_rects = false; //draws collision points
std::vector<std::pair<int,int>> collision_rects;

bool quit = false;

int main(int argc, char* argv[])
{

  if(!init()) {
    std::cerr<<"FAILED TO INITIALIZE. \n";
  } else {

    //load font
    Font = TTF_OpenFont( "base/PotetoFont.ttf", 30 );
    if(Font == NULL)
      std::cerr<<"Failed to load PotetoFont.ttf!"<<SDL_GetError()<<"\n";

    main_state state;

    //interpolation values
    unsigned int t_accum=0,
                 lt=0,
                 ct=0,
                 t=0;

    timestep = 33;

    while(!quit)
    {
      while( SDL_PollEvent( &e ) != 0 ) {
        //User requests quit
        if( e.type == SDL_QUIT )
          quit = true;
        //input
        state.input();
      }

      ct = SDL_GetTicks();
      t_accum += ct - lt;
      lt = ct;

      while(t_accum >= timestep ){
        t += timestep; /* this is our actual time, in milliseconds. */
        t_accum -= timestep;

        //logic
        state.logic();

      }

      //Clear screen with red so we can see any bugs.
      SDL_SetRenderDrawColor( Renderer, 0, 0, 0, 255 );
      SDL_RenderClear( Renderer );

      const double interpolation = (float)t_accum / (timestep); //interpolation.

      //render
      state.render(interpolation);

      //Update Screen
      SDL_RenderPresent( Renderer );

      SDL_Delay(3); //to save cpu
    }
  }

  SDL_DestroyRenderer( Renderer );
  SDL_DestroyWindow( Window );

  Window = NULL;
  Renderer = NULL;

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();

  return 0;
}
