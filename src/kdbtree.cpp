#include "kdbtree.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <math.h>

constexpr u_int UNSIGNED_MAX = 4294967295;
constexpr size_t MAX_LOADED_EMBEDDINGS = 1000;
constexpr bool DEBUG = false;
// uncomment to disable assert()
// #define NDEBUG

KDBTree::KDBTree(const std::string &folderpath) : _root(nullptr),
                                                  _vector_count(0),
                                                  _layers_count(1),
                                                  _dim(0),
                                                  _folderpath(folderpath),
                                                  _max_loaded_embeddings(MAX_LOADED_EMBEDDINGS),
                                                  node_count(0) {}

KDBTree::KDBTree(u_int dim, const std::string &folderpath) : _root(nullptr),
                                                             _vector_count(0),
                                                             _layers_count(1),
                                                             _dim(dim),
                                                             _folderpath(folderpath),
                                                             _max_loaded_embeddings(MAX_LOADED_EMBEDDINGS),
                                                             node_count(0) {}

KDBTree::KDBTree(u_int dim, const std::string &folderpath, u_int starting_layers) : _root(nullptr),
                                                                                    _vector_count(0),
                                                                                    _layers_count(starting_layers),
                                                                                    _dim(dim),
                                                                                    _folderpath(folderpath),
                                                                                    _max_loaded_embeddings(MAX_LOADED_EMBEDDINGS),
                                                                                    node_count(0)
{
    _root = __generate_until_n_layers(_root, starting_layers);
}

KDBTree::KDBTree(KDBNode *root, size_t vectorCount, int layers, const std::string &folderpath, int nodeCount) : _root(root),
                                                                                                                _vector_count(vectorCount),
                                                                                                                _layers_count(layers),
                                                                                                                _dim(root->dimensions()),
                                                                                                                _folderpath(folderpath),
                                                                                                                node_count(nodeCount) {}

void KDBTree::__inorder_traversal_delete(KDBNode *curr)
{
    if (curr->left != nullptr)
    {
        __inorder_traversal_delete(curr->left);
        curr->left = nullptr;
    }
    if (curr->right != nullptr)
    {
        __inorder_traversal_delete(curr->right);
        curr->right = nullptr;
    }
    delete curr;
}

int KDBTree::__get_node_id_from_filepath(std::__fs::filesystem::path file_path) const
{
    std::string f = file_path.filename().string();
    bool started_num = false;
    std::string num = "";
    for (u_int i = f.size() - 1; i != UNSIGNED_MAX; i--)
    {
        char curr = f.at(i);
        if (started_num)
        {
            if (isdigit(curr))
            {
                num = curr + num;
            }
            else
            {
                break;
            }
        }
        if (curr == '.')
        {
            started_num = true;
        }
    }
    return std::stoi(num);
}

std::vector<std::__fs::filesystem::path> KDBTree::__get_folder_files(const std::string &folderpath) const
{
    std::vector<std::__fs::filesystem::path> files;
    if (std::__fs::filesystem::exists(folderpath) && std::__fs::filesystem::is_directory(folderpath))
    {
        for (const auto &f : std::__fs::filesystem::directory_iterator(folderpath))
        {
            if (f.is_regular_file())
            {
                files.push_back(f.path());
            }
        }
    }
    else
    {
        std::cerr << "ERROR [kdbtree.cpp][__get_folder_files]: Folder name \"" + _folderpath + "\" does not exist";
        throw std::runtime_error("");
    }
    std::sort(files.begin(), files.end(), [](const std::__fs::filesystem::path &a, const std::__fs::filesystem::path &b)
              { return a.string() < b.string(); });
    return files;
}

KDBNode *KDBTree::__load_node(const std::string &folderpath, int fileIndex)
{
    // make kdbnode reading file
    std::vector<std::__fs::filesystem::path> files = __get_folder_files(folderpath);
    std::__fs::filesystem::path filePath = files.at(fileIndex);
    fileIndex++;
    std::ifstream inputFile(filePath.string(), std::ios::binary);
    if (!inputFile.is_open())
    {
        std::cerr << "ERROR [kdbtree.cpp][__load_node]: Could not open input file: (" + filePath.string() + ")" << std::endl;
        throw std::runtime_error("");
    }
    if (inputFile.eof())
    {
        std::cerr << "ERROR [kdbtree.cpp][__load_node]: Empty file on open: (" + filePath.string() + ")." << std::endl;
        throw std::runtime_error("");
    }

    // read 1 byte embedding datatype
    int datatypeByteCount = 0;
    inputFile.read(reinterpret_cast<char *>(&datatypeByteCount), sizeof(u_char));

    // read 2 bytes of embedding dimension
    u_int dim = 0;
    inputFile.read(reinterpret_cast<char *>(&dim), sizeof(u_char) * 2);

    // read 3 bytes of amount of embeddings
    int nodeSize = 0;
    inputFile.read(reinterpret_cast<char *>(&nodeSize), (sizeof(u_char) * 3));

    size_t nodeId = __get_node_id_from_filepath(filePath);
    KDBNode *currNode = new KDBNode(filePath.root_path().string(), true, nodeId, dim, datatypeByteCount);
    std::vector<Embedding *> embeddingsList;
    for (int i = 0; i < nodeSize; i++)
    {
        float *currEmbedding = new float[dim];
        for (u_int fnum = 0; fnum < dim; fnum++)
        {
            float currFloat;
            inputFile.read(reinterpret_cast<char *>(&currFloat), sizeof(currFloat));
            currEmbedding[fnum] = currFloat;
        }
        short metadataLength;
        inputFile.read(reinterpret_cast<char *>(&metadataLength), sizeof(metadataLength));
        char *metadataBuffer = new char[metadataLength];
        inputFile.read(metadataBuffer, metadataLength);
        std::string metadata(metadataBuffer, metadataLength);
        embeddingsList.push_back(new Embedding(currEmbedding, dim, metadata));
    }
    currNode->left = nullptr;
    currNode->right = nullptr;
    currNode->embeddings = embeddingsList;
    currNode->set_size(nodeSize);
    currNode->set_loaded(true);
    return currNode;
}

KDBTree::KDBTree(KDBTree &&other) noexcept
{
    _root = other._root;
    _vector_count = other._vector_count;
    _layers_count = other._layers_count;
    _dim = other._dim;
    _folderpath = other._folderpath;
    node_count = other.node_count;

    other._root = nullptr;
    other._vector_count = 0;
    other._layers_count = 0;
    other._dim = 0;
    other._folderpath = "";
    other.node_count = 0;
}

KDBTree::~KDBTree()
{
    __inorder_traversal_delete(_root);
}

bool KDBTree::__inorder_traversal_equality(KDBNode *a, KDBNode *b)
{
    if (!a && !b)
    {
        return true;
    }
    if (a && b)
    {
        if (a->size() != b->size())
        {
            std::cout << "FALSE [vectordb.cpp][__inorder_traversal_equality]: Node embedding count not equal, (" + std::to_string(a->size()) + ", " + std::to_string(b->size()) + ")." << std::endl;
            return false;
        }
        for (u_int i = 0; i < a->size(); i++)
        {
            for (u_int j = 0; j < a->dimensions(); j++)
            {
                if (a->embeddings.at(i)->at(j) != b->embeddings.at(i)->at(j))
                {
                    std::cout << "FALSE [vectordb.cpp][__inorder_traversal_equality]: Embedding value not equal, (" + std::to_string(a->embeddings.at(i)->at(j)) + ", " + std::to_string(b->embeddings.at(i)->at(j)) + ")." << std::endl;
                    return false;
                }
            }
        }
        bool left = __inorder_traversal_equality(a->left, b->left);
        bool right = __inorder_traversal_equality(a->right, b->right);
        return left && right;
    }
    std::cout << "FALSE [vectordb.cpp][__inorder_traversal_equality]: Tree does not contain nodes at same positions, (" + std::to_string(a != nullptr) + ", " + std::to_string(b != nullptr) + ")." << std::endl;
    return false;
}

bool KDBTree::operator==(KDBTree &other)
{
    KDBNode *thisRoot = this->root();
    KDBNode *otherRoot = other.root();
    return __inorder_traversal_equality(thisRoot, otherRoot);
}

KDBTree &KDBTree::operator=(KDBTree &&other) noexcept
{
    __inorder_traversal_delete(_root);
    _root = other._root;
    other._root = nullptr;
    _vector_count = other._vector_count;
    other._vector_count = 0;
    _layers_count = other._layers_count;
    other._layers_count = 0;
    _dim = other._dim;
    other._dim = 0;
    _folderpath = other._folderpath;
    other._folderpath = "";
    node_count = other.node_count;
    other.node_count = 0;
    return *this;
}

void KDBTree::setRoot(KDBNode *root)
{
    _root = root;
}

void KDBTree::dimensions(u_int dim)
{
    _dim = dim;
}

u_int KDBTree::dimensions() const
{
    return _dim;
}

void KDBTree::folderpath(const std::string &folderpath)
{
    _folderpath = folderpath;
}

void KDBTree::layers(size_t layers)
{
    _layers_count = layers;
}

size_t KDBTree::layers() const
{
    return _layers_count;
}
size_t KDBTree::vectors() const
{
    return _vector_count;
}
KDBNode *KDBTree::root()
{
    return _root;
}

KDBNode *KDBTree::__generate_until_n_layers(KDBNode *curr, u_int layers)
{
    if (layers <= 0)
    {
        return nullptr;
    }
    if (curr != nullptr)
    {
        KDBNode *left = __generate_until_n_layers(curr->left, layers - 1);
        KDBNode *right = __generate_until_n_layers(curr->right, layers - 1);
        if (left != nullptr && right != nullptr)
        {
            curr->left = left;
            curr->right = right;
        }
        return nullptr;
    }
    else
    {
        curr = new KDBNode(_folderpath, _dim, node_count);
        node_count++;
        curr->left = __generate_until_n_layers(curr->left, layers - 1);
        curr->right = __generate_until_n_layers(curr->right, layers - 1);
        return curr;
    }
}

bool KDBTree::insertEmbedding(Embedding &embedding)
{
    assert(_root);
    KDBNode *curr = _root;
    int dim = 0;
    while (!curr->isleaf())
    {
        if (embedding[dim] <= 0.5)
        {
            curr = curr->left;
        }
        else
        {
            curr = curr->right;
        }
        dim = (dim + 1) % embedding.dimensions;
    }
    assert(curr->isleaf());
    curr->insertEmbedding(embedding);
    _vector_count++;
    if (curr->size() >= _max_loaded_embeddings && _layers_count < pow(2, _dim - 1))
    {
        if (DEBUG)
        {
            std::cout << "NOTE [kdbtree.cpp][insertEmbedding]: Embedding limit reached (" + std::to_string(curr->size()) + ", " + std::to_string(curr->__get_node_id()) + ")" << std::endl;
        }
        __increase_layer_one();
        return true;
    }
    return false;
}

void KDBTree::__cycle_cache(KDBNode *to_insert)
{
    if (_loaded_embeddings.size() == _max_loaded_embeddings)
    {
        KDBNode *lruNode = _loaded_embeddings.front();
        _loaded_embeddings.pop();
        lruNode->clear();
    }
    _loaded_embeddings.push(to_insert);
}

KDBNode *KDBTree::__read_files_left_to_right(std::vector<std::__fs::filesystem::path> &files, int &fileIndex, int &nodeCount, int &dimensions, int layer, size_t &vectorCount)
{
    assert(layer > 0);
    if (layer == 1)
    {
        // make kdbnode reading file
        std::__fs::filesystem::path filePath = files.at(fileIndex);
        fileIndex++;
        std::ifstream inputFile(filePath.string(), std::ios::binary);
        if (!inputFile.is_open())
        {
            std::cerr << "Could not open input file: (" + filePath.string() + ")." << std::endl;
            throw std::runtime_error("");
        }
        if (inputFile.eof())
        {
            std::cerr << "Empty file on open: (" + filePath.string() + ")." << std::endl;
            throw std::runtime_error("");
        }

        // read 1 byte embedding datatype
        u_char datatypeByteCountRead = 0;
        inputFile.read(reinterpret_cast<char *>(&datatypeByteCountRead), sizeof(datatypeByteCountRead));

        // read 2 bytes of embedding dimension
        short dimRead;
        inputFile.read(reinterpret_cast<char *>(&dimRead), sizeof(dimRead));
        u_int dim = int(dimRead);
        dimensions = dim;

        // read 3 bytes of amount of embeddings
        int nodeSize = 0;
        inputFile.read(reinterpret_cast<char *>(&nodeSize), (sizeof(datatypeByteCountRead) * 3));

        size_t nodeId = __get_node_id_from_filepath(filePath);
        KDBNode *currNode = new KDBNode(filePath.root_path().string(), true, nodeId, dim, nodeCount);
        currNode->set_data_type_byte_count(int(datatypeByteCountRead));
        nodeCount++;
        std::vector<Embedding *> embeddingsList;
        for (int i = 0; i < nodeSize; i++)
        {
            float *currEmbedding = new float[dim];
            for (u_int fnum = 0; fnum < dim; fnum++)
            {
                float currFloat;
                inputFile.read(reinterpret_cast<char *>(&currFloat), sizeof(currFloat));
                currEmbedding[fnum] = currFloat;
            }
            short metadataLength;
            inputFile.read(reinterpret_cast<char *>(&metadataLength), sizeof(metadataLength));
            char *metadataBuffer = new char[metadataLength];
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
    KDBNode *curr = new KDBNode(_folderpath, 0, nodeCount);
    nodeCount++;
    curr->left = __read_files_left_to_right(files, fileIndex, nodeCount, dimensions, layer - 1, vectorCount);
    curr->right = __read_files_left_to_right(files, fileIndex, nodeCount, dimensions, layer - 1, vectorCount);
    return curr;
}

std::pair<Embedding *, float> KDBTree::query(const std::string &folderpath, const Embedding &queryEmbedding)
{
    int fileIndex = 0;
    KDBNode *curr = _root;
    int dim = 0;
    while (!curr->isleaf())
    {
        if (queryEmbedding[dim] <= 0.5)
        {
            curr = curr->left;
        }
        else
        {
            curr = curr->right;
            fileIndex += (_layers_count - 1 - dim) * (_layers_count - 1 - dim);
        }
        dim = (dim + 1) & _dim;
    }
    if (!std::__fs::filesystem::exists(folderpath))
    {
        std::cerr << "ERROR [kdbtree.cpp][query]: " + folderpath + " doesn't exist" << std::endl;
        throw std::runtime_error("");
    }
    if (!curr->loaded() && std::__fs::filesystem::exists(folderpath))
    {
        curr->clear();
        curr = __load_node(_folderpath, fileIndex);
        __cycle_cache(curr);
    }
    // TODO: implement LRU cache later
    float max_cosine_similarity = -2;
    Embedding *max_embedding = nullptr;
    for (Embedding *e : curr->embeddings)
    {
        float cosine_similarity = queryEmbedding.cosine_similarity(e);
        if (!(cosine_similarity <= 1 && cosine_similarity >= 0))
        {
            std::cerr << "ERROR [kdbtree.cpp][query]: cosine_similarity > 1 || cosine_similarity < 0 ... " + std::to_string(cosine_similarity) + "." << std::endl;
            throw std::runtime_error("");
        }
        assert(cosine_similarity <= 1 && cosine_similarity >= 0);
        if (cosine_similarity > max_cosine_similarity)
        {
            max_cosine_similarity = cosine_similarity;
            max_embedding = e;
        }
    }
    if (max_embedding == nullptr)
    {
        assert(curr->embeddings.size() <= 0);
        max_embedding = new Embedding(dimensions());
    }
    return std::pair<Embedding *, float>(max_embedding, max_cosine_similarity);
}

void KDBTree::__increase_layer_n(int layerIncrease)
{
    for (int i = 0; i < layerIncrease; i++)
    {
        __increase_layer_one_helper(_root);
        _layers_count++;
    }
}

void KDBTree::__increase_layer_one()
{
    __increase_layer_one_helper(_root);
    _layers_count++;
}

void KDBTree::__increase_node_one(KDBNode *curr, int splitIndex)
{
    KDBNode *leftNode = new KDBNode(_folderpath, true, node_count, dimensions(), curr->datatypebytecount());
    node_count++;
    KDBNode *rightNode = new KDBNode(_folderpath, true, node_count, dimensions(), curr->datatypebytecount());
    node_count++;
    for (Embedding *currEmbedding : curr->embeddings)
    {
        if (currEmbedding->at(splitIndex) <= 0.5)
        {
            leftNode->insertEmbedding(*currEmbedding);
        }
        else
        {
            rightNode->insertEmbedding(*currEmbedding);
        }
    }
    // leftNode->write_on_disk();
    // rightNode->write_on_disk();
    // curr->delete_on_disk();
    curr->clear();
    curr->left = leftNode;
    curr->right = rightNode;
}

void KDBTree::__increase_layer_one_helper(KDBNode *curr)
{
    if (curr == nullptr)
    {
        std::cerr << "ERROR [kdbtree.cpp][__increase_layer_one_helper]: Current KDBNode is a nullptr when it shouldn't be.";
    }
    if (curr->left || curr->right)
    {
        __increase_layer_one_helper(curr->left);
        __increase_layer_one_helper(curr->right);
        return;
    }

    int splitIndex = (_layers_count - 1) % dimensions();
    KDBNode *leftNode = new KDBNode(_folderpath, true, node_count, dimensions(), curr->datatypebytecount());
    node_count++;
    KDBNode *rightNode = new KDBNode(_folderpath, true, node_count, dimensions(), curr->datatypebytecount());
    node_count++;
    for (Embedding *currEmbedding : curr->embeddings)
    {
        if (currEmbedding->at(splitIndex) <= 0.5)
        {
            leftNode->insertEmbedding(*currEmbedding);
        }
        else
        {
            rightNode->insertEmbedding(*currEmbedding);
        }
    }
    curr->clear();
    curr->left = leftNode;
    curr->right = rightNode;
}

std::ostream &operator<<(std::ostream &os, KDBTree &obj)
{
    KDBNode *root = obj.root();
    std::queue<KDBNode *> q;
    q.push(root);
    u_int curr_square = 1;
    for (u_int i = 1; i <= obj.node_count; i++)
    {
        if (q.empty())
        {
            assert(false);
            return os;
        }
        KDBNode *top = q.front();
        if (top->left)
        {
            q.push(top->left);
        }
        if (top->right)
        {
            q.push(top->right);
        }
        q.pop();
        os << top->size();
        if ((pow(2, curr_square)) - 1 == i)
        {
            os << "\n";
            curr_square++;
        }
        else
        {
            os << ", ";
        }
    }
    return os;
}
