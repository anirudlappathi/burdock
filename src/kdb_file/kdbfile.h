#pragma once
#include "../kdbtree.h"
#include "../vectordb.h"
#include "../embedding.h"
#include <fstream>

class KDBFile {
    private:
        
        std::string _folder_name = "./testing";
        bool _increase_efficiency = true;
        bool _normalized = true;
        int _exp_of_n_vectors = 12;


        int __size_of_float(float f);
        int __get_node_id_from_filepath(std::filesystem::path file_path);
        void __write_nodes_left_to_right(KDBNode* curr);

        KDBNode* __read_files_left_to_right(std::vector<std::filesystem::path>& files, int& fileIndex, int& nodeCount, int layer, size_t& vectorCount );

        void __delete_folder(std::string folder_name);

    public:
        void __delete_folder();

        KDBFile() = default;
        KDBFile(std::string folder_name);
        KDBFile(std::string folder_name, bool increase_efficiency, bool normalized);

        bool WriteToFile(VectorDatabase& vdb);
        bool WriteToFile(KDBTree* kdbtree);

        VectorDatabase* ReadFromFolder();
        VectorDatabase* ReadFromFolder(std::string folderpath);
};