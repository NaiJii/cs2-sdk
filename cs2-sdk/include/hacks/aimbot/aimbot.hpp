#pragma once

#include <math/types/vector.hpp>
#include <hacks/aimbot/pid.hpp>
#include <interfaces/ccsgoinput.hpp>

class C_CSPlayerPawn;

class CAimbot {
   public:
    static CAimbot& Get() {
        static CAimbot inst;
        return inst;
    }

    bool IsEnabled();
    void Run(CMoveData* moveData);
   private:

    void RCS(Vector& angles, C_CSPlayerPawn* pawn);
    Vector Smooth(const Vector& from, const Vector& to);

    PIDController_t pid[2];
};
