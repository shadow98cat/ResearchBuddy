	//**********************//
	//* Research Buddy 1.1 *//
	//**********************//

include("multiplay/skirmish/ResearchBuddyBuild.js.inc");

////* Research Paths (Targets / Goals) *////

const RBPath = [
/////// Power and AutoRepair ///////
		"R-Struc-Factory-Cyborg",
		"R-Struc-PowerModuleMk1",
		"R-Struc-Power-Upgrade01c",
		"R-Struc-Power-Upgrade03a",
		"R-Sys-Autorepair-General",
/////// Prerequisite research 2 ///////
		"R-Defense-HardcreteWall",
		"R-Struc-RepairFacility",
		"R-Defense-HardcreteGate",
		"R-Vehicle-Prop-Tracks",
		"R-Comp-CommandTurret01",
/////// Vehicle Bodies - Snake ///////
		"R-Vehicle-Body11",
/////// VTOLs and Sensor Upgrades to get to WS Sensor Tower ///////
		"R-Sys-Sensor-WSTower",
/////// Sensor Range Upgrades ///////
		"R-Sys-Sensor-Upgrade03",
/////// Wall / Hardpoint Defense Upgrades 1 ///////
		"R-Defense-WallUpgrade03",
/////// Vehicle Armor Upgrades 1 ///////
		"R-Vehicle-Metals03",
/////// Vehicle Armor Heat Upgrades 1 ///////
		"R-Vehicle-Armor-Heat03",
/////// Vehicle Engine Upgrades 1 ///////
		"R-Vehicle-Engine03",
/////// Vehicle Armor Upgrades 2 ///////
		"R-Vehicle-Metals06",
/////// Vehicle Bodies - Nexus ///////
		"R-Vehicle-Body10",
/////// Hardened Sensor Tower ///////
		"R-Sys-Sensor-Tower02",
/////// Repair Facility Upgrade 1 ///////
		"R-Struc-RprFac-Upgrade01",
/////// Vehicle Armor Heat Upgrades 2 ///////
		"R-Vehicle-Armor-Heat06",
/////// Vehicle Engine Upgrades 3 ///////
		"R-Vehicle-Engine09",
/////// Vehicle Bodies - Dragon ///////
		"R-Vehicle-Body14",
/////// Neural Synapse Research Brain Mk3 ///////
		"R-Struc-Research-Upgrade09",
/////// Wall / Hardpoint Defense Upgrades 2 ///////
		"R-Defense-WallUpgrade06",
/////// Base Structure Materials Upgrades 1 ///////
		"R-Struc-Materials03",
/////// VTOL Transports and ReArming Upgrades ///////
		"R-Cyborg-Transport",
		"R-Struc-VTOLPad-Upgrade03",
		"R-SuperTransport",
		"R-Struc-VTOLPad-Upgrade06",
/////// Satellite Uplink Center ///////
		"R-Sys-Sensor-UpLink",
/////// Repair Facility Upgrades 2 ///////
		"R-Struc-RprFac-Upgrade06",
/////// Wall / Hardpoint Defense Upgrades 3 ///////
		"R-Defense-WallUpgrade09",
/////// Base Structure Materials Upgrades 2 ///////
		"R-Struc-Materials06",
/////// Factory Upgrade 4 ///////
		"R-Struc-Factory-Upgrade09",
/////// Vehicle Armor Heat Upgrades 3 ///////
		"R-Vehicle-Armor-Heat09",
/////// Wall / Hardpoint Defense Upgrades 4 ///////
		"R-Defense-WallUpgrade12",
/////// Base Structure Materials Upgrades 3 ///////
		"R-Struc-Materials09",
/////// Commander Upgrades ///////
		"R-Comp-CommandTurret04",
/////// Cyborg Armor Upgrades 1 ///////
		"R-Cyborg-Metals03",
/////// Cyborg Armor Heat Upgrades 1 ///////
		"R-Cyborg-Armor-Heat03",
/////// Sensor Upgrades 2 ///////
		"R-Sys-RadarDetector01",
		"R-Sys-CBSensor-Turret01",
		"R-Sys-VTOLStrike-Turret01",
		"R-Sys-VTOLCBS-Turret01",
/////// Cyborg Armor Upgrades 2 ///////
		"R-Cyborg-Metals06",
/////// Cyborg Armor Heat Upgrades 2 ///////
		"R-Cyborg-Armor-Heat06",
/////// Cyborg Armor Upgrades 3 ///////
		"R-Cyborg-Metals09",
/////// Cyborg Armor Heat Upgrades 3 ///////
		"R-Cyborg-Armor-Heat09",
/////// Miscellaneous ///////
		"R-Sys-Resistance-Circuits",
		"R-Sys-MobileRepairTurretHvy",
		"R-Defense-TankTrap01",
/////// Vehicle Bodies - Bug and Big Cat(NOT required for Nexus or Dragon bodies) ///////
		"R-Vehicle-Body02",
		"R-Vehicle-Body04",
		"R-Vehicle-Body06",
		"R-Vehicle-Body08",
		"R-Vehicle-Body09",
		"R-Vehicle-Body12",
];


////* Definitions *////

const	lab = "A0ResearchFacility";
const	command = "A0CommandCentre";
const	generator = "A0PowerGenerator";
const	derrick = "A0ResourceExtractor";
const	oilres = "OilResource";
const	borgfac = "A0CyborgFactory";

const	pmod = "A0PowMod1";
const	rmod = "A0ResearchModule1";

const	BASE_SIZE = 3;		// home location
const	EXTREME_HIGH_POWER = 3000;
const	HIGH_POWER = 2000;
const	MEDIUM_POWER = 1000;
const	LOW_POWER = 500;
const	EXTREME_LOW_POWER = 250;

const	MAX_LABS_EASY = 2;
const	MAX_LABS_MEDIUM = 3;
const	MAX_LABS_HARD = 4;

////* Functions *////

// not sure this check is necessary ... why not
function structureReady(struct) {
	return structureIdle(struct) && (struct.status == BUILT);
}

// random integer between 0 and max-1 (for convenience)
function random(max) {
	if (max<=0)
		return 0;
	return Math.floor(Math.random() * max);
}

function powerCheck(level) {
	if (playerPower(me) >= (level))
		return true;
	return false;
}

////* Researching Technologies *////

// make a particular lab follow a certain research path
// even though pursueResearch() accepts paths, 
// its behaviour seems to be less deterministic than this
function followResearchPathRB(lab,path) {
	for (var j=0; j<path.length; ++j)
		if (pursueResearch(lab,path[j])) {
			return true;
		}
	return false;
}

// come up with something from the path
function doResearchNow() {
	if	(!powerCheck(EXTREME_LOW_POWER))
		return false;
	var lablist=enumStruct(me, RESEARCH_LAB);
	for (var i=0; i<lablist.length; ++i)
	if (structureReady(lablist[i])) {
		if (followResearchPathRB(lablist[i],RBPath)) {  // research to do here
				continue;
		}
	}
}

////* Difficulty Settings *////

// Throttles the research, more aggressive the greater the difficulty
//Easy level AI's get 25% penalty on oil production on 3.2+ per the followings page
//Insane level AI's get 2X oil production on 3.2+ per the followings page
//https://warzone.atlassian.net/wiki/display/mod/Difficulty+Levels
function startingLabCheck() {	// this does work Right!!!!
	var labCount = enumStruct(me, lab).length;
	if (labCount <= 0)
		return 0;
	var labsOverEASY=(labCount - MAX_LABS_EASY);
	if (labsOverEASY < 0) {
		var labsOverEASY=0
	}
	var labsOverMEDIUM=(labCount - MAX_LABS_MEDIUM);
	if (labsOverMEDIUM < 0) {
		var labsOverMEDIUM=0
	}
	var labsOverHARD=(labCount - MAX_LABS_HARD);
	if (labsOverHARD < 0) {
		var labsOverHARD=0
	}
	switch(difficulty) {
		case EASY:
			return labsOverEASY;
		case MEDIUM:
			return labsOverMEDIUM;
		case HARD:
			return labsOverHARD;
		case INSANE:
			return 0;
	}
}

function researchDelay() {	// this does work Right!!!!
	switch(difficulty) {
		case EASY:
			return 5000+90000*startingLabCheck();
		case MEDIUM:
			return 5000+60000*startingLabCheck();
		case HARD:
			return 5000+30000*startingLabCheck();
		case INSANE:
			return 5000;
	}
}

function setupResearch() {
	if (gameTime > 60000) {
		removeTimer("doResearchNow");
		queue("doResearchNow");
	}
	setTimer("doResearchNow",	researchDelay());
}

// checks for difficulty level
function RBdiff() {
	switch(difficulty) {
		case EASY:
			return 0;
		case MEDIUM:
			return 1;
		case HARD:
			return 2;
		case INSANE:
			return 3;
	}
}

////* Events *////

// something was just researched - will only fire if the labcount is not to high
function eventResearched(tech, labparam) {
	if (startingLabCheck() <= 0) {
	queue("doResearchNow");
	}
}

// something was just built
function eventStructureBuilt(struct, droid) {
	queue("executeBuildOrder");
}

// start of skirmish match
function eventStartLevel() {
	queue("BuildBase");
	queue("setupResearch");
	setTimer("setupResearch", 4000*60);
}