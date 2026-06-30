#include "Task_Empty.h"

using namespace BT_Geometry;

namespace Action
{
    BT::NodeStatus Task_Empty::tick()
    {
        auto bb_opt = getInput<CPPBlackBoard*>("BB");
        if (!bb_opt) return BT::NodeStatus::FAILURE;
        CPPBlackBoard* BB = bb_opt.value();

        Vector3 tgtPos = BB->TargetLocaion_Cartesian;
        Vector3 myPos = BB->MyLocation_Cartesian;

        // --- 거리 계산 ---
        float D = static_cast<float>(BB->Distance);
        if (D < 1.0f) D = static_cast<float>((tgtPos - myPos).length());

        // --- 조건: 리드 허용 거리 & AA 제한 ---
        float AA = static_cast<float>(std::fabs(BB->MyAspectAngle_Degree));  // 목표 0°
        if (D < LEAD_D_MIN || D > LEAD_D_MAX || AA > AA_MAX)
        {
            // 조건 미충족 → 폴백으로 넘김
            // (시간 타이머는 아래에서 초기화)
            std::cout << "[Lead] skip: D=" << D << " (allow "
                << LEAD_D_MIN << "~" << LEAD_D_MAX << "), AA=" << AA << " deg\n";
            // 리드 타이머 리셋
            static float lead_timer = 0.0f;
            lead_timer = 0.0f;
            return BT::NodeStatus::FAILURE;
        }

        // --- 리드 유지 시간 관리(데코레이터 없이 내부 시간 제한) ---
        // BB->DeltaSecond 가 블랙보드에 존재한다고 가정(사용자 환경 정보 기반)
        static float lead_timer = 0.0f;
        float dt = 0.0f;
        if (BB->DeltaSecond > 0.0f) dt = static_cast<float>(BB->DeltaSecond);
        lead_timer += dt;
        if (lead_timer > LEAD_TIME_MAX)
        {
            std::cout << "[Lead] stop: time_limit " << lead_timer << "s > " << LEAD_TIME_MAX << "s\n";
            lead_timer = 0.0f;
            return BT::NodeStatus::FAILURE;
        }

        // --- 리드 타이밍 ---
        float Vm = static_cast<float>(BB->MySpeed_MS);
        if (Vm < 1.0f) Vm = 1.0f;
        float t_lead = clampf(D / Vm, T_MIN, T_MAX);

        // --- 타깃 속도 벡터 추정 ---
        Vector3 Vt = BB->PredictedTargetVelocity;
        float vt_len = static_cast<float>(Vt.length());
        if (vt_len < 0.1f)
        {
            Vector3 Ht = BB->TargetForwardVector;
            if (!normalize(Ht))
            {
                std::cout << "[Lead] skip: invalid target forward\n";
                lead_timer = 0.0f;
                return BT::NodeStatus::FAILURE;
            }
            Vt = Ht * BB->TargetSpeed_MS;
            vt_len = static_cast<float>(Vt.length());
            if (vt_len < 0.1f)
            {
                std::cout << "[Lead] skip: target too slow\n";
                lead_timer = 0.0f;
                return BT::NodeStatus::FAILURE;
            }
        }

        // --- 기본 리드 지점 ---
        Vector3 VP = tgtPos + Vt * t_lead;

        Vector3 Ht_dir = BB->TargetForwardVector;
        if (!normalize(Ht_dir)) {
            // forward 벡터가 비정상일 때는 Vt 방향으로 대체
            Ht_dir = Vt;
            if (!normalize(Ht_dir)) {
                // 이것도 실패면 오프셋 스킵
                Ht_dir = Vector3(0, 0, 0);
            }
        }
        VP = VP + Ht_dir * 50.0f;   // ← 상대 진행방향으로 50m 앞으로

        // --- 목표 유지 보정: AA → 0°, AO(LOS각) → 2° ---
        // 준비 벡터
        Vector3 Ht = BB->TargetForwardVector;  normalize(Ht);
        Vector3 Hm = BB->MyForwardVector;      normalize(Hm);
        Vector3 LOSv = tgtPos - myPos;         normalize(LOSv);

        // 거리 스케일(가까우면 보정 약, 멀면 보정 강)
        float Dscale = std::min(D, LEAD_D_MAX);

        // 1) AA 보정(→0°): 타깃 테일(-Ht) 방향으로 소량 보정
        VP = VP + (-Ht) * (K_AA * AA * 0.001f * Dscale);

        // 2) AO 보정(→2°): lateral_dir = Hm - LOS*(Hm·LOS)
        float ao_err = static_cast<float>(BB->MyAngleOff_Degree) - 2.0f; // 목표 2°
        Vector3 lateral = Hm - LOSv * static_cast<float>(Hm.dot(LOSv));
        if (lateral.length() > 1e-6) normalize(lateral);
        VP = VP + (-lateral) * (K_AO * ao_err * 0.001f * Dscale);

        // --- 출력 & 성공 ---
        BB->VP_Cartesian = VP;
        std::cout << "[Lead] OK  D=" << D
            << " Vm=" << BB->MySpeed_MS
            << " |Vt|=" << vt_len
            << " t_lead=" << t_lead
            << " AA=" << AA
            << " AO=" << BB->MyAngleOff_Degree
            << " t_used=" << lead_timer << "s\n";

        return BT::NodeStatus::SUCCESS;
    }
}
