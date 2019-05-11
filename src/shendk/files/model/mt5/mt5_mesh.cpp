#include "shendk/files/model/mt5/mt5_mesh.h"

#include <iostream>

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
    vertexCount = header.vertexCount;

    stream.seekg(header.verticesOffset, std::ios::beg);
    for (int i = 0; i < header.vertexCount; i++) {
        float posX, posY, posZ, normX, normY, normZ;
        stream.read(reinterpret_cast<char*>(&posX), sizeof(float));
        stream.read(reinterpret_cast<char*>(&posY), sizeof(float));
        stream.read(reinterpret_cast<char*>(&posZ), sizeof(float));
        stream.read(reinterpret_cast<char*>(&normX), sizeof(float));
        stream.read(reinterpret_cast<char*>(&normY), sizeof(float));
        stream.read(reinterpret_cast<char*>(&normZ), sizeof(float));
        vertexPositions.push_back(Vector3f(posX, posY, posZ));
        vertexNormals.push_back(Vector3f(normX, normY, normZ));
    }

    if (parentNode && parentNode->mesh) {
        Matrix4f matrix = node->getTransformMatrixSelf().invert();
        for (int i = 0; i < parentNode->mesh->vertexCount; i++) {
            Vector3f pos = parentNode->mesh->vertexPositions[i];
            Vector3f norm = parentNode->mesh->vertexNormals[i];
            pos = pos.transformPosition(matrix);
            norm = norm.transformPosition(matrix);
            vertexPositions.push_back(pos);
            vertexNormals.push_back(norm);
        }
    }

    int64_t instructionOffset;
    stream.seekg(header.facesOffset, std::ios::beg);
    while (!stream.eof()) {
        InstructionType stripType;
        instructionOffset = stream.tellg();
        stream.read(reinterpret_cast<char*>(&stripType), sizeof(uint16_t));
        stream.seekg(instructionOffset, std::ios::beg);

        if (stripType == InstructionType::End_0080) { break; }

        switch(stripType) {
            case InstructionType::Zero_0000:
                instructions.push_back(std::shared_ptr<Instruction>(new InBasic(stream)));
                continue;
            case InstructionType::Skip_FFFF:
                instructions.push_back(std::shared_ptr<Instruction>(new InBasic(stream)));
                continue;

            case InstructionType::StripAttrib_0200:
            case InstructionType::StripAttrib_0300:
            case InstructionType::StripAttrib_0400:
            case InstructionType::StripAttrib_0500:
            case InstructionType::StripAttrib_0600:
            case InstructionType::StripAttrib_0700:
            {
                std::shared_ptr<InAttributes>attr = std::shared_ptr<InAttributes>(new InAttributes(stream));
                state.attributes = *attr.get();
                instructions.push_back(attr);
                continue;
            }
            case InstructionType::Texture_0900:
            {
                std::shared_ptr<InTexture>tex = std::shared_ptr<InTexture>(new InTexture(stream));
                state.texture = *tex.get();
                instructions.push_back(tex);
                continue;
            }
            case InstructionType::Unknown1_0800:
            case InstructionType::Unknown1_0A00:
            {
                std::shared_ptr<InUnknown1>u1 = std::shared_ptr<InUnknown1>(new InUnknown1(stream));
                state.unknown1 = *u1.get();
                instructions.push_back(u1);
                continue;
            }
            case InstructionType::UvSize_0B00:
            {
                std::shared_ptr<InUvSize>uv = std::shared_ptr<InUvSize>(new InUvSize(stream));
                state.uvSize = *uv.get();
                instructions.push_back(uv);
                continue;
            }
            case InstructionType::Unknown2_0E00:
            case InstructionType::Unknown2_0F00:
            {
                std::shared_ptr<InUnknown2>u2 = std::shared_ptr<InUnknown2>(new InUnknown2(stream));
                state.unknown2 = *u2.get();
                instructions.push_back(u2);
                continue;
            }
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
                instructions.push_back(std::shared_ptr<Instruction>(new InStrip(stream, this)));
                continue;
            default:
                stripType = InstructionType::End_0080;
                break;
        }
        if (stripType == InstructionType::End_0080) { break; }
    }
}

void MT5Mesh::write(std::ostream& stream) {

}

}
}
