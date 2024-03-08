
#include "../game/g_local.h"
#include "../game/q_shared.h"
#include "../game/botlib.h"      //bot lib interface
#include "../game/be_aas.h"
#include "../game/be_ea.h"
#include "../game/be_ai_gen.h"
#include "../game/be_ai_goal.h"
#include "../game/be_ai_move.h"
#include "botai.h"          //bot ai interface

#include "ai_cast.h"

#define INVENTORY_ENVIRONMENTSUIT   52

/*
==================
BotIsDead
==================
*/
qboolean BotIsDead(bot_state_t* bs) {
	return (bs->cur_ps.pm_type == PM_DEAD);
}

/*
==================
BotIsObserver
==================
*/
qboolean BotIsObserver(bot_state_t* bs) {
	char buf[MAX_INFO_STRING];
	if (bs->cur_ps.pm_type == PM_SPECTATOR) {
		return qtrue;
	}
	engine->trap_GetConfigstring(CS_PLAYERS + bs->client, buf, sizeof(buf));
	if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
BotIntermission
==================
*/
qboolean BotIntermission(bot_state_t* bs) {
	//NOTE: we shouldn't look at the game code...
	if (level.intermissiontime) {
		return qtrue;
	}
	return (bs->cur_ps.pm_type == PM_FREEZE || bs->cur_ps.pm_type == PM_INTERMISSION);
}


/*
==============
BotInLava
==============
*/
qboolean BotInLava(bot_state_t* bs) {
	vec3_t feet;

	VectorCopy(bs->origin, feet);
	feet[2] -= 23;
	return (engine->trap_AAS_PointContents(feet) & CONTENTS_LAVA);
}

/*
==============
BotInSlime
==============
*/
qboolean BotInSlime(bot_state_t* bs) {
	vec3_t feet;

	VectorCopy(bs->origin, feet);
	feet[2] -= 23;
	return (engine->trap_AAS_PointContents(feet) & CONTENTS_SLIME);
}

/*
==================
EntityIsDead
==================
*/
qboolean EntityIsDead(aas_entityinfo_t* entinfo) {
	playerState_t ps;

	if (entinfo->number >= 0 && entinfo->number < MAX_CLIENTS) {
		//retrieve the current client state
		BotAI_GetClientState(entinfo->number, &ps);
		if (ps.pm_type != PM_NORMAL) {
			return qtrue;
		}
	}
	return qfalse;
}

/*
==================
EntityIsInvisible
==================
*/
qboolean EntityIsInvisible(aas_entityinfo_t* entinfo) {
	if (entinfo->powerups & (1 << PW_INVIS)) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
EntityIsShooting
==================
*/
qboolean EntityIsShooting(aas_entityinfo_t* entinfo) {
	if (entinfo->flags & EF_FIRING) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
EntityIsChatting
==================
*/
qboolean EntityIsChatting(aas_entityinfo_t* entinfo) {
	if (entinfo->flags & EF_TALK) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
EntityHasQuad
==================
*/
qboolean EntityHasQuad(aas_entityinfo_t* entinfo) {
	if (entinfo->powerups & (1 << PW_QUAD)) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
BotSetupForMovement
==================
*/
void BotSetupForMovement(bot_state_t* bs) {
	bot_initmove_t initmove;

	memset(&initmove, 0, sizeof(bot_initmove_t));
	VectorCopy(bs->cur_ps.origin, initmove.origin);
	VectorCopy(bs->cur_ps.velocity, initmove.velocity);
	VectorCopy(bs->cur_ps.origin, initmove.viewoffset);
	initmove.viewoffset[2] += bs->cur_ps.viewheight;
	initmove.entitynum = bs->entitynum;
	initmove.client = bs->client;
	initmove.thinktime = bs->thinktime;
	//set the onground flag
	if (bs->cur_ps.groundEntityNum != ENTITYNUM_NONE) {
		initmove.or_moveflags |= MFL_ONGROUND;
	}
	//set the teleported flag
	if ((bs->cur_ps.pm_flags & PMF_TIME_KNOCKBACK) && (bs->cur_ps.pm_time > 0)) {
		initmove.or_moveflags |= MFL_TELEPORTED;
	}
	//set the waterjump flag
	if ((bs->cur_ps.pm_flags & PMF_TIME_WATERJUMP) && (bs->cur_ps.pm_time > 0)) {
		initmove.or_moveflags |= MFL_WATERJUMP;
	}
	//set presence type
	if (bs->cur_ps.pm_flags & PMF_DUCKED) {
		initmove.presencetype = PRESENCE_CROUCH;
	}
	else { initmove.presencetype = PRESENCE_NORMAL; }
	//
	if (bs->walker > 0.5) {
		initmove.or_moveflags |= MFL_WALK;
	}
	//
	VectorCopy(bs->viewangles, initmove.viewangles);
	//
	engine->trap_BotInitMoveState(bs->ms, &initmove);
}

/*
==================
BotPointAreaNum
==================
*/
int BotPointAreaNum(vec3_t origin) {
	int areanum, numareas, areas[1];
	vec3_t end, ofs;
#define BOTAREA_JIGGLE_DIST     32

	areanum = engine->trap_AAS_PointAreaNum(origin);
	if (areanum) {
		return areanum;
	}
	VectorCopy(origin, end);
	end[2] += 10;
	numareas = engine->trap_AAS_TraceAreas(origin, end, areas, NULL, 1);
	if (numareas > 0) {
		return areas[0];
	}

	// Ridah, jiggle them around to look for a fuzzy area, helps LARGE characters reach destinations that are against walls
	ofs[2] = 10;
	for (ofs[0] = -BOTAREA_JIGGLE_DIST; ofs[0] <= BOTAREA_JIGGLE_DIST; ofs[0] += BOTAREA_JIGGLE_DIST * 2) {
		for (ofs[1] = -BOTAREA_JIGGLE_DIST; ofs[1] <= BOTAREA_JIGGLE_DIST; ofs[1] += BOTAREA_JIGGLE_DIST * 2) {
			VectorAdd(origin, ofs, end);
			numareas = engine->trap_AAS_TraceAreas(origin, end, areas, NULL, 1);
			if (numareas > 0) {
				return areas[0];
			}
		}
	}

	return 0;
}

/*
==================
BotCheckAir
==================
*/
void BotCheckAir(bot_state_t* bs) {
	if (bs->inventory[INVENTORY_ENVIRONMENTSUIT] <= 0) {
		if (engine->trap_AAS_PointContents(bs->eye) & (CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA)) {
			return;
		}
	}
	bs->lastair_time = engine->trap_AAS_Time();
}