#include "Task_LeadPursuit.h"
#include <algorithm>

namespace Action {  // ★ 추가

    static inline float clampf(float v, float lo, float hi) {
        return std::max(lo, std::min(hi, v));
    }

    BT::NodeStatus Task_LeadPursuit::tick() {
        auto bb_res = getInput<CPPBlackBoard*>("BB");
        if (!bb_res) {
            std::cerr << "[Task_LeadPursuit] BB nullptr\n";
            return BT::NodeStatus::FAILURE;
        }
        CPPBlackBoard* BB = bb_res.value();

        const float D = BB->Distance;
        if (D < 250.0f) { // [변경]
            std::cout << "[Task_LeadPursuit] too close (D=" << D << "), FAIL\n";
            return BT::NodeStatus::FAILURE; // [변경]
        }

        const float tgt_spd = std::max(1.0f, BB->TargetSpeed_MS);
        const float lead_time = std::max(0.3f, std::min(2.0f, D / tgt_spd)); // [변경]

        BB->VP_Cartesian = BB->TargetLocaion_Cartesian + BB->PredictedTargetVelocity * lead_time;

        std::cout << "[Task_LeadPursuit] lead_time=" << lead_time
            << " VP=(" << BB->VP_Cartesian.X << "," << BB->VP_Cartesian.Y << "," << BB->VP_Cartesian.Z << ")\n";
        return BT::NodeStatus::SUCCESS;
    }

} // namespace Action
