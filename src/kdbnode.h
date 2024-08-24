#pragma once
#include "embedding.h"
#include <string>
#include <filesystem>
#include <vector>
class KDBNode
{
private:
    std::string _file_location;
    std::string _folder_location;
    bool _loaded = false;
    size_t _node_id;
    size_t _size;
    u_int _dim;
    u_char _data_type_byte_count = static_cast<u_char>(4);

    std::string __id_to_n_len_id(int nIdSize, size_t nodeId) const;
    int __get_node_id_from_filepath(std::__fs::filesystem::path file_path) const;

    enum class datatype
    {
        FLOAT,
        DOUBLE
    };

public:
    KDBNode *left = nullptr;
    KDBNode *right = nullptr;
    std::vector<Embedding *> embeddings;

    KDBNode() = delete;
    KDBNode(const std::string &folderlocation, u_int dim, size_t node_id);
    KDBNode(const std::string &folderlocation, bool is_written, size_t node_id, u_int dim, u_char data_type_byte_count);
    KDBNode(KDBNode &other) noexcept;
    KDBNode(KDBNode &&other) noexcept;
    ~KDBNode();

    void filelocation(const std::string &filelocation);
    void set_loaded(bool is_written);
    void set_data_type_byte_count(u_char data_type_byte_count);
    void set_size(int size);

    std::pair<std::string, bool> filelocation();
    u_int dimensions() const;
    size_t size() const;
    bool loaded() const;
    bool isleaf() const;
    size_t __get_node_id() const;
    u_char datatypebytecount() const;

    void clear();
    void write_on_disk();
    void delete_on_disk();

    bool insertEmbedding(Embedding &embedding);
    bool loadEmbeddings(std::string filename);

    friend std::ostream &operator<<(std::ostream &os, const KDBNode &obj);
};