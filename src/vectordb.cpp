#include "vectordb.h"
#include <stdexcept>
#include <fstream>

#define UNSIGNED_MAX 4294967295

union {
    float input;
    u_char bytes[4];
} floatBytes;

VectorDatabase::VectorDatabase(std::string folderPath):
    _database_name(folderPath),
    _kdbtree(folderPath) {
    std::vector<std::filesystem::path> files = __get_folder_files(folderPath);
    if (files.empty()) {
        std::cerr << "WARNING [vectordb.cpp][constructor(string)]: Folderpath does not contain any files";
        throw std::runtime_error("");
    }
    int layers = log2(files.size()) + 1;
    int dimensions = -1;
    int currFile = 0;
    int nodeCount = 0;
    size_t vectorCount = 0;
    _kdbtree.setRoot(__read_files_left_to_right(files, currFile, nodeCount, dimensions, layers, vectorCount));
    assert(dimensions != -1);

    _kdbtree.dimensions(dimensions);
    _kdbtree.layers(layers);
    _kdbtree.node_count = nodeCount;
    _id = folderPath;
    _folder_path = folderPath;
    _increase_efficiency = true;
    _normalized = true;
    _exp_of_n_vectors = 4;
    _file_count = files.size();
}

VectorDatabase::VectorDatabase(std::string folderPath, size_t dim):
                        _database_name(folderPath),
                        _id(folderPath),
                        _folder_path(folderPath),
                        _kdbtree(dim, folderPath, 1),
                        _increase_efficiency(true),
                        _normalized(true),
                        _exp_of_n_vectors(1),
                        _file_count(0) {}

VectorDatabase::VectorDatabase(std::string database_name, std::string folder_path, size_t file_count, size_t dim, KDBTree&& kdbtree):
    _database_name(database_name),
    _id("1"),
    _folder_path(folder_path),
    _kdbtree(std::move(kdbtree)),
    _file_count(file_count) {}

VectorDatabase::VectorDatabase(VectorDatabase&& other) noexcept :
    _database_name(other._database_name),
    _id(other._id),
    _folder_path(other._folder_path),
    _kdbtree(std::move(other._kdbtree)),
    _file_count(other._file_count) {
    other._database_name = "";
    other._id = "";
    other._folder_path = "";
    other._file_count = 0;
}

int VectorDatabase::__size_of_float(float f) {
    _increase_efficiency = true;
    _normalized = true;
    _exp_of_n_vectors = 12;
    int res = 0;
    while (f > 0) {
        f /= 10;
        res++;
    }
    return res;
}

void VectorDatabase::__delete_folder(bool coutWarning) {
    __delete_folder(_folder_path, coutWarning);
}

void VectorDatabase::__delete_folder(std::string folder_name, bool coutWarning) {
    if (coutWarning && (!std::filesystem::exists(_folder_path) || !std::filesystem::is_directory(_folder_path))) {
        std::cout << "WARNING [vectordb.cpp][__delete_folder]: Folder name \"" + _folder_path + "\" does not exist to delete" << std::endl;
    }
    std::filesystem::remove_all(folder_name);
}

int VectorDatabase::__get_node_id_from_filepath(std::filesystem::path file_path) {
    std::string f = file_path.filename().string();
    bool started_num = false;
    std::string num = "";
    for (u_int i = f.size() - 1; i != UNSIGNED_MAX; i--) {
        char curr = f.at(i);
        if (started_num) {
            if (isdigit(curr)) {
                num = curr + num;
            } else {
                break;
            }
        }
        if (curr == '.') {
            started_num = true;
        }
    }
    return std::stoi(num);
}

KDBNode* VectorDatabase::__read_files_left_to_right(std::vector<std::filesystem::path>& files, int& fileIndex, int& nodeCount, int& dimensions, int layer, size_t& vectorCount) {
    assert(layer > 0);
    if (layer <= 1) {
        // make kdbnode reading file
        std::filesystem::path filePath = files.at(fileIndex);
        fileIndex++;
        std::ifstream inputFile(filePath.string(), std::ios::binary);
        if (!inputFile.is_open()) {
            std::cerr << "ERROR [vectordb.cpp][__read_files_left_to_right]: Could not open input file: (" + filePath.string() + ")." << std::endl;
            throw std::runtime_error("");
        }
        if (inputFile.eof()) {
            std::cerr << "ERROR [vectordb.cpp][__read_files_left_to_right]: Empty file on open: (" + filePath.string() + ")." << std::endl;
            throw std::runtime_error("");
        }

        // read 1 byte embedding datatype
        u_char datatypeByteCountUC = 0;
        inputFile.read(reinterpret_cast<char*>(&datatypeByteCountUC), sizeof(u_char));
        int datatypeByteCount = int(datatypeByteCountUC);

        // read 2 bytes of embedding dimension
        short dimShort;
        inputFile.read(reinterpret_cast<char*>(&dimShort), sizeof(u_char) * 2);
        u_int dim = int(dimShort);
        dimensions = dim;

        // read 4 bytes of amount of embeddings
        int nodeSize = 0;
        inputFile.read(reinterpret_cast<char*>(&nodeSize), sizeof(u_char) * 4);

        size_t nodeId = __get_node_id_from_filepath(filePath);
        KDBNode* currNode = new KDBNode(_folder_path, true, nodeId, dim, nodeCount);
        nodeCount++;
        std::vector<Embedding*> embeddingsList;
        for (int i = 0; i < nodeSize; i++) {
            float* currEmbedding = new float[dim];
            for (u_int fnum = 0; fnum < dim; fnum++) {
                float currFloat;
                inputFile.read(reinterpret_cast<char*>(&currFloat), sizeof(currFloat));
                currEmbedding[fnum] = currFloat;
            }
            short metadataLength;
            inputFile.read(reinterpret_cast<char*>(&metadataLength), sizeof(metadataLength));
            char* metadataBuffer = new char[metadataLength];
            inputFile.read(metadataBuffer, metadataLength);
            std::string metadata(metadataBuffer, metadataLength);
            embeddingsList.push_back(new Embedding(currEmbedding, dim, metadata));
            vectorCount++;
        }
        currNode->left = nullptr;
        currNode->right = nullptr;
        currNode->embeddings = embeddingsList;
        currNode->set_size(nodeSize);
        currNode->filelocation(filePath.filename());
        return currNode;
    }
    KDBNode* curr = new KDBNode(_folder_path, 0, nodeCount);
    nodeCount++;
    curr->left = __read_files_left_to_right(files, fileIndex, nodeCount, dimensions, layer - 1, vectorCount);
    curr->right = __read_files_left_to_right(files, fileIndex, nodeCount, dimensions, layer - 1, vectorCount);
    return curr;
}

void VectorDatabase::__write_nodes_left_to_right(KDBNode* curr) {
    if (!curr) {
        return;
    }
    if (!curr->isleaf()) {
        __write_nodes_left_to_right(curr->left);
        __write_nodes_left_to_right(curr->right);
        return;
    }
    assert(curr->dimensions() == dimensions());
    std::pair<std::string, bool> node_filename = curr->filelocation();
    std::string folderpath;
    std::string filepath;
    if (_folder_path.at(0) == '/' || (_folder_path.at(0) == '~' && _folder_path.at(1) == '/')) {
        folderpath = _folder_path;
        filepath = _folder_path + "/" + node_filename.first;
    } else {
        folderpath = "./" + _folder_path;
        filepath = "./" + _folder_path + "/" + node_filename.first;
    }

    if (!std::filesystem::exists(folderpath)) {
        std::filesystem::create_directory(folderpath);
    }

    std::ifstream check_file_exists(filepath);    
    if (!check_file_exists) {
        std::ofstream creating_file(filepath);
        curr->set_loaded(true);
        creating_file.close();
    } else {
        curr->set_loaded(true);
        check_file_exists.close();
    }
    std::ofstream write_file(filepath, std::ios::binary | std::ios::app);

    // 1 byte - data type byte size
    u_char dt_byte_count = curr->datatypebytecount();
    write_file.write(reinterpret_cast<const char*>(&dt_byte_count), 1);

    // 2 bytes - writing the dimension size of the embeddings
    u_int dim = dimensions();
    write_file.write(reinterpret_cast<const char*>(&dim), 2);

    // 4 bytes - number of embeddings
    int csize = curr->size();
    write_file.write(reinterpret_cast<const char*>(&csize), 4);

    for (Embedding* emb : curr->embeddings) {
        // embedding data len(4 * dim)
        for (u_int i = 0; i < emb->dimensions; i++) {
            floatBytes.input = emb->at(i);
            for (u_char& byte : floatBytes.bytes) {
                write_file.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
            }
        }

        // length of metadata len(2)
        int metadata_length = emb->metadata_length();
        assert(metadata_length < 65536);
        write_file.write(reinterpret_cast<const char*>(&metadata_length), 2);

        // metadata (given in len)
        std::string metadata = emb->get_metadata();
        write_file.write(metadata.data(), metadata.size());
    }
    write_file.close();
}

bool VectorDatabase::insert(Embedding& insertEmbedding) {
    // SIMULATING USING A EMBEDDING MODEL TO GET AN EMBEDDING OF K DIMENSION
    if (insertEmbedding.dimensions != dimensions()) {
        std::cerr << "ERROR [vectordb.cpp][insert]: Query dimension of embedding does not match Vector Database embedding dimension" << std::endl;
        throw std::runtime_error("");
    }
    bool increased_size = _kdbtree.insertEmbedding(insertEmbedding);
    return increased_size;
}

bool VectorDatabase::insert(Embedding&& insertEmbedding) {
    // SIMULATING USING A EMBEDDING MODEL TO GET AN EMBEDDING OF K DIMENSION
    if (insertEmbedding.dimensions != dimensions()) {
        std::cerr << "ERROR [vectordb.cpp][insert]: Query dimension of embedding does not match Vector Database embedding dimension" << std::endl;
        throw std::runtime_error("");
    }
    bool insert_success = _kdbtree.insertEmbedding(insertEmbedding);
    return insert_success;
}

std::pair<Embedding*, float> VectorDatabase::query(Embedding& queryEmbedding) {
    if (queryEmbedding.dimensions != dimensions()) {
        std::cerr << "ERROR [vectordb.cpp][query]: Query dimension of embedding does not match Vector Database embedding dimension" << std::endl;
        throw std::runtime_error("");
    }
    return _kdbtree.query(_folder_path, queryEmbedding);
}

u_int VectorDatabase::dimensions() {
    return _kdbtree.dimensions();
}

size_t VectorDatabase::layers() {
    return _kdbtree.layers();
}

size_t VectorDatabase::vectors() {
    return _kdbtree.vectors();
}

KDBTree& VectorDatabase::tree() {
    return _kdbtree;
}

VectorDatabase& VectorDatabase::operator=(VectorDatabase&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    _database_name = other._database_name;
    _id = other._id;
    _folder_path = other._folder_path;
    _kdbtree = std::move(other._kdbtree);
    _file_count = other._file_count;
    return *this;
}

bool VectorDatabase::WriteToFile() {
    KDBNode* root = _kdbtree.root();
    __write_nodes_left_to_right(root);
    return true;
}

std::vector<std::filesystem::path> VectorDatabase::__get_folder_files(std::string folderpath) {
    std::vector<std::filesystem::path> files;
    if (std::filesystem::exists(folderpath) && std::filesystem::is_directory(folderpath)) {
        for (const auto& f : std::filesystem::directory_iterator(folderpath)) {
            if (f.is_regular_file()) {
                files.push_back(f.path());
            }
        }
    } else {
        std::cerr << "ERROR [vectordb.cpp][__get_folder_files]: Folder name \"" + folderpath + "\" does not exist";
        throw std::runtime_error("");
    }
    std::sort(files.begin(), files.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) {
        return a.string() < b.string();
    });
    return files;
}

VectorDatabase& VectorDatabase::__load_all(std::string folderpath) {
    std::vector<std::filesystem::path> files = __get_folder_files(folderpath);
    int layers = log2(files.size()) + 1;
    int dimensions = -1;
    int currFile = 0;
    int nodeCount = 0;
    size_t vectorCount = 0;
    KDBNode* rootNode = __read_files_left_to_right(files, currFile, nodeCount, dimensions, layers, vectorCount);
    assert(dimensions != -1);
    KDBTree* kdbtree = new KDBTree(rootNode, vectorCount, layers, folderpath, nodeCount);
    VectorDatabase* vdb = new VectorDatabase(folderpath, folderpath, files.size(), dimensions, std::move(*kdbtree));
    return *vdb;
}