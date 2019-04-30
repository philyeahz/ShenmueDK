#include "shendk/files/model/mt5/mt5_mesh.h"

namespace shendk {
namespace mt5 {

MT5Mesh::MT5Mesh(std::istream& stream, MT5Node* owner) {
    read(stream, owner);
}

MT5Mesh::~MT5Mesh() {}

void MT5Mesh::read(std::istream& stream, MT5Node* owner) {
    node = owner;
    if (node->parent) {
        parentNode = dynamic_cast<MT5Node*>(node->parent);
    }

    stream.read(reinterpret_cast<char*>(&header), sizeof(MT5Mesh::Header));
    node->center = Vector3f(header.centerX, header.centerY, header.centerZ);
    node->radius = header.radius;

    // initialize states
    State state;
    uint16_t textureIndex = 0;
    //Attributes attributes;

    int64_t instructionOffset;
    stream.seekg(header.facesOffset, std::ios::beg);
    while (!stream.eof()) {
        InstructionType stripType;
        instructionOffset = stream.tellg();
        stream.read(reinterpret_cast<char*>(&stripType), sizeof(uint16_t));
        stream.seekg(instructionOffset, std::ios::beg);

        if (stripType == InstructionType::End_0080) {
            break;
        }

        switch(stripType) {
            case InstructionType::Zero_0000:
                instructions.push_back(Instruction(stream));
                continue;
            case InstructionType::Skip_FFFF:
                instructions.push_back(Instruction(stream));
                continue;

            case InstructionType::StripAttrib_0200:
            case InstructionType::StripAttrib_0300:
            case InstructionType::StripAttrib_0400:
            case InstructionType::StripAttrib_0500:
            case InstructionType::StripAttrib_0600:
            case InstructionType::StripAttrib_0700:
                instructions.push_back(InAttributes(stream));
                continue;

            case InstructionType::Texture_0900:
                instructions.push_back(InTexture(stream));
                continue;

            case InstructionType::Unknown1_0800:
            case InstructionType::Unknown1_0A00:
                instructions.push_back(InUnknown1(stream));
                continue;

            case InstructionType::Unknown2_0B00:
                instructions.push_back(InUnknown2(stream));
                continue;

            case InstructionType::Unknown3_0E00:
            case InstructionType::Unknown3_0F00:
                instructions.push_back(InUnknown3(stream));
                continue;

            case InstructionType::Strip_1000:
            case InstructionType::Strip_1100:
            case InstructionType::Strip_1200:
            case InstructionType::Strip_1300:
            case InstructionType::Strip_1400:
            case InstructionType::Strip_1800:
            case InstructionType::Strip_1900:
            case InstructionType::Strip_1A00:
            case InstructionType::Strip_1B00:
            case InstructionType::Strip_1C00:
                instructions.push_back(InStrip(stream, state));
                continue;

            default:
                break;
        }
    }
}

void MT5Mesh::write(std::ostream& stream) {

}

}
}
