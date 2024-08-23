#pragma once
#include "metadata.h"
#include <iostream>
#include <vector>
#include <random>

class Embedding {
private:
    float* _embedding;
    std::string _metadata;

    void __load_magnitude();
    float __dot_product(const Embedding* other) const; 

public:
    float _magnitude;
    u_int dimensions;

    Embedding() = delete;

    Embedding(u_int dim);
    Embedding(Embedding& other);
    Embedding(std::vector<float>& insert_embedding);
    Embedding(std::vector<float>& insert_embedding, std::string metadata);
    Embedding(float* insert_embedding, u_int dim, std::string metadata);
    ~Embedding();

    float* get_embedding();
    std::string get_metadata();
    size_t metadata_length() const;

    void set(u_int index, float f);
    void set(int index, float f);
    float at(u_int index) const;
    float at(int index) const;

    float cosine_similarity(Embedding* other);

    float operator[](int index) const;

    friend std::ostream& operator<<(std::ostream& os, const Embedding& obj);
};