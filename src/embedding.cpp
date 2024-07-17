#include "embedding.h"
#include <stdexcept>
#include <random>

Embedding::Embedding(unsigned dim): dimensions(dim) {
  // PURELY TESTING
  _embedding = new float[dim];
  for (unsigned i = 0; i < dimensions; i++) {
    _embedding[i] = (float)rand()/RAND_MAX;
  }
  _metadata = "Testing Metadata";
}

Embedding::Embedding(Embedding& other) {
  if (this == &other) {
    return;
  }
  dimensions = other.dimensions;
  _metadata = other._metadata;
  _embedding = new float[dimensions];
  for (unsigned i = 0; i < dimensions; i++) {
    _embedding[i] = other._embedding[i];
  }
}

Embedding::Embedding(std::vector<float>& insert_embedding):
  dimensions(insert_embedding.size()) {
    _embedding = new float[dimensions];
    for (unsigned i = 0; i < dimensions; i++) {
      _embedding[i] = insert_embedding.at(i);
    }
}

Embedding::Embedding(std::vector<float>& insert_embedding, std::string metadata):
  _metadata(metadata),
  dimensions(insert_embedding.size()) {

    _embedding = new float[dimensions];
    for (unsigned i = 0; i < dimensions; i++) {
      _embedding[i] = insert_embedding.at(i);
    }
}

Embedding::Embedding(float* insert_embedding, unsigned dim, std::string metadata):
  _embedding(insert_embedding),
  _metadata(metadata),
  dimensions(dim) {}

Embedding::~Embedding() {
  if (_embedding != nullptr) {
    delete[] _embedding;
    _embedding = nullptr;
  }
}

float* Embedding::GetEmbedding() {
  return _embedding;
}

std::string Embedding::GetMetadata() {
  return _metadata;
}

float* Embedding::GetEmbeddingData() const {
  return _embedding;
}

size_t Embedding::GetMetadataLength() const{
  return _metadata.size();
}

void Embedding::set(unsigned index, float f) {
  if (index >= dimensions) {
    throw std::invalid_argument("Index out of bounds");
  }
  _embedding[index] = f;
}

void Embedding::set(int index, float f) {
  if (index < 0 || index >= (int)dimensions) {
    throw std::invalid_argument("Index out of bounds");
  }
  _embedding[index] = f;
}

float Embedding::get(unsigned index) const {
  if (index < 0 || index >= dimensions) {
    throw std::invalid_argument("Index out of bounds");
  }
  return _embedding[index];
}

float Embedding::get(int index) const {
  if (index < 0 || index >= (int)dimensions) {
    throw std::invalid_argument("Index out of bounds");
  }
  return _embedding[index];
}


float Embedding::operator[](int index) const {
  if (index < 0 || index >= (int)dimensions) {
    throw std::invalid_argument("More layers than dimensions");
  }
  return _embedding[index];
}

std::ostream& operator<<(std::ostream& os, const Embedding& obj) {
  if (obj._embedding == nullptr) {
    os << "nullptr";
    return os;
  }
    os << "[";
    for (unsigned i = 0; i < obj.dimensions; ++i) {
        os << obj._embedding[i];
        if (i < obj.dimensions - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}