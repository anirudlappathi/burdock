#include "vectordb.h"
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <cassert>

#define UNSIGNED_MAX 4294967295
#define MAX_LOADED_EMBEDDINGS 1000
#define DEBUG false
#define DEBUG2 true

#define TEST_FOLDER_PATH "/Users/anirud/Downloads/CSProjects/burdock/build/testing"


bool test_file_write_and_read(int value, unsigned int count) {
    VectorDatabase vdb(TEST_FOLDER_PATH, value);
    vdb.__delete_folder(false);
    for (unsigned i = 0; i < count; i++) {
        Embedding* e = new Embedding(value);
        if (DEBUG) { std::cout << *e << std::endl; }
        vdb.insert(*e);
    }

    if (DEBUG2) { std::cout << vdb.tree() << std::endl; }
    vdb.WriteToFile();
    VectorDatabase newVDB(TEST_FOLDER_PATH);
    std::cout << newVDB.tree() << std::endl;
    
    if (vdb.tree() != newVDB.tree()) {
        if (DEBUG) { std::cout << "FAILED: " << value << ", " << count << std::endl; }
        return false;
    }
    newVDB.__delete_folder(false);
    return true;
}

bool test_query(int value, int count) {
    VectorDatabase vdb(TEST_FOLDER_PATH, value);
    vdb.__delete_folder(DEBUG);
    for (int i = 0; i < count; i++) {
        Embedding e(value);
        if (DEBUG) {
            std::cout << e << std::endl;
        }
        vdb.insert(e);
    }
    vdb.WriteToFile();

    VectorDatabase vdb2(TEST_FOLDER_PATH);
    if (vdb.dimensions() != vdb2.dimensions()) {
        std::cerr << vdb.dimensions() << " != " << vdb2.dimensions() << std::endl;
    }
    std::vector<float> a;
    for (int i = 0; i < value; i++) {
        a.push_back(1);
    }
    Embedding e(a);
    auto b = vdb2.query(e);
    vdb2.__delete_folder(false);
    return true;
}

int main() {

    std::cout << test_query(-1, 9102) << std::endl;

    // int dim = 3;
    // VectorDatabase vdb(TEST_FOLDER_PATH, dim);
    // bool increased = false;
    // for (unsigned i = 0; i < MAX_LOADED_EMBEDDINGS; i++) {
    //     Embedding* e = new Embedding(dim);
    //     increased |= vdb.insert(*e);
    //     if (increased) {
    //         std::cout << i << ", " << *e << std::endl;
    //     }
    // }
    // std::cout << (increased == true) << std::endl;

    // VectorDatabase vdb("/Users/anirud/Downloads/CSProjects/burdock/build/testing", 5);
    // for (int i = 0; i < 16; i++) {
    //     Embedding e(5);
    //     vdb.insert(e);
    // }
    // vdb.WriteToFile();

    // VectorDatabase vdb2("/Users/anirud/Downloads/CSProjects/burdock/build/testing");
    // std::vector<float> a;
    // for (int i = 0; i < 5; i++) {
    //     a.push_back(1);
    // }
    // Embedding e(a);
    // auto b = vdb2.query(e);
    // std::cout << *b.first << ", " << b.second << std::endl;
}