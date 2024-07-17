#include "vectordb.h"
#include <stdexcept>

VectorDatabase::VectorDatabase(std::string database_name, size_t dim):
                        _database_name(database_name),
                        _file_count(0),
                        _dim(dim) {
                            _id = "1";
                            _folder_path = "dbfiles";
                            _file_count = 0;
                            _kdbtree = new KDBTree(dim);
                        }

VectorDatabase::VectorDatabase(std::string database_name, std::string folder_path, size_t file_count, size_t dim, KDBTree* kdbtree):
    _database_name(database_name),
    _id("1"),
    _folder_path(folder_path),
    _file_count(file_count),
    _dim(dim),
    _kdbtree(kdbtree) {}

bool VectorDatabase::Upsert(Embedding& insert_embedding) {
    // SIMULATING USING A EMBEDDING MODEL TO GET AN EMBEDDING OF K DIMENSION
    if (insert_embedding.dimensions != _dim) {
        throw std::invalid_argument("Dimension of embedding doesn't match Vector Database embedding dimension");
    }
    bool insert_success = _kdbtree->InsertEmbedding(insert_embedding);
    return insert_success;
}

size_t VectorDatabase::GetVectorCount() {
    return _kdbtree->GetVectorCount();
}
KDBTree* VectorDatabase::GetKDBTree() {
    return _kdbtree;
}