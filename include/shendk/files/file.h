#pragma once

#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace shendk {

	/**
	* @brief Base class for all file classes
	**/
    struct File {

        virtual ~File() {}

        /**
        * @brief Reads a file.
        * @param filepath Path of the file.
        **/
        void read(const std::string& filepath) {
            if (!fs::exists(filepath)) return;
            std::ifstream fstream(filepath, std::ios::binary);
            if (fstream.is_open()) {
                read(fstream);
                fstream.close();
            } else {
                throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
            }
        }

        void read(std::ifstream& stream) {
            _read(stream);
        }

        /**
        * @brief Writes a file.
        * @param filepath Path of the file.
        **/
        void write(const std::string& filepath) {
            std::ofstream fstream(filepath, std::ios::binary);
            if (fstream.is_open()) {
                _write(fstream);
                fstream.close();
            }
            else {
                throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
            }
        }

        void write(std::ofstream& stream) {
            _write(stream);
        }


	protected:
        virtual bool _isValid();
        virtual void _read(std::ifstream& stream) = 0;
        virtual void _write(std::ofstream& stream) = 0;
	};
}
