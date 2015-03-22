#ifndef TEXTURE_WRAPPER_H_INCLUDED
#define TEXTURE_WRAPPER_H_INCLUDED

#include "global.h"

//Texture wrapper class
class TextureImage
{
private:
    //The actual hardware texture
    SDL_Texture* The_Texture;

    //Image dimensions
    int image_width;
    int image_height;
public:
    //Initializes variables
    TextureImage();

    //Deallocates memory
    ~TextureImage();

    //Loads image at specified path
    bool loadFromFile( std::string path );
    bool loadFromFile_Greyscale( std::string path );

    //Creates image from font string
    bool loadFromRenderedText( std::string textureText, SDL_Color textColor, TTF_Font* font );
    bool loadFromRenderedTextWrapped( std::string textureText, SDL_Color textColor, TTF_Font* font, int width );

    //Does a rect
    void FillRect(int w, int h, SDL_Color color);

    //Deallocates texture
    void free();

    //Set alpha modulation
    void setAlpha( Uint8 alpha );

    //Renders texture at given point
    void render( int x, int y, SDL_Rect *clip = NULL, bool Stretch = false );

    //Gets image dimensions
    int getWidth(){return image_width;}
    int getHeight(){return image_height;}

    SDL_Texture* getTexture(){return The_Texture;}
};


#endif // TEXTURE_WRAPPER_H_INCLUDED
