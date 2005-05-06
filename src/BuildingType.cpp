/*
  Copyright (C) 2001-2004 Stephane Magnenat & Luc-Olivier de Charrière
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

#include "BuildingType.h"
#include "GlobalContainer.h"
#include <Toolkit.h>
#include <assert.h>

BuildingType::BuildingType()
{
	gameSpritePtr = NULL;
	miniSpritePtr = NULL;
}

void BuildingType::loadFromConfigFile(const ConfigBlock *configBlock)
{
	configBlock->load(type, "type");

	configBlock->load(gameSprite, "gameSprite");
	configBlock->load(gameSpriteImage, "gameSpriteImage");
	configBlock->load(gameSpriteCount, "gameSpriteCount");
	configBlock->load(miniSprite, "miniSprite");
	configBlock->load(miniSpriteImage, "miniSpriteImage");
	
	configBlock->load(hueImage,"hueImage");
	configBlock->load(flagImage,"flagImage");
	configBlock->load(crossConnectMultiImage,"crossConnectMultiImage");
	
	assert(NB_ABILITY == 16);
	configBlock->load(upgrade[0], "upgradeStopWalk");
	configBlock->load(upgrade[1], "upgradeStopSwim");
	configBlock->load(upgrade[2], "upgradeStopFly");
	configBlock->load(upgrade[3], "upgradeWalk");
	configBlock->load(upgrade[4], "upgradeSwim");
	configBlock->load(upgrade[5], "upgradeFly");
	configBlock->load(upgrade[6], "upgradeBuild");
	configBlock->load(upgrade[7], "upgradeHarvest");
	configBlock->load(upgrade[8], "upgradeAttackSpeed");
	configBlock->load(upgrade[9], "upgradeAttackStrength");
	configBlock->load(upgrade[10], "upgradeMagicAttack");
	configBlock->load(upgrade[11], "upgradeMagicCreateWood");
	configBlock->load(upgrade[12], "upgradeMagicCreateCorn");
	configBlock->load(upgrade[13], "upgradeMagicCreateAlga");
	configBlock->load(upgrade[14], "upgradeArmor");
	configBlock->load(upgrade[15], "upgradeHP");
	configBlock->load(upgradeTime[0], "upgradeTimeStopWalk");
	configBlock->load(upgradeTime[1], "upgradeTimeStopSwim");
	configBlock->load(upgradeTime[2], "upgradeTimeStopFly");
	configBlock->load(upgradeTime[3], "upgradeTimeWalk");
	configBlock->load(upgradeTime[4], "upgradeTimeSwim");
	configBlock->load(upgradeTime[5], "upgradeTimeFly");
	configBlock->load(upgradeTime[6], "upgradeTimeBuild");
	configBlock->load(upgradeTime[7], "upgradeTimeHarvest");
	configBlock->load(upgradeTime[8], "upgradeTimeAttackSpeed");
	configBlock->load(upgradeTime[9], "upgradeTimeAttackStrength");
	configBlock->load(upgradeTime[10], "upgradeTimeMagicAttack");
	configBlock->load(upgradeTime[11], "upgradeTimeMagicCreateWood");
	configBlock->load(upgradeTime[12], "upgradeTimeMagicCreateCorn");
	configBlock->load(upgradeTime[13], "upgradeTimeMagicCreateAlga");
	configBlock->load(upgradeTime[14], "upgradeTimeArmor");
	configBlock->load(upgradeTime[15], "upgradeTimeHP");
	
	configBlock->load(foodable, "foodable");
	configBlock->load(fillable, "fillable");
	
	assert(NB_UNIT_TYPE == 3);
	configBlock->load(zonable[0], "zonableWorker");
	configBlock->load(zonable[1], "zonableExplorer");
	configBlock->load(zonable[2], "zonableWarrior");
	configBlock->load(zonableForbidden, "zonableForbidden");

	configBlock->load(canFeedUnit, "canFeedUnit");
	configBlock->load(timeToFeedUnit, "timeToFeedUnit");
	configBlock->load(canHealUnit, "canHealUnit");
	configBlock->load(timeToHealUnit, "timeToHealUnit");
	configBlock->load(insideSpeed, "insideSpeed");
	configBlock->load(canExchange, "canExchange");

	configBlock->load(width, "width");
	configBlock->load(height, "height");
	configBlock->load(decLeft, "decLeft");
	configBlock->load(decTop, "decTop");
	configBlock->load(isVirtual, "isVirtual");
	configBlock->load(isCloacked, "isCloacked");
	configBlock->load(shootingRange, "shootingRange");
	configBlock->load(shootDamage, "shootDamage");
	configBlock->load(shootSpeed, "shootSpeed");
	configBlock->load(shootRythme, "shootRythme");
	configBlock->load(maxBullets, "maxBullets");
	configBlock->load(multiplierStoneToBullets, "multiplierStoneToBullets");

	configBlock->load(unitProductionTime, "unitProductionTime");
	configBlock->load(ressourceForOneUnit, "ressourceForOneUnit");
	
	assert(MAX_NB_RESSOURCES == 15);
	configBlock->load(maxRessource[0], "maxWood");
	configBlock->load(maxRessource[1], "maxCorn");
	configBlock->load(maxRessource[2], "maxPapyrus");
	configBlock->load(maxRessource[3], "maxStone");
	configBlock->load(maxRessource[4], "maxAlgue");
	configBlock->load(maxRessource[5], "maxFruit0");
	configBlock->load(maxRessource[6], "maxFruit1");
	configBlock->load(maxRessource[7], "maxFruit2");
	configBlock->load(maxRessource[8], "maxFruit3");
	configBlock->load(maxRessource[9], "maxFruit4");
	configBlock->load(maxRessource[10], "maxFruit5");
	configBlock->load(maxRessource[11], "maxFruit6");
	configBlock->load(maxRessource[12], "maxFruit7");
	configBlock->load(maxRessource[13], "maxFruit8");
	configBlock->load(maxRessource[14], "maxFruit9");
	configBlock->load(multiplierRessource[0], "multiplierWood");
	configBlock->load(multiplierRessource[1], "multiplierCorn");
	configBlock->load(multiplierRessource[2], "multiplierPapyrus");
	configBlock->load(multiplierRessource[3], "multiplierStone");
	configBlock->load(multiplierRessource[4], "multiplierAlgue");
	configBlock->load(multiplierRessource[5], "multiplierFruit0");
	configBlock->load(multiplierRessource[6], "multiplierFruit1");
	configBlock->load(multiplierRessource[7], "multiplierFruit2");
	configBlock->load(multiplierRessource[8], "multiplierFruit3");
	configBlock->load(multiplierRessource[9], "multiplierFruit4");
	configBlock->load(multiplierRessource[10], "multiplierFruit5");
	configBlock->load(multiplierRessource[11], "multiplierFruit6");
	configBlock->load(multiplierRessource[12], "multiplierFruit7");
	configBlock->load(multiplierRessource[13], "multiplierFruit8");
	configBlock->load(multiplierRessource[14], "multiplierFruit9");

	configBlock->load(maxUnitInside, "maxUnitInside");
	configBlock->load(maxUnitWorking, "maxUnitWorking");

	configBlock->load(hpInit, "hpInit");
	configBlock->load(hpMax, "hpMax");
	configBlock->load(hpInc, "hpInc");
	configBlock->load(armor, "armor");
	configBlock->load(level, "level");
	configBlock->load(shortTypeNum, "shortTypeNum");
	configBlock->load(isBuildingSite, "isBuildingSite");

	configBlock->load(defaultUnitStayRange, "defaultUnitStayRange");
	configBlock->load(maxUnitStayRange, "maxUnitStayRange");

	configBlock->load(viewingRange, "viewingRange");
	configBlock->load(regenerationSpeed, "regenerationSpeed");
	
	configBlock->load(prestige, "prestige");
	
	// regenerate local parameters
	if ((!globalContainer->runNoX) && (type != "null"))
	{
		gameSpritePtr = Toolkit::getSprite(gameSprite.c_str());
		if (miniSpriteImage >= 0)
			miniSpritePtr = Toolkit::getSprite(miniSprite.c_str());
	}
}
