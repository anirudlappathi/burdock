#include "kdbnode.h"
#include <iostream>
#include <cassert>

#define S_IDSIZE 8

std::string KDBNode::__id_to_n_len_id(int nIdSize, size_t nodeId) {
    std::string stringNodeId = std::to_string(nodeId);
    int stringNodeIdLength = int(stringNodeId.size());
    if (stringNodeIdLength > nIdSize) {
        std::cerr << "stringNodeId size is greater than given id size";
    }
    for (int i = 0; i < nIdSize - stringNodeIdLength; i++) {
        stringNodeId = "0" + stringNodeId;
    }
    return stringNodeId;
}

KDBNode::KDBNode(unsigned dim, size_t nodeId): 
_node_id(nodeId),
_size(0),
_dim(dim) {
    if (dim > 65535) {
        throw std::invalid_argument("Dimension cannot be less than or equal to 0 or greater than 65536");
    }
    _file_location = "dbfiles" + __id_to_n_len_id(S_IDSIZE, nodeId) + ".vdb";
    bool loading_success = LoadEmbeddings(_file_location);
}

KDBNode::KDBNode(std::string file_location, bool is_written, size_t nodeId, unsigned dim, unsigned char data_type_byte_count):
    _is_written(is_written),
    _node_id(nodeId),
    _dim(dim),
    _data_type_byte_count(data_type_byte_count) {

}

std::pair<std::string, bool> KDBNode::GetFileLocation() {
    return std::pair<std::string, bool>(_file_location, _is_written);
}

unsigned KDBNode::GetDimension() {
    return _dim;
}

size_t KDBNode::_GetNodeId() {
    return _node_id;
}

void KDBNode::SetSize(int size) {
    _size = size;
}

size_t KDBNode::GetSize() {
    _size = embeddings.size();
    return _size;
}

void KDBNode::SetIsWritten(bool is_written) {
    _is_written = is_written;
}

void KDBNode::SetDTByteCount(unsigned char data_type_byte_count) {
    _data_type_byte_count = data_type_byte_count;
}

unsigned char KDBNode::GetDTByte() {
    return _data_type_byte_count;
}

bool KDBNode::InsertEmbedding(Embedding& embedding) {
    if (_size == 0) {
        bool loading_success = LoadEmbeddings(_file_location);
    }
    assert(embedding.dimensions == _dim);
    // create copy of embedding
    Embedding* heap_embedding = new Embedding(embedding);
    embeddings.push_back(heap_embedding);
    _size++;
    return true;
}

bool KDBNode::LoadEmbeddings(std::string filename) {
    // load embeddings from file into memory
    if (_dim == 0) {
        return false;
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, const KDBNode& obj) {
    if (obj.embeddings.empty()) {
        os << "{\n\tNO_EMBEDDINGS\n}";
        return os;
    }
    os << "{\n";
    for (auto it = obj.embeddings.begin(); it != obj.embeddings.end(); it++) {
        os << "\t" << **it << "\n";
    }
    os << "}" << std::endl;
    return os;
}

