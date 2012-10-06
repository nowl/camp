#include "images.hpp"

void
ImageNameCache::cache(std::string name, std::string image_name)
{
    _cache[name] = image_name;
}

std::string
ImageNameCache::get(std::string name)
{
    return _cache[name];
}
