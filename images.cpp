#include "images.hpp"

void
ImageNameCache::cache(const std::string& name,
                      const std::string& image_name)
{
    _cache[name] = image_name;
}

const std::string&
ImageNameCache::get(const std::string& name) const
{
    return _cache.find(name)->second;
}
