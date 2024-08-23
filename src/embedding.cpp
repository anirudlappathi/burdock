#include "embedding.h"
#include <cassert>
#include <stdexcept>
#include <random>

Embedding::Embedding(u_int dim): 
  _magnitude(0),
  dimensions(dim) {
  // PURELY TESTING
  _embedding = new float[dim];
  for (u_int i = 0; i < dimensions; i++) {
    _embedding[i] = (float)rand()/RAND_MAX;
    _magnitude += pow(_embedding[i], 2);
  }
  _magnitude = sqrt(_magnitude);
  _metadata = "Testing Metadata";
}

Embedding::Embedding(Embedding& other): 
  _magnitude(0) {
  if (this == &other) {
    return;
  }
  dimensions = other.dimensions;
  _metadata = other._metadata;
  _embedding = new float[dimensions];
  for (u_int i = 0; i < dimensions; i++) {
    _embedding[i] = other._embedding[i];
    _magnitude += pow(_embedding[i], 2);
  }
  _magnitude = sqrt(_magnitude);
}

Embedding::Embedding(std::vector<float>& insert_embedding):
  _magnitude(0),
  dimensions(insert_embedding.size()) {
    _embedding = new float[dimensions];
    for (u_int i = 0; i < dimensions; i++) {
      _embedding[i] = insert_embedding.at(i);
      _magnitude += pow(_embedding[i], 2);
    }
    _magnitude = sqrt(_magnitude);
}

Embedding::Embedding(std::vector<float>& insert_embedding, std::string metadata):
  _metadata(metadata),
  _magnitude(0),
  dimensions(insert_embedding.size()) {

    _embedding = new float[dimensions];
    for (u_int i = 0; i < dimensions; i++) {
      _embedding[i] = insert_embedding.at(i);
      _magnitude += pow(_embedding[i], 2);
    }
    _magnitude = sqrt(_magnitude);
}

Embedding::Embedding(float* insert_embedding, u_int dim, std::string metadata):
  _embedding(insert_embedding),
  _metadata(metadata),
  _magnitude(0),
  dimensions(dim) {}

Embedding::~Embedding() {
  if (_embedding != nullptr) {
    delete[] _embedding;
    _embedding = nullptr;
  }
}

float* Embedding::get_embedding() {
  return _embedding;
}

std::string Embedding::get_metadata() {
  return _metadata;
}

size_t Embedding::metadata_length() const {
  return _metadata.size();
}

void Embedding::set(u_int index, float f) {
  if (index < 0 || index >= dimensions) {
    std::cerr << "ERROR [embedding.cpp][set(u_int, float)]: Index out of bounds: (" + std::to_string(index) + " != 0-" + std::to_string(dimensions) + ")" << std::endl;
    throw std::runtime_error("");
  }
  _embedding[index] = f;
}

void Embedding::set(int index, float f) {
  if (index < 0 || index >= (int)dimensions) {
    std::cerr << "ERROR [embedding.cpp][set(int, float)]: Index out of bounds: (" + std::to_string(index) + " != 0-" + std::to_string(dimensions) + ")" << std::endl;
    throw std::runtime_error("");
  }
  _embedding[index] = f;
}

float Embedding::at(u_int index) const {
  if (index < 0 || index >= dimensions) {
    std::cerr << "ERROR [embedding.cpp][at(u_int)]: Index out of bounds: (" + std::to_string(index) + " != 0-" + std::to_string(dimensions) + ")" << std::endl;
    throw std::runtime_error("");
  }
  return _embedding[index];
}

float Embedding::at(int index) const {
  if (index < 0 || index >= (int)dimensions) {
    std::cerr << "ERROR [embedding.cpp][at(int)]: Index out of bounds: (" + std::to_string(index) + " != 0-" + std::to_string(dimensions) + ")" << std::endl;
    throw std::runtime_error("");
  }
  return _embedding[index];
}

float Embedding::operator[](int index) const {
  if (index < 0 || index >= (int)dimensions) {
    std::cerr << "ERROR [embedding.cpp][operator[](int)]: Index out of bounds: (" + std::to_string(index) + " != 0-" + std::to_string(dimensions) + ")" << std::endl;
    throw std::runtime_error("");
  }
  return _embedding[index];
}

float Embedding::__dot_product(const Embedding* other) const {
  assert(other->dimensions == dimensions);
  float dotProduct = 0;
  for (u_int i = 0; i < dimensions; i++) {
    dotProduct += _embedding[i] * other->_embedding[i];
  }
  return dotProduct;
}

void Embedding::__load_magnitude() {
  if (_magnitude != -3) {
    return;
  }
  _magnitude = 0;
  for (u_int i = 0; i < dimensions; i++) {
    _magnitude += pow(_embedding[i], 2);
  }
  _magnitude = sqrt(_magnitude);
}

float Embedding::cosine_similarity(Embedding* other) {
  if (_magnitude && other->_magnitude) {
    return __dot_product(other) / (_magnitude * other->_magnitude);
  }
  if ((!_magnitude) ^ (!other->_magnitude)) {
    return 1 / std::max(_magnitude, other->_magnitude); 
  }
  return 1;
}



std::ostream& operator<<(std::ostream& os, const Embedding& obj) {
  if (obj._embedding == nullptr) {
    os << "nullptr";
    return os;
  }
    os << "[";
    for (u_int i = 0; i < obj.dimensions; ++i) {
        os << obj._embedding[i];
        if (i == 3) {
          i = obj.dimensions - 3;
          continue;
        }
        if (i < obj.dimensions - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}