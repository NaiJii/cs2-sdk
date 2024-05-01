#include "pch.hpp"

#include <interfaces/engineclient.hpp>

#include <memory/memory.hpp>
#include <constants/constants.hpp>

#include <virtual/virtual.hpp>

CEngineClient* CEngineClient::Get() {
    static const auto inst = CMemory::GetInterface(CConstants::ENGINE_LIB, "Source2EngineToClient001");
    return inst.Get<CEngineClient*>();
}

bool CEngineClient::IsInGame() { return vt::CallMethod<bool>(this, 35); }

int CEngineClient::GetLocalPlayer() {
    int index = -1;

    vt::CallMethod<void>(this, 47, &index, 0);

    return index + 1;
}

int CEngineClient::GetEngineBuildNumber() { return vt::CallMethod<int>(this, 80); }
