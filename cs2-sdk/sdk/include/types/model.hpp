#pragma once

#include <schemamgr/schema_manager.hpp>

#include <math/types/vector.hpp>

#include <types/handle.hpp>

enum BoneFlags_t : uint32_t {
    FLAG_NO_BONE_FLAGS = 0x0,
    FLAG_BONEFLEXDRIVER = 0x4,
    FLAG_CLOTH = 0x8,
    FLAG_PHYSICS = 0x10,
    FLAG_ATTACHMENT = 0x20,
    FLAG_ANIMATION = 0x40,
    FLAG_MESH = 0x80,
    FLAG_HITBOX = 0x100,
    FLAG_BONE_USED_BY_VERTEX_LOD0 = 0x400,
    FLAG_BONE_USED_BY_VERTEX_LOD1 = 0x800,
    FLAG_BONE_USED_BY_VERTEX_LOD2 = 0x1000,
    FLAG_BONE_USED_BY_VERTEX_LOD3 = 0x2000,
    FLAG_BONE_USED_BY_VERTEX_LOD4 = 0x4000,
    FLAG_BONE_USED_BY_VERTEX_LOD5 = 0x8000,
    FLAG_BONE_USED_BY_VERTEX_LOD6 = 0x10000,
    FLAG_BONE_USED_BY_VERTEX_LOD7 = 0x20000,
    FLAG_BONE_MERGE_READ = 0x40000,
    FLAG_BONE_MERGE_WRITE = 0x80000,
    FLAG_ALL_BONE_FLAGS = 0xfffff,
    BLEND_PREALIGNED = 0x100000,
    FLAG_RIGIDLENGTH = 0x200000,
    FLAG_PROCEDURAL = 0x400000,
};

enum BoneIndex_t {
    BONE_SPINE_0 = 1,
    BONE_SPINE_1 = 2,
    BONE_SPINE_2 = 3,
    BONE_SPINE_3 = 4,
    BONE_NECK_0 = 5,
    BONE_HEAD_0 = 6,
    BONE_ARM_UPPER_L = 8,
    BONE_ARM_LOWER_L = 9,
    BONE_HAND_L = 10,
    BONE_ARM_UPPER_R = 13,
    BONE_ARM_LOWER_R = 14,
    BONE_HAND_R = 15,
    BONE_LEG_UPPER_L = 22,
    BONE_LEG_LOWER_L = 23,
    BONE_ANKLE_L = 24,
    BONE_LEG_UPPER_R = 25,
    BONE_LEG_LOWER_R = 26,
    BONE_ANKLE_R = 27,
};

struct BoneData_t {
    Vector position;
    float scale;
    float rotation[4];
};

class CModel {
   public:
    uint32_t GetBoneParent(int index);
    uint32_t GetBoneFlags(int index);
    const char* GetBoneName(int index);
    uint32_t BoneCount();
};

class CModelState {
   public:
    SCHEMA(CStrongHandle<CModel>, m_hModel, "CModelState", "m_hModel");

    PAD(0x80);
    BoneData_t* bones;
};

class CSkeletonInstance {
   public:
    SCHEMA(CModelState, m_modelState, "CSkeletonInstance", "m_modelState");

    void GetBoneData(BoneData_t& bone, int index);
    void CalculateWorldSpaceBones(uint32_t flags);
};
