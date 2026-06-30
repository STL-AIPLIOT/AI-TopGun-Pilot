#pragma once

#include "../../behaviortree_cpp_v3/action_node.h"
#include "../../behaviortree_cpp_v3/bt_factory.h"
#include "../../../Geometry/Vector3.h"
#include "../../../Geometry/EulerAngle.h"
#include "../../../Geometry/Quaternion.h"
#include "../Functions.h"
#include "../BlackBoard/CPPBlackBoard.h"

using namespace BT;

namespace Action
{
	class EnergyCompare : public SyncActionNode
	{
	public:
		EnergyCompare(const std::string& name, const NodeConfiguration& config);
		~EnergyCompare();

		static PortsList providedPorts();
		NodeStatus tick() override;
	};
}
