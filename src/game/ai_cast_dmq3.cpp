#include "g_local.h"
#include "../game/botlib.h"      //bot lib interface
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "botai.h"          //bot ai interface

#include "ai_cast.h"
#define MAX_WAYPOINTS           128

bot_waypoint_t botai_waypoints[MAX_WAYPOINTS];
bot_waypoint_t* botai_freewaypoints;

//NOTE: not using a cvar which can be updated because the game should be reloaded anyway
int gametype;       //game type

// Rafael gameskill
int gameskill;

/*
==================
BotFreeWaypoints
==================
*/
void BotFreeWaypoints(bot_waypoint_t* wp) {
	bot_waypoint_t* nextwp;

	for (; wp; wp = nextwp) {
		nextwp = wp->next;
		wp->next = botai_freewaypoints;
		botai_freewaypoints = wp;
	}
}

/*
==================
BotInitWaypoints
==================
*/
void BotInitWaypoints(void) {
	int i;

	botai_freewaypoints = NULL;
	for (i = 0; i < MAX_WAYPOINTS; i++) {
		botai_waypoints[i].next = botai_freewaypoints;
		botai_freewaypoints = &botai_waypoints[i];
	}
}

/*
==================
BotSetupDeathmatchAI
==================
*/
void BotSetupDeathmatchAI(void) {
	int ent, modelnum;
	char model[128];

	gametype = trap_Cvar_VariableIntegerValue("g_gametype");
	gameskill = trap_Cvar_VariableIntegerValue("g_gameskill");

	int max_bspmodelindex = 0;
	for (ent = trap_AAS_NextBSPEntity(0); ent; ent = trap_AAS_NextBSPEntity(ent)) {
		if (!trap_AAS_ValueForBSPEpairKey(ent, "model", model, sizeof(model))) {
			continue;
		}
		if (model[0] == '*') {
			modelnum = atoi(model + 1);
			if (modelnum > max_bspmodelindex) {
				max_bspmodelindex = modelnum;
			}
		}
	}
	//initialize the waypoint heap
	BotInitWaypoints();
}

/*
==================
BotShutdownDeathmatchAI
==================
*/
void BotShutdownDeathmatchAI(void) {
}

