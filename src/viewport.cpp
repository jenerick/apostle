#include <sstream>
#include <fstream>
#include <iostream>

#include "viewport.hpp"
#include "tools.hpp"

Viewport::Viewport( int width, int height, int tileSize )
{
    isAnimating = false;

    this->width = width;
    this->height = height;

    isClipSet = false;

    curClipX = this->width;
    curClipY = this->height;

    lasClipX = curClipX;
    lasClipY = curClipY;

    this->tileSize = tileSize;

    viewSurf = SDL_CreateRGBSurface(
        0,
        (this->width+2)*tileSize, (this->height+2)*tileSize,
        24,
        // color mask //
        0x00FF0000, // R
        0x0000FF00, // G
        0x000000FF, // B
        0xFF000000  // A
    );

    // set up the palette
    std::ifstream is( "palettefile.txt" );
    std::string line;

    while (std::getline(is, line)) {
        std::istringstream iss(line);
        line = iss.str();
        std::string key = "";
        std::string s_value = "";

        int i = 0;
        for (i = i; i < line.length(); i++) {
            if (line.at(i) == ':') {
                break;
            } else {
                key += line.at(i);
            }
        }

        bool html_begun = false;
        for (i = i; i < line.length(); i++) {
            if (html_begun) {
                if (line.at(i) != ' ') {
                    s_value += line.at(i);
                }
            } else if (line.at(i) == '#') {
                html_begun = true;
                s_value += line.at(i);
            }
        }

        palette[key] = Tools::Math::hex_stage( s_value );
    }

}

Viewport::~Viewport()
{
    SDL_FreeSurface( viewSurf );
}

SDL_Surface *Viewport::drawSurface( Actor *actor, World *world )
{
    entity_position x = 0;
    entity_position y = 0;

    actor->getPosition( &x, &y );

    // the exterior clip anchor.
    // where (0,0) is the top-left corner of the world space.
    curClipX = x-((width-1)/2);
    curClipY = y-((height-1)/2);

    world->fixClip( &curClipX, &curClipY, width, height );

    lasClipX = curClipX;
    lasClipY = curClipY;

    SDL_Rect rect;
    rect.w = tileSize;
    rect.h = tileSize;

    Uint32 color = 0xFF000000;

    int i = 0;
    int j = 0;

    do {
        rect.y = (j*rect.h);

        do {
            rect.x = (i*rect.w);

            switch( world->getTileType(curClipX+i, curClipY+j) ) {
                case TILE_ACTOR:
                color = palette["actor"];
                break;

                case TILE_TREE:
                color = palette["tree"];
                break;

                case TILE_DIRT:
                color = palette["dirt"];
                break;

                case TILE_WATER:
                color = palette["water"];
                break;

                case TILE_BUSH:
                color = palette["bush"];
                break;

                case TILE_BLANK:
                color = palette["blank"];
                break;
            }

            SDL_FillRect( viewSurf, &rect, color );
            i++;
        } while ( i < width );

        i = 0;
        j++;
    } while ( j < height );

    return viewSurf;
}

void Viewport::clearSurface()
{
    SDL_FillRect( viewSurf, NULL, 0x00000000 );
}
