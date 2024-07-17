#include "metadata.h"
#include <stdexcept>

Metadata::Metadata(std::map<std::string, std::string> all_metadata): _metadata(all_metadata) {}

std::string Metadata::at(const std::string key) const {
  if (_metadata.find(key) == _metadata.end()) {
    throw std::invalid_argument("Index not in range of metadata");
  }
  return _metadata.at(key);
} 

std::vector<std::pair<std::string, std::string>> Metadata::data() const {
  std::vector<std::pair<std::string, std::string>> all_metastrings;
  for (const auto& [key, value] : _metadata) {
    std::pair kv_pair(key, value);
    all_metastrings.push_back(kv_pair);
  }
  return all_metastrings;
}
