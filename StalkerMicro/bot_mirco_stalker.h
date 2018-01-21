#pragma once
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

namespace sc2
{
	class StalkerMicroBot : public Agent
	{
	public:
		virtual void OnGameStart() final;
		virtual void OnStep() final;

	private:
		const Unit* GetNearestUnit(const Point2D& from, Unit::Alliance alliace, UnitTypeID target_type_ID);

		bool GetAveragePosition(Unit::Alliance alliace, UnitTypeID target_type_ID, Point2D& position);

		const Unit* GetNextNearestUnit(const Point2D& from, const Point2D& curTarget, Unit::Alliance alliace);

		const Unit* cur_enemy;
		float atk;
		float range;
		float armor;
	};
}
