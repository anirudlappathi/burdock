#pragma once
#include <iostream>
#include <string>
#include <stdexcept>

class BurdockError : public std::exception {
    public:
        BurdockError(const std::string& message, const std::string& filename, const int line, const int code)
            : _message(message), _filename(filename), _line(line), _code(code) {};

        virtual std::string toString() const = 0;

        virtual const char* what() const noexcept override {
            return _message.c_str();
        }

        std::string getFile() const {
            return _filename;
        }

        int getLine() const {
            return _line;
        }

        int getCode() const {
            return _code;
        }

    protected:
        std::string _message;
        std::string _filename;
        int _line;
        int _code; 
}

class OutOfRange : public BurdockError {
    // const char* what() const noexcept override {
    //     std::cerr << "ERROR [vectordb.cpp][insert]: Query dimension of embedding does not match Vector Database embedding dimension" << std::endl;
    // }
} 