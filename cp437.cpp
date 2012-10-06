#include "log.hpp"

#include "game.hpp"

static const char *fileloc = "Codepage-437-solid.png";

char num_to_hex(unsigned int n)
{
    switch(n)
    {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    case 10: return 'a';
    case 11: return 'b';
    case 12: return 'c';
    case 13: return 'd';
    case 14: return 'e';
    case 15: return 'f';
    }

    return '0';
}

std::string int_to_hex(unsigned int n)
{
    std::string r;
    r += num_to_hex(n/16);
    r += num_to_hex(n%16);
    return r;
}

void load_cp437()
{
    auto im = Game::Instance()->engine.getImageLoader();

    for(int j=0; j<8; j++)
        for(int i=0; i<32; i++)
            im->load(int_to_hex(j*32+i), fileloc, 8+9*i, 8+16*j, 9, 16);
}
