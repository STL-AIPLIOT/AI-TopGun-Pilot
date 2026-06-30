#include "SetBFMMode_HABFM.h"
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace Action;

BT::NodeStatus SetBFMMode_HABFM::tick()
{
    auto bb_res = getInput<CPPBlackBoard*>("BB");
    if (!bb_res)
    {
        std::cerr << "[SetBFMMode_HABFM] BB nullptr\n";
        return BT::NodeStatus::FAILURE;
    }
    CPPBlackBoard* BB = bb_res.value();

    const bool sight = BB->EnemyInSight;
    const double aa = BB->MyAspectAngle_Degree;   // 0: 같은 방향, 180: 정반대(헤드온)
    const double D = BB->Distance;
    const int    ec = BB->EnergyCompareResult;    // >=0 권장

    // [개선] HABFM 진입 창: |AA-180| <= 40°, 800m <= D <= 2000m, 에너지 ≥ 0, 시야 必
    const bool aa_ok = (std::abs(aa - 180.0) <= 40.0);
    const bool dist_ok = (D >= 800.0 && D <= 2000.0);
    const bool e_ok = (ec >= 0);

    if (sight && aa_ok && dist_ok && e_ok)
    {
        BB->BFM = HABFM;
        std::cout << "[SetBFMMode_HABFM] Enter HABFM | AA=" << aa
            << ", D=" << D << ", E=" << ec << "\n";
        return BT::NodeStatus::SUCCESS;
    }

    std::cout << "[SetBFMMode_HABFM] Blocked | sight=" << sight
        << ", AA=" << aa << ", D=" << D << ", E=" << ec << "\n";
    return BT::NodeStatus::FAILURE;
}
