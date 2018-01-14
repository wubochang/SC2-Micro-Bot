#pragma once
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

namespace sc2 {
	class MarineMicroBotBC : public Agent
	{
	public: 
		virtual void OnGameStart() final;
		virtual void OnStep() final;

	private:
		const Unit* GetNearestUnit(const Point2D& from, Unit::Alliance alliace, UnitTypeID target_type_ID);

		bool GetAveragePosition(Unit::Alliance alliace, UnitTypeID target_type_ID, Point2D& position);

		bool move_back_;

		Point2D backup_start_;
		Point2D backup_target_;

		bool no_enemy;
	};

	class BanelingBotBC :public Agent
	{
	public:
		virtual void OnStep() final;

	private:
		const Unit* GetNearestUnit(const Point2D& from, Unit::Alliance alliace, UnitTypeID target_type_ID);

	};
}
