#include "Task_EvasiveRollOrScissors.h"
#include <algorithm>
#include <chrono>

using namespace Action;

static inline float clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

BT::NodeStatus Task_EvasiveRollOrScissors::tick()
{
    auto bb_res = getInput<CPPBlackBoard*>("BB");
    if (!bb_res)
    {
        std::cerr << "[Task_EvasiveRollOrScissors] BB 포인터 가져오기 실패\n";
        return BT::NodeStatus::FAILURE;
    }

    CPPBlackBoard* BB = bb_res.value();

    // 상황 변수 (이미 BB에 존재한다고 가정)
    const float D = BB->Distance;                 // m
    const float dv = BB->TargetSpeed_MS - BB->MySpeed_MS; // 적-아군 속도차(+)면 적이 빠름
    const int   ecmp = BB->EnergyCompareResult;      // >0 우세, 0 동등, <0 열세

    // ───────────────────────── 방향 선택 정책 ─────────────────────────
    // 1) 초근접 방어 (D < 250): 급격한 횡기동 (좌/우) 우선
    // 2) 에너지 열세(ecmp<0) & 적이 빠름(dv>0): 롤/스크리저(수평 위주)로 과속 유도
    // 3) 에너지 우세(ecmp>0): 롤+상승(롤링 클라임)로 고각 확보
    // 4) 그 외: 좌/우 랜덤 이탈
    enum class Dir { Right, Left, Up };
    Dir choice;

    if (D < 250.0f) {
        choice = ((std::rand() & 1) ? Dir::Right : Dir::Left); // [변경] 근접시 수평 회피 고정
    }
    else if (ecmp < 0 && dv > 0) {
        choice = ((std::rand() & 1) ? Dir::Right : Dir::Left); // 열세+적이 빠름 → 수평 회피
    }
    else if (ecmp > 0) {
        choice = Dir::Up; // 우세 → 상승 성향
    }
    else {
        // 안전한 난수 엔진 (씨드: 고해상도 시간)
        static thread_local std::mt19937 rng(
            static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
        );
        std::uniform_int_distribution<int> uni(0, 2);
        int rnd = uni(rng);
        choice = (rnd == 0 ? Dir::Right : (rnd == 1 ? Dir::Left : Dir::Up));
    }

    // ───────────────────────── 오프셋 크기 정책 ─────────────────────────
    // D, dv에 따라 가변. 가까울수록 작게, 멀수록 크게.
    float side = clampf(200.0f + 0.6f * D, 250.0f, 700.0f); // 좌/우
    float up = clampf(150.0f + 0.3f * D, 200.0f, 500.0f); // 상승

    // 속도차가 크면(적이 빠름) 더 크게 이격
    if (dv > 10.0f) {
        side = clampf(side * 1.2f, 250.0f, 800.0f);
    }

    Vector3 evasive_offset;
    switch (choice) {
    case Dir::Right: evasive_offset = BB->MyRightVector * side; break;
    case Dir::Left:  evasive_offset = BB->MyRightVector * (-side); break;
    case Dir::Up:    evasive_offset = BB->MyUpVector * up;   break;
    }

    // 최종 가상 목표점
    BB->VP_Cartesian = BB->MyLocation_Cartesian + evasive_offset;

    std::cout << "[Task_EvasiveRollOrScissors] 회피: "
        << (choice == Dir::Right ? "Right" : choice == Dir::Left ? "Left" : "Up")
        << " | D=" << D << ", dv=" << dv
        << " | side=" << side << ", up=" << up << "\n";

    return BT::NodeStatus::SUCCESS;
}
