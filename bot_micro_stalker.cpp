#include "bot_mirco_stalker.h"

#include <iostream>
#include <algorithm>

#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"

namespace sc2 
{
	void StalkerMicroBot::OnGameStart()
	{
		const ObservationInterface* obs = Observation();
		auto unit_data = obs->GetUnitTypeData();
		UnitTypeData stalker_data = unit_data.at(static_cast<uint32_t>(UNIT_TYPEID::PROTOSS_STALKER));
		auto sw = stalker_data.weapons;
		for (auto w : sw)
		{
			if (w.type == Weapon::TargetType::Ground)
			{
				atk = w.damage_;
				range = w.range;
				armor = stalker_data.armor;
				break;
			}
		}
		/*
		auto is_ground = [](const Weapon::TargetType& ti)
		{
			return ti == Weapon::TargetType::Ground;
		};
		auto p = std::find(sw.begin(), sw.end(), is_ground);
		if (p != sw.end())
		{
			atk = p->damage_;
		}
		else
		{
			atk = -1;
		}
		*/
	}

	void StalkerMicroBot::OnStep()
	{
		const ObservationInterface* obs = Observation();;
		ActionInterface* action = Actions();

		Point2D sp;
		GetAveragePosition(Unit::Alliance::Self, UNIT_TYPEID::PROTOSS_STALKER, sp);

		Units units = obs->GetUnits(Unit::Alliance::Self);
		int cnt = 0;
		auto es =  GetNearestUnit(sp, Unit::Alliance::Enemy, UNIT_TYPEID::PROTOSS_STALKER);
		if (!es)
			return;
		float es_health = es->health + es->shield;

		bool moveFowardFlag = false;

		for (const auto& u : units)
		{
			switch (static_cast<UNIT_TYPEID>(u->unit_type))
			{
			case UNIT_TYPEID::PROTOSS_STALKER:
			{
				Vector2D diff = sp - es->pos;
				Normalize2D(diff);
				if ((u->health + u->shield) / u->health_max < 0.5 ) 
				{
					action->UnitCommand(u, ABILITY_ID::EFFECT_BLINK_STALKER, sp + diff * 1.5);
				}
				else
				{
					if (es_health > 0)
					{
						if (u->weapon_cooldown == 0)
						{
							action->UnitCommand(u, ABILITY_ID::ATTACK, es);
							if (range >= Distance2D(es->pos, u->pos))
							{
								es_health -= atk - armor;
							}
						}
					}
					else
					{
						es = GetNextNearestUnit(sp, es->pos, Unit::Alliance::Enemy);
						if (!es)
							return;
						es_health = es->health;
					}
				}


				break;
			}
			default:
				break;
			}
		}
	}

	const Unit* StalkerMicroBot::GetNearestUnit(const Point2D& from, Unit::Alliance alliace, UnitTypeID target_type_ID)
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

	const Unit* StalkerMicroBot::GetNextNearestUnit(const Point2D& from, const Point2D& curTarget, Unit::Alliance alliace)
	{
		const ObservationInterface* obs = Observation();
		Units units = obs->GetUnits(alliace);

		const Unit* res = NULL;

		if (units.empty())
		{
			return res;
		}

		float distance = std::numeric_limits<float>::max();

		float curDistance = Distance2D(curTarget, from);

		for (const auto& u : units)
		{
			float d = DistanceSquared2D(u->pos, from);
			if (d > 0 && d < distance && d > curDistance)
			{
				distance = d;
				res = u;
			}
		}

		return res;
	}

	bool StalkerMicroBot::GetAveragePosition(Unit::Alliance alliace, UnitTypeID unit_type, Point2D& position)
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

}