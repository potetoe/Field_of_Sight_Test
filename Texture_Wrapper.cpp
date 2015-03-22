
#include "Texture_Wrapper.h"

TextureImage::TextureImage()
{
	//Initialize
	The_Texture = NULL;
	image_width = 0;
	image_height = 0;
}

TextureImage::~TextureImage()
{
	//Deallocate
	free();
}

bool TextureImage::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		std::cerr<<"Unable to load image! Error:"<<SDL_GetError()<<"\n";
		std::cerr << "Path:" << path << '\n';
	}
	else
	{
    //Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( Renderer, loadedSurface );

		if( newTexture == NULL )
		{
			std::cerr<<"Unable to create texture: Error:"<<SDL_GetError()<<"\n";
      std::cerr << "Path:" << path << '\n';
		}
		else
		{
			//Get image dimensions
			image_width = loadedSurface->w;
			image_height = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	The_Texture = newTexture;

	return The_Texture != NULL;
}

bool TextureImage::loadFromFile_Greyscale( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );

	if( loadedSurface == NULL )
	{
		std::cerr<<"Unable to load image! Error:"<<SDL_GetError()<<"\n";
		std::cerr << "Path:" << path << '\n';
	}
	else
	{

		loadedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_ARGB8888, 0);

    Uint32 * pixels = (Uint32 *)loadedSurface->pixels;

    for (int y = 0; y < loadedSurface->h; y++)
    {
        for (int x = 0; x < loadedSurface->w; x++)
        {
            Uint32 pixel = pixels[y * loadedSurface->w + x];
            // TODO convert pixel to grayscale here
            Uint8 r = pixel >> 16 & 0xFF;
            Uint8 g = pixel >> 8 & 0xFF;
            Uint8 b = pixel & 0xFF;

            Uint8 v = (r + g + b) / 3;

            pixel = (0xFF << 24) | (v << 16) | (v << 8) | v;

            if(r == 0 && g == 0xFF && b == 0xFF)
            {
              v = 0;
            }
            else
            {
              pixels[y * loadedSurface->w + x] = pixel;
            }
        }
    }

    //loadedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_ARGB8888, 0);

    //Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( Renderer, loadedSurface );

		if( newTexture == NULL )
		{
			std::cerr<<"Unable to create texture! Error:"<<SDL_GetError()<<"\n";
      std::cerr << "Path:" << path << '\n';
		}
		else
		{
			//Get image dimensions
			image_width = loadedSurface->w;
			image_height = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	The_Texture = newTexture;

	SDL_SetTextureColorMod( The_Texture, 100,100,100);

	return The_Texture != NULL;
}

bool TextureImage::loadFromRenderedText( std::string textureText, SDL_Color textColor, TTF_Font* font )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended( font, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        The_Texture = SDL_CreateTextureFromSurface( Renderer, textSurface );
		if( The_Texture == NULL )
		{
			std::cerr<<"Unable to create texture from rendered text! Error:"<<SDL_GetError()<<"\n";
		}
		else
		{
			//Get image dimensions
			image_width = textSurface->w;
			image_height = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else if( image_width != 0 && image_height != 0 )
	{
		std::cerr<<"Unable to render text surface! Error:"<<SDL_GetError()<<"\n";
	}


	//Return success
	return The_Texture != NULL;
}

bool TextureImage::loadFromRenderedTextWrapped( std::string textureText, SDL_Color textColor, TTF_Font* font, int width )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped( font, textureText.c_str(), textColor,width );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        The_Texture = SDL_CreateTextureFromSurface( Renderer, textSurface );
		if( The_Texture == NULL )
		{
			std::cerr<<"Unable to create texture from rendered text! Error:"<<SDL_GetError()<<"\n";;
		}
		else
		{
			//Get image dimensions
			image_width = textSurface->w;
			image_height = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else if( image_width != 0 && image_height != 0 )
	{
		std::cerr<<"Unable to render text surface! Error:"<<SDL_GetError()<<"\n";;
	}


	//Return success
	return The_Texture != NULL;
}

void TextureImage::FillRect(int w, int h, SDL_Color color)
{
  //Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
	if( loadedSurface == NULL )
	{
		std::cerr<<"Unable to load image! Error:"<<SDL_GetError()<<"\n";
	}
	else
	{
	  SDL_FillRect(loadedSurface, NULL, SDL_MapRGB(loadedSurface->format, color.r, color.g, color.b));

    //Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( Renderer, loadedSurface );

    SDL_SetTextureAlphaMod( newTexture, color.a );

		if( newTexture == NULL )
		{
			std::cerr<<"Unable to create texture: Error:"<<SDL_GetError()<<"\n";
		}
		else
		{
			//Get image dimensions
			image_width = loadedSurface->w;
			image_height = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	The_Texture = newTexture;
}

void TextureImage::free()
{
	//Free texture if it exists
	if( The_Texture != NULL )
	{
		SDL_DestroyTexture( The_Texture );
		The_Texture = NULL;
	}
}

void TextureImage::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( The_Texture, alpha );
}

void TextureImage::render( int x, int y, SDL_Rect *clip, bool Stretch )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, image_width, image_height };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	if( Stretch ) //essentially if on map.
    renderQuad = {x,y,main_length,main_length};

	//Render to screen
	SDL_RenderCopy(Renderer, The_Texture, clip, &renderQuad);
}
