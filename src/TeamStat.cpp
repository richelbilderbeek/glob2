/*
  Copyright (C) 2001, 2002, 2003 Stephane Magnenat & Luc-Olivier de Charrière
  for any question or comment contact us at nct@ysagoon.com or nuage@ysagoon.com

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "TeamStat.h"
#include "Team.h"
#include "Game.h"
#include "GlobalContainer.h"
#include <GraphicContext.h>
#include <Toolkit.h>
#include <StringTable.h>
#include <SupportFunctions.h>

TeamStats::TeamStats()
{
	statsIndex=0;
	smoothedIndex=0;
	endOfGameStatIndex=0;
	
	memset(stats, 0, sizeof(TeamStat)*STATS_SIZE);
	memset(smoothedStats, 0, sizeof(TeamSmoothedStat)*STATS_SMOOTH_SIZE);
	memset(endOfGameStats, 0, sizeof(EndOfGameStat)*END_OF_GAME_STATS_SIZE);
}

TeamStats::~TeamStats()
{
	
}

void TeamStats::step(Team *team)
{
	// handle end of game stat step
	if ((team->game->stepCounter & 0x1FF) == 0)
	{
		// we copy stats to end of game stat
		endOfGameStats[endOfGameStatIndex].value[EndOfGameStat::TYPE_UNITS] = stats[statsIndex].totalUnit;
		endOfGameStats[endOfGameStatIndex].value[EndOfGameStat::TYPE_BUILDINGS] = stats[statsIndex].totalBuilding;
		endOfGameStats[endOfGameStatIndex].value[EndOfGameStat::TYPE_PRESTIGE] = team->prestige;

		endOfGameStatIndex++;
		endOfGameStatIndex%=END_OF_GAME_STATS_SIZE;
	}
	
	// handle in game stat step
	TeamSmoothedStat &smoothedStat=smoothedStats[smoothedIndex];
	memset(&smoothedStat, 0, sizeof(TeamSmoothedStat));
	for (int i=0; i<1024; i++)
	{
		Unit *u=team->myUnits[i];
		if ((u)&&(u->medical==Unit::MED_FREE)&&(u->activity==Unit::ACT_RANDOM))
		{
			smoothedStat.isFree[(int)u->typeNum]++;
			smoothedStat.totalFree++;
		}
	}
	
	{
		std::list<Building *> foodable=team->foodable;
		for (std::list<Building *>::iterator bi=foodable.begin(); bi!=foodable.end(); ++bi)
			smoothedStat.totalNeeded+=(*bi)->maxUnitWorking-(*bi)->unitsWorking.size();
	}
	{
		std::list<Building *> fillable=team->fillable;
		for (std::list<Building *>::iterator bi=fillable.begin(); bi!=fillable.end(); ++bi)
			smoothedStat.totalNeeded+=(*bi)->maxUnitWorking-(*bi)->unitsWorking.size();
	}
	{
		std::list<Building *> zonable=team->zonable[WORKER];
		for (std::list<Building *>::iterator bi=zonable.begin(); bi!=zonable.end(); ++bi)
			smoothedStat.totalNeeded+=(*bi)->maxUnitWorking-(*bi)->unitsWorking.size();
	}
	
	smoothedIndex++;
	smoothedIndex%=STATS_SMOOTH_SIZE;
	if (smoothedIndex)
		return;
	
	TeamSmoothedStat maxStat;
	memset(&maxStat, 0, sizeof(TeamSmoothedStat));
	for (int i=0; i<STATS_SMOOTH_SIZE; i++)
	{
		TeamSmoothedStat &smoothedStat=smoothedStats[i];

		if (smoothedStat.totalFree>maxStat.totalFree)
			maxStat.totalFree=smoothedStat.totalFree;
		for (int j=0; j<NB_UNIT_TYPE; j++)
			if (smoothedStat.isFree[j]>maxStat.isFree[j])
				maxStat.isFree[j]=smoothedStat.isFree[j];
		if (smoothedStat.totalNeeded>maxStat.totalNeeded)
			maxStat.totalNeeded=smoothedStat.totalNeeded;
	}

	// We change current stats:
	statsIndex++;
	statsIndex%=STATS_SIZE;
	TeamStat &stat=stats[statsIndex];
	memset(&stat, 0, sizeof(TeamStat));
	for (int i=0; i<1024; i++)
	{
		Unit *u=team->myUnits[i];
		if (u)
		{
			stat.totalUnit++;
			stat.numberUnitPerType[(int)u->typeNum]++;

			if (u->isUnitHungry())
				if (u->hp<u->performance[HP])
					stat.needFoodCritical++;
				else
					stat.needFood++;
			else if (u->medical==Unit::MED_DAMAGED)
				stat.needHeal++;
			else
			{
				stat.needNothing++;
				if (u->activity==Unit::ACT_RANDOM)
				{
					stat.isFree[(int)u->typeNum]++;
					stat.totalFree++;
				}
			}
			for (int j=0; j<NB_ABILITY; j++)
			{
				if (u->performance[j])
					stat.upgradeState[j][u->level[j]]++;
			}
		}
	}

	for (int i=0; i<1024; i++)
	{
		if (team->myBuildings[i])
		{
			stat.totalBuilding++;
			stat.numberBuildingPerType[team->myBuildings[i]->type->type]++;
			int tabLevel=((team->myBuildings[i]->type->level)<<1)+1-team->myBuildings[i]->type->isBuildingSite;
			assert(tabLevel>=0);
			assert(tabLevel<=5);
			stat.numberBuildingPerTypePerLevel[team->myBuildings[i]->type->type][tabLevel]++;
		}
	}
	
	// We override unsmoothed stats:
	stat.totalFree=maxStat.totalFree;
	for (int j=0; j<NB_UNIT_TYPE; j++)
		stat.isFree[j]=maxStat.isFree[j];
	stat.totalNeeded=maxStat.totalNeeded;
}

void TeamStats::drawText(int pos)
{
	// local variable to speed up access
	GraphicContext *gfx=globalContainer->gfx;
	Font *font=globalContainer->littleFont;
	StringTable *strings=Toolkit::getStringTable();
	int textStartPosX=gfx->getW()-124;
	int textStartPosY=pos;
	
	TeamStat &newStats=stats[statsIndex];
	
	// general
	gfx->drawString(textStartPosX, textStartPosY, font, strings->getString("[Statistics]"));
	gfx->drawString(textStartPosX, textStartPosY+17, font, GAG::nsprintf("%d %s", newStats.totalUnit, strings->getString("[Units]")).c_str());
	if (newStats.totalUnit)
	{
		// worker
		int free=newStats.isFree[0]-newStats.totalNeeded;
		int seeking=newStats.totalNeeded;
		if (free<0)
		{
			free=0;
			seeking=newStats.isFree[0];
		}
		gfx->drawString(textStartPosX, textStartPosY+34, font, GAG::nsprintf("%d %s (%.0f %%)", newStats.numberUnitPerType[0], strings->getString("[workers]"), ((float)newStats.numberUnitPerType[0])*100.0f/((float)newStats.totalUnit)).c_str());
		gfx->drawString(textStartPosX+5, textStartPosY+46, font, GAG::nsprintf("%s %d %s", strings->getString("[of which]"), free, strings->getString("[free]")).c_str());
		gfx->drawString(textStartPosX+5, textStartPosY+56, font, GAG::nsprintf("%s %d %s", strings->getString("[and]"), seeking, strings->getString("[seeking a job]")).c_str());

		// explorer
		gfx->drawString(textStartPosX, textStartPosY+73, font, GAG::nsprintf("%d %s (%.0f %%)", newStats.numberUnitPerType[1], strings->getString("[explorers]"), ((float)newStats.numberUnitPerType[1])*100.0f/((float)newStats.totalUnit)).c_str());
		gfx->drawString(textStartPosX+5, textStartPosY+85, font, GAG::nsprintf("%s %d %s", strings->getString("[of which]"), newStats.isFree[1], strings->getString("[free]")).c_str());
		// warrior
		gfx->drawString(textStartPosX, textStartPosY+102, font, GAG::nsprintf("%d %s (%.0f %%)", newStats.numberUnitPerType[2], strings->getString("[warriors]"), ((float)newStats.numberUnitPerType[2])*100.0f/((float)newStats.totalUnit)).c_str());
		gfx->drawString(textStartPosX+5, textStartPosY+114, font, GAG::nsprintf("%s %d %s", strings->getString("[of which]"), newStats.isFree[2], strings->getString("[free]")).c_str());

		// living state
		gfx->drawString(textStartPosX, textStartPosY+131, font, GAG::nsprintf("%d %s (%.0f %%)", newStats.needNothing, strings->getString("[are ok]"), ((float)newStats.needNothing)*100.0f/((float)newStats.totalUnit)).c_str());
		gfx->drawString(textStartPosX, textStartPosY+143, font, GAG::nsprintf("%d %s (%.0f %%)", newStats.needFood, strings->getString("[are hungry]"), ((float)newStats.needFood)*100.0f/((float)newStats.totalUnit)).c_str());
		gfx->drawString(textStartPosX, textStartPosY+155, font, GAG::nsprintf("%d %s (%.0f %%)", newStats.needFoodCritical, strings->getString("[are dying hungry]"), ((float)newStats.needFoodCritical)*100.0f/((float)newStats.totalUnit)).c_str());
		gfx->drawString(textStartPosX, textStartPosY+167, font, GAG::nsprintf("%d %s (%.0f %%)", newStats.needHeal, strings->getString("[are wonded]"), ((float)newStats.needHeal)*100.0f/((float)newStats.totalUnit)).c_str());

		// upgrade state
		gfx->drawString(textStartPosX, textStartPosY+184, globalContainer->littleFont, GAG::nsprintf("%s %d/%d/%d/%d", strings->getString("[Walk]"), newStats.upgradeState[WALK][0], newStats.upgradeState[WALK][1], newStats.upgradeState[WALK][2], newStats.upgradeState[WALK][3]).c_str());
		gfx->drawString(textStartPosX, textStartPosY+196, globalContainer->littleFont, GAG::nsprintf("%s %d/%d/%d/%d", strings->getString("[Swim]"), newStats.upgradeState[SWIM][0], newStats.upgradeState[SWIM][1], newStats.upgradeState[SWIM][2], newStats.upgradeState[SWIM][3]).c_str());
		gfx->drawString(textStartPosX, textStartPosY+208, globalContainer->littleFont, GAG::nsprintf("%s %d/%d/%d/%d", strings->getString("[Build]"), newStats.upgradeState[BUILD][0], newStats.upgradeState[BUILD][1], newStats.upgradeState[BUILD][2], newStats.upgradeState[BUILD][3]).c_str());
		gfx->drawString(textStartPosX, textStartPosY+220, globalContainer->littleFont, GAG::nsprintf("%s %d/%d/%d/%d", strings->getString("[Harvest]"), newStats.upgradeState[HARVEST][0], newStats.upgradeState[HARVEST][1], newStats.upgradeState[HARVEST][2], newStats.upgradeState[HARVEST][3]).c_str());
		gfx->drawString(textStartPosX, textStartPosY+232, globalContainer->littleFont, GAG::nsprintf("%s %d/%d/%d/%d", strings->getString("[At. speed]"), newStats.upgradeState[ATTACK_SPEED][0], newStats.upgradeState[ATTACK_SPEED][1], newStats.upgradeState[ATTACK_SPEED][2], newStats.upgradeState[ATTACK_SPEED][3]).c_str());
		gfx->drawString(textStartPosX, textStartPosY+244, globalContainer->littleFont, GAG::nsprintf("%s %d/%d/%d/%d", strings->getString("[At. strength]"), newStats.upgradeState[ATTACK_STRENGTH][0], newStats.upgradeState[ATTACK_STRENGTH][1], newStats.upgradeState[ATTACK_STRENGTH][2], newStats.upgradeState[ATTACK_STRENGTH][3]).c_str());
	}
}

void TeamStats::drawStat(int pos)
{
	assert(STATS_SIZE==128);// We have graphical constraints
	
	// local variable to speed up access
	GraphicContext *gfx=globalContainer->gfx;
	Font *font=globalContainer->littleFont;
	StringTable *strings=Toolkit::getStringTable();
	int textStartPos=gfx->getW()-124;
	int startPoxY=pos;
	
	// compute total units
	/*int maxUnit=0;
	int i;
	for (i=0; i<STATS_SIZE; i++)
	{
		if (stats[i].totalUnit>maxUnit)
			maxUnit=stats[i].totalUnit;
	}*/
	int maxWorker=0;
	for (int i=0; i<STATS_SIZE; i++)
		if (stats[i].numberUnitPerType[0]>maxWorker)
			maxWorker=stats[i].numberUnitPerType[0];

	if (maxWorker==0)
		return;

	// captions
	{
		gfx->drawString(textStartPos, startPoxY, font, strings->getString("[Statistics]"));

		int dec=0;
		const char *Total=strings->getString("[Total]");
		const char *free=strings->getString("[free]");
		const char *seeking=strings->getString("[seeking]");
		const char *slash="/";
		int sLen=font->getStringWidth(slash);

		font->pushColor(34, 66, 163);
		gfx->drawString(textStartPos, startPoxY+20, font, Total);
		font->popColor();

		dec+=font->getStringWidth(Total);
		gfx->drawString(textStartPos+dec, startPoxY+20, font, "/");
		dec+=sLen;

		font->pushColor(22, 229, 40);
		gfx->drawString(textStartPos+dec, startPoxY+20, font, free);
		font->popColor();

		dec+=font->getStringWidth(free);
		gfx->drawString(textStartPos+dec, startPoxY+20, font, "/");
		dec+=sLen;

		font->pushColor(150, 50, 50);
		gfx->drawString(textStartPos+dec, startPoxY+20, font, seeking);
		font->popColor();

		dec=0;
		const char *Free=strings->getString("[Free]");
		const char *hungry=strings->getString("[hungry]");
		const char *starving=strings->getString("[starving]");
		const char *wounded=strings->getString("[wounded]");

		font->pushColor(22, 229, 40);
		gfx->drawString(textStartPos, startPoxY+104, font, Free);
		font->popColor();

		dec+=font->getStringWidth(Free);
		gfx->drawString(textStartPos+dec, startPoxY+104, font, "/");
		dec+=sLen;

		font->pushColor(224, 210, 17);
		gfx->drawString(textStartPos+dec, startPoxY+104, font, hungry);
		font->popColor();

		dec+=font->getStringWidth(hungry);
		gfx->drawString(textStartPos+dec, startPoxY+104, font, "/");
		dec+=sLen;

		font->pushColor(249, 167, 14);
		gfx->drawString(textStartPos+dec, startPoxY+104, font, starving);
		font->popColor();

		dec+=font->getStringWidth(starving);
		gfx->drawString(textStartPos+dec, startPoxY+104, font, "/");
		dec+=sLen;

		font->pushColor(250, 25, 25);
		gfx->drawString(textStartPos+dec, startPoxY+104, font, wounded);
		font->popColor();
	}

	// graph
	for (int i=0; i<128; i++)
	{
		int index=(statsIndex+i+1)&0x7F;
		
		int free=stats[index].isFree[WORKER]-stats[index].totalNeeded;
		int seeking=stats[index].totalNeeded;
		if (free<0)
		{
			free=0;
			seeking=stats[index].isFree[WORKER];
		}
		
		int nbFree=(free*64)/maxWorker;
		int nbSeeking=(seeking*64)/maxWorker;
		int nbTotal=(stats[index].numberUnitPerType[WORKER]*64)/maxWorker;
		
		globalContainer->gfx->drawVertLine(globalContainer->gfx->getW()-128+i, startPoxY+ 36 +64-nbTotal, nbTotal-nbFree-nbSeeking, 34, 66, 163);
		globalContainer->gfx->drawVertLine(globalContainer->gfx->getW()-128+i, startPoxY+ 36 +64-nbFree-nbSeeking, nbFree, 22, 229, 40);
		globalContainer->gfx->drawVertLine(globalContainer->gfx->getW()-128+i, startPoxY+ 36 +64-nbSeeking, nbSeeking, 150, 50, 50);

		int nbOk, nbNeedFood, nbNeedFoodCritical, nbNeedHeal;
		if (stats[index].totalUnit)
		{
			// to avoid some roundoff errors
			if (stats[index].needNothing>0)
			{
				nbNeedHeal=(stats[index].needHeal*64)/stats[index].totalUnit;
				nbNeedFood=(stats[index].needFood*64)/stats[index].totalUnit;
				nbNeedFoodCritical=(stats[index].needFoodCritical*64)/stats[index].totalUnit;
				nbOk=64-(nbNeedHeal+nbNeedFood+nbNeedFoodCritical);
			}
			else if (stats[index].needFood>0)
			{
				nbNeedHeal=(stats[index].needHeal*64)/stats[index].totalUnit;
				nbNeedFood=(stats[index].needFood*64)/stats[index].totalUnit;
				nbNeedFoodCritical=(stats[index].needFoodCritical*64)/stats[index].totalUnit;
				nbNeedFood=64-(nbNeedHeal+nbNeedFoodCritical);
				nbOk=0;
			}
			else if (stats[index].needFoodCritical>0)
			{
				nbNeedHeal=(stats[index].needHeal*64)/stats[index].totalUnit;
				nbNeedFood=0;
				nbNeedFoodCritical=64-nbNeedHeal;
				nbOk=0;
			}
			else
			{
				nbOk=nbNeedFood=nbNeedFoodCritical=0;
				nbNeedHeal=64;
			}
		}
		else
		{
			nbOk=nbNeedFood=nbNeedHeal=nbNeedFoodCritical=0;
		}
		globalContainer->gfx->drawVertLine(globalContainer->gfx->getW()-128+i, startPoxY+ 120 +64-nbNeedHeal-nbNeedFoodCritical-nbNeedFood-nbOk, nbOk, 22, 229, 40);
		globalContainer->gfx->drawVertLine(globalContainer->gfx->getW()-128+i, startPoxY+ 120 +64-nbNeedHeal-nbNeedFoodCritical-nbNeedFood, nbNeedFood, 224, 210, 17);
		globalContainer->gfx->drawVertLine(globalContainer->gfx->getW()-128+i, startPoxY+ 120 +64-nbNeedHeal-nbNeedFoodCritical, nbNeedFoodCritical, 249, 167, 14);
		globalContainer->gfx->drawVertLine(globalContainer->gfx->getW()-128+i, startPoxY+ 120 +64-nbNeedHeal, nbNeedHeal, 250, 25, 25);
	}
}

int TeamStats::getFreeUnits(int type)
{
	return (stats[statsIndex].isFree[type]);
}

int TeamStats::getTotalUnits(int type)
{
	return (stats[statsIndex].numberUnitPerType[type]);
}

int TeamStats::getUnitsNeeded()
{
	return (stats[statsIndex].totalNeeded);
}
