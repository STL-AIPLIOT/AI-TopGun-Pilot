#pragma once
#include "../../behaviortree_cpp_v3/action_node.h"
#include "../BlackBoard/CPPBlackBoard.h"
#include <iostream>
#include <random>

namespace Action
{
    class Task_EvasiveRollOrScissors : public BT::SyncActionNode
    {
    public:
        Task_EvasiveRollOrScissors(const std::string& name, const BT::NodeConfiguration& config)
            : BT::SyncActionNode(name, config) {}

        static BT::PortsList providedPorts()
        {
            return { BT::InputPort<CPPBlackBoard*>("BB") };
        }

        BT::NodeStatus tick() override;
    };
}
