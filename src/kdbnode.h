#pragma once
#include <string>
#include <vector>
#include "embedding.h"

enum datatype{
    FLOAT,
    DOUBLE
};

class KDBNode {
    private:

        std::string _file_location;
        bool _is_written;
        size_t _node_id;
        size_t _size;
        unsigned _dim;
        unsigned char _data_type_byte_count = static_cast<unsigned char>(4);

        size_t _GetNodeId();
        std::string __id_to_n_len_id(int nIdSize, size_t nodeId);

    public:

        bool is_leaf;
        KDBNode* left;
        KDBNode* right;
        std::vector<Embedding*> embeddings;


        KDBNode(unsigned dim, size_t node_id);
        KDBNode(std::string file_location, bool is_written, size_t node_id, unsigned dim, unsigned char data_type_byte_count);

        void SetIsWritten(bool is_written);
        void SetDTByteCount(unsigned char data_type_byte_count);
        void SetSize(int size);

        std::pair<std::string, bool> GetFileLocation();
        unsigned GetDimension();
        size_t GetSize();
        unsigned char GetDTByte();

        bool InsertEmbedding(Embedding& embedding);
        bool LoadEmbeddings(std::string filename);

        friend std::ostream& operator<<(std::ostream& os, const KDBNode& obj);
};