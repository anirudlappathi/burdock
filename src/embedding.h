#pragma once
#include "metadata.h"
#include <iostream>
#include <vector>
#include <random>

class Embedding {
  private:
      float* _embedding;
      // Metadata _metadata;
      std::string _metadata;

  public:
      unsigned dimensions;

      Embedding(unsigned dim);
      Embedding(Embedding& other);
      Embedding(std::vector<float>& insert_embedding);
      Embedding(std::vector<float>& insert_embedding, std::string metadata);
      Embedding(float* insert_embedding, unsigned dim, std::string metadata);
      ~Embedding();

      float* GetEmbedding();
      std::string GetMetadata();
      float* GetEmbeddingData() const;
      size_t GetMetadataLength() const;

      void set(unsigned index, float f);
      void set(int index, float f);
      float get(unsigned index) const;
      float get(int index) const;

      float operator[](int index) const;

      friend std::ostream& operator<<(std::ostream& os, const Embedding& obj);
};
