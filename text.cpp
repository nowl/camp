#include "sdl_driver.hpp"

#include "text.hpp"

Text::Text(std::string text, unsigned int fitWidth, int renderLevel)
    : renderLevel(renderLevel)
{
}

void Text::draw(float x, float y, float r, float g, float b)
{
    /*
    GLuint texture = _game->engine.getImageLoader()->get("41");
    _game->engine.getSDLDriver()->drawImage(texture,
                                            10,
                                            10,
                                            _game->cellWidth, _game->cellHeight,
                                            r, g, b);
    */
}
