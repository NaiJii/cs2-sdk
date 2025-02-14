#pragma once

#include <cache/entities/base.hpp>

class CEntityInstance;
class CCachedPlayer;
class C_CSPlayerPawn;

class CMatchCache {
   public:
    using CachedEntityPtr = std::unique_ptr<CCachedBaseEntity>;

    static CMatchCache& Get() {
        static CMatchCache inst;
        return inst;
    }

    static const auto& GetCachedEntities() { return Get().m_CachedEntities; }
    static auto& GetLock() { return Get().m_EntitiesLock; }
    static const auto& GetSpectators() { return Get().m_Spectators; }

    void Initialize();

    void AddEntity(CEntityInstance* inst, CBaseHandle handle);
    void RemoveEntity(CEntityInstance* inst, CBaseHandle handle);
    void UpdateSpectators();

    CachedEntityPtr& GetEntityByIndex(int i);
    CCachedPlayer* GetLocalPlayer();
    C_CSPlayerPawn* GetLocalPawn(bool aliveCheck = false);

   private:
    CachedEntityPtr CreateCachedEntityPointer(C_BaseEntity* ent);

    std::mutex m_EntitiesLock;
    std::unordered_map<int, CachedEntityPtr> m_CachedEntities;
    std::vector<std::pair<const char*, uint8_t>> m_Spectators;
};
