#include "kdbtree.h"
#include "embedding.h"
#include <cassert>
#include <iostream>
#include <math.h>
// uncomment to disable assert()
// #define NDEBUG


KDBTree::KDBTree(unsigned Dim):
    _layers_count(4),
    _dim(Dim),
    node_count(0) {
    _root = GenerateUntilNLayers(_root, _layers_count);
}

KDBTree::KDBTree(unsigned Dim, unsigned starting_layers):
    _layers_count(starting_layers),
    _dim(Dim),
    node_count(0) {
    _root = GenerateUntilNLayers(_root, starting_layers);
}

KDBTree::KDBTree(KDBNode* root, size_t vectorCount, int layers, int nodeCount):
    _root(root),
    _vector_count(vectorCount),
    _layers_count(layers),
    _dim(root->GetDimension()), 
    node_count(nodeCount) {}

bool KDBTree::__inorder_traversal_equality(KDBNode* a, KDBNode* b) {
    if (!a && !b) {
        return true;
    }
    if (a && b) {
        if (a->GetSize() != b->GetSize()) {
            std::cout << "Node embedding size not equal";
            return false;
        }
        for (unsigned i = 0; i < a->GetSize(); i++) {
            for (unsigned j = 0; j < a->GetDimension(); j++) {
                if (a->embeddings.at(i)->get(j) != b->embeddings.at(i)->get(j)) {
                    std::cout << "Node value not equal" << std::endl; 
                    return false;
                }
            }
        }
        return __inorder_traversal_equality(a->left, b->left) 
            && __inorder_traversal_equality(a->right, b->right);
    }
    std::cout << "Uneven tree" << std::endl;
    return false;
}

bool KDBTree::operator==(KDBTree& other) {
    KDBNode* thisRoot = this->GetRoot();
    KDBNode* otherRoot = other.GetRoot();
    return __inorder_traversal_equality(thisRoot, otherRoot);
}

size_t KDBTree::GetLayerCount() const {
    return _layers_count;
}
size_t KDBTree::GetVectorCount() const {
    return _vector_count;
}
KDBNode* KDBTree::GetRoot() {
    return _root;
}

KDBNode* KDBTree::GenerateUntilNLayers(KDBNode* curr, unsigned layers) {
    if (layers <= 0) {
        return nullptr;
    }
    if (curr != nullptr) {
        KDBNode* left = GenerateUntilNLayers(curr->left, layers - 1);
        KDBNode* right = GenerateUntilNLayers(curr->right, layers - 1);
        if (left != nullptr && right != nullptr) {
            curr->left = left;
            curr->right = right;
        }
        return nullptr;
    } else {
        curr = new KDBNode(_dim, node_count);
        node_count++;
        curr->left = GenerateUntilNLayers(curr->left, layers - 1);
        curr->right = GenerateUntilNLayers(curr->right, layers - 1);
        if (curr->left == nullptr && curr->right == nullptr) {
            curr->is_leaf = true;
        }
        return curr;
    }
}

bool KDBTree::InsertEmbedding(Embedding& embedding) {
    assert(_root);
    KDBNode* curr = _root;
    for (unsigned dim = 0; dim < _layers_count - 1; dim++) {
        unsigned currDim =  dim % embedding.dimensions;
        if (embedding[currDim] <= 0.5) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    assert(curr->is_leaf);
    bool insert_success = curr->InsertEmbedding(embedding);
    _vector_count++;
    assert(insert_success);
    return insert_success;
}

std::ostream& operator<<(std::ostream& os, KDBTree& obj) {
    const size_t layer_count = obj.GetLayerCount();
    KDBNode* root = obj.GetRoot();
    std::queue<KDBNode*> q;
    q.push(root);
    unsigned curr_square = 1;
    std::cout << "obj.node_count: " << obj.node_count << std::endl;
    for (unsigned i = 1; i <= obj.node_count; i++) {
        if (q.empty()) {
            assert(false);
            return os;
        }
        KDBNode* top = q.front();
        if (top->left) {
            q.push(top->left);
        }
        if (top->right) {
            q.push(top->right);
        }
        q.pop();
        os << top->GetSize();
        if ((pow(2, curr_square)) - 1 == i) {
            os << "\n";
            curr_square++;
        } else {
            os << ", ";
        }
    }
    return os;
}



