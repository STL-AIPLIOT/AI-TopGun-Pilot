#pragma once
#include "../../behaviortree_cpp_v3/decorator_node.h"
#include "../BlackBoard/CPPBlackBoard.h"

namespace Action
{
    class DECO_CounterAttackCheck : public BT::DecoratorNode
    {
    public:
        DECO_CounterAttackCheck(const std::string& name, const BT::NodeConfiguration& config)
            : BT::DecoratorNode(name, config) {}

        static BT::PortsList providedPorts()
        {
            return { BT::InputPort<CPPBlackBoard*>("BB") };
        }

        BT::NodeStatus tick() override;
    };
}
