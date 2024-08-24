#include "kdbnode.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

constexpr bool DEBUG = true;
constexpr size_t S_IDSIZE = 8;

KDBNode::KDBNode(const std::string &folderlocation, u_int dim, size_t nodeId) : _file_location("dbfiles" + __id_to_n_len_id(S_IDSIZE, nodeId) + ".brdk"),
                                                                                _folder_location(folderlocation),
                                                                                _node_id(nodeId),
                                                                                _size(0),
                                                                                _dim(dim)
{
    if (dim > 65535)
    {
        std::cerr << "ERROR [kdbnode.cpp][KDBNode(folder, dim, nodeid)]: Dimension cannot be less than or equal to 0 or greater than 65536" << std::endl;
        throw std::runtime_error("");
    }
}

KDBNode::KDBNode(const std::string &folderlocation, bool is_written, size_t nodeId, u_int dim, u_char data_type_byte_count) : _file_location("dbfiles" + __id_to_n_len_id(S_IDSIZE, nodeId) + ".brdk"),
                                                                                                                              _folder_location(folderlocation),
                                                                                                                              _loaded(is_written),
                                                                                                                              _node_id(nodeId),
                                                                                                                              _dim(dim),
                                                                                                                              _data_type_byte_count(data_type_byte_count)
{
    if (dim > 65535)
    {
        std::cerr << "ERROR [kdbnode.cpp][KDBNode(string, bool, size_t, uint, uchar)]: Dimension cannot be less than or equal to 0 or greater than 65536" << std::endl;
        throw std::runtime_error("");
    }
}

KDBNode::KDBNode(KDBNode &other) noexcept
{
    _file_location = other._file_location;
    _folder_location = other._folder_location;
    _loaded = other._loaded;
    _size = other._size;
    _dim = other._dim;
    _data_type_byte_count = other._data_type_byte_count;

    left = other.left;
    right = other.right;
    for (Embedding *otherEmbedding : other.embeddings)
    {
        Embedding *newEmbedding = new Embedding(*otherEmbedding);
        embeddings.push_back(newEmbedding);
    }
}

KDBNode::KDBNode(KDBNode &&other) noexcept : _file_location(other._file_location),
                                             _folder_location(other._folder_location),
                                             _loaded(other._loaded),
                                             _node_id(other._node_id),
                                             _size(other._size),
                                             _dim(other._dim),
                                             _data_type_byte_count(other._data_type_byte_count),
                                             left(other.left),
                                             right(other.right),
                                             embeddings(other.embeddings)
{
    other._file_location = "";
    other._folder_location = "";
    other._loaded = false;
    other._node_id = 0;
    other._size = 0;
    other._dim = 0;
    other._data_type_byte_count = 0;
    other.left = nullptr;
    other.right = nullptr;
    other.embeddings = std::vector<Embedding *>();
}

KDBNode::~KDBNode()
{
    if (left != nullptr || right != nullptr)
    {
        std::cout << "WARNING [kdbnode.cpp][~KDBNode]: Deleting non-leaf node" << std::endl;
    }
    left = nullptr;
    right = nullptr;
    for (Embedding *e : embeddings)
    {
        delete e;
    }
}

std::string KDBNode::__id_to_n_len_id(int nIdSize, size_t nodeId) const
{
    std::string stringNodeId = std::to_string(nodeId);
    int stringNodeIdLength = int(stringNodeId.size());
    if (stringNodeIdLength > nIdSize)
    {
        std::cerr << "stringNodeId size is greater than given id size";
        throw std::runtime_error("");
    }
    for (int i = 0; i < nIdSize - stringNodeIdLength; i++)
    {
        stringNodeId = "0" + stringNodeId;
    }
    return stringNodeId;
}

std::pair<std::string, bool> KDBNode::filelocation()
{
    return std::pair<std::string, bool>(_file_location, _loaded);
}

u_int KDBNode::dimensions() const
{
    return _dim;
}

bool KDBNode::loaded() const
{
    return _loaded;
}

void KDBNode::clear()
{
    if (!isleaf() && DEBUG)
    {
        std::cerr << "WARNING [kdbnode.cpp][clear]: Clearing a non-leaf node is not ever necessary" << std::endl;
        throw std::runtime_error("");
    }
    _file_location = "";
    _loaded = false;
    _size = 0;
    assert(left == nullptr);
    assert(right == nullptr);
    embeddings = std::vector<Embedding *>();
}

bool KDBNode::isleaf() const
{
    assert(!((left == nullptr) ^ (right == nullptr)));
    if (left && right)
    {
        return false;
    }
    return true;
}

size_t KDBNode::__get_node_id() const
{
    return _node_id;
}

u_char KDBNode::datatypebytecount() const
{
    return _data_type_byte_count;
}

void KDBNode::filelocation(const std::string &filelocation)
{
    _file_location = filelocation;
}

void KDBNode::set_size(int size)
{
    _size = size;
}

size_t KDBNode::size() const
{
    // const _size = embeddings.size();
    return embeddings.size();
}

void KDBNode::set_loaded(bool is_written)
{
    _loaded = is_written;
}

void KDBNode::set_data_type_byte_count(u_char data_type_byte_count)
{
    _data_type_byte_count = data_type_byte_count;
}

bool KDBNode::insertEmbedding(Embedding &embedding)
{
    if (!_loaded)
    {
        loadEmbeddings(_file_location);
    }
    assert(embedding.dimensions == _dim);
    Embedding *heap_embedding = new Embedding(embedding);
    embeddings.push_back(heap_embedding);
    _size++;
    return true;
}

bool KDBNode::loadEmbeddings(std::string filename)
{
    // load embeddings from file into memory
    if (filename == "")
    {
        return false;
    }
    return true;
}

void KDBNode::write_on_disk()
{
    assert(isleaf());
    assert(_file_location != "");
    assert(_folder_location != "");

    std::string folderpath = _folder_location;
    std::string filepath;

    if (_folder_location.at(_folder_location.size() - 1) != '/')
    {
        filepath = _folder_location + "/" + _file_location;
    }
    else
    {
        filepath = _folder_location + _file_location;
    }

    if (!std::__fs::filesystem::exists(folderpath))
    {
        std::__fs::filesystem::create_directory(folderpath);
    }

    std::ifstream check_file_exists(filepath);
    set_loaded(true);
    if (!check_file_exists)
    {
        std::ofstream creating_file(filepath);
        creating_file.close();
    }
    else
    {
        check_file_exists.close();
    }
    std::ofstream write_file(filepath, std::ios::binary | std::ios::app);

    // 1 byte data type byte size
    u_char dt_byte_count = datatypebytecount();
    write_file.write(reinterpret_cast<const char *>(&dt_byte_count), sizeof(u_char));

    // 2 bytes writing the dimension size of the embeddings
    assert(dimensions() == dimensions());
    u_int embeddingsSize = dimensions();
    write_file.write(reinterpret_cast<const char *>(&embeddingsSize), sizeof(u_char) * 2);

    // 4 bytes number of embeddings
    int numEmbeddings = size();
    write_file.write(reinterpret_cast<const char *>(&numEmbeddings), sizeof(u_char) * 4);

    // write embeddings
    union
    {
        float input;
        u_char bytes[4];
    } floatBytes;

    for (const Embedding *emb : embeddings)
    {
        // embedding data len(4 * dim)
        for (u_int i = 0; i < emb->dimensions; i++)
        {
            floatBytes.input = emb->at(i);
            for (const u_char &byte : floatBytes.bytes)
            {
                write_file.write(reinterpret_cast<const char *>(&byte), sizeof(byte));
            }
        }

        // length of metadata len(2)
        int metadata_length = emb->metadata_length();
        assert(metadata_length < 65536);
        write_file.write(reinterpret_cast<const char *>(&metadata_length), sizeof(u_char) * 2);

        // metadata (given in len)
        std::string metadata = emb->get_metadata();
        write_file.write(metadata.data(), metadata.size());
    }
    write_file.close();
}

void KDBNode::delete_on_disk()
{
    assert(_file_location != "");
    assert(_folder_location != "");

    std::string folderpath = _folder_location;
    std::string filepath;

    if (_folder_location.at(_folder_location.size() - 1) != '/')
    {
        filepath = _folder_location + "/" + _file_location;
    }
    else
    {
        filepath = _folder_location + _file_location;
    }

    if (!std::__fs::filesystem::exists(folderpath))
    {
        std::cerr << "ERROR [kdbnode.cpp][delete_on_disk]: Deleting file from folderpath that does not exist on disk (" + folderpath + ")" << std::endl;
        throw std::runtime_error("");
    }
    int status = remove(filepath.c_str());
    if (status == 0)
    {
        std::cerr << "ERROR [kdbnode.cpp][delete_on_disk]: Error deleting filepath from disk (" + filepath + ")" << std::endl;
        throw std::runtime_error("");
    }
}

std::ostream &operator<<(std::ostream &os, const KDBNode &obj)
{
    if (obj.embeddings.empty())
    {
        os << "{\n\tNO_EMBEDDINGS\n}";
        return os;
    }
    os << "{\n";
    for (auto it = obj.embeddings.begin(); it != obj.embeddings.end(); it++)
    {
        os << "\t" << **it << "\n";
    }
    os << "}" << std::endl;
    return os;
}
