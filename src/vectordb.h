#pragma once
#include "kdbtree.h"
#include <stdlib.h>

class VectorDatabase {
    private:
        std::string _database_name;
        std::string _id;
        std::string _folder_path;

        size_t _file_count;
        size_t _dim;

        KDBTree* _kdbtree;

    public:
        VectorDatabase(std::string database_name, size_t dim);
        VectorDatabase(std::string database_name, std::string folder_path, size_t file_count, size_t dim, KDBTree* kdbtree);
        
        bool Upsert(Embedding& insert_embedding);
        size_t GetVectorCount();
        KDBTree* GetKDBTree();
};