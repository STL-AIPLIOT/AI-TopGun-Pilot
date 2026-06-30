#include "PredictManeuver.h"
#include <cmath>

namespace Action
{
	PredictManeuver::PredictManeuver(const std::string& name, const NodeConfiguration& config)
		: SyncActionNode(name, config)
	{
	}

	PredictManeuver::~PredictManeuver() {}

	PortsList PredictManeuver::providedPorts()
	{
		return {
			InputPort<CPPBlackBoard*>("BB")
		};
	}

	NodeStatus PredictManeuver::tick()
	{
		Optional<CPPBlackBoard*> BB = getInput<CPPBlackBoard*>("BB");
		if (!BB || !(*BB))
			return NodeStatus::FAILURE;

		const Vector3& currentPos = (*BB)->TargetLocaion_Cartesian;
		const EulerAngle& currentRot = (*BB)->TargetRotation_EDegree;
		float currentYaw = currentRot.Yaw;

		if (prevPositions.size() >= historySize)
			prevPositions.pop_front();
		if (prevHeadings.size() >= historySize)
			prevHeadings.pop_front();

		prevPositions.push_back(currentPos);
		prevHeadings.push_back(currentYaw);

		if (prevHeadings.size() < historySize)
			return NodeStatus::SUCCESS;

		float sumDelta = 0.0f;
		for (size_t i = 1; i < prevHeadings.size(); ++i)
			sumDelta += prevHeadings[i] - prevHeadings[i - 1];

		float avgDelta = sumDelta / (historySize - 1);

		if (avgDelta > 1.5f)
			(*BB)->PredictedTurnDirection = "LEFT";
		else if (avgDelta < -1.5f)
			(*BB)->PredictedTurnDirection = "RIGHT";
		else
			(*BB)->PredictedTurnDirection = "STRAIGHT";

		return NodeStatus::SUCCESS;
	}
}
