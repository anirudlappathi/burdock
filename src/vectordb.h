#pragma once
#include "kdbtree.h"
#include <string>

class VectorDatabase {
    private:
        std::string _database_name;
        std::string _id;
        std::string _folder_path;

        KDBTree _kdbtree;

        bool _increase_efficiency = true;
        bool _normalized = true;
        int _exp_of_n_vectors;

        size_t _file_count;

        int __size_of_float(float f);
        int __get_node_id_from_filepath(std::filesystem::path file_path);
        void __write_nodes_left_to_right(KDBNode* curr);
        KDBNode* __read_files_left_to_right(std::vector<std::filesystem::path>& files, int& fileIndex, int& nodeCount, int& dimensions, int layer, size_t& vectorCount);
        void __delete_folder(std::string folder_name, bool coutWarning = true);
        VectorDatabase& __load_all(std::string folderpath);
        std::vector<std::filesystem::path> __get_folder_files(std::string folderpath);

    public:

        // private  
        void __delete_folder(bool coutWarning = true);

        // read from file
        VectorDatabase() = delete;
    
        VectorDatabase(std::string folderPath);
        VectorDatabase(std::string folderPath, size_t dim);
        VectorDatabase(std::string database_name, std::string folder_path, size_t file_count, size_t dim, KDBTree&& kdbtree);

        VectorDatabase(VectorDatabase&& other) noexcept;
        VectorDatabase& operator=(VectorDatabase&& other) noexcept;
        
        bool insert(Embedding& insertEmbedding);
        bool insert(Embedding&& insertEmbedding);
        std::pair<Embedding*, float> query(Embedding& queryEmbedding);

        u_int dimensions();
        size_t layers();
        size_t vectors();
        KDBTree& tree();

        bool WriteToFile();
};