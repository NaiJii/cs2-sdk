#pragma once

#include <hacks/aimbot/lagcomp.hpp>
#include <cache/entities/base.hpp>

class CCSPlayerController;

class CCachedPlayer : public CCachedBaseEntity {
   public:
    enum class Team { UNKNOWN = 0, SPECTATOR, TERRORIST, COUNTER_TERRORIST };

    Type GetType() const override { return Type::PLAYER; }

    auto Get() const { return CCachedBaseEntity::Get<CCSPlayerController>(); }

    bool IsValid(bool aliveCheck = true);
    // bool IsValid() override;
    void DrawESP() override;
    void CalculateDrawInfo() override;
    void Reset();

    Team GetTeam();
    bool IsEnemyWithTeam(Team team);
    bool IsLocalPlayer();

    float visibleSince;
    float dot;
    float fitts;

    std::deque<CRecord> records;
};
