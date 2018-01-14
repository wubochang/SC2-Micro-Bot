#include "bot_micro_bc.h"

#include <iostream>
#include <algorithm>

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

namespace sc2{

	void MarineMicroBotBC::OnGameStart()
	{
		move_back_ = false;
		no_enemy = false;
	}

	void MarineMicroBotBC::OnStep()
	{
		const ObservationInterface* obs = Observation();;
		ActionInterface* action = Actions();

		Units units = obs->GetUnits(Unit::Alliance::Self);
		for (const auto& u : units)
		{
			switch (static_cast<UNIT_TYPEID>(u->unit_type))
			{
			case UNIT_TYPEID::TERRAN_MARINE:
			{

				auto bb = GetNearestUnit(u->pos, Unit::Alliance::Enemy, UNIT_TYPEID::ZERG_BANELING);

				if (!bb)
				{
					if (!no_enemy)
					{
						std::cout << "Moving marine together" << std::endl;
						Point2D mp;
						GetAveragePosition(Unit::Alliance::Self, UNIT_TYPEID::TERRAN_MARINE, mp);
						action->UnitCommand(u, ABILITY_ID::MOVE, mp);
						no_enemy = true;
						return;
					}
				}
				else
				{
					auto is_stimpack = [](const BuffID& buff)
					{
						return buff == BUFF_ID::STIMPACK;
					};
					if (std::find_if(u->buffs.begin(), u->buffs.end(), is_stimpack) == u->buffs.end())
					{
						action->UnitCommand(u, ABILITY_ID::EFFECT_STIM_MARINE);
					}

					auto dist = Distance2D(u->pos, bb->pos);
					if ((u->weapon_cooldown == 0 || dist >= 5.0f ) && dist > 3.5f)
					{
						// can attack
						move_back_ = false;
					}
					else
					{
						//if (!move_back_)
						{
							Vector2D diff;
							Point2D bp = bb->pos;
							diff = u->pos - bp;
							Normalize2D(diff);

							Vector2D split;
							auto nm = GetNearestUnit(u->pos, Unit::Alliance::Self, UNIT_TYPEID::TERRAN_MARINE);
							split = u->pos - nm->pos;
							Normalize2D(split);

							backup_start_ = u->pos;
							backup_target_ = u->pos + (diff) * 10.0f;
							auto backup_target_with_split_ = backup_target_ + split * 10.0f;
							if (Distance2D(backup_target_, bp) < Distance2D(backup_target_with_split_, bp))
							{
								std::cout << "Spliting!" << std::endl;
								backup_target_ = backup_target_with_split_;
							}

							move_back_ = true;
						}
					}
				}

				if (!move_back_) {
					action->UnitCommand(u, ABILITY_ID::ATTACK, bb);
				}
				else {
					if (Distance2D(u->pos, backup_target_) < 1.5f) {
						move_back_ = false;
					}

					action->UnitCommand(u, ABILITY_ID::SMART, backup_target_);
				}
				break;
			}
			default:
				break;
			}

		}

	}

	bool MarineMicroBotBC::GetAveragePosition(Unit::Alliance alliace, UnitTypeID unit_type, Point2D& position)
	{
		const ObservationInterface* observation = Observation();
		Units units = observation->GetUnits(alliace);

		if (units.empty()) {
			return false;
		}

		position = Point2D(0.0f, 0.0f);
		unsigned int count = 0;

		for (const auto& u : units) {
			if (u->unit_type == unit_type) {
				position += u->pos;
				++count;
			}
		}

		position /= (float)count;

		return true;
	}

	const Unit* MarineMicroBotBC::GetNearestUnit(const Point2D& from, Unit::Alliance alliace, UnitTypeID target_type_ID)
	{
		const ObservationInterface* obs = Observation();
		Units units = obs->GetUnits(alliace);

		const Unit* res = NULL;

		if (units.empty())
		{
			return res;
		}

		float distance = std::numeric_limits<float>::max();

		for (const auto& u : units)
		{
			if (u->unit_type == target_type_ID)
			{
				float d = DistanceSquared2D(u->pos, from);
				if (d > 0 && d < distance)
				{
					distance = d;
					res = u;
				}
			}
		}

		return res;
	}


	void BanelingBotBC::OnStep()
	{
		const ObservationInterface* obs = Observation();;
		ActionInterface* action = Actions();

		Units units = obs->GetUnits(Unit::Alliance::Self);
		for (const auto& u : units)
		{
			switch (static_cast<UNIT_TYPEID>(u->unit_type))
			{
			case UNIT_TYPEID::ZERG_BANELING:
			{
				auto nm = GetNearestUnit(u->pos, Unit::Alliance::Enemy, UNIT_TYPEID::TERRAN_MARINE);
				if (!nm) return;
				std::cout << "Dubao gun qiangbing" << std::endl;
				action->UnitCommand(u, ABILITY_ID::ATTACK, nm);
				break;
			}
			default:
				break;
			}
		}
	}

	const Unit* BanelingBotBC::GetNearestUnit(const Point2D& from, Unit::Alliance alliace, UnitTypeID target_type_ID)
	{
		const ObservationInterface* obs = Observation();
		Units units = obs->GetUnits(alliace);

		const Unit* res = NULL;

		if (units.empty())
		{
			return res;
		}

		float distance = std::numeric_limits<float>::max();

		for (const auto& u : units)
		{
			if (u->unit_type == target_type_ID)
			{
				float d = DistanceSquared2D(u->pos, from);
				if (d > 0 && d < distance)
				{
					distance = d;
					res = u;
				}
			}
		}

		return res;
	}


}