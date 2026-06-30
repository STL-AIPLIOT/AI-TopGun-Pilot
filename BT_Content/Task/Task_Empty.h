#pragma once

#include "../../behaviortree_cpp_v3/action_node.h"
#include "../../behaviortree_cpp_v3/bt_factory.h"
#include "../BlackBoard/CPPBlackBoard.h"

#include "../../../Geometry/Vector3.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace Action
{
    class Task_Empty : public BT::SyncActionNode
    {
    public:
        Task_Empty(const std::string& name, const BT::NodeConfiguration& config)
            : BT::SyncActionNode(name, config) {}

        static BT::PortsList providedPorts()
        {
            return { BT::InputPort<CPPBlackBoard*>("BB") };
        }

        BT::NodeStatus tick() override;

    private:
        // 리드 타이밍(초)
        static constexpr float T_MIN = 0.6f;
        static constexpr float T_MAX = 2.0f;

        // 리드 허용 거리(ft → m). 멀리서만 짧게 접근 가속
        static constexpr float FT2M = 0.3048f;
        static constexpr float LEAD_FT_MIN = 3000.0f;   // 1500 ft
        static constexpr float LEAD_FT_MAX = 15000.0f;   // 3000 ft
        static constexpr float LEAD_D_MIN = LEAD_FT_MIN * FT2M;
        static constexpr float LEAD_D_MAX = LEAD_FT_MAX * FT2M;

        // 리드 유지 시간 제한(과용 방지)
        static constexpr float LEAD_TIME_MAX = 2.5f;    // s

        // AA 제한(너무 크면 위험)
        static constexpr float AA_MAX = 45.0f;         // deg

        // 목표 유지 보정 게인 (deg → m 스케일 포함, 거리 비례 사용)
        static constexpr float K_AA = 4.0f;          // AA → 0°
        static constexpr float K_AO = 2.0f;          // AO → 2°

        static inline float clampf(float v, float lo, float hi) { return std::max(lo, std::min(v, hi)); }

        static inline bool normalize(BT_Geometry::Vector3& v)
        {
            const double n = v.length();
            if (n < 1e-6) return false;
            v = v / n;
            return true;
        }
    };
}
