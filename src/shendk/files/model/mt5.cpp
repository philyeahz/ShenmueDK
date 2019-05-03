#include "shendk/files/model/mt5.h"

#include "shendk/files/model/mt5/mt5_node.h"
#include "shendk/types/texture_id.h"

namespace shendk {

MT5::MT5() = default;
MT5::MT5(const std::string& filepath) { read(filepath); }

void MT5::_read(std::istream& stream) {

    // read header
    stream.read(reinterpret_cast<char*>(&header), sizeof(MT5::Header));
    if (!isValid(header.signature)) throw new std::runtime_error("Invalid signature for MT5 file!\n");

    // read nodes
    stream.seekg(baseOffset + header.firstNodeOffset, std::ios::beg);
    model->rootNode = new mt5::MT5Node(stream, baseOffset);

    // read appended nodes (textures and other stuff)
    int64_t nodeOffset;
    Node::Header nodeHeader;
    stream.seekg(baseOffset + header.nodesSize, std::ios::beg);
    while(!stream.eof()) {
        nodeOffset = stream.tellg();
        stream.read(reinterpret_cast<char*>(&nodeHeader), sizeof(Node::Header));
        if (nodeHeader.signature == 0x44584554) { //TEXD
            stream.seekg(nodeOffset, std::ios::beg);
            if (!texd) {
                texd = new mt5::TEXD(stream);
            } else {
                throw new std::runtime_error("TEXD node already exists!");
            }
        } else if (nodeHeader.signature == 0x454D414E) { //NAME
            stream.seekg(nodeOffset, std::ios::beg);
            if (!name) {
                name = new mt5::NAME(stream);
            } else {
                throw new std::runtime_error("NAME node already exists!");
            }
        } else if (nodeHeader.signature == 0x4C584554) { //TEXL
            stream.seekg(nodeOffset, std::ios::beg);
            if (!texl) {
                texl = new mt5::TEXL(stream);
            } else {
                throw new std::runtime_error("TEXL node already exists!");
            }
        } else if (nodeHeader.signature == 0x4C525450) { //PTRL
            if (!ptrl) {
                ptrl = new mt5::PTRL(stream);
            } else {
                throw new std::runtime_error("PTRL node already exists!");
            }
        } else if (nodeHeader.signature == 0x4E584554) { // TEXN
            stream.seekg(nodeOffset, std::ios::beg);
            TEXN texn;
            texn.read(stream);
            texnEntries.push_back(texn);
        }
        stream.seekg(nodeOffset + nodeHeader.size, std::ios::beg);
    }

    // check if texture count is satisfied
    if (texd) {
        uint64_t textureCount = texnEntries.size();
        if (name) { textureCount += name->textureIDs.size(); }
        if (texd->textureCount != textureCount) {
            throw new std::runtime_error("Found textures don't match the given texture count!");
        }
    }
}

void MT5::_write(std::ostream& stream) {
    throw new std::runtime_error("Not implemented");
    stream.write(reinterpret_cast<char*>(&header), sizeof(MT5::Header));
}

bool MT5::_isValid(uint32_t signature) {
    if (this->signature != signature)
        return false;
    return true;
}

}