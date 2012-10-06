#ifndef __IMAGES_HPP__
#define __IMAGES_HPP__

#include <unordered_map>
#include <string>

class ImageNameCache {
public:
    void cache(std::string name, std::string image_name);
    std::string get(std::string name);
    
private:
    std::unordered_map<std::string, std::string> _cache;
};

#endif  // __IMAGES_HPP__
