#include "vectordb.h"
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <cassert>

// constexpr u_int UNSIGNED_MAX = 4294967295;
// constexpr size_t MAX_LOADED_EMBEDDINGS = 1000;
constexpr bool DEBUG = false;
constexpr bool DEBUG2 = true;
const std::string TEST_FOLDER_PATH = "/Users/anirud/Downloads/CSProjects/burdock/build/testing";

bool test_file_write_and_read(int value, unsigned int count)
{
    VectorDatabase vdb(TEST_FOLDER_PATH, value);
    vdb.__delete_folder(false);
    for (unsigned i = 0; i < count; i++)
    {
        Embedding *e = new Embedding(value);
        if (DEBUG)
        {
            std::cout << *e << std::endl;
        }
        vdb.insert(*e);
    }

    if (DEBUG2)
    {
        std::cout << vdb.tree() << std::endl;
    }
    vdb.WriteToFile();
    VectorDatabase newVDB(TEST_FOLDER_PATH);
    std::cout << newVDB.tree() << std::endl;

    if (vdb.tree() != newVDB.tree())
    {
        if (DEBUG)
        {
            std::cout << "FAILED: " << value << ", " << count << std::endl;
        }
        return false;
    }
    newVDB.__delete_folder(false);
    return true;
}

bool test_query(int value, int count)
{
    VectorDatabase vdb(TEST_FOLDER_PATH, value);
    vdb.__delete_folder(DEBUG);
    for (int i = 0; i < count; i++)
    {
        Embedding e(value);
        if (DEBUG)
        {
            std::cout << e << std::endl;
        }
        vdb.insert(e);
    }
    vdb.WriteToFile();

    VectorDatabase vdb2(TEST_FOLDER_PATH);
    if (vdb.dimensions() != vdb2.dimensions())
    {
        std::cerr << vdb.dimensions() << " != " << vdb2.dimensions() << std::endl;
    }
    std::vector<float> a;
    for (int i = 0; i < value; i++)
    {
        a.push_back(1);
    }
    Embedding e(a);
    vdb2.query(e);
    vdb2.__delete_folder(false);
    return true;
}

int main()
{
    std::cout << test_query(1, 9102) << std::endl;
}