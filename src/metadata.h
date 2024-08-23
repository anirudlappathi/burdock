#pragma once
#include <vector>
#include <map>
#include <string>

class Metadata {
  private:
    std::map<std::string, std::string> _metadata;
    size_t _length;
  public:
    Metadata();
    Metadata(std::map<std::string, std::string> all_metadata); 

    std::string at(const std::string key) const; 
    std::vector<std::pair<std::string, std::string> > data() const;
    size_t size() const {
      return _length;
    }
};
