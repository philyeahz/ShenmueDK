#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>
#include <filesystem>

#include "shendk/files/file.h"
#include "shendk/files/container/ipac.h"
#include "shendk/files/container/gz.h"
#include "shendk/utils/memstream.h"

namespace fs = std::filesystem;

namespace shendk {

struct PKS : File {

    struct Header {
        uint32_t signature;
        uint32_t ipacOffset;
        uint32_t unknown1;
        uint32_t unknown2;
    };

    PKS() = default;

    PKS(const std::string& filepath) {
        read(filepath);
    }

    ~PKS() {}

    PKS::Header header;
    IPAC ipac;

protected:
    virtual void _read(std::istream& stream) {
        int64_t baseOffset = stream.tellg();

        std::istream* _stream = &stream;

        if (GZ::testGzip(stream)) {
            uint64_t bufferSize;
            char* decompressed = GZ::inflateStream(stream, bufferSize);
            if (decompressed == nullptr) {
                return;
            }
            _stream = new imstream(decompressed, bufferSize);
        } else {
            _stream->seekg(baseOffset, std::ios::beg);
        }

        _stream->read(reinterpret_cast<char*>(&header), sizeof(PKS::Header));
        ipac.read(*_stream);
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&header), sizeof(PKS::Header));
        ipac.write(stream);
    }
};
}