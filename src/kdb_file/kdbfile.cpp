#include "kdbfile.h"
#include "../kdbtree.h"
#include <filesystem>
#include <algorithm>
#include <cassert>

#define UNSIGNED_MAX 4294967295

KDBFile::KDBFile(std::string folder_name): 
                _folder_name(folder_name) {};

KDBFile::KDBFile(std::string folder_name,
                 bool increase_efficiency, 
                 bool normalized): 
                 _folder_name(folder_name), 
                 _increase_efficiency(increase_efficiency), 
                 _normalized(normalized) {}

int KDBFile::__size_of_float(float f) {
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

void KDBFile::__write_nodes_left_to_right(KDBNode* curr) {
    if (!curr) {
        return;
    }
    if (curr->is_leaf) {
        std::pair<std::string, bool> node_filename = curr->GetFileLocation();

        std::string folderpath;
        std::string filepath;
        if (_folder_name.at(0) == '/' || (_folder_name.at(0) == '~' && _folder_name.at(1) == '/')) {
            folderpath = _folder_name;
            filepath = _folder_name + "/" + node_filename.first;
        } else {
            folderpath = "./" + _folder_name;
            filepath = "./" + _folder_name + "/" + node_filename.first;
        }

        if (!std::filesystem::exists(folderpath)) {
            std::filesystem::create_directory(folderpath);
        }

        std::ifstream check_file_exists(filepath);    
        if (!check_file_exists) {
            std::ofstream creating_file(filepath);
            curr->SetIsWritten(true);
            creating_file.close();
        } else {
            curr->SetIsWritten(true);
            check_file_exists.close();
        }
        std::ofstream write_file(filepath, std::ios::binary | std::ios::app);

        // 1 byte data type byte size
        unsigned char dt_byte_count = curr->GetDTByte();
        write_file.write(reinterpret_cast<const char*>(&dt_byte_count), sizeof(dt_byte_count));

        // 2 bytes writing the dimension size of the embeddings
        unsigned embeddingsSize = curr->GetDimension();
        unsigned char embeddingsSizeByte1 = static_cast<unsigned char>(embeddingsSize & 0xFF);
        unsigned char embeddingsSizeByte2 = static_cast<unsigned char>((embeddingsSize >> 8) & 0xFF);
        write_file.write(reinterpret_cast<const char*>(&embeddingsSizeByte1), sizeof(embeddingsSizeByte1));
        write_file.write(reinterpret_cast<const char*>(&embeddingsSizeByte2), sizeof(embeddingsSizeByte2));

        // 3 bytes number of embeddings
        int numEmbeddings =  curr->GetSize();
        unsigned char numEmbeddingsByte1 = static_cast<unsigned char>(numEmbeddings & 0xFF);
        unsigned char numEmbeddingsByte2 = static_cast<unsigned char>((numEmbeddings >> 8) & 0xFF);
        unsigned char numEmbeddingsByte3 = static_cast<unsigned char>((numEmbeddings >> 16) & 0xFF);
        write_file.write(reinterpret_cast<const char*>(&numEmbeddingsByte1), sizeof(numEmbeddingsByte1));
        write_file.write(reinterpret_cast<const char*>(&numEmbeddingsByte2), sizeof(numEmbeddingsByte2));
        write_file.write(reinterpret_cast<const char*>(&numEmbeddingsByte3), sizeof(numEmbeddingsByte3));

        // write embeddings
        union {
            float input;
            unsigned char bytes[4];
        } floatBytes;

        for (Embedding* emb : curr->embeddings) {
            // embedding data len(4 * dim)
            for (unsigned i = 0; i < emb->dimensions; i++) {
                floatBytes.input = emb->get(i);
                for (unsigned char& byte : floatBytes.bytes) {
                    write_file.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
                }
            }

            // length of metadata len(2)
            int metadata_length = emb->GetMetadataLength();
            assert(metadata_length < 65536);
            unsigned char byte1 = static_cast<unsigned char>(metadata_length & 0xFF);
            unsigned char byte2 = static_cast<unsigned char>((metadata_length >> 8) & 0xFF);
            write_file.write(reinterpret_cast<const char*>(&byte1), sizeof(byte1));
            write_file.write(reinterpret_cast<const char*>(&byte2), sizeof(byte2));

            // metadata (given in len)
            std::string metadata = emb->GetMetadata();
            write_file.write(metadata.data(), metadata.size());
        }
        write_file.close();
    }
    __write_nodes_left_to_right(curr->left);
    __write_nodes_left_to_right(curr->right);
}

bool KDBFile::WriteToFile(VectorDatabase& vdb) {
    KDBNode* root = vdb.GetKDBTree()->GetRoot();
    __write_nodes_left_to_right(root);
    return true;
}

bool KDBFile::WriteToFile(KDBTree* kdbtree) {
    KDBNode* root = kdbtree->GetRoot();
    __write_nodes_left_to_right(root);
    return true;
}

VectorDatabase* KDBFile::ReadFromFolder() {
    return ReadFromFolder(_folder_name);
}

VectorDatabase* KDBFile::ReadFromFolder(std::string folderpath) {
    std::vector<std::filesystem::path> files;
    if (std::filesystem::exists(folderpath) && std::filesystem::is_directory(folderpath)) {
        for (const auto& f : std::filesystem::directory_iterator(folderpath)) {
            if (f.is_regular_file()) {
                files.push_back(f.path());
            }
        }
    } else {
        std::cerr << "Folder name \"" + _folder_name + "\" does not exist";
    }
    std::sort(files.begin(), files.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) {
        return a.string() < b.string();
    });
    int layers = log2(files.size()) + 1;
    int currFile = 0;
    int nodeCount = 0;
    size_t vectorCount = 0;
    KDBNode* rootNode = __read_files_left_to_right(files, currFile, nodeCount, layers, vectorCount);
    KDBTree* kdbtree = new KDBTree(rootNode, vectorCount, layers, nodeCount);
    size_t dim = kdbtree->GetRoot()->GetDimension();
    VectorDatabase* returnVDB = new VectorDatabase(folderpath, folderpath, files.size(), dim,kdbtree);
    return returnVDB;
}

int KDBFile::__get_node_id_from_filepath(std::filesystem::path file_path) {
    std::string f = file_path.filename().string();
    bool started_num = false;
    std::string num = "";
    for (unsigned i = f.size() - 1; i != UNSIGNED_MAX; i--) {
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

KDBNode* KDBFile::__read_files_left_to_right(std::vector<std::filesystem::path>& files, int& fileIndex, int& nodeCount, int layer, size_t& vectorCount) {
    assert(layer > 0);
    if (layer == 1) {
        // make kdbnode reading file
        std::filesystem::path filePath = files.at(fileIndex);
        fileIndex++;
        std::ifstream inputFile(filePath.string(), std::ios::binary);
        if (!inputFile.is_open()) {
            std::cerr << "Could not open input file: (" + filePath.string() + ")." << std::endl;
        }
        if (inputFile.eof()) {
            std::cerr << "Empty file on open: (" + filePath.string() + ")." << std::endl;
        }

        // read 1 byte embedding datatype
        unsigned char datatypeByteCountUC = 0;
        inputFile.read(reinterpret_cast<char*>(&datatypeByteCountUC), sizeof(datatypeByteCountUC));
        int datatypeByteCount = int(datatypeByteCountUC);

        // read 2 bytes of embedding dimension
        short dimShort;
        inputFile.read(reinterpret_cast<char*>(&dimShort), sizeof(dimShort));
        unsigned int dim = int(dimShort);

        // read 3 bytes of amount of embeddings
        int nodeSize = 0;
        inputFile.read(reinterpret_cast<char*>(&nodeSize), (sizeof(datatypeByteCountUC) * 3));

        size_t nodeId = __get_node_id_from_filepath(filePath);
        KDBNode* currNode = new KDBNode(filePath.root_path().string(), true, nodeId, dim, nodeCount);
        nodeCount++;
        std::vector<Embedding*> embeddingsList;
        for (int i = 0; i < nodeSize; i++) {
            float* currEmbedding = new float[dim];
            for (unsigned fnum = 0; fnum < dim; fnum++) {
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
        currNode->is_leaf = true;
        currNode->left = nullptr;
        currNode->right = nullptr;
        currNode->embeddings = embeddingsList;
        currNode->SetSize(nodeSize);
        return currNode;
    }
    KDBNode* curr = new KDBNode(0, nodeCount);
    nodeCount++;
    curr->left = __read_files_left_to_right(files, fileIndex, nodeCount, layer - 1, vectorCount);
    curr->right = __read_files_left_to_right(files, fileIndex, nodeCount, layer - 1, vectorCount);
    return curr;
}

void KDBFile::__delete_folder() {
    __delete_folder(_folder_name);
}
void KDBFile::__delete_folder(std::string folder_name) {
    if (!std::filesystem::exists(_folder_name) || !std::filesystem::is_directory(_folder_name)) {
        std::cerr << "Folder name \"" + _folder_name + "\" does not exist to delete" << std::endl;
    }
    std::filesystem::remove_all(folder_name);
}