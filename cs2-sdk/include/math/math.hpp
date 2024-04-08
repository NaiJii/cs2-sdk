#pragma once

#include <math/types/transform.hpp>
#include <math/types/vmatrix.hpp>
#include <math/types/vector.hpp>

struct ImVec2;

class CMath {
   public:
    static CMath& Get() {
        static CMath inst;
        return inst;
    }

    void UpdateViewMatrix(VMatrix* viewMatrix);

    bool WorldToScreen(const Vector& in, ImVec2& out);
    void TransformAABB(const matrix3x4_t& transform, const Vector& minsIn, const Vector& maxsIn, Vector& minsOut, Vector& maxsOut);
    Vector CalculateAngle(const Vector& src, const Vector& dst) const;
    float Fov(const Vector& angSrc, const Vector& angDst) const;

    constexpr float Deg2Rad(float deg) const { return deg * ((float)(float)M_PI / 180.f); }
    constexpr float Rad2Deg(float rad) const { return rad * (180.f / (float)(float)M_PI); }

   private:
    VMatrix m_ViewMatrix;
};
