#pragma once
#include <array>
#include <vector>
#include <queue>
#include "embedding.h"
#include "kdbnode.h"

// Implementation of a KD-Tree and a B+ Tree combined
class KDBTree {
  private:
    KDBNode* _root;
    size_t _vector_count;
    size_t _layers_count;
    unsigned _dim;

    ~KDBTree();

    KDBNode* GenerateUntilNLayers(KDBNode* curr, unsigned layers);
    bool __inorder_traversal_equality(KDBNode* a, KDBNode* b);
  public:

    size_t node_count;

    KDBTree(unsigned Dim);
    KDBTree(std::vector<Embedding> vector_list);
    KDBTree(unsigned Dim, unsigned starting_layers_count);
    KDBTree(KDBNode* root, size_t  vectorCount, int layers, int nodeCount);

    bool operator==(KDBTree& other);

    bool InsertEmbedding(Embedding& embedding);
    
    size_t GetLayerCount() const;
    size_t GetVectorCount() const;
    KDBNode* GetRoot();


    friend std::ostream& operator<<(std::ostream& os, KDBTree& obj);
};
