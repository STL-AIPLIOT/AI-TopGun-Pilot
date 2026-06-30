#pragma once
#include "../../behaviortree_cpp_v3/action_node.h"
#include "../../behaviortree_cpp_v3/bt_factory.h"
#include "../../../Geometry/Vector3.h"
#include "../Functions.h"
#include "../BlackBoard/CPPBlackBoard.h"
using namespace BT;

namespace Action {
    class Task_ClimbToSafeAltitude : public SyncActionNode {
    public:
        Task_ClimbToSafeAltitude(const std::string& name, const NodeConfiguration& config);
        ~Task_ClimbToSafeAltitude();
        static PortsList providedPorts();
        NodeStatus tick() override;
    };
}
