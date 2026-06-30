#include "Task_OneCircleAttack.h"
#include <iostream>
#include <algorithm>

using namespace Action;

static inline float clampf(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }

BT::NodeStatus Task_OneCircleAttack::tick()
{
    auto bb_res = getInput<CPPBlackBoard*>("BB");
    if (!bb_res)
    {
        std::cerr << "[Task_OneCircleAttack] BB nullptr\n";
        return BT::NodeStatus::FAILURE;
    }
    CPPBlackBoard* BB = bb_res.value();

    // 조건: 내가 더 빠름(우세) → One-Circle
    const float myV = BB->MySpeed_MS;
    const float tgV = BB->TargetSpeed_MS;
    if (!(myV > tgV)) {
        // 원본도 조건 불만족 시 FAIL. (Fallback에서 TwoCircle로 이동)
        return BT::NodeStatus::FAILURE;  // 원본 동일 동작 유지 :contentReference[oaicite:11]{index=11}
    }

    const float D = BB->Distance;
    const float dv = myV - tgV;                // >0 (우세)
    // [개선] 인사이드 컷: 우측(-) 내측으로 파고들되, 거리/속도차 기반 가변
    float side_in = clampf(200.0f + 0.3f * D + 8.0f * dv, 250.0f, 650.0f); // 내측(작게)
    float forward = clampf(100.0f + 0.2f * D, 120.0f, 400.0f);           // 소폭 전진

    // 우측 내측(음수)으로 파고들며 약간 전진해 진로 선점
    BB->VP_Cartesian = BB->TargetLocaion_Cartesian
        - BB->TargetRightVector * side_in
        + BB->TargetForwardVector * (forward);

    std::cout << "[Task_OneCircleAttack] One-Circle | dv=" << dv << ", D=" << D
        << " | side_in=" << side_in << ", fwd=" << forward << "\n";
    return BT::NodeStatus::SUCCESS;
}
