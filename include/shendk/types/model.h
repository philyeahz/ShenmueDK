#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include <map>

#include "shendk/types/vector.h"
#include "shendk/types/matrix.h"
#include "shendk/types/bounding_box.h"
#include "shendk/files/file.h"
#include "shendk/types/texture.h"

namespace shendk {

enum class BoneID : uint8_t {
    Root = 0,
    Spine = 1,
    Hip = 14,
    RightUpperLeg = 16,
    RightLowerLeg = 17,
    RightFoot = 18,
    RightFootToes = 19,
    LeftUpperLeg = 21,
    LeftLowerLeg = 22,
    LeftFoot = 23,
    LeftFootToes = 24,
    RightShoulder = 4,
    RightUpperArm = 5,
    RightLowerArm = 6,
    RightWrist = 7,
    RightRiggedHand = 8,
    RightHand = 191,
    RightHandIndexUpper = 28,
    RightHandIndexLower = 29,
    RightHandFingerUpper = 31,
    RightHandFingerLower = 32,
    RightHandThumb = 25,
    LeftShoulder = 9,
    LeftUpperArm = 10,
    LeftLowerArm = 11,
    LeftWrist = 12,
    LeftRiggedHand = 13,
    LeftHand = 190,
    LeftHandIndexUpper = 43,
    LeftHandIndexLower = 44,
    LeftHandFingerUpper = 46,
    LeftHandFingerLower = 47,
    LeftHandThumb = 40,
    Head = 189,
    Jaw = 188,
    None = 0xFF
};

static std::map<BoneID, std::string> BoneName = {
    {BoneID::Root, "Root"},
    {BoneID::Spine, "Spine"},
    {BoneID::Hip, "Hip"},
    {BoneID::RightUpperLeg, "RightUpperLeg"},
    {BoneID::RightLowerLeg, "RightLowerLeg"},
    {BoneID::RightFoot, "RightFoot"},
    {BoneID::RightFootToes, "RightFootToes"},
    {BoneID::LeftUpperLeg, "LeftUpperLeg"},
    {BoneID::LeftLowerLeg, "LeftLowerLeg"},
    {BoneID::LeftFoot, "LeftFoot"},
    {BoneID::LeftFootToes, "LeftFootToes"},
    {BoneID::RightShoulder, "RightShoulder"},
    {BoneID::RightUpperArm, "RightUpperArm"},
    {BoneID::RightLowerArm, "RightLowerArm"},
    {BoneID::RightWrist, "RightWrist"},
    {BoneID::RightHand, "RightHand"},
    {BoneID::RightHandIndexUpper, "RightHandIndexUpper"},
    {BoneID::RightHandIndexLower, "RightHandIndexLower"},
    {BoneID::RightHandFingerUpper, "RightHandFingerUpper"},
    {BoneID::RightHandFingerLower, "RightHandFingerLower"},
    {BoneID::RightHandThumb, "RightHandThumb"},
    {BoneID::LeftShoulder, "LeftShoulder"},
    {BoneID::LeftUpperArm, "LeftUpperArm"},
    {BoneID::LeftLowerArm, "LeftLowerArm"},
    {BoneID::LeftWrist, "LeftWrist"},
    {BoneID::LeftHand, "LeftHand"},
    {BoneID::LeftHandIndexUpper, "LeftHandIndexUpper"},
    {BoneID::LeftHandIndexLower, "LeftHandIndexLower"},
    {BoneID::LeftHandFingerUpper, "LeftHandFingerUpper"},
    {BoneID::LeftHandFingerLower, "LeftHandFingerLower"},
    {BoneID::LeftHandThumb, "LeftHandThumb"},
    {BoneID::Head, "Head"},
    {BoneID::Jaw, "Jaw"},
    {BoneID::None, "None"}
};

enum class TypeID : uint8_t {
    Unknown = 0x00, // Sometimes used by character bones (still rendered tho)
    Static = 0xFF
};

enum class LayerID : uint8_t {
    Static = 0x00,   // Always rendered
    Trigger = 0x02,  // Not rendered
    Unknown = 0x06,  // Mesh fence, unusable door, background pipes
    Unknown2 = 0x09, // Out of bounds terrain,
    Unknown3 = 0x04, // Lamp mesh, Roof flag post, some wall?, some windows?
    Unknown4 = 0x11, // Lamps
    Unknown5 = 0x14, // windows
    Unknown6 = 0x16, // background meshes
};

enum class TextureWrapMode {
    Clamp,
    Repeat,
    MirroredRepeat
};

enum class PrimitiveType {
    Triangles,
    TriangleStrip
};

struct Material {
    uint32_t textureIndex;
    std::shared_ptr<Texture> texture;
    TextureWrapMode textureWrapMode;
    bool unlit;
    bool transparent;
};


struct Vertex {
    Vector3f position;
    Vector3f normal;

    Vertex();
    Vertex(Vector3f pos, Vector3f norm);
};

struct Triangle {
    Triangle();
    Triangle(Vector3f _p1, Vector3f _p2, Vector3f _p3);
    Triangle(Vector3f _p1, Vector3f _p2, Vector3f _p3,
             Vector3f _n1, Vector3f _n2, Vector3f _n3);

    const Vector3f& center();
    Vector3f faceNormal();
    static Vector3f faceNormal(const Vector3f& _p1, const Vector3f& _p2, const Vector3f& _p3);
    static Vector3f faceCenter(const Vector3f& _p1, const Vector3f& _p2, const Vector3f& _p3);
    static float faceArea(const Vector3f& _p1, const Vector3f& _p2, const Vector3f& _p3);
    static float difference(const Triangle& t1, const Triangle& t2);

private:
    AABoundingBox bounds;
    Vector3f p1;
    Vector3f p2;
    Vector3f p3;
    Vector3f n1;
    Vector3f n2;
    Vector3f n3;
    Vector3f m_center;
    Vector3f m_faceNormal;
};


/** @brief Vertex buffer containing each vertex attribute as own array of varying lengths.
 *         This is due to the fact that a single vertex can have the same position and normal,
 *         but not the same texture coordinates. */
struct VertexBuffer {
    std::vector<Vector3f> positions;
    std::vector<Vector3f> normals;
    std::vector<Vector2f> texcoords;
    std::vector<Vector4f> colors;
    std::vector<float> weights;
    std::vector<BoneID> joints;
    std::vector<uint32_t> nodes;

    /** @brief transformed positions */
    std::vector<Vector3f> t_positions;
    /** @brief transformed normals */
    std::vector<Vector3f> t_normals;

    uint32_t vertexCount() { return static_cast<uint32_t>(positions.size()); }

    // byte arrays getter (for glTF)
    std::vector<uint8_t> getPositionData(Matrix4f matrix = Matrix4f::identity());
    std::vector<uint8_t> getNormalData(Matrix4f matrix = Matrix4f::identity());
    std::vector<uint8_t> getTexcoordData();
    std::vector<uint8_t> getColorData();
    std::vector<uint8_t> getWeightsData();
    std::vector<uint8_t> getJointsData();
};

struct MeshSurface {
    PrimitiveType type;
    Material material;
    std::vector<uint32_t> positionIndices;
    std::vector<uint32_t> normalIndices;
    std::vector<uint32_t> texcoordIndices;
    std::vector<uint32_t> colorIndices;
    std::vector<uint32_t> weightIndices;
    std::vector<uint32_t> jointIndices;
    std::vector<uint32_t> nodeIndices;

    int indexCount() { return positionIndices.size(); }
    bool hasPosition() { return positionIndices.size() > 0; }
    bool hasNormal() { return normalIndices.size() > 0; }
    bool hasTexcoord() { return texcoordIndices.size() > 0; }
    bool hasColor() { return colorIndices.size() > 0; }
    bool hasWeight() { return weightIndices.size() > 0; }
    bool hasJoint() { return jointIndices.size() > 0; }
    bool hasNode() { return nodeIndices.size() > 0; }

    int lengthBytes() { return stride() * indexCount(); }

    int stride() {
        int stride = 0;
        if (hasPosition()) {
            stride += sizeof(Vector3f);
        }
        if (hasNormal()) {
            stride += sizeof(Vector3f);
        }
        if (hasTexcoord()) {
            stride += sizeof(Vector2f);
        }
        /*if (hasColor()) {
            stride += sizeof(Vector4f);
        }*/
        return stride;
    }

    void mergeSurface(MeshSurface& rhs);
    void convertToTriangles();
};


struct ModelNode;

struct NodeMesh {

    NodeMesh(ModelNode* node);

    ModelNode* node;
    uint32_t vertexCount = 0;
    uint32_t vertexBufferOffset = 0;

    std::vector<MeshSurface> surfaces;

    VertexBuffer& vertexBuffer();
    uint32_t getIndex(int32_t index);
};

struct Model;

struct ModelNode : std::enable_shared_from_this<ModelNode> {

    ModelNode(Model* model);
    virtual ~ModelNode();

    std::vector<ModelNode*> getAllNodes(bool includeSibling = true, bool includeChildren = true);
    Matrix4f getTransformMatrixSelf();
    Matrix4f getTransformMatrix();
    Matrix4f getCenteredTransformMatrixSelf();
    Matrix4f getCenteredTransformMatrix();

    BoneID getBoneID();

    Model* model;
    uint32_t index = 0;

    uint32_t id = 0;
    Vector3f position;
    Vector3f rotation;
    Vector3f scale;
    Vector3f center = Vector3f::zero();
    float radius = 0.0f;

    ModelNode* child = nullptr;
    ModelNode* nextSibling = nullptr;
    ModelNode* parent = nullptr;

    NodeMesh* mesh = nullptr;

    std::string name = "";
};

struct Model {
    std::shared_ptr<ModelNode> rootNode;
    std::vector<Texture> textures;
    VertexBuffer vertexBuffer;

    uint32_t nodeCount = 0;

    void cleanMesh(bool removeBackfaces = true, bool weldSimilar = false, bool removeUnused = true,
                   float weldThreshold = 0.000005f, float backfaceAngle = 270.0f);
};


enum class IKBoneID : uint8_t {
    Root = 0,
    Hip = 1,

    RightUpperLeg = 5,
    RightFootIKTarget = 8,
    RightFoot = 9,

    LeftUpperLeg = 12,
    LeftFootIKTarget = 15,
    LeftFoot = 16,

    Torso = 18,
    UpperTorsoIKTarget = 20,
    Neck = 21,
    HeadLookAtTarget = 23,

    RightShoulder = 25,
    RightArm = 26,
    RightHandIKTarget = 29,
    RightHand = 30,

    LeftShoulder = 31,
    LeftArm = 32,
    LeftHandIKTarget = 33,
    LeftHandIKTarget_Dupe = 35,
    LeftHand = 36,

    None = 0xFF
};

static std::map<IKBoneID, std::string> IKBoneNames {
    {IKBoneID::Root, "Root"},
    {IKBoneID::Hip, "Hip" },

    {IKBoneID::RightUpperLeg, "RightUpperLeg"},
    {IKBoneID::RightFootIKTarget, "RightFootIKTarget" },
    {IKBoneID::RightFoot, "RightFoot"},

    {IKBoneID::LeftUpperLeg, "LeftUpperLeg" },
    {IKBoneID::LeftFootIKTarget, "LeftFootIKTarget" },
    {IKBoneID::LeftFoot, "LeftFoot" },

    {IKBoneID::Torso, "Torso" },
    {IKBoneID::UpperTorsoIKTarget, "UpperTorsoIKTarget" },
    {IKBoneID::Neck, "Neck" },
    {IKBoneID::HeadLookAtTarget, "HeadLookAtTarget" },

    {IKBoneID::RightShoulder, "RightShoulder" },
    {IKBoneID::RightArm, "RightArm" },
    {IKBoneID::RightHandIKTarget, "RightHandIKTarget"},
    {IKBoneID::RightHand, "RightHand"},

    {IKBoneID::LeftShoulder, "LeftShoulder" },
    {IKBoneID::LeftArm, "LeftArm" },
    {IKBoneID::LeftHandIKTarget, "LeftHandIKTarget" },
    {IKBoneID::LeftHandIKTarget_Dupe, "LeftHandIKTarget_Dupe"},
    {IKBoneID::LeftHand, "LeftHand"},
    {IKBoneID::None, "None"}
};


static std::map<IKBoneID, BoneID> IKBoneMap {
    {IKBoneID::Root, BoneID::Root},
    {IKBoneID::Hip, BoneID::Hip},
    {IKBoneID::RightUpperLeg, BoneID::RightUpperLeg},
    {IKBoneID::RightFootIKTarget, BoneID::RightLowerLeg}, // no IK rig yet, map to lower leg
    {IKBoneID::RightFoot, BoneID::RightFoot},

    {IKBoneID::LeftUpperLeg, BoneID::LeftUpperLeg},
    {IKBoneID::LeftFootIKTarget, BoneID::LeftLowerLeg}, // no IK rig yet, map to lower leg
    {IKBoneID::LeftFoot, BoneID::LeftFoot},

    {IKBoneID::Torso, BoneID::Spine},
    {IKBoneID::UpperTorsoIKTarget, BoneID::Spine}, // no IK rig yet, map to spine
    {IKBoneID::Neck, BoneID::Head}, // no IK rig yet, map to head
    {IKBoneID::HeadLookAtTarget, BoneID::Head}, // no IK rig yet, map to head

    {IKBoneID::RightShoulder, BoneID::RightShoulder},
    {IKBoneID::RightArm, BoneID::RightUpperArm},
    {IKBoneID::RightHandIKTarget, BoneID::RightLowerArm}, // no IK rig yet, map to lower arm
    {IKBoneID::RightHand, BoneID::RightHand},

    {IKBoneID::LeftShoulder, BoneID::LeftShoulder},
    {IKBoneID::LeftArm, BoneID::LeftUpperArm},
    {IKBoneID::LeftHandIKTarget, BoneID::LeftLowerArm}, // no IK rig yet, map to lower arm
    {IKBoneID::LeftHand, BoneID::LeftHand},
    {IKBoneID::LeftHandIKTarget_Dupe, BoneID::Root},
};

enum class Interpolation {
    STEP,
    LINEAR
};

static std::map<Interpolation, std::string> InterpolationString {
    {Interpolation::STEP, "STEP"},
    {Interpolation::LINEAR, "LINEAR"}
};

struct KeyFrame {
	int nodeID;
    float time;
    Matrix4f transform;
    Interpolation interpolation;
};

struct Sequence {
    std::vector<KeyFrame> frames;
};

struct Animation {
    std::string name;
    std::map<uint32_t, Sequence> sequences;
};

}
