#include <catch2/catch_all.hpp>
#include "vectordb.cpp"

#define CATCH_CONFIG_MAIN
#define MAX_LOADED_EMBEDDINGS 1000
#define DEBUG false

#define TEST_FOLDER_PATH "/Users/anirud/Downloads/CSProjects/burdock/build/testing"

bool test_file_write_and_read(int value, unsigned int count) {
    INFO("Value of v: " << value);
    INFO("Value of c: " << count);
    VectorDatabase vdb(TEST_FOLDER_PATH, value);
    vdb.__delete_folder(false);
    for (unsigned i = 0; i < count; i++) {
        Embedding* e = new Embedding(value);
        if (DEBUG) { std::cout << *e << std::endl; }
        vdb.insert(*e);
    }

    vdb.WriteToFile();
    VectorDatabase newVDB(TEST_FOLDER_PATH);
    
    if (vdb.tree() != newVDB.tree()) {
        if (DEBUG) { std::cout << "FAILED: " << value << ", " << count << std::endl; }
        return false;
    }
    newVDB.__delete_folder(false);
    return true;
}

bool test_query(int value, int count) {
    INFO("Value of v: " << value);
    INFO("Value of c: " << count);
    VectorDatabase vdb(TEST_FOLDER_PATH, value);
    vdb.__delete_folder(false);
    for (int i = 0; i < count; i++) {
        Embedding e(value);
        if (DEBUG) { std::cout << e << std::endl; }
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






TEST_CASE("Save dim:5 count:3", "[save][vectordb][small]") {
    REQUIRE(test_file_write_and_read(5, 3));
}

TEST_CASE("Save Small", "[save][vectordb][small]") {
    for (int v = 1; v < 10; v++) {
        for (int c = 1; c < 10; c++) {
            REQUIRE(test_file_write_and_read(v, c));
        }
    }
}

TEST_CASE("Save Large", "[save][vectordb][large]") {
    std::vector<int> values = {1, 2, 10, 500, 1537, 13, 14, 15}; 
    std::vector<int> counts = {100, 1, 9102, 120, 50, 12, 13, 14};
    for (int v : values) {
        for (int c : counts) {
            INFO("Value of v: " << v);
            INFO("Value of c: " << c);
            REQUIRE(test_file_write_and_read(v, c));
        }
    }
}

// TEST_CASE("Save Odd Large", "[save][vectordb][odd]") {
//     std::vector<int> values = {-1, -10, -15, -20, -1230981}; 
//     std::vector<int> counts = {100, 1, 9102, 120, 50, 12, 13, 14};
//     for (int v : values) {
//         for (int c : counts) {
//             INFO("Value of v: " << v);
//             INFO("Value of c: " << c);
//             REQUIRE_THROWS(test_file_write_and_read(v, c));
//         }
//     }
// }

TEST_CASE("Query Embedding Size 1", "[query][vectordb][small][size1]") {
    int v = 1;
    for (int c = 1; c < 10; c++) {
        REQUIRE(test_query(v, c));
    }
}

TEST_CASE("Query Count Size 1", "[query][vectordb][small][size1]") {
    int c = 1;
    for (int v = 1; v < 20; v++) {
        REQUIRE(test_query(v, c));
    }
}

TEST_CASE("Query Small", "[query][vectordb][small]") {
    std::vector<int> values = {1};
    std::vector<int> counts = {100};
    for (int v : values) {
        for (int c : counts) {
            REQUIRE(test_query(v, c));
        }
    }
}

TEST_CASE("Query Large", "[query][vectordb][large]") {
    std::vector<int> values = {1, 2, 10, 500, 1537, 13, 14, 15};
    std::vector<int> counts = {100, 1, 9102, 120, 50, 12, 13, 14};
    for (int v : values) {
        for (int c : counts) {
            REQUIRE(test_query(v, c));
        }
    }
}

TEST_CASE("Expand Database Small", "[increase][vectordb][small]") {
    int dim = 3;
    VectorDatabase vdb(TEST_FOLDER_PATH, dim);
    bool increased = false;
    for (unsigned i = 0; i < MAX_LOADED_EMBEDDINGS; i++) {
        Embedding* e = new Embedding(dim);
        increased |= vdb.insert(*e);
    }
    REQUIRE(increased == true);
}