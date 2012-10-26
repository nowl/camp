#ifndef __IMAGES_HPP__
#define __IMAGES_HPP__

#include <unordered_map>
#include <string>

class ImageNameCache {
public:
    void cache(const std::string& name, const std::string& image_name);
    const std::string& get(const std::string& name) const;
    
private:
    std::unordered_map<std::string, std::string> _cache;
};

#endif  // __IMAGES_HPP__
