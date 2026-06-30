#pragma once

#include "../../behaviortree_cpp_v3/action_node.h"
#include "../../behaviortree_cpp_v3/bt_factory.h"
#include "../../../Geometry/Vector3.h"
#include "../../../Geometry/EulerAngle.h"
#include "../../../Geometry/Quaternion.h"
#include "../Functions.h"
#include "../BlackBoard/CPPBlackBoard.h"

#include <deque>
#include <string>

using namespace BT;

namespace Action
{
	class PredictManeuver : public SyncActionNode
	{
	public:
		PredictManeuver(const std::string& name, const NodeConfiguration& config);
		~PredictManeuver();

		static PortsList providedPorts();
		NodeStatus tick() override;

	private:
		std::deque<Vector3> prevPositions;
		std::deque<float> prevHeadings;
		const size_t historySize = 5;
	};
}
