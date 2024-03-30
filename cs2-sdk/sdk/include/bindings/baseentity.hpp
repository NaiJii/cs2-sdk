#pragma once

#include <bindings/entityinstance.hpp>
#include <bindings/gamescenenode.hpp>

#include <bindings/collisionproperty.hpp>
#include <bindings/hitbox.hpp>

#include <virtual/virtual.hpp>

struct BBox_t;

class C_BaseEntity : public CEntityInstance {
   public:
    bool IsPlayerController();
    bool IsWeapon();
    bool IsProjectile();
    bool IsPlantedC4();
    bool IsChicken();
    bool IsHostage();

    bool CalculateBBoxByCollision(BBox_t& out);
    bool CalculateBBoxByHitbox(BBox_t& out);

    CHitBoxSet* GetHitboxSet(int i);
    int HitboxToWorldTransforms(CHitBoxSet* hitBoxSet, CTransform* hitboxToWorld);
    bool GetHitboxPosition(int i, Vector& out);

    SCHEMA(CGameSceneNode*, m_pGameSceneNode, "C_BaseEntity", "m_pGameSceneNode");
    SCHEMA(CBaseHandle, m_hOwnerEntity, "C_BaseEntity", "m_hOwnerEntity");
    SCHEMA(CCollisionProperty*, m_pCollision, "C_BaseEntity", "m_pCollision");
    SCHEMA(uint8_t, m_iTeamNum, "C_BaseEntity", "m_iTeamNum");
    SCHEMA(int, m_iHealth, "C_BaseEntity", "m_iHealth");

    VIRTUAL_METHOD(void, GetEyePos, 160, (Vector* angles), angles);
    VIRTUAL_METHOD(void, GetEyeAngles, 161, (Vector* angles), angles);
};
