#pragma once
#include "../../behaviortree_cpp_v3/action_node.h"
#include "../../behaviortree_cpp_v3/bt_factory.h"
#include "../BlackBoard/CPPBlackBoard.h"
#include "../../../Geometry/Vector3.h"
#include "../Functions.h"
#include <algorithm>
#include <cmath>
#include <string>

namespace Action
{
    class Task_MinimizeAngleOff : public BT::SyncActionNode
    {
    public:
        Task_MinimizeAngleOff(const std::string& name, const BT::NodeConfiguration& config)
            : BT::SyncActionNode(name, config) {}

        static BT::PortsList providedPorts()
        {
            return {
                BT::InputPort<CPPBlackBoard*>("BB"),
                BT::InputPort<double>("LookAheadMin"),
                BT::InputPort<double>("LookAheadMax")
            };
        }

        BT::NodeStatus tick() override;
    };
}