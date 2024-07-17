#include "vectordb.h"
#include "kdb_file/kdbfile.h"
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <cassert>

#define UNSIGNED_MAX 4294967295

bool test(std::vector<int> values, std::vector<unsigned int> counts) {
    assert(values.size() == counts.size());
    for (unsigned iv = 0; iv < values.size(); iv++) {
        for (unsigned iv2 = 0; iv2 < values.size(); iv2++) {
            int val = values[iv];
            unsigned int cnt = counts[iv2];
            VectorDatabase vdb("test", val);
            for (unsigned i = 0; i < cnt; i++) {
                Embedding* e = new Embedding(val);
                vdb.Upsert(*e);
            }


            KDBFile kdbf("/Users/anirud/Downloads/CSProjects/burdock/build/testing");
            kdbf.WriteToFile(vdb);
            VectorDatabase* newVDB = kdbf.ReadFromFolder("/Users/anirud/Downloads/CSProjects/burdock/build/testing");
            
            assert(*vdb.GetKDBTree() == *newVDB->GetKDBTree());
            kdbf.__delete_folder();
        }
    }
    return true;
}

int main() {
    std::cout << test({1, 2, 10, 500, 1537, 13, 14, 15}, {100, 1, 9102, 120, 50, 12, 13, 14}) << std::endl;
}