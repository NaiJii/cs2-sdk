#include "pch.hpp"
#include <hacks/aimbot/aimbot.hpp>

#include <vars/vars.hpp>

#include <interfaces/engineclient.hpp>
#include <interfaces/enginetrace.hpp>
#include <interfaces/ccsgoinput.hpp>
#include <interfaces/globalvars.hpp>
#include <interfaces/cvar.hpp>

#include <cache/entities/player.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>

#include <math/math.hpp>

#include <logger/logger.hpp>

#include <renderer/renderer.hpp>
#include <imgui/imgui.h>

bool CAimbot::IsEnabled() {
    if (!g_Vars.m_EnableAimbot || !CEngineClient::Get()->IsInGame()) return false;
    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    if (!cachedLocal || !cachedLocal->IsValid()) return false;
    CCSPlayerController* localController = cachedLocal->Get();
    if (!localController) return false;
    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    if (!localPawn) return false;

    Vector* _punch = localPawn->GetLastAimPunch();
    // if the aim punch is invalid, keep the last one
    if (_punch) {
        punch = *_punch;
        punchDelta = (*_punch - oldPunch).NormalizedAngle();
        oldPunch = *_punch;
    }

    C_CSWeaponBaseGun* weapon = localPawn->GetActiveWeapon();
    if (!weapon) return false;
    CCSWeaponBaseVData* weaponData = weapon->GetWeaponData();
    if (!weaponData || !weaponData->IsGun()) return false;

    return true;
}

void CAimbot::Run(CMoveData* moveData) {
    if (!IsEnabled()) {
        Invalidate();
        return;
    }

    CMoveData& lastMove = *moveData;
    CCachedPlayer* cachedLocal = CMatchCache::Get().GetLocalPlayer();
    CCSPlayerController* localController = cachedLocal->Get();
    C_CSPlayerPawn* localPawn = localController->m_hPawn().Get();
    C_CSWeaponBaseGun* weapon = localPawn->GetActiveWeapon();
    CCSWeaponBaseVData* weaponData = weapon->GetWeaponData();
    const int weaponType = weaponData->m_WeaponType();
    const bool isFiring = localPawn->m_iShotsFired() > 1 && weaponType != WEAPONTYPE_SHOTGUN && weaponType != WEAPONTYPE_SNIPER_RIFLE;
    Vector localPos;
    localPawn->GetEyePos(&localPos);

    Vector targetAngle;
    perfectAngle = lastMove.viewAngles;

    rcsAngle = lastMove.viewAngles - punchDelta * 2.f;
    rcsAngle.z = 0.f;
    rcsAngle.NormalizeAngle();

    if (isFiring) {
        lastMove.viewAngles.x -= punchDelta.x * 2.f * g_Vars.m_RecoilX;
        lastMove.viewAngles.y -= punchDelta.y * 2.f * g_Vars.m_RecoilY;
        lastMove.viewAngles.NormalizeAngle();
    }

    curAngle = lastMove.viewAngles;
    curAngle.z = 0.f;

    const float inaccuracy = weapon->GetInaccuracy();

    if (inaccuracy > 0.05f) {
        Invalidate();
        return;
    }

    if (g_Vars.m_EnableTriggerbot && !isFiring && weapon->m_nNextPrimaryAttackTick() < localController->m_nTickBase()) {
        const Vector end = localPos + rcsAngle.ToVector().Normalized() * 4096.f;
        GameTrace_t trace;
        if ( CEngineTrace::Get()->TraceShape(localPos, end, localPawn, 0x1C1003, 4, &trace) )
        {
            if (trace.hitEntity && trace.hitEntity->IsPlayerPawn()) {
                C_CSPlayerPawn* hitPawn = static_cast<C_CSPlayerPawn*>(trace.hitEntity);
                static ConVar* mp_teammates_are_enemies = CCVar::Get()->GetCvarByName("mp_teammates_are_enemies");
                if (mp_teammates_are_enemies->GetValue<bool>() ? true : hitPawn->m_iTeamNum() != localPawn->m_iTeamNum())
                    lastMove.Scroll(IN_ATTACK);
            }
        }
    }

    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());

    const auto& cachedEntities = CMatchCache::GetCachedEntities();
    target = nullptr;
    Vector targetPos;

    float currentFov = std::numeric_limits<float>::max();
    for (const auto& it : cachedEntities) {
        const auto& cachedEntity = it.second;
        if (!cachedEntity->IsValid() || cachedEntity->GetType() != CCachedBaseEntity::Type::PLAYER) continue;

        CCachedPlayer* cachedPlayer = dynamic_cast<CCachedPlayer*>(cachedEntity.get());
        if (!cachedPlayer || !cachedPlayer->IsEnemyWithTeam(cachedLocal->GetTeam())) continue;

        CCSPlayerController* controller = cachedPlayer->Get();
        if (!controller->m_bPawnIsAlive()) continue;

        C_CSPlayerPawn* pawn = controller->m_hPawn().Get();
        if (!pawn || pawn->m_bGunGameImmunity()) continue;

        Vector pos;
        if (!pawn->GetHitboxPosition(0, pos)) continue;

        if (IsInSmoke(localPos, pos)) {
            cachedPlayer->visibleSince = 0.f;
            cachedPlayer->dot = 0.f;
            continue;
        }

        GameTrace_t trace;
        if (!CEngineTrace::Get()->TraceShape(localPos, pos, localPawn, 0x1C1003, 4, &trace)) {
            cachedPlayer->visibleSince = 0.f;
            cachedPlayer->dot = 0.f;
            continue;
        }

        if (trace.fraction < 0.97f) {
            cachedPlayer->visibleSince = 0.f;
            cachedPlayer->dot = 0.f;
            continue;
        }

        Vector angle = CMath::Get().CalculateAngle(localPos, pos);

        cachedPlayer->visibleSince += CGlobalVars::Get()->intervalPerTick;
        cachedPlayer->dot = -pawn->m_angEyeAngles().ToVector().Normalized().DotProduct(angle.ToVector().Normalized());

        if (cachedPlayer->visibleSince < 0.15f) continue;
        const float fov = CMath::Get().Fov(rcsAngle, angle);
        if (fov < currentFov) {
            target = cachedPlayer;
            targetPos = pos;
            targetAngle = angle;
            currentFov = fov;
        }
    }

    if (const bool isSwitchingTargets = oldTarget && oldTarget != target; isSwitchingTargets)
        lastTargetSwitchTime = CGlobalVars::Get()->currentTime;

    oldTarget = target;

    if (const bool inputDown = (lastMove.buttonsHeld & IN_ATTACK || lastMove.buttonsHeld & IN_ATTACK2); inputDown)
        lastActiveTime = CGlobalVars::Get()->currentTime;

    const bool isSwitching = lastTargetSwitchTime - CGlobalVars::Get()->currentTime <= 0.15f;
    const float mouseLength = (float)std::hypot(lastMove.mouseDx, lastMove.mouseDy);
    const bool shouldAim = currentFov <= g_Vars.m_AimFov && CGlobalVars::Get()->currentTime - lastActiveTime <= 0.2f;

    if (target && shouldAim) {
        perfectAngle = targetAngle - punch * 2;
        perfectAngle.NormalizeAngle();

        const Vector smoothedAngle = curAngle + Smooth(rcsAngle, perfectAngle);
        const Vector curDelta = (perfectAngle - lastMove.viewAngles).NormalizedAngle();
        const Vector smoothedDelta = (perfectAngle - smoothedAngle).NormalizedAngle();

        // only apply the angle if it's beneficial
        if ( fabsf(curDelta.x) > fabsf(smoothedDelta.x) )
            lastMove.viewAngles.x = smoothedAngle.x;
        
        if ( fabsf(curDelta.y) > fabsf(smoothedDelta.y) )
            lastMove.viewAngles.y = smoothedAngle.y;
    } else {
        Invalidate();
    }

    lastMove.viewAngles.NormalizeAngle();
}

void CAimbot::Update() {
    //targetScreen = ImVec2(0, 0);
    //if (!target) return;
    //CMath::Get().WorldToScreen(targetPos, targetScreen);
}

void CAimbot::Render() {
    auto drawList = CRenderer::GetBackgroundDrawList();
    if (!drawList) return;

    if (target) {
        drawList->AddCircle(targetScreen, 5.f, IM_COL32(255, 255, 255, 255));
    }
}

void CAimbot::Invalidate() {
    target = nullptr;
    pid[0].Reset();
    pid[1].Reset();
    targetScreen = ImVec2(0, 0);
}

bool CAimbot::IsInSmoke(const Vector& start, const Vector& end) {
    static auto func = signatures::LineGoesThroughSmoke.GetPtrAs<float(*)(const Vector&, const Vector&, uintptr_t)>();
    return func && func(start, end, 0) >= 1.0f;
}

Vector CAimbot::Smooth(const Vector& from, const Vector& to) {
    const float bonus = target->dot >= g_Vars.m_ReactionTreshold ? 1.f : g_Vars.m_Bonus + 1.f;  // [1.f - 2.f]
    const PIDConfig_t pidCfg{
        .m_KP = g_Vars.m_KP * 0.3f * bonus, .m_KI = g_Vars.m_KI * 0.3f * bonus, .m_kd = 0.f, .m_Damp = 1.f - g_Vars.m_Damp};

    Vector delta = (to - from).NormalizedAngle();
    delta.x = pid[0].Step(delta.x, 0.015f, pidCfg);
    delta.y = pid[1].Step(delta.y, 0.015f, pidCfg);
    return delta;
}
