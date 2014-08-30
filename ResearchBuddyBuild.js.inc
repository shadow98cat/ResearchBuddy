	//****************************//
	//* Research Buddy Build 1.0 *//
	//****************************//

////* Functions *////

function addTruckToSomeGroup(truck) {
	groupAddDroid(builderGroup,truck);
}

// make sure builderGroup is full
function PopulateBuilderGroup() {
	builderGroup=newGroup();
	var list = enumDroid(me);
	for (var i=0; i<list.length; ++i) {
		var droid=list[i];
		if (droid.droidType == DROID_CONSTRUCT || droid.name.indexOf("Engineer")>-1) {
			addTruckToSomeGroup(droid);
			// the next line helps avoiding some strange problem when droids that are initially
			// buried into the ground don't move out of the way when a building is being
			// placed right above them
			orderDroidLoc(droid,DORDER_MOVE,droid.x+random(2)+1,droid.y+random(2)+1); 
		}
	}
}

// checks if a truck is busy doing something
function isTruckBusy(truck) {
	if (truck.order == DORDER_BUILD)
		return true;
	if (truck.order == DORDER_HELPBUILD)
		return true;
	if (truck.order == DORDER_LINEBUILD)
		return true;
	if (truck.order == DORDER_DEMOLISH)
		return true;
	return false;
}

// makes sure we have at least one operational structure of that type
function iHaveStruct(stat) {
	var list=enumStruct(me,stat)
	for (var i=0; i<list.length; ++i)
		if (list[i].status==BUILT)
			return true;
	return false;
}

function recycleCC() {
    var trucks = enumDroid(me, DROID_CONSTRUCT);
	var CClist = enumStruct(me, HQ)
	if (CClist.length < 1 || trucks.length < 1) {
		return false;
	}
    var list = CClist;
    for (var i = 0; i < list.length; ++i)
        for (var j = 0; j < trucks.length; ++j) if (!isTruckBusy(trucks[j])) {
            if (droidCanReach(trucks[j], list[i].x, list[i].y)) {
                orderDroidObj(trucks[0], DORDER_DEMOLISH, list[i]);
				return true;
            }
		}
	return false;
}
 
// estimates enemy activity at particular point
function dangerLevel(x,y) {
	return (safeDest(me,x,y)?0:1); // a non-cheating procedure here
}

 // a constructor for Derrick Stat object
function constructDerrickStat(pos) {
	this.x = pos.x; 
	this.y = pos.y;
	this.numBuilders = 0; // >0 if some trucks were sent recently to build derrick here
	this.hotness = -1; // the amount of times this derrick was rebuilt
}

// finds an appropriate derrickStats entry for derrick
function findDerrickStat(obj) {
	if (typeof(derrickStats)=="undefined")
		derrickStats=[];
	for (var i=0; i<derrickStats.length; ++i) {
		if (derrickStats[i].x == obj.x && derrickStats[i].y == obj.y)
			return i;
	}
	var n=derrickStats.length;
	derrickStats[n]=new constructDerrickStat(obj);
	return n;
}

// returns the power generator owner nearest to (x,y)	// I had to change command to generator because RB doesn't always have CC
function getNearestCCPlayer(x,y,all) {
	var minDist=Infinity, minPlayer;
	for (var ally=0; ally<maxPlayers; ++ally) if (allianceExistsBetween(me,ally) || me==ally || all==1) {
		var list=enumStruct(ally,generator);	// I had to change command to generator because RB doesn't always have CC
		for (var i=0; i<list.length; ++i) {
			var d=distBetweenTwoPoints(list[i].x,list[i].y,x,y);
			if (d<minDist) {
				minDist=d;
				minPlayer=ally;
			}
		}
	}
	return minPlayer;
}
 
 // returns a droid to the base
function forceReturnToBase(droid) {
	orderDroidLoc(droid, DORDER_MOVE, basePosition.x+1, basePosition.y+1);
}

// is the droid already at the base?
function droidIsAtHome(droid) {
	return distBetweenTwoPoints(droid.x, droid.y, basePosition.x, basePosition.y) <= BASE_SIZE;
}

// returns a droid to the base if not at the base yet
function returnToBase(droid) {
	if (!droidIsAtHome(droid))
		forceReturnToBase(droid);
}

// returns builderGroup to the base if not at the base yet
function returnToBaseBG() {
	var droidlist = enumGroup(builderGroup);
		for (var i=0; i<droidlist.length; ++i)
			if (!isTruckBusy(droidlist[i])) {
				returnToBase(droidlist[i]);
			}
}

// order free oil-oriented trucks to build oil derricks
function huntForOil() {
	var droidlist = enumGroup(builderGroup);
	var list = enumFeature(-1, oilres);
	var oillist = [];
	if (typeof(huntForOil.cachedOilList)=="undefined" || gameTime - huntForOil.cacheUpdateTime > 10000) {
		for (var j=0; j<list.length; ++j) {
			var stat=findDerrickStat(list[j]);
			if (derrickStats[stat].numBuilders<=0) {
				var danger=false;
				if (list.length > 20) {
					if (!safeDest(me,list[j].x,list[j].y))
						danger=true;
				} else {
					if (dangerLevel(list[j].x,list[j].y)>0)
						danger=true;
				}
				if (!danger || random(15)==0) {
					var tmp=getNearestCCPlayer(list[j].x,list[j].y,0); // friendly AIs shouldn't capture your oil
					if (typeof(tmp)=="undefined" || tmp==me) {	// check to see if: 1. no ally exists "undefined" or 2. My CC is closer to oil than allies.
						var n=oillist.length;	// only then do we build the oil list and get going
						oillist[n]=list[j];
						oillist[n].stat=stat;
					}
				}
			}
		}
		huntForOil.cachedOilList = oillist;
		huntForOil.cacheUpdateTime = gameTime;
	} else {
		oillist = huntForOil.cachedOilList;
	}
	if (oillist.length <= 0) { // not much to do here ...
		returnToBaseBG();
		return false;
	}
	for (var i=0; i<droidlist.length; ++i) if (!isTruckBusy(droidlist[i])) {
		var range1=Infinity;
		var k=-1;
		for (var j=0; j<oillist.length; ++j) {
			if (derrickStats[oillist[j].stat].numBuilders<=0) {
				if (droidCanReach(droidlist[i],oillist[j].x,oillist[j].y)) {
					var range2=distBetweenTwoPoints(droidlist[i].x, droidlist[i].y, oillist[j].x, oillist[j].y);
					if (range2<range1) {
						range1=range2;
						k=j;
					}
				}
			}
		}
		if (k!=-1) {
			orderDroidBuild(droidlist[i], DORDER_BUILD, derrick, oillist[k].x, oillist[k].y);
		}
	}
}

// order the builderGroup to build some base structure near basePosition
function buildBasicStructure(struct) {
	if (typeof(buildBasicStructure.lastCall)=="undefined")
		buildBasicStructure.lastCall=gameTime;
	if (gameTime - buildBasicStructure.lastCall < 2000)
		return false;
	if (typeof(struct)=="undefined")
		return false;
	var droidlist = enumGroup(builderGroup);
	if (droidlist.length == 0) 
		return false;
	var found=false;
	for (var i=0; i<droidlist.length; ++i)
		if (!isTruckBusy(droidlist[i])) {	// if truck is not busy
			if (!found) {
				loc=pickStructLocation(
					droidlist[i], 
					struct, 
					basePosition.x,	// the location of basePosition X coordinate
					basePosition.y,	// the location of basePosition Y coordinate
					0
				);
				if (typeof(loc)=="undefined")
					return false;
				if (dangerLevel(loc.x,loc.y)>0)
					return false;
				found=true;
			}
			orderDroidBuild(droidlist[i], DORDER_BUILD, struct, loc.x, loc.y);
			buildBasicStructure.lastCall=gameTime;
		}
	return found;
}

function BuildOrderRB() {
	// Builds initial structures in the following order:
	//     oil, oil, generator, lab, lab, oil, lab, .
	var derrickCount = enumStruct(me, derrick).length;
	var genCount = enumStruct(me, generator).length;
	var labCount = enumStruct(me, lab).length;
	var borgfacs = enumStruct(me, borgfac).length;
	if (genCount < 1 && isStructureAvailable(generator,me))
		return buildBasicStructure(generator);
	if (derrickCount < 2)
		return huntForOil();
	if (iHaveStruct(command))
		return recycleCC();
	if (borgfacs < 1 && playerPower(me) > LOW_POWER && isStructureAvailable(borgfac,me) && RBdiff() > 1)
		return buildBasicStructure(borgfac);
	if (derrickCount - 4*genCount > 0)	//nice math to always build just enough generators
		return buildBasicStructure(generator);
	if (labCount < MAX_LABS_EASY)
		return buildBasicStructure(lab);
	if (derrickCount < 3 && RBdiff() > 0)
		return huntForOil();
	if (labCount < MAX_LABS_MEDIUM && playerPower(me) > LOW_POWER && isStructureAvailable(lab,me) && RBdiff() > 0)	// works perfectly
		return buildBasicStructure(lab);
	if (derrickCount < 4 && RBdiff() > 1)
		return huntForOil();
	if (labCount < MAX_LABS_HARD && playerPower(me) > LOW_POWER && isStructureAvailable(lab,me) && RBdiff() > 1)	// works perfectly
		return buildBasicStructure(lab);
	if (derrickCount < 5 && RBdiff() > 2)
		return huntForOil();
	if (derrickCount >= 5 && labCount < 5 && playerPower(me) > LOW_POWER && isStructureAvailable(lab,me) && RBdiff() > 2)	// works perfectly
		return buildBasicStructure(lab);
	if (derrickCount < 8 && RBdiff() > 2)
		return huntForOil();
	if (!isTruckBusy) { returnToBaseBG(); }
}

// finish buildings that were never finished yet
function finishBuildings() {
	var droidlist; 
	var list = enumStruct(me);
	var found = false;
	for (var j=0; j<list.length; ++j) {
		if (list[j].status == BEING_BUILT) {
			if (list[j].stattype != HQ) {
				if (dangerLevel(list[j].x,list[j].y)<=0) {
					droidlist=enumGroup(builderGroup);
						for (var i=0; i<droidlist.length; ++i)	
							if (!isTruckBusy(droidlist[i]))
								orderDroidObj(droidlist[i], DORDER_HELPBUILD, list[j]), found=true;
				}
			}
		}
		if (found) return true;
	}
	return found;
}

// build a specific module
function buildModule(struct, moduleType) {
	if (typeof(struct)=="undefined")
		return false;
	if (!isStructureAvailable(moduleType, me))
		return false;
	if (playerPower(me) < LOW_POWER)
		return false;
	var droidlist = enumGroup(builderGroup);
	for (var i=0; i<droidlist.length; ++i)	
		if (!isTruckBusy(droidlist[i]))
			if (orderDroidBuild(droidlist[i], DORDER_BUILD, moduleType, struct.x, struct.y))
				return true;
	return false;
}

// build different sorts of modules
function upgradeStructures() {
	if (gameTime < 60000)
		return false;
	var list;
	list = enumStruct(me, generator);
	for (var i=0; i<list.length; ++i) 
		if (list[i].modules < 1) 
			if (buildModule(list[i], pmod)) 
				return true;
	list = enumStruct(me, lab);
	if (playerPower(me) > MEDIUM_POWER) {
		for (var i=0; i<list.length; ++i) 
			if (list[i].modules < 1) 
				if (buildModule(list[i], rmod)) 
					return true;
	}
	return false;
}

// the main function responsible for building things
// calls other functions in a particular order
function executeBuildOrder() {
	if (!finishBuildings())
		if (!BuildOrderRB())
			if (!upgradeStructures())
	return false;
}

function BuildBase() {
	basePosition=startPositions[me];
	queue("PopulateBuilderGroup",	2000);
	setTimer("executeBuildOrder",	3000);
	setTimer("returnToBaseBG",	60000);
}