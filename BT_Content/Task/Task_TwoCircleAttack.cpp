#include "Task_TwoCircleAttack.h"
#include <iostream>
#include <algorithm>

using namespace Action;

static inline float clampf(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }

BT::NodeStatus Task_TwoCircleAttack::tick()
{
    auto bb_res = getInput<CPPBlackBoard*>("BB");
    if (!bb_res)
    {
        std::cerr << "[Task_TwoCircleAttack] BB nullptr\n";
        return BT::NodeStatus::FAILURE;
    }
    CPPBlackBoard* BB = bb_res.value();

    // 조건: 내가 더 느림(열세) → Two-Circle
    const float myV = BB->MySpeed_MS;
    const float tgV = BB->TargetSpeed_MS;
    if (!(myV < tgV)) {
        return BT::NodeStatus::FAILURE;  // 원본 동일 동작 유지 :contentReference[oaicite:14]{index=14}
    }

    const float D = BB->Distance;
    const float dv = tgV - myV;                 // >0 (열세)
    // [개선] 바깥 원: 우측(+) 바깥으로 크게 벌리며, 거리/속도차 기반 가변
    float side_out = clampf(350.0f + 0.4f * D + 10.0f * dv, 400.0f, 900.0f); // 바깥(크게)
    float forward = clampf(80.0f + 0.15f * D, 100.0f, 350.0f);            // 약간 전진

    BB->VP_Cartesian = BB->TargetLocaion_Cartesian
        + BB->TargetRightVector * side_out
        + BB->TargetForwardVector * (forward);

    std::cout << "[Task_TwoCircleAttack] Two-Circle | dv=" << dv << ", D=" << D
        << " | side_out=" << side_out << ", fwd=" << forward << "\n";
    return BT::NodeStatus::SUCCESS;
}
