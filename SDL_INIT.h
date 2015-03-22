#ifndef SDL_INIT_H_INCLUDED
#define SDL_INIT_H_INCLUDED

#include "global.h"

bool init()
{
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		std::cerr<<"SDL could not initialize! Error:"<<SDL_GetError()<<"\n";
		success = false;
	}
	else
	{
		//Enable VSync
		if( !SDL_SetHint( SDL_HINT_RENDER_VSYNC, "0" ) )
		{
			std::cerr<<"Warning: VSync not enabled! Error:"<<SDL_GetError()<<"\n";
		}

		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) )
		{
			std::cerr<<"Warning: Linear texture filtering not enabled! Error:"<<SDL_GetError()<<"\n";
		}

		//Create window
		Window = SDL_CreateWindow( "Field_of_View_Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( Window == NULL )
		{
			std::cerr<<"Window could not be created! Error:"<<SDL_GetError()<<"\n";
			success = false;
		}
		else
		{
			//Create renderer for window
			Renderer = SDL_CreateRenderer( Window, -1, SDL_RENDERER_ACCELERATED );
      if( Renderer == NULL )
      {
        std::cerr<<"Renderer could not be created! Error:"<<SDL_GetError()<<"\n";
        success = false;

      }

      //Initialize PNG loading
      int imgFlags = IMG_INIT_PNG;
      if( !( IMG_Init( imgFlags ) & imgFlags ) )
      {
        std::cerr<<"SDL_image could not initialize! Error:"<<SDL_GetError()<<"\n";
        success = false;
      }

      else
      {
        //Initialize SDL_ttf
        if( TTF_Init() == -1 )
        {
          std::cerr<<"SDL_ttf could not initialize! Error:"<<SDL_GetError()<<"\n";
          success = false;
        }
      }
		}
	}
	return success;
}

#endif // SDL_INIT_H_INCLUDED
