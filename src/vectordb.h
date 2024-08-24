#pragma once
#include "kdbtree.h"
#include <string>

class VectorDatabase
{
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
    int __get_node_id_from_filepath(std::__fs::filesystem::path file_path) const;
    void __write_nodes_left_to_right(KDBNode *curr);
    KDBNode *__read_files_left_to_right(std::vector<std::__fs::filesystem::path> &files, int &fileIndex, int &nodeCount, int &dimensions, int layer, size_t &vectorCount);
    void __delete_folder(const std::string &folder_name, bool coutWarning = true) const;
    VectorDatabase &__load_all(const std::string &folderpath);
    std::vector<std::__fs::filesystem::path> __get_folder_files(const std::string &folderpath) const;
    union
    {
        float input;
        u_char bytes[4];
    } floatBytes;

public:
    // private
    void __delete_folder(bool coutWarning = true) const;

    // read from file
    VectorDatabase() = delete;

    explicit VectorDatabase(const std::string &folderPath);
    VectorDatabase(const std::string &folderPath, size_t dim);
    VectorDatabase(const std::string &database_name, const std::string &folder_path, size_t file_count, KDBTree &&kdbtree);

    VectorDatabase(VectorDatabase &&other) noexcept;
    VectorDatabase &operator=(VectorDatabase &&other) noexcept;

    bool insert(Embedding &insertEmbedding);
    bool insert(Embedding &&insertEmbedding);
    std::pair<Embedding *, float> query(Embedding &queryEmbedding);

    u_int dimensions() const;
    size_t layers() const;
    size_t vectors() const;
    KDBTree &tree();

    bool WriteToFile();
};