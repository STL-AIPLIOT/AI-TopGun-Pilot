#include "Task_AntiOvershoot.h"
#include <algorithm>

namespace Action {  // ★ 추가

    static inline float clampf(float v, float lo, float hi) {
        return std::max(lo, std::min(hi, v));
    }

    BT::NodeStatus Task_AntiOvershoot::tick() {
        auto bb_res = getInput<CPPBlackBoard*>("BB");
        if (!bb_res) {
            std::cerr << "[Task_AntiOvershoot] BB nullptr\n";
            return BT::NodeStatus::FAILURE;
        }
        CPPBlackBoard* BB = bb_res.value();

        const float D = BB->Distance;
        const float dv = BB->MySpeed_MS - BB->TargetSpeed_MS;
        const bool trigger = (D < 600.0f) && (dv > 12.0f);           // [변경]

        if (trigger) {
            const float back = std::max(150.0f, std::min(450.0f, 150.0f + 10.0f * dv)); // [변경]
            BB->VP_Cartesian = BB->TargetLocaion_Cartesian - BB->TargetForwardVector * back;
            std::cout << "[Task_AntiOvershoot] triggered: D=" << D
                << ", dv=" << dv << ", back=" << back << "\n";
        }
        else {
            BB->VP_Cartesian = BB->TargetLocaion_Cartesian;
        }
        return BT::NodeStatus::SUCCESS;
    }

} // namespace Action
