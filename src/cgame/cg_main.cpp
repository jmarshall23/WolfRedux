/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein single player GPL Source Code (RTCW SP Source Code).  

RTCW SP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW SP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW SP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW SP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW SP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

/*
 * name:		cg_main.c
 *
 * desc:		initialization and primary entry point for cgame
 *
*/


#include "cg_local.h"

int forceModelModificationCount = -1;

cg_t cg;
cgs_t cgs;
centity_t cg_entities[MAX_GENTITIES];
weaponInfo_t cg_weapons[MAX_WEAPONS];
itemInfo_t cg_items[MAX_ITEMS];

vmCvar_t cg_railTrailTime;
vmCvar_t cg_centertime;
vmCvar_t cg_runpitch;
vmCvar_t cg_runroll;
vmCvar_t cg_bobup;
vmCvar_t cg_bobpitch;
vmCvar_t cg_bobroll;
vmCvar_t cg_swingSpeed;
vmCvar_t cg_shadows;
vmCvar_t cg_gibs;
vmCvar_t cg_drawTimer;
vmCvar_t cg_drawFPS;
vmCvar_t cg_drawSnapshot;
vmCvar_t cg_draw3dIcons;
vmCvar_t cg_drawIcons;
vmCvar_t cg_youGotMail;         //----(SA)	added
vmCvar_t cg_drawAmmoWarning;
vmCvar_t cg_drawCrosshair;
vmCvar_t cg_drawCrosshairNames;
vmCvar_t cg_drawCrosshairPickups;
vmCvar_t cg_hudAlpha;
vmCvar_t cg_weaponCycleDelay;       //----(SA)	added
vmCvar_t cg_cycleAllWeaps;
vmCvar_t cg_useWeapsForZoom;
vmCvar_t cg_drawAllWeaps;
vmCvar_t cg_drawRewards;
vmCvar_t cg_crosshairSize;
vmCvar_t cg_crosshairAlpha;     //----(SA)	added
vmCvar_t cg_crosshairX;
vmCvar_t cg_crosshairY;
vmCvar_t cg_crosshairHealth;
vmCvar_t cg_draw2D;
vmCvar_t cg_drawFrags;
vmCvar_t cg_teamChatsOnly;
vmCvar_t cg_drawStatus;
vmCvar_t cg_animSpeed;
vmCvar_t cg_drawSpreadScale;
vmCvar_t cg_debugAnim;
vmCvar_t cg_debugPosition;
vmCvar_t cg_debugEvents;
vmCvar_t cg_errorDecay;
vmCvar_t cg_nopredict;
vmCvar_t cg_noPlayerAnims;
vmCvar_t cg_showmiss;
vmCvar_t cg_footsteps;
vmCvar_t cg_markTime;
vmCvar_t cg_brassTime;
vmCvar_t cg_viewsize;
vmCvar_t cg_letterbox;
vmCvar_t cg_drawGun;
vmCvar_t cg_drawFPGun;
vmCvar_t cg_drawGamemodels;
vmCvar_t cg_cursorHints;
vmCvar_t cg_hintFadeTime;       //----(SA)	added
vmCvar_t cg_gun_frame;
vmCvar_t cg_gun_x;
vmCvar_t cg_gun_y;
vmCvar_t cg_gun_z;
vmCvar_t cg_tracerChance;
vmCvar_t cg_tracerWidth;
vmCvar_t cg_tracerLength;
vmCvar_t cg_tracerSpeed;
vmCvar_t cg_autoswitch;
vmCvar_t cg_ignore;
vmCvar_t cg_simpleItems;
vmCvar_t cg_fov;
vmCvar_t cg_zoomFov;
vmCvar_t cg_zoomStepBinoc;
vmCvar_t cg_zoomStepSniper;
vmCvar_t cg_zoomStepSnooper;
vmCvar_t cg_zoomStepFG;         //----(SA)	added
vmCvar_t cg_zoomDefaultBinoc;
vmCvar_t cg_zoomDefaultSniper;
vmCvar_t cg_zoomDefaultSnooper;
vmCvar_t cg_zoomDefaultFG;      //----(SA)	added
vmCvar_t cg_reticles;
vmCvar_t cg_reticleBrightness;      //----(SA)	added
vmCvar_t cg_thirdPerson;
vmCvar_t cg_thirdPersonRange;
vmCvar_t cg_thirdPersonAngle;
vmCvar_t cg_stereoSeparation;
vmCvar_t cg_lagometer;
vmCvar_t cg_drawAttacker;
vmCvar_t cg_synchronousClients;
vmCvar_t cg_teamChatTime;
vmCvar_t cg_teamChatHeight;
vmCvar_t cg_stats;
vmCvar_t cg_buildScript;
vmCvar_t cg_forceModel;
vmCvar_t cg_coronafardist;
vmCvar_t cg_coronas;
vmCvar_t cg_paused;
vmCvar_t cg_blood;
vmCvar_t cg_predictItems;
vmCvar_t cg_deferPlayers;
vmCvar_t cg_drawTeamOverlay;
vmCvar_t cg_enableBreath;
vmCvar_t cg_autoactivate;
vmCvar_t cg_useSuggestedWeapons;    //----(SA)	added
vmCvar_t cg_emptyswitch;
vmCvar_t cg_particleDist;
vmCvar_t cg_particleLOD;
vmCvar_t cg_blinktime;      //----(SA)	added

vmCvar_t cg_smoothClients;
vmCvar_t pmove_fixed;
vmCvar_t pmove_msec;

// Rafael - particle switch
vmCvar_t cg_wolfparticles;
// done

// Ridah
vmCvar_t cg_gameType;
vmCvar_t cg_bloodTime;
vmCvar_t cg_norender;
vmCvar_t cg_skybox;

// Rafael
vmCvar_t cg_gameSkill;
// done

vmCvar_t cg_reloading;      //----(SA)	added

// JPW NERVE
vmCvar_t cg_medicChargeTime;
vmCvar_t cg_engineerChargeTime;
vmCvar_t cg_LTChargeTime;
vmCvar_t cg_soldierChargeTime;
vmCvar_t cg_redlimbotime;
vmCvar_t cg_bluelimbotime;
// jpw

vmCvar_t cg_hunkUsed;
vmCvar_t cg_soundAdjust;
vmCvar_t cg_expectedhunkusage;

vmCvar_t cg_showAIState;

vmCvar_t cg_notebook;
vmCvar_t cg_notebookpages;          // bitflags for the currently accessable pages.  if they wanna cheat, let 'em.  Most won't, or will wait 'til they actually play it.

vmCvar_t cg_currentSelectedPlayer;
vmCvar_t cg_currentSelectedPlayerName;
vmCvar_t cg_cameraMode;
vmCvar_t cg_cameraOrbit;
vmCvar_t cg_cameraOrbitDelay;
vmCvar_t cg_timescaleFadeEnd;
vmCvar_t cg_timescaleFadeSpeed;
vmCvar_t cg_timescale;
vmCvar_t cg_smallFont;
vmCvar_t cg_bigFont;
vmCvar_t cg_hudFiles;

vmCvar_t cg_animState;
vmCvar_t cg_missionStats;
vmCvar_t cg_waitForFire;

vmCvar_t cg_loadWeaponSelect;

// NERVE - SMF - Wolf multiplayer configuration cvars
vmCvar_t mp_playerType;
vmCvar_t mp_team;
vmCvar_t mp_weapon;
vmCvar_t mp_pistol;
vmCvar_t mp_item1;
vmCvar_t mp_item2;
vmCvar_t mp_mapDesc;
vmCvar_t mp_mapTitle;
vmCvar_t mp_itemDesc;
// -NERVE - SMF

typedef struct {
	vmCvar_t    *vmCvar;
	char        *cvarName;
	char        *defaultString;
	int cvarFlags;
} cvarTable_t;

cvarTable_t cvarTable[] = {
	{ &cg_ignore, "cg_ignore", "0", 0 },  // used for debugging
	{ &cg_autoswitch, "cg_autoswitch", "2", CVAR_ARCHIVE },
	{ &cg_drawGun, "cg_drawGun", "1", CVAR_ARCHIVE },
	{ &cg_drawGamemodels, "cg_drawGamemodels", "1", CVAR_CHEAT },
	{ &cg_drawFPGun, "cg_drawFPGun", "1", CVAR_ARCHIVE },
	{ &cg_gun_frame, "cg_gun_frame", "0", CVAR_TEMP },
	{ &cg_cursorHints, "cg_cursorHints", "1", CVAR_ARCHIVE },
	{ &cg_hintFadeTime, "cg_hintFadeTime", "500", CVAR_ARCHIVE }, //----(SA)	added
	{ &cg_zoomFov, "cg_zoomfov", "22.5", CVAR_ARCHIVE },
	{ &cg_zoomDefaultBinoc, "cg_zoomDefaultBinoc", "22.5", CVAR_ARCHIVE },
	{ &cg_zoomDefaultSniper, "cg_zoomDefaultSniper", "15", CVAR_ARCHIVE },
	{ &cg_zoomDefaultSnooper, "cg_zoomDefaultSnooper", "40", CVAR_ARCHIVE },
	{ &cg_zoomDefaultFG, "cg_zoomDefaultFG", "55", CVAR_ARCHIVE },                //----(SA)	added
	{ &cg_zoomStepBinoc, "cg_zoomStepBinoc", "3", CVAR_ARCHIVE },
	{ &cg_zoomStepSniper, "cg_zoomStepSniper", "2", CVAR_ARCHIVE },
	{ &cg_zoomStepSnooper, "cg_zoomStepSnooper", "5", CVAR_ARCHIVE },
	{ &cg_zoomStepFG, "cg_zoomStepFG", "10", CVAR_ARCHIVE },          //----(SA)	added
	{ &cg_fov, "cg_fov", "90", CVAR_ARCHIVE | CVAR_CHEAT }, // JPW NERVE added cheat protect	NOTE: there is already a dmflag (DF_FIXED_FOV) to allow server control of this cheat
	{ &cg_viewsize, "cg_viewsize", "100", CVAR_ARCHIVE },
	{ &cg_letterbox, "cg_letterbox", "0", CVAR_TEMP },    //----(SA)	added
	{ &cg_stereoSeparation, "cg_stereoSeparation", "0.4", CVAR_ARCHIVE  },
	{ &cg_shadows, "cg_shadows", "1", CVAR_ARCHIVE  },
	{ &cg_gibs, "cg_gibs", "1", CVAR_ARCHIVE  },
	{ &cg_draw2D, "cg_draw2D", "1", CVAR_ARCHIVE  },
	{ &cg_drawSpreadScale, "cg_drawSpreadScale", "1", CVAR_ARCHIVE },
	{ &cg_drawFrags, "cg_drawFrags", "1", CVAR_ARCHIVE },
	{ &cg_drawStatus, "cg_drawStatus", "1", CVAR_ARCHIVE  },
	{ &cg_drawTimer, "cg_drawTimer", "0", CVAR_ARCHIVE  },
	{ &cg_drawFPS, "cg_drawFPS", "0", CVAR_ARCHIVE  },
	{ &cg_drawSnapshot, "cg_drawSnapshot", "0", CVAR_ARCHIVE  },
	{ &cg_draw3dIcons, "cg_draw3dIcons", "1", CVAR_ARCHIVE  },
	{ &cg_drawIcons, "cg_drawIcons", "1", CVAR_ARCHIVE  },
	{ &cg_drawAmmoWarning, "cg_drawAmmoWarning", "1", CVAR_ARCHIVE  },
	{ &cg_drawAttacker, "cg_drawAttacker", "1", CVAR_ARCHIVE  },
	{ &cg_drawCrosshair, "cg_drawCrosshair", "4", CVAR_ARCHIVE },
	{ &cg_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE },
	{ &cg_drawCrosshairPickups, "cg_drawCrosshairPickups", "1", CVAR_ARCHIVE },
	{ &cg_drawRewards, "cg_drawRewards", "1", CVAR_ARCHIVE },
	{ &cg_hudAlpha, "cg_hudAlpha", "0.8", CVAR_ARCHIVE },
	{ &cg_useWeapsForZoom,  "cg_useWeapsForZoom", "1", CVAR_ARCHIVE },
	{ &cg_weaponCycleDelay, "cg_weaponCycleDelay", "150", CVAR_ARCHIVE }, //----(SA)	added
	{ &cg_cycleAllWeaps,    "cg_cycleAllWeaps", "1", CVAR_ARCHIVE },
	{ &cg_drawAllWeaps,     "cg_drawAllWeaps",   "1", CVAR_ARCHIVE },
	{ &cg_crosshairSize, "cg_crosshairSize", "24", CVAR_ARCHIVE },
	{ &cg_crosshairAlpha, "cg_crosshairAlpha", "0.5", CVAR_ARCHIVE }, //----(SA)	added
	{ &cg_crosshairHealth, "cg_crosshairHealth", "1", CVAR_ARCHIVE },
	{ &cg_crosshairX, "cg_crosshairX", "0", CVAR_ARCHIVE },
	{ &cg_crosshairY, "cg_crosshairY", "0", CVAR_ARCHIVE },
	{ &cg_brassTime, "cg_brassTime", "1250", CVAR_ARCHIVE },
	{ &cg_simpleItems, "cg_simpleItems", "0", CVAR_ARCHIVE },
	{ &cg_reticles, "cg_reticles", "1", CVAR_CHEAT },
	{ &cg_reticleBrightness, "cg_reticleBrightness", "0.7", CVAR_ARCHIVE },
	{ &cg_markTime, "cg_marktime", "20000", CVAR_ARCHIVE },
	{ &cg_lagometer, "cg_lagometer", "1", CVAR_ARCHIVE },
	{ &cg_railTrailTime, "cg_railTrailTime", "400", CVAR_ARCHIVE  },
	{ &cg_gun_x, "cg_gunX", "0", CVAR_CHEAT },
	{ &cg_gun_y, "cg_gunY", "0", CVAR_CHEAT },
	{ &cg_gun_z, "cg_gunZ", "0", CVAR_CHEAT },
	{ &cg_centertime, "cg_centertime", "3", CVAR_CHEAT },
	{ &cg_runpitch, "cg_runpitch", "0.002", CVAR_ARCHIVE},
	{ &cg_runroll, "cg_runroll", "0.005", CVAR_ARCHIVE },
	{ &cg_bobup, "cg_bobup", "0.005", CVAR_ARCHIVE },
	{ &cg_bobpitch, "cg_bobpitch", "0.002", CVAR_ARCHIVE },
	{ &cg_bobroll, "cg_bobroll", "0.002", CVAR_ARCHIVE },

	// JOSEPH 10-27-99
	{ &cg_autoactivate, "cg_autoactivate", "1", CVAR_ARCHIVE },
	{ &cg_emptyswitch, "cg_emptyswitch", "0", CVAR_ARCHIVE },
	// END JOSEPH

//----(SA)	added
	{ &cg_particleDist, "cg_particleDist", "1024", CVAR_ARCHIVE },
	{ &cg_particleLOD, "cg_particleLOD", "0", CVAR_ARCHIVE },
	{ &cg_useSuggestedWeapons, "cg_useSuggestedWeapons", "1", CVAR_ARCHIVE }, //----(SA)	added
//----(SA)	end

	// Ridah, more fluid rotations
	{ &cg_swingSpeed, "cg_swingSpeed", "0.1", CVAR_CHEAT },   // was 0.3 for Q3
	{ &cg_bloodTime, "cg_bloodTime", "120", CVAR_ARCHIVE },
	{ &cg_hunkUsed, "com_hunkUsed", "0", 0 },
	{ &cg_soundAdjust, "hunk_soundadjust", "0", 0 },

	{ &cg_skybox, "cg_skybox", "1", CVAR_CHEAT },
	// done.

	{ &cg_animSpeed, "cg_animspeed", "1", CVAR_CHEAT },
	{ &cg_debugAnim, "cg_debuganim", "0", CVAR_CHEAT },
	{ &cg_debugPosition, "cg_debugposition", "0", CVAR_CHEAT },
	{ &cg_debugEvents, "cg_debugevents", "0", CVAR_CHEAT },
	{ &cg_errorDecay, "cg_errordecay", "100", 0 },
	{ &cg_nopredict, "cg_nopredict", "0", 0 },
	{ &cg_noPlayerAnims, "cg_noplayeranims", "0", CVAR_CHEAT },
	{ &cg_showmiss, "cg_showmiss", "0", 0 },
	{ &cg_footsteps, "cg_footsteps", "1", CVAR_CHEAT },
	{ &cg_tracerChance, "cg_tracerchance", "0.4", CVAR_CHEAT },
	{ &cg_tracerWidth, "cg_tracerwidth", "0.8", CVAR_CHEAT },
	{ &cg_tracerSpeed, "cg_tracerSpeed", "4500", CVAR_CHEAT },
	{ &cg_tracerLength, "cg_tracerlength", "160", CVAR_CHEAT },
	{ &cg_thirdPersonRange, "cg_thirdPersonRange", "40", 0 },
	{ &cg_thirdPersonAngle, "cg_thirdPersonAngle", "0", CVAR_CHEAT },
	{ &cg_thirdPerson, "cg_thirdPerson", "0", 0 },
	{ &cg_teamChatTime, "cg_teamChatTime", "3000", CVAR_ARCHIVE  },
	{ &cg_teamChatHeight, "cg_teamChatHeight", "8", CVAR_ARCHIVE  },
	{ &cg_forceModel, "cg_forceModel", "0", CVAR_ARCHIVE  },
	{ &cg_coronafardist, "cg_coronafardist", "1536", CVAR_ARCHIVE },
	{ &cg_coronas, "cg_coronas", "1", CVAR_ARCHIVE },
	{ &cg_predictItems, "cg_predictItems", "1", CVAR_ARCHIVE },
	{ &cg_deferPlayers, "cg_deferPlayers", "1", CVAR_ARCHIVE },
	{ &cg_drawTeamOverlay, "cg_drawTeamOverlay", "0", CVAR_ARCHIVE },
	{ &cg_stats, "cg_stats", "0", 0 },
	{ &cg_blinktime, "cg_blinktime", "100", CVAR_ARCHIVE},         //----(SA)	added

	{ &cg_enableBreath, "g_enableBreath", "1", CVAR_SERVERINFO},
	{ &cg_cameraOrbit, "cg_cameraOrbit", "0", CVAR_CHEAT},
	{ &cg_cameraOrbitDelay, "cg_cameraOrbitDelay", "50", CVAR_ARCHIVE},
	{ &cg_timescaleFadeEnd, "cg_timescaleFadeEnd", "1", 0},
	{ &cg_timescaleFadeSpeed, "cg_timescaleFadeSpeed", "0", 0},
	{ &cg_timescale, "timescale", "1", 0},
//	{ &cg_smoothClients, "cg_smoothClients", "0", CVAR_USERINFO | CVAR_ARCHIVE},
	{ &cg_cameraMode, "com_cameraMode", "0", CVAR_CHEAT},

	{ &pmove_fixed, "pmove_fixed", "0", 0},
	{ &pmove_msec, "pmove_msec", "8", 0},
	{ &cg_smallFont, "ui_smallFont", "0.25", CVAR_ARCHIVE},
	{ &cg_bigFont, "ui_bigFont", "0.4", CVAR_ARCHIVE},
	{ &cg_hudFiles, "cg_hudFiles", "ui/hud.txt", CVAR_ARCHIVE},

	{ &cg_teamChatsOnly, "cg_teamChatsOnly", "0", CVAR_ARCHIVE },
	// the following variables are created in other parts of the system,
	// but we also reference them here

	{ &cg_buildScript, "com_buildScript", "0", 0 },   // force loading of all possible data amd error on failures
	{ &cg_paused, "cl_paused", "0", CVAR_ROM },

	{ &cg_blood, "com_blood", "1", CVAR_ARCHIVE },
	{ &cg_synchronousClients, "g_syncronousClients", "0", 0 },    // communicated by systeminfo
	{ &cg_currentSelectedPlayer, "cg_currentSelectedPlayer", "0", CVAR_ARCHIVE},
	{ &cg_currentSelectedPlayerName, "cg_currentSelectedPlayerName", "", CVAR_ARCHIVE},

	// Rafael - particle switch
	{ &cg_wolfparticles, "cg_wolfparticles", "1", CVAR_ARCHIVE },
	// done

	// Ridah
	{ &cg_gameType, "g_gametype", "0", 0 }, // communicated by systeminfo
	{ &cg_norender, "cg_norender", "0", 0 },  // only used during single player, to suppress rendering until the server is ready

	{ &cg_gameSkill, "g_gameskill", "2", 0 }, // communicated by systeminfo	// (SA) new default '2' (was '1')

	{ &cg_reloading, "g_reloading", "0", 0 }, //----(SA)	added

	// JPW NERVE
	{ &cg_medicChargeTime,  "g_medicChargeTime", "10000", 0 }, // communicated by systeminfo
	{ &cg_LTChargeTime, "g_LTChargeTime", "30000", 0 }, // communicated by systeminfo
	{ &cg_engineerChargeTime,   "g_engineerChargeTime", "30000", 0 }, // communicated by systeminfo
	{ &cg_soldierChargeTime,    "g_soldierChargeTime", "20000", 0 }, // communicated by systeminfo
	{ &cg_bluelimbotime,        "g_bluelimbotime", "30000", 0 }, // communicated by systeminfo
	{ &cg_redlimbotime,         "g_redlimbotime","30000", 0 }, // communicated by systeminfo
	// jpw

	{ &cg_notebook, "cl_notebook", "0", CVAR_ROM },
	{ &cg_notebookpages, "cg_notebookpages", "0", CVAR_ROM},
//	{ &cg_youGotMail, "cg_youGotMail", "0", CVAR_ROM},	// used to display notebook new-info icon
	{ &cg_youGotMail, "cg_youGotMail", "0", 0},   // used to display notebook new-info icon

	{ &cg_animState, "cg_animState", "0", CVAR_CHEAT},
	{ &cg_missionStats, "g_missionStats", "0", CVAR_ROM},
	{ &cg_waitForFire, "cl_waitForFire", "0", CVAR_ROM},

	{ &cg_loadWeaponSelect, "cg_loadWeaponSelect", "0", CVAR_ROM},

	{ &cg_expectedhunkusage, "com_expectedhunkusage", "0", CVAR_ROM},

	// NERVE - SMF
	{ &mp_playerType, "mp_playerType", "0", 0 },
	{ &mp_team, "mp_team", "0", 0 },
	{ &mp_weapon, "mp_weapon", "0", 0 },
	{ &mp_pistol, "mp_pistol", "0", 0 },
	{ &mp_item1, "mp_item1", "0", 0 },
	{ &mp_item2, "mp_item2", "0", 0 },
	{ &mp_mapDesc, "mp_mapDesc", "", 0 },
	{ &mp_mapTitle, "mp_mapTitle", "", 0 },
	{ &mp_itemDesc, "mp_itemDesc", "", 0 },
	// -NERVE - SMF

	{ &cg_showAIState, "cg_showAIState", "0", CVAR_CHEAT},
};
int cvarTableSize = sizeof( cvarTable ) / sizeof( cvarTable[0] );

/*
=================
CG_RegisterCvars
=================
*/
void CG_RegisterCvars( void ) {
	int i;
	cvarTable_t *cv;
	char var[MAX_TOKEN_CHARS];

	engine->trap_Cvar_Set( "cg_letterbox", "0" ); // force this for people who might have it in their

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		engine->trap_Cvar_Register( cv->vmCvar, cv->cvarName,
							cv->defaultString, cv->cvarFlags );
	}

	// see if we are also running the server on this machine
	engine->trap_Cvar_VariableStringBuffer( "sv_running", var, sizeof( var ) );
	cgs.localServer = atoi( var );

	forceModelModificationCount = cg_forceModel.modificationCount;

	engine->trap_Cvar_Register( NULL, "model", DEFAULT_MODEL, CVAR_USERINFO | CVAR_ARCHIVE );
	engine->trap_Cvar_Register( NULL, "head", DEFAULT_HEAD, CVAR_USERINFO | CVAR_ARCHIVE );


}

/*
===================
CG_ForceModelChange
===================
*/
// TTimo: unused
/*
static void CG_ForceModelChange( void ) {
	int		i;

	for (i=0 ; i<MAX_CLIENTS ; i++) {
		const char		*clientInfo;

		clientInfo = CG_ConfigString( CS_PLAYERS+i );
		if ( !clientInfo[0] ) {
			continue;
		}
		CG_NewClientInfo( i );
	}
}
*/

/*
=================
CG_UpdateCvars
=================
*/
void CG_UpdateCvars( void ) {
	int i;
	cvarTable_t *cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		engine->trap_Cvar_Update( cv->vmCvar );
	}
/* RF, disabled this, not needed anymore
	// if force model changed
	if ( forceModelModificationCount != cg_forceModel.modificationCount ) {
		forceModelModificationCount = cg_forceModel.modificationCount;
		CG_ForceModelChange();
	}
*/
}


int CG_CrosshairPlayer( void ) {
	if ( cg.time > ( cg.crosshairClientTime + 1000 ) ) {
		return -1;
	}
	return cg.crosshairClientNum;
}

int CG_LastAttacker( void ) {
	if ( !cg.attackerTime ) {
		return -1;
	}
	return cg.snap->ps.persistant[PERS_ATTACKER];
}

void QDECL CG_Printf( const char *msg, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, msg );
	vsprintf( text, msg, argptr );
	va_end( argptr );

	engine->trap_Print( text );
}

void QDECL CG_Error( const char *msg, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, msg );
	vsprintf( text, msg, argptr );
	va_end( argptr );

	engine->trap_Error( text );
}

// TTimo: was commented out for Mac, guarding
#if !defined( CGAME_HARD_LINKED ) || defined( __MACOS__ )
// this is only here so the functions in q_shared.c and bg_*.c can link (FIXME)

void QDECL Com_Error( int level, const char *error, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, error );
	vsprintf( text, error, argptr );
	va_end( argptr );

	CG_Error( "%s", text );
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, msg );
	vsprintf( text, msg, argptr );
	va_end( argptr );

	CG_Printf( "%s", text );
}

#endif

/*
================
CG_Argv
================
*/
const char *CG_Argv( int arg ) {
	static char buffer[MAX_STRING_CHARS];

	engine->trap_Argv( arg, buffer, sizeof( buffer ) );

	return buffer;
}


//========================================================================
void CG_SetupDlightstyles( void ) {
	int i, j;
	char        *str;
	char        *token;
	int entnum;
	centity_t   *cent;

	cg.lightstylesInited = qtrue;

	for ( i = 1; i < MAX_DLIGHT_CONFIGSTRINGS; i++ )
	{
		str = (char *) CG_ConfigString( CS_DLIGHTS + i );
		if ( !strlen( str ) ) {
			break;
		}

		token = COM_Parse( &str );   // ent num
		entnum = atoi( token );
		cent = &cg_entities[entnum];

		token = COM_Parse( &str );   // stylestring
		Q_strncpyz( cent->dl_stylestring, token, strlen( token ) );

		token = COM_Parse( &str );   // offset
		cent->dl_frame      = atoi( token );
		cent->dl_oldframe   = cent->dl_frame - 1;
		if ( cent->dl_oldframe < 0 ) {
			cent->dl_oldframe = strlen( cent->dl_stylestring );
		}

		token = COM_Parse( &str );   // sound id
		cent->dl_sound = atoi( token );

		token = COM_Parse( &str );   // attenuation
		cent->dl_atten = atoi( token );

		for ( j = 0; j < strlen( cent->dl_stylestring ); j++ ) {

			cent->dl_stylestring[j] += cent->dl_atten;  // adjust character for attenuation/amplification

			// clamp result
			if ( cent->dl_stylestring[j] < 'a' ) {
				cent->dl_stylestring[j] = 'a';
			}
			if ( cent->dl_stylestring[j] > 'z' ) {
				cent->dl_stylestring[j] = 'z';
			}
		}

		cent->dl_backlerp   = 0.0;
		cent->dl_time       = cg.time;
	}

}

//========================================================================

/*
=================
CG_RegisterItemSounds

The server says this item is used on this level
=================
*/
static void CG_RegisterItemSounds( int itemNum ) {
	gitem_t         *item;
	char data[MAX_QPATH];
	char            *s, *start;
	int len;

	item = &bg_itemlist[ itemNum ];

	if ( item->pickup_sound ) {
		engine->trap_S_RegisterSound( item->pickup_sound );
	}

	// parse the space seperated precache string for other media
	s = item->sounds;
	if ( !s || !s[0] ) {
		return;
	}

	while ( *s ) {
		start = s;
		while ( *s && *s != ' ' ) {
			s++;
		}

		len = s - start;
		if ( len >= MAX_QPATH || len < 5 ) {
			CG_Error( "PrecacheItem: %s has bad precache string",
					  item->classname );
			return;
		}
		memcpy( data, start, len );
		data[len] = 0;
		if ( *s ) {
			s++;
		}

		if ( !strcmp( data + len - 3, "wav" ) ) {
			engine->trap_S_RegisterSound( data );
		}
	}
}


//----(SA)	added

// this is the only thing that sets a cap on # items.  would like it to be adaptable.
// (rather than 256 max items with pickup name fixed at 32 chars)

/*
==============
CG_LoadPickupNames
==============
*/
#define MAX_BUFFER          20000
static void CG_LoadPickupNames( void ) {
	char buffer[MAX_BUFFER];
	char *text;
	char filename[MAX_QPATH];
	fileHandle_t f;
	int len, i;
	char *token;

	Com_sprintf( filename, MAX_QPATH, "text/pickupnames.txt" );
	len = engine->trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		CG_Printf( S_COLOR_RED "WARNING: pickup name file (pickupnames.txt not found in main/text)\n" );
		return;
	}
	if ( len > MAX_BUFFER ) {
		CG_Error( "%s is too big, make it smaller (max = %i bytes)\n", filename, MAX_BUFFER );
	}

	// load the file into memory
	engine->trap_FS_Read( buffer, len, f );
	buffer[len] = 0;
	engine->trap_FS_FCloseFile( f );
	// parse the list
	text = buffer;

	for ( i = 0; i < bg_numItems; i++ ) {
		token = COM_ParseExt( &text, qtrue );
		if ( !token[0] ) {
			break;
		}
		if ( !Q_stricmp( token, "---" ) ) {   // no name.  use hardcoded value
			if ( bg_itemlist[i].pickup_name && strlen( bg_itemlist[i].pickup_name ) ) {
				Com_sprintf( cgs.itemPrintNames[i], MAX_QPATH, bg_itemlist[ i ].pickup_name );
			} else {
				cgs.itemPrintNames[i][0] = 0;
			}
		} else {
			Com_sprintf( cgs.itemPrintNames[i], MAX_QPATH, token );
		}
	}
}

// a straight dupe right now so I don't mess anything up while adding this
static void CG_LoadTranslationStrings( void ) {
	
}


static void CG_LoadTranslateStrings( void ) {
	CG_LoadPickupNames();
	CG_LoadTranslationStrings();    // right now just centerprint
}

//----(SA)	end


/*
=================
CG_RegisterSounds

called during a precache command
=================
*/
static void CG_RegisterSounds( void ) {
	int i;
	char items[MAX_ITEMS + 1];
	char name[MAX_QPATH];
	const char  *soundName;

	// Ridah, init sound scripts
	CG_SoundInit();
	// done.

	cgs.media.n_health = engine->trap_S_RegisterSound( "sound/items/n_health.wav" );
	cgs.media.noFireUnderwater = engine->trap_S_RegisterSound( "sound/weapons/underwaterfire.wav" ); //----(SA)	added

	cgs.media.snipersound = engine->trap_S_RegisterSound( "sound/weapons/mauser/mauserf1.wav" );
	cgs.media.tracerSound = engine->trap_S_RegisterSound( "sound/weapons/machinegun/buletby1.wav" );
	cgs.media.selectSound = engine->trap_S_RegisterSound( "sound/weapons/change.wav" );
	cgs.media.wearOffSound = engine->trap_S_RegisterSound( "sound/items/wearoff.wav" );
	cgs.media.useNothingSound = engine->trap_S_RegisterSound( "sound/items/use_nothing.wav" );
	cgs.media.gibSound = engine->trap_S_RegisterSound( "sound/player/gibsplt1.wav" );
	cgs.media.gibBounce1Sound = engine->trap_S_RegisterSound( "sound/player/gibimp1.wav" );
	cgs.media.gibBounce2Sound = engine->trap_S_RegisterSound( "sound/player/gibimp2.wav" );
	cgs.media.gibBounce3Sound = engine->trap_S_RegisterSound( "sound/player/gibimp3.wav" );

//	cgs.media.teleInSound = engine->trap_S_RegisterSound( "sound/world/telein.wav" );
//	cgs.media.teleOutSound = engine->trap_S_RegisterSound( "sound/world/teleout.wav" );
//	cgs.media.respawnSound = engine->trap_S_RegisterSound( "sound/items/respawn1.wav" );


	cgs.media.grenadebounce[GRENBOUNCE_DEFAULT][0]  = engine->trap_S_RegisterSound( "sound/weapons/grenade/hgrenb1a.wav" );
	cgs.media.grenadebounce[GRENBOUNCE_DEFAULT][1]  = engine->trap_S_RegisterSound( "sound/weapons/grenade/hgrenb2a.wav" );
	cgs.media.grenadebounce[GRENBOUNCE_DIRT][0]     = engine->trap_S_RegisterSound( "sound/weapons/grenade/hg_dirt1a.wav" );
	cgs.media.grenadebounce[GRENBOUNCE_DIRT][1]     = engine->trap_S_RegisterSound( "sound/weapons/grenade/hg_dirt2a.wav" );
	cgs.media.grenadebounce[GRENBOUNCE_WOOD][0]     = engine->trap_S_RegisterSound( "sound/weapons/grenade/hg_wood1a.wav" );
	cgs.media.grenadebounce[GRENBOUNCE_WOOD][1]     = engine->trap_S_RegisterSound( "sound/weapons/grenade/hg_wood2a.wav" );
	cgs.media.grenadebounce[GRENBOUNCE_METAL][0]    = engine->trap_S_RegisterSound( "sound/weapons/grenade/hg_metal1a.wav" );
	cgs.media.grenadebounce[GRENBOUNCE_METAL][1]    = engine->trap_S_RegisterSound( "sound/weapons/grenade/hg_metal2a.wav" );

	cgs.media.dynamitebounce1 = engine->trap_S_RegisterSound( "sound/weapons/dynamite/dynamite_bounce.wav" );

	cgs.media.fbarrelexp1 = engine->trap_S_RegisterSound( "sound/weapons/flamebarrel/fbarrela.wav" );
	cgs.media.fbarrelexp2 = engine->trap_S_RegisterSound( "sound/weapons/flamebarrel/fbarrelb.wav" );


	cgs.media.fkickwall = engine->trap_S_RegisterSound( "sound/weapons/melee/fstatck.wav" );
	cgs.media.fkickflesh = engine->trap_S_RegisterSound( "sound/weapons/melee/fstatck.wav" );
	cgs.media.fkickmiss = engine->trap_S_RegisterSound( "sound/weapons/melee/fstmiss.wav" );


	cgs.media.noAmmoSound = engine->trap_S_RegisterSound( "sound/weapons/noammo.wav" );

	cgs.media.talkSound = engine->trap_S_RegisterSound( "sound/player/talk.wav" );
	cgs.media.landSound = engine->trap_S_RegisterSound( "sound/player/land1.wav" );

	cgs.media.watrInSound = engine->trap_S_RegisterSound( "sound/player/watr_in.wav" );
	cgs.media.watrOutSound = engine->trap_S_RegisterSound( "sound/player/watr_out.wav" );
	cgs.media.watrUnSound = engine->trap_S_RegisterSound( "sound/player/watr_un.wav" );

	cgs.media.underWaterSound = engine->trap_S_RegisterSound( "sound/world/underwater03.wav" );

	for ( i = 0 ; i < 4 ; i++ ) {
		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/step%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_NORMAL][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/boot%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_BOOT][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/flesh%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_FLESH][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/mech%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_MECH][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/energy%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_ENERGY][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/splash%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_SPLASH][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/clank%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_METAL][i] = engine->trap_S_RegisterSound( name );


		// (SA) Wolf footstep sound registration
		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/wood%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_WOOD][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/grass%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_GRASS][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/gravel%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_GRAVEL][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/roof%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_ROOF][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/snow%i.wav", i + 1 );
		cgs.media.footsteps[FOOTSTEP_SNOW][i] = engine->trap_S_RegisterSound( name );

		Com_sprintf( name, sizeof( name ), "sound/player/footsteps/carpet%i.wav", i + 1 );    //----(SA)
		cgs.media.footsteps[FOOTSTEP_CARPET][i] = engine->trap_S_RegisterSound( name );
	}

	// only register the items that the server says we need
	strcpy( items, CG_ConfigString( CS_ITEMS ) );

	for ( i = 1 ; i < bg_numItems ; i++ ) {
		if ( items[ i ] == '1' || cg_buildScript.integer ) {
			CG_RegisterItemSounds( i );
		}
	}

	for ( i = 1 ; i < MAX_SOUNDS ; i++ ) {
		soundName = CG_ConfigString( CS_SOUNDS + i );
		if ( !soundName[0] ) {
			break;
		}
		if ( soundName[0] == '*' ) {
			continue;   // custom sound
		}

		// Ridah, register sound scripts seperately
		if ( !strstr( soundName, ".wav" ) ) {
			cgs.gameSounds[i] = CG_SoundScriptPrecache( soundName );    //----(SA)	shouldn't this be okay?  The cs index is reserved anyway, so it can't hurt, right?
			cgs.gameSoundTypes[i] = 2;
		} else {
			cgs.gameSounds[i] = engine->trap_S_RegisterSound( soundName );
			cgs.gameSoundTypes[i] = 1;
		}
	}

	//----(SA)	added
	cgs.media.grenadePulseSound4 = engine->trap_S_RegisterSound( "sound/weapons/grenade/grenpulse4.wav" );
	cgs.media.grenadePulseSound3 = engine->trap_S_RegisterSound( "sound/weapons/grenade/grenpulse3.wav" );
	cgs.media.grenadePulseSound2 = engine->trap_S_RegisterSound( "sound/weapons/grenade/grenpulse2.wav" );
	cgs.media.grenadePulseSound1 = engine->trap_S_RegisterSound( "sound/weapons/grenade/grenpulse1.wav" );
	//----(SA)	end

	//----(SA) added
	cgs.media.debBounce1Sound = engine->trap_S_RegisterSound( "sound/world/block.wav" );
	cgs.media.debBounce2Sound = engine->trap_S_RegisterSound( "sound/world/brick.wav" );
	cgs.media.debBounce3Sound = engine->trap_S_RegisterSound( "sound/world/brick2.wav" );

	// Ridah
	cgs.media.flameSound = engine->trap_S_RegisterSound( "sound/weapons/flamethrower/fl_fire.wav" );
	cgs.media.flameBlowSound = engine->trap_S_RegisterSound( "sound/weapons/flamethrower/fl_blow.wav" );
	cgs.media.flameStartSound = engine->trap_S_RegisterSound( "sound/weapons/flamethrower/fl_start.wav" );
	cgs.media.flameStreamSound = engine->trap_S_RegisterSound( "sound/weapons/flamethrower/fl_stream.wav" );
	cgs.media.flameCrackSound = engine->trap_S_RegisterSound( "sound/world/firecrack1.wav" );
	cgs.media.boneBounceSound = engine->trap_S_RegisterSound( "sound/world/boardbreak.wav" );    // TODO: need a real sound for this

	cgs.media.lightningSounds[0] = engine->trap_S_RegisterSound( "sound/world/electzap1.wav" );
	cgs.media.lightningSounds[1] = engine->trap_S_RegisterSound( "sound/world/electzap2.wav" );
	cgs.media.lightningSounds[2] = engine->trap_S_RegisterSound( "sound/world/electzap3.wav" );
	cgs.media.lightningZap = engine->trap_S_RegisterSound( "sound/world/electrocute.wav" );

	// precache sound scripts that get called from the cgame
	cgs.media.bulletHitFleshScript = CG_SoundScriptPrecache( "bulletHitFlesh" );
	cgs.media.bulletHitFleshMetalScript = CG_SoundScriptPrecache( "bulletHitFleshMetal" );

	cgs.media.teslaZapScript = CG_SoundScriptPrecache( "teslaZap" );
	cgs.media.teslaLoopSound = engine->trap_S_RegisterSound( "sound/weapons/tesla/loop.wav" );

	cgs.media.batsFlyingLoopSound = engine->trap_S_RegisterSound( "sound/world/bats_flying.wav" );

	// FIXME: only needed with item
//	cgs.media.flightSound = engine->trap_S_RegisterSound( "sound/items/flight.wav" );
//	cgs.media.medkitSound = engine->trap_S_RegisterSound ("sound/items/use_medkit.wav");
	cgs.media.elecSound = engine->trap_S_RegisterSound( "sound/items/use_elec.wav" );
	cgs.media.fireSound = engine->trap_S_RegisterSound( "sound/items/use_fire.wav" );
	cgs.media.waterSound = engine->trap_S_RegisterSound( "sound/items/use_water.wav" );
	cgs.media.wineSound = engine->trap_S_RegisterSound( "sound/pickup/holdable/use_wine.wav" );       //----(SA)	modified
	cgs.media.bookSound = engine->trap_S_RegisterSound( "sound/pickup/holdable/use_book.wav" );       //----(SA)	added
	cgs.media.staminaSound = engine->trap_S_RegisterSound( "sound/pickup/holdable/use_stamina.wav" ); //----(SA)	added
	cgs.media.quadSound = engine->trap_S_RegisterSound( "sound/items/damage3.wav" );
	cgs.media.sfx_ric1 = engine->trap_S_RegisterSound( "sound/weapons/machinegun/ric1.wav" );
	cgs.media.sfx_ric2 = engine->trap_S_RegisterSound( "sound/weapons/machinegun/ric2.wav" );
	cgs.media.sfx_ric3 = engine->trap_S_RegisterSound( "sound/weapons/machinegun/ric3.wav" );
//	cgs.media.sfx_railg = engine->trap_S_RegisterSound ("sound/weapons/railgun/railgf1a.wav");
	cgs.media.sfx_rockexp = engine->trap_S_RegisterSound( "sound/weapons/rocket/rocklx1a.wav" );
	cgs.media.sfx_dynamiteexp = engine->trap_S_RegisterSound( "sound/weapons/dynamite/dynamite_exp.wav" );
	cgs.media.sfx_dynamiteexpDist = engine->trap_S_RegisterSound( "sound/weapons/dynamite/dynamite_exp_dist.wav" );   //----(SA)	added


	cgs.media.sfx_spearhit = engine->trap_S_RegisterSound( "sound/weapons/speargun/spearhit.wav" );

	cgs.media.sfx_knifehit[0] = engine->trap_S_RegisterSound( "sound/weapons/knife/knife_hit1.wav" ); // hitting player
	cgs.media.sfx_knifehit[1] = engine->trap_S_RegisterSound( "sound/weapons/knife/knife_hit2.wav" );
	cgs.media.sfx_knifehit[2] = engine->trap_S_RegisterSound( "sound/weapons/knife/knife_hit3.wav" );
	cgs.media.sfx_knifehit[3] = engine->trap_S_RegisterSound( "sound/weapons/knife/knife_hit4.wav" );

	cgs.media.sfx_knifehit[4] = engine->trap_S_RegisterSound( "sound/weapons/knife/knife_hitwall1.wav" ); // hitting wall

	cgs.media.sfx_bullet_metalhit[0] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_metal1.wav" );
	cgs.media.sfx_bullet_metalhit[1] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_metal2.wav" );
	cgs.media.sfx_bullet_metalhit[2] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_metal3.wav" );

	cgs.media.sfx_bullet_woodhit[0] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_wood1.wav" );
	cgs.media.sfx_bullet_woodhit[1] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_wood2.wav" );
	cgs.media.sfx_bullet_woodhit[2] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_wood3.wav" );

	cgs.media.sfx_bullet_roofhit[0] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_roof1.wav" );
	cgs.media.sfx_bullet_roofhit[1] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_roof2.wav" );
	cgs.media.sfx_bullet_roofhit[2] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_roof3.wav" );

	cgs.media.sfx_bullet_ceramichit[0] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_ceramic1.wav" );
	cgs.media.sfx_bullet_ceramichit[1] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_ceramic2.wav" );
	cgs.media.sfx_bullet_ceramichit[2] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_ceramic3.wav" );

	cgs.media.sfx_bullet_glasshit[0] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_glass1.wav" );
	cgs.media.sfx_bullet_glasshit[1] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_glass2.wav" );
	cgs.media.sfx_bullet_glasshit[2] = engine->trap_S_RegisterSound( "sound/weapons/bullethit_glass3.wav" );


	cgs.media.sparkSounds[0] = engine->trap_S_RegisterSound( "sound/world/saarc2.wav" );
	cgs.media.sparkSounds[1] = engine->trap_S_RegisterSound( "sound/world/arc2.wav" );


//----(SA)	doors and kick

	//----(SA)	removed some unnecessary stuff

	engine->trap_S_RegisterSound( "sound/weapons/melee/fstatck.wav" );
	engine->trap_S_RegisterSound( "sound/weapons/melee/fstmiss.wav" );

	engine->trap_S_RegisterSound( "sound/Loogie/spit.wav" );
	engine->trap_S_RegisterSound( "sound/Loogie/sizzle.wav" );

}


//===================================================================================



/*
=================
CG_RegisterGraphics

This function may execute for a couple of minutes with a slow disk.
=================
*/
static void CG_RegisterGraphics( void ) {
	char name[1024];

	int i;
	char items[MAX_ITEMS + 1];
	static char     *sb_nums[11] = {
		"gfx/2d/numbers/zero_32b",
		"gfx/2d/numbers/one_32b",
		"gfx/2d/numbers/two_32b",
		"gfx/2d/numbers/three_32b",
		"gfx/2d/numbers/four_32b",
		"gfx/2d/numbers/five_32b",
		"gfx/2d/numbers/six_32b",
		"gfx/2d/numbers/seven_32b",
		"gfx/2d/numbers/eight_32b",
		"gfx/2d/numbers/nine_32b",
		"gfx/2d/numbers/minus_32b",
	};


	// clear any references to old media
	memset( &cg.refdef, 0, sizeof( cg.refdef ) );
	engine->trap_R_ClearScene();

	CG_LoadingString( cgs.mapname );

	engine->trap_R_LoadWorldMap( cgs.mapname );

	// precache status bar pics
	CG_LoadingString( "game media" );


	CG_LoadingString( " - textures" );

	for ( i = 0 ; i < 11 ; i++ ) {
		cgs.media.numberShaders[i] = engine->trap_R_RegisterShader( sb_nums[i] );
	}


	cgs.media.smokePuffShader = engine->trap_R_RegisterShader( "smokePuff" );

	// Rafael - blood pool
	//cgs.media.bloodPool = engine->trap_R_RegisterShader ("bloodPool");

	// RF, blood cloud
	cgs.media.bloodCloudShader = engine->trap_R_RegisterShader( "bloodCloud" );

	// Rafael - cannon
	cgs.media.smokePuffShaderdirty = engine->trap_R_RegisterShader( "smokePuffdirty" );
	cgs.media.smokePuffShaderb1 = engine->trap_R_RegisterShader( "smokePuffblack1" );
	cgs.media.smokePuffShaderb2 = engine->trap_R_RegisterShader( "smokePuffblack2" );
	cgs.media.smokePuffShaderb3 = engine->trap_R_RegisterShader( "smokePuffblack3" );
	cgs.media.smokePuffShaderb4 = engine->trap_R_RegisterShader( "smokePuffblack4" );
	cgs.media.smokePuffShaderb5 = engine->trap_R_RegisterShader( "smokePuffblack5" );
	// done

	// Rafael - bleedanim
	for ( i = 0; i < 5; i++ ) {
		cgs.media.viewBloodAni[i] = engine->trap_R_RegisterShader( va( "viewBloodBlend%i", i + 1 ) );
	}
	cgs.media.viewFlashBlood = engine->trap_R_RegisterShader( "viewFlashBlood" );
	for ( i = 0; i < 16; i++ ) {
		cgs.media.viewFlashFire[i] = engine->trap_R_RegisterShader( va( "viewFlashFire%i", i + 1 ) );
	}
	// done

	// Rafael bats
	for ( i = 0; i < 10; i++ ) {
		cgs.media.bats[i] = engine->trap_R_RegisterShader( va( "bats%i",i + 1 ) );
	}
	// done

	cgs.media.smokePuffRageProShader = engine->trap_R_RegisterShader( "smokePuffRagePro" );
	cgs.media.shotgunSmokePuffShader = engine->trap_R_RegisterShader( "shotgunSmokePuff" );

	cgs.media.bloodTrailShader = engine->trap_R_RegisterShader( "bloodTrail" );
	cgs.media.lagometerShader = engine->trap_R_RegisterShader( "lagometer" );
	cgs.media.connectionShader = engine->trap_R_RegisterShader( "disconnected" );

	cgs.media.nailPuffShader = engine->trap_R_RegisterShader( "nailtrail" );


//	cgs.media.reticleShaderSimple = engine->trap_R_RegisterShader( "gfx/misc/reticlesimple" );		// TODO: remove
	cgs.media.reticleShaderSimpleQ = engine->trap_R_RegisterShader( "gfx/misc/reticlesimple_quarter" );

	cgs.media.snooperShaderSimple = engine->trap_R_RegisterShader( "gfx/misc/snoopersimple" );

//	cgs.media.binocShaderSimple = engine->trap_R_RegisterShader( "gfx/misc/binocsimple" );			// TODO: remove
	cgs.media.binocShaderSimpleQ = engine->trap_R_RegisterShader( "gfx/misc/binocsimple_quarter" );  //----(SA)	added


	// Rafael
	// cgs.media.snowShader = engine->trap_R_RegisterShader ( "snowPuff" );
	cgs.media.snowShader = engine->trap_R_RegisterShader( "snow_tri" );

	cgs.media.oilParticle = engine->trap_R_RegisterShader( "oilParticle" );
	cgs.media.oilSlick = engine->trap_R_RegisterShader( "oilSlick" );

	cgs.media.waterBubbleShader = engine->trap_R_RegisterShader( "waterBubble" );

	cgs.media.tracerShader = engine->trap_R_RegisterShader( "gfx/misc/tracer" );
	cgs.media.selectShader = engine->trap_R_RegisterShader( "gfx/2d/select" );


	cgs.media.hintShaders[HINT_ACTIVATE]            = engine->trap_R_RegisterShader( "gfx/2d/usableHint" );
	cgs.media.hintShaders[HINT_NOACTIVATE]          = engine->trap_R_RegisterShader( "gfx/2d/notUsableHint" );
	cgs.media.hintShaders[HINT_DOOR]                = engine->trap_R_RegisterShader( "gfx/2d/doorHint" );
	cgs.media.hintShaders[HINT_DOOR_ROTATING]       = engine->trap_R_RegisterShader( "gfx/2d/doorRotateHint" );
	cgs.media.hintShaders[HINT_DOOR_LOCKED]         = engine->trap_R_RegisterShader( "gfx/2d/doorLockHint" );
	cgs.media.hintShaders[HINT_DOOR_ROTATING_LOCKED] = engine->trap_R_RegisterShader( "gfx/2d/doorRotateLockHint" );
	cgs.media.hintShaders[HINT_MG42]                = engine->trap_R_RegisterShader( "gfx/2d/mg42Hint" );
	cgs.media.hintShaders[HINT_BREAKABLE]           = engine->trap_R_RegisterShader( "gfx/2d/breakableHint" );
	cgs.media.hintShaders[HINT_CHAIR]               = engine->trap_R_RegisterShader( "gfx/2d/chairHint" );
	cgs.media.hintShaders[HINT_ALARM]               = engine->trap_R_RegisterShader( "gfx/2d/alarmHint" );
	cgs.media.hintShaders[HINT_HEALTH]              = engine->trap_R_RegisterShader( "gfx/2d/healthHint" );
	cgs.media.hintShaders[HINT_TREASURE]            = engine->trap_R_RegisterShader( "gfx/2d/treasureHint" );
	cgs.media.hintShaders[HINT_KNIFE]               = engine->trap_R_RegisterShader( "gfx/2d/knifeHint" );
	cgs.media.hintShaders[HINT_LADDER]              = engine->trap_R_RegisterShader( "gfx/2d/ladderHint" );
	cgs.media.hintShaders[HINT_BUTTON]              = engine->trap_R_RegisterShader( "gfx/2d/buttonHint" );
	cgs.media.hintShaders[HINT_WATER]               = engine->trap_R_RegisterShader( "gfx/2d/waterHint" );
	cgs.media.hintShaders[HINT_CAUTION]             = engine->trap_R_RegisterShader( "gfx/2d/cautionHint" );
	cgs.media.hintShaders[HINT_DANGER]              = engine->trap_R_RegisterShader( "gfx/2d/dangerHint" );
	cgs.media.hintShaders[HINT_SECRET]              = engine->trap_R_RegisterShader( "gfx/2d/secretHint" );
	cgs.media.hintShaders[HINT_QUESTION]            = engine->trap_R_RegisterShader( "gfx/2d/questionHint" );
	cgs.media.hintShaders[HINT_EXCLAMATION]         = engine->trap_R_RegisterShader( "gfx/2d/exclamationHint" );
	cgs.media.hintShaders[HINT_CLIPBOARD]           = engine->trap_R_RegisterShader( "gfx/2d/clipboardHint" );
	cgs.media.hintShaders[HINT_WEAPON]              = engine->trap_R_RegisterShader( "gfx/2d/weaponHint" );
	cgs.media.hintShaders[HINT_AMMO]                = engine->trap_R_RegisterShader( "gfx/2d/ammoHint" );
	cgs.media.hintShaders[HINT_ARMOR]               = engine->trap_R_RegisterShader( "gfx/2d/armorHint" );
	cgs.media.hintShaders[HINT_POWERUP]             = engine->trap_R_RegisterShader( "gfx/2d/powerupHint" );
	cgs.media.hintShaders[HINT_HOLDABLE]            = engine->trap_R_RegisterShader( "gfx/2d/holdableHint" );
	cgs.media.hintShaders[HINT_INVENTORY]           = engine->trap_R_RegisterShader( "gfx/2d/inventoryHint" );
	cgs.media.hintShaders[HINT_EXIT]                = engine->trap_R_RegisterShader( "gfx/2d/exitHint" );
	cgs.media.hintShaders[HINT_NOEXIT]              = cgs.media.hintShaders[HINT_EXIT];
	cgs.media.hintShaders[HINT_EXIT_FAR]            = cgs.media.hintShaders[HINT_EXIT];
	cgs.media.hintShaders[HINT_NOEXIT_FAR]          = cgs.media.hintShaders[HINT_EXIT];

//	cgs.media.hintShaders[HINT_PLYR_FRIEND]			= engine->trap_R_RegisterShader( "gfx/2d/hintPlrFriend" );
//	cgs.media.hintShaders[HINT_PLYR_NEUTRAL]		= engine->trap_R_RegisterShader( "gfx/2d/hintPlrNeutral" );
//	cgs.media.hintShaders[HINT_PLYR_ENEMY]			= engine->trap_R_RegisterShader( "gfx/2d/hintPlrEnemy" );
//	cgs.media.hintShaders[HINT_PLYR_UNKNOWN]		= engine->trap_R_RegisterShader( "gfx/2d/hintPlrUnknown" );

//	cgs.media.hintShaders[HINT_BUILD]				= engine->trap_R_RegisterShader( "gfx/2d/buildHint" );		// DHM - Nerve

	cgs.media.youGotMailShader      = engine->trap_R_RegisterShader( "gfx/2d/yougotmail" );    //----(SA)	added
	cgs.media.youGotObjectiveShader = engine->trap_R_RegisterShader( "gfx/2d/yougotobjective" );   //----(SA)	added

//----(SA)	end

	for ( i = 0 ; i < NUM_CROSSHAIRS ; i++ ) {
		cgs.media.crosshairShader[i] = engine->trap_R_RegisterShaderNoMip( va( "gfx/2d/crosshair%c", 'a' + i ) );
	}

	cgs.media.crosshairFriendly =  engine->trap_R_RegisterShader( "gfx/2d/friendlycross" );  //----(SA)	added

	cgs.media.backTileShader = engine->trap_R_RegisterShader( "gfx/2d/backtile" );
	cgs.media.noammoShader = engine->trap_R_RegisterShader( "icons/noammo" );

	// powerup shaders
//	cgs.media.quadShader = engine->trap_R_RegisterShader("powerups/quad" );
//	cgs.media.quadWeaponShader = engine->trap_R_RegisterShader("powerups/quadWeapon" );
//	cgs.media.battleSuitShader = engine->trap_R_RegisterShader("powerups/battleSuit" );
//	cgs.media.battleWeaponShader = engine->trap_R_RegisterShader("powerups/battleWeapon" );
//	cgs.media.invisShader = engine->trap_R_RegisterShader("powerups/invisibility" );
//	cgs.media.regenShader = engine->trap_R_RegisterShader("powerups/regen" );
//	cgs.media.hastePuffShader = engine->trap_R_RegisterShader("hasteSmokePuff" );

	// DHM - Nerve :: Allow flags again, will change later to more appropriate models
	if ( cgs.gametype == GT_CTF || cgs.gametype == GT_WOLF || cg_buildScript.integer ) {
		cgs.media.redFlagModel = engine->trap_R_RegisterModel( "models/flags/r_flag.md3" );
		//cgs.media.redFlagModel = engine->trap_R_RegisterModel( "models/powerups/keys/chalice.md3" );
		cgs.media.blueFlagModel = engine->trap_R_RegisterModel( "models/flags/b_flag.md3" );
	}

//	if ( cgs.gametype >= GT_TEAM || cg_buildScript.integer ) {
//		cgs.media.friendShader = engine->trap_R_RegisterShader( "sprites/foe" );
//		cgs.media.redQuadShader = engine->trap_R_RegisterShader("powerups/blueflag" );
//		cgs.media.teamStatusBar = engine->trap_R_RegisterShader( "gfx/2d/colorbar.tga" );
//	}

	CG_LoadingString( " - models" );

	cgs.media.machinegunBrassModel = engine->trap_R_RegisterModel( "models/weapons2/shells/m_shell.md3" );
	cgs.media.panzerfaustBrassModel = engine->trap_R_RegisterModel( "models/weapons2/shells/pf_shell.md3" );
	cgs.media.smallgunBrassModel = engine->trap_R_RegisterModel( "models/weapons2/shells/sm_shell.md3" );

	cgs.media.debBlock[0] = engine->trap_R_RegisterModel( "models/mapobjects/debris/brick1.md3" );
	cgs.media.debBlock[1] = engine->trap_R_RegisterModel( "models/mapobjects/debris/brick2.md3" );
	cgs.media.debBlock[2] = engine->trap_R_RegisterModel( "models/mapobjects/debris/brick3.md3" );
	cgs.media.debBlock[3] = engine->trap_R_RegisterModel( "models/mapobjects/debris/brick4.md3" );
	cgs.media.debBlock[4] = engine->trap_R_RegisterModel( "models/mapobjects/debris/brick5.md3" );
	cgs.media.debBlock[5] = engine->trap_R_RegisterModel( "models/mapobjects/debris/brick6.md3" );

	cgs.media.debRock[0] = engine->trap_R_RegisterModel( "models/mapobjects/debris/rubble1.md3" );
	cgs.media.debRock[1] = engine->trap_R_RegisterModel( "models/mapobjects/debris/rubble2.md3" );
	cgs.media.debRock[2] = engine->trap_R_RegisterModel( "models/mapobjects/debris/rubble3.md3" );


	cgs.media.debWood[0] = engine->trap_R_RegisterModel( "models/gibs/wood/wood1.md3" );
	cgs.media.debWood[1] = engine->trap_R_RegisterModel( "models/gibs/wood/wood2.md3" );
	cgs.media.debWood[2] = engine->trap_R_RegisterModel( "models/gibs/wood/wood3.md3" );
	cgs.media.debWood[3] = engine->trap_R_RegisterModel( "models/gibs/wood/wood4.md3" );
	cgs.media.debWood[4] = engine->trap_R_RegisterModel( "models/gibs/wood/wood5.md3" );
	cgs.media.debWood[5] = engine->trap_R_RegisterModel( "models/gibs/wood/wood6.md3" );
//	cgs.media.debWoodl = engine->trap_R_RegisterModel( "models/mapobjects/debris/woodxl.md3" );
//	cgs.media.debWoodm = engine->trap_R_RegisterModel( "models/mapobjects/debris/woodm.md3" );
//	cgs.media.debWoods = engine->trap_R_RegisterModel( "models/mapobjects/debris/woodsm.md3" );

	cgs.media.debFabric[0] = engine->trap_R_RegisterModel( "models/shards/fabric1.md3" );
	cgs.media.debFabric[1] = engine->trap_R_RegisterModel( "models/shards/fabric2.md3" );
	cgs.media.debFabric[2] = engine->trap_R_RegisterModel( "models/shards/fabric3.md3" );

	//----(SA) end

//	cgs.media.medicReviveShader = engine->trap_R_RegisterShader( "sprites/medic_revive" );	//----(SA)	commented out from MP
	cgs.media.balloonShader = engine->trap_R_RegisterShader( "sprites/balloon3" );

	for ( i = 0; i < MAX_AISTATES; i++ ) {
		cgs.media.aiStateShaders[i] = engine->trap_R_RegisterShader( va( "sprites/aistate%i", i + 1 ) );
	}

	cgs.media.bloodExplosionShader = engine->trap_R_RegisterShader( "bloodExplosion" );

	//cgs.media.bleedExplosionShader = engine->trap_R_RegisterShader( "bleedExplosion" );

	//----(SA)	water splash
	//cgs.media.waterSplashModel = engine->trap_R_RegisterModel("models/weaphits/bullet.md3");
	//cgs.media.waterSplashShader = engine->trap_R_RegisterShader( "waterSplash" );
	//----(SA)	end

	//cgs.media.spearModel = engine->trap_R_RegisterModel("models/weaphits/spear.md3");	//----(SA)

	//cgs.media.bulletFlashModel = engine->trap_R_RegisterModel("models/weaphits/bullet.md3");
	//cgs.media.ringFlashModel = engine->trap_R_RegisterModel("models/weaphits/ring02.md3");
	//cgs.media.dishFlashModel = engine->trap_R_RegisterModel("models/weaphits/boom01.md3");
//	cgs.media.teleportEffectModel = engine->trap_R_RegisterModel( "models/misc/telep.md3" );
//	cgs.media.teleportEffectShader = engine->trap_R_RegisterShader( "teleportEffect" );

//	cgs.media.batModel = engine->trap_R_RegisterModel( "models/mapobjects/bat/bat.md3" );

//	cgs.media.medalImpressive = engine->trap_R_RegisterShaderNoMip( "medal_impressive" );
//	cgs.media.medalExcellent = engine->trap_R_RegisterShaderNoMip( "medal_excellent" );
//	cgs.media.medalGauntlet = engine->trap_R_RegisterShaderNoMip( "medal_gauntlet" );

	// Ridah, spark particles
	cgs.media.sparkParticleShader = engine->trap_R_RegisterShader( "sparkParticle" );
	cgs.media.smokeTrailShader = engine->trap_R_RegisterShader( "smokeTrail" );
//	cgs.media.fireTrailShader = engine->trap_R_RegisterShader( "fireTrail" );
	cgs.media.lightningBoltShader = engine->trap_R_RegisterShader( "lightningBolt" );
	//cgs.media.lightningBoltShaderGreen = engine->trap_R_RegisterShader( "lightningBoltGreen" );	//----(SA)	alternate lightning color
	cgs.media.flamethrowerFireStream = engine->trap_R_RegisterShader( "flamethrowerFireStream" );
	cgs.media.flamethrowerBlueStream = engine->trap_R_RegisterShader( "flamethrowerBlueStream" );
	//cgs.media.flamethrowerFuelStream = engine->trap_R_RegisterShader( "flamethrowerFuelStream" );
	//cgs.media.flamethrowerFuelShader = engine->trap_R_RegisterShader( "flamethrowerFuel" );
	cgs.media.onFireShader2 = engine->trap_R_RegisterShader( "entityOnFire1" );
	cgs.media.onFireShader = engine->trap_R_RegisterShader( "entityOnFire2" );
	//cgs.media.dripWetShader2 = engine->trap_R_RegisterShader( "dripWet2" );
	//cgs.media.dripWetShader = engine->trap_R_RegisterShader( "dripWet1" );
	cgs.media.viewFadeBlack = engine->trap_R_RegisterShader( "viewFadeBlack" );
	cgs.media.sparkFlareShader = engine->trap_R_RegisterShader( "sparkFlareParticle" );

	// spotlight
	// shaders
	cgs.media.spotLightShader = engine->trap_R_RegisterShader( "spotLight" );
	cgs.media.spotLightBeamShader = engine->trap_R_RegisterShader( "lightBeam" );

	// models
	cgs.media.spotLightBaseModel = engine->trap_R_RegisterModel( "models/mapobjects/light/searchlight1_b.md3" );
	cgs.media.spotLightLightModel = engine->trap_R_RegisterModel( "models/mapobjects/light/searchlight1_l.md3" );
	cgs.media.spotLightLightModelBroke = engine->trap_R_RegisterModel( "models/mapobjects/light/searchlight_l_broke.md3" );

	// end spotlight

	cgs.media.lightningHitWallShader = engine->trap_R_RegisterShader( "lightningHitWall" );
	cgs.media.lightningWaveShader = engine->trap_R_RegisterShader( "lightningWave" );
	cgs.media.bulletParticleTrailShader = engine->trap_R_RegisterShader( "bulletParticleTrail" );
	cgs.media.smokeParticleShader = engine->trap_R_RegisterShader( "smokeParticle" );

	// DHM - Nerve :: bullet hitting dirt
	cgs.media.dirtParticle1Shader = engine->trap_R_RegisterShader( "dirt_splash" );
	cgs.media.dirtParticle2Shader = engine->trap_R_RegisterShader( "water_splash" );
	//cgs.media.dirtParticle3Shader = engine->trap_R_RegisterShader( "dirtParticle3" );

	cgs.media.teslaDamageEffectShader = engine->trap_R_RegisterShader( "teslaDamageEffect" );
	cgs.media.teslaAltDamageEffectShader = engine->trap_R_RegisterShader( "teslaAltDamageEffect" );
	cgs.media.viewTeslaDamageEffectShader = engine->trap_R_RegisterShader( "viewTeslaDamageEffect" );
	cgs.media.viewTeslaAltDamageEffectShader = engine->trap_R_RegisterShader( "viewTeslaAltDamageEffect" );
	// done.

	cgs.media.railCoreShader = engine->trap_R_RegisterShader( "railCore" );  // (SA) for debugging server traces


	cgs.media.thirdPersonBinocModel = engine->trap_R_RegisterModel( "models/powerups/holdable/binocs_thirdperson.md3" ); //----(SA)	added
	cgs.media.cigModel = engine->trap_R_RegisterModel( "models/players/infantryss/acc/cig.md3" );    //----(SA)	added

	// RF, not used anymore
	//cgs.media.targetEffectExplosionShader	= engine->trap_R_RegisterShader( "targetEffectExplode" );
	//cgs.media.rocketExplosionShader			= engine->trap_R_RegisterShader( "rocketExplosion" );
	//cgs.media.grenadeExplosionShader		= engine->trap_R_RegisterShader( "grenadeExplosion" );

	// zombie shot
	//cgs.media.zombieLoogie = engine->trap_R_RegisterModel( "models/mapobjects/bodyparts/zom_loog.md3" );
	cgs.media.flamebarrel = engine->trap_R_RegisterModel( "models/furniture/barrel/barrel_a.md3" );
	//----(SA) end

	cgs.media.mg42muzzleflash = engine->trap_R_RegisterModel( "models/weapons2/machinegun/mg42_flash.md3" );
	// cgs.media.mg42muzzleflashgg = engine->trap_R_RegisterModel ("models/weapons2/machinegun/mg42_flash_gg.md3" );

	cgs.media.planemuzzleflash = engine->trap_R_RegisterModel( "models/mapobjects/vehicles/gunflare.md3" );

	cgs.media.crowbar = engine->trap_R_RegisterModel( "models/weapons2/wrench/wrench.md3" );

	// Rafael shards
	cgs.media.shardGlass1 = engine->trap_R_RegisterModel( "models/shards/glass1.md3" );
	cgs.media.shardGlass2 = engine->trap_R_RegisterModel( "models/shards/glass2.md3" );
	cgs.media.shardWood1 = engine->trap_R_RegisterModel( "models/shards/wood1.md3" );
	cgs.media.shardWood2 = engine->trap_R_RegisterModel( "models/shards/wood2.md3" );
	cgs.media.shardMetal1 = engine->trap_R_RegisterModel( "models/shards/metal1.md3" );
	cgs.media.shardMetal2 = engine->trap_R_RegisterModel( "models/shards/metal2.md3" );
	cgs.media.shardCeramic1 = engine->trap_R_RegisterModel( "models/shards/ceramic1.md3" );
	cgs.media.shardCeramic2 = engine->trap_R_RegisterModel( "models/shards/ceramic2.md3" );
	// done

	cgs.media.shardRubble1 = engine->trap_R_RegisterModel( "models/mapobjects/debris/brick000.md3" );
	cgs.media.shardRubble2 = engine->trap_R_RegisterModel( "models/mapobjects/debris/brick001.md3" );
	cgs.media.shardRubble3 = engine->trap_R_RegisterModel( "models/mapobjects/debris/brick002.md3" );

	for ( i = 0; i < MAX_LOCKER_DEBRIS; i++ )
	{
		Com_sprintf( name, sizeof( name ), "models/mapobjects/debris/personal%i.md3", i + 1 );
		cgs.media.shardJunk[i] = engine->trap_R_RegisterModel( name );
	}

	memset( cg_items, 0, sizeof( cg_items ) );
	memset( cg_weapons, 0, sizeof( cg_weapons ) );

	CG_LoadTranslateStrings();  //----(SA)	added.  for localization, read on-screen print names from text file

// TODO: FIXME:  REMOVE REGISTRATION OF EACH MODEL FOR EVERY LEVEL LOAD


	//----(SA)	okay, new stuff to intialize rather than doing it at level load time (or "give all" time)
	//			(I'm certainly not against being efficient here, but I'm tired of the rocket launcher effect only registering
	//			sometimes and want it to work for sure for this demo)

///////////
// code is almost complete for doing this correctly.  will remove when that is complete.
	CG_LoadingString( " - weapons" );
	for ( i = WP_KNIFE; i < WP_GAUNTLET; i++ ) {
//		CG_LoadingString( va("   - %d", i) );
		CG_RegisterWeapon( i );
	}
///////////
// END


	// only register the items that the server says we need
	strcpy( items, CG_ConfigString( CS_ITEMS ) );

	CG_LoadingString( " - items" );

	for ( i = 1 ; i < bg_numItems ; i++ ) {
		if ( items[ i ] == '1' || cg_buildScript.integer ) {

// TODO: get weapons added to the list that are 'set' from a script
			CG_LoadingItem( i );
			CG_RegisterItemVisuals( i );
		}
	}

	// wall marks
	cgs.media.bulletMarkShader = engine->trap_R_RegisterShader( "gfx/damage/bullet_mrk" );
	cgs.media.burnMarkShader = engine->trap_R_RegisterShader( "gfx/damage/burn_med_mrk" );
	cgs.media.holeMarkShader = engine->trap_R_RegisterShader( "gfx/damage/hole_lg_mrk" );
	cgs.media.shadowMarkShader = engine->trap_R_RegisterShader( "markShadow" );
	cgs.media.shadowFootShader = engine->trap_R_RegisterShader( "markShadowFoot" );
	cgs.media.shadowTorsoShader = engine->trap_R_RegisterShader( "markShadowTorso" );
	cgs.media.wakeMarkShader = engine->trap_R_RegisterShader( "wake" );
	cgs.media.wakeMarkShaderAnim = engine->trap_R_RegisterShader( "wakeAnim" ); // (SA)

	cgs.media.bulletMarkShaderMetal = engine->trap_R_RegisterShader( "gfx/damage/metal_mrk" );
	cgs.media.bulletMarkShaderWood = engine->trap_R_RegisterShader( "gfx/damage/wood_mrk" );
	cgs.media.bulletMarkShaderCeramic = engine->trap_R_RegisterShader( "gfx/damage/ceramic_mrk" );
	cgs.media.bulletMarkShaderGlass = engine->trap_R_RegisterShader( "gfx/damage/glass_mrk" );

	for ( i = 0 ; i < 5 ; i++ ) {
		char name[32];
		//Com_sprintf( name, sizeof(name), "textures/decals/blood%i", i+1 );
		//cgs.media.bloodMarkShaders[i] = engine->trap_R_RegisterShader( name );
		Com_sprintf( name, sizeof( name ), "blood_dot%i", i + 1 );
		cgs.media.bloodDotShaders[i] = engine->trap_R_RegisterShader( name );
	}

	CG_LoadingString( " - inline models" );

	// register the inline models
	cgs.numInlineModels = engine->trap_CM_NumInlineModels();
	for ( i = 1 ; i < cgs.numInlineModels ; i++ ) {
		char name[10];
		vec3_t mins, maxs;
		int j;

		Com_sprintf( name, sizeof( name ), "*%i", i );
		cgs.inlineDrawModel[i] = engine->trap_R_RegisterModel( name );
		engine->trap_R_ModelBounds( cgs.inlineDrawModel[i], mins, maxs );
		for ( j = 0 ; j < 3 ; j++ ) {
			cgs.inlineModelMidpoints[i][j] = mins[j] + 0.5 * ( maxs[j] - mins[j] );
		}
	}

	CG_LoadingString( " - server models" );

	// register all the server specified models
	for ( i = 1 ; i < MAX_MODELS ; i++ ) {
		const char      *modelName;

		modelName = CG_ConfigString( CS_MODELS + i );
		if ( !modelName[0] ) {
			break;
		}
		cgs.gameModels[i] = engine->trap_R_RegisterModel( modelName );
	}

	CG_LoadingString( " - particles" );
	CG_ClearParticles();

	for ( i = 1; i < MAX_PARTICLES_AREAS; i++ )
	{
		{
			int rval;

			rval = CG_NewParticleArea( CS_PARTICLES + i );
			if ( !rval ) {
				break;
			}
		}
	}

//	cgs.media.cursor = engine->trap_R_RegisterShaderNoMip( "menu/art/3_cursor2" );
	cgs.media.sizeCursor = engine->trap_R_RegisterShaderNoMip( "ui/assets/sizecursor.tga" );
	cgs.media.selectCursor = engine->trap_R_RegisterShaderNoMip( "ui/assets/selectcursor.tga" );
	CG_LoadingString( " - game media done" );

}

/*
===================
CG_RegisterClients

===================
*/
static void CG_RegisterClients( void ) {
	int i;

	for ( i = 0 ; i < MAX_CLIENTS ; i++ ) {
		const char      *clientInfo;

		clientInfo = CG_ConfigString( CS_PLAYERS + i );
		if ( !clientInfo[0] ) {
			continue;
		}
		CG_LoadingClient( i );
		CG_NewClientInfo( i );
	}
}

//===========================================================================

/*
=================
CG_ConfigString
=================
*/
const char *CG_ConfigString( int index ) {
	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		CG_Error( "CG_ConfigString: bad index: %i", index );
	}
	return cgs.gameState.stringData + cgs.gameState.stringOffsets[ index ];
}

//==================================================================

/*
======================
CG_StartMusic

======================
*/
void CG_StartMusic( void ) {
	char    *s;
	char parm1[MAX_QPATH], parm2[MAX_QPATH];

	// start the background music
	s = (char *)CG_ConfigString( CS_MUSIC );
	Q_strncpyz( parm1, COM_Parse( &s ), sizeof( parm1 ) );
	Q_strncpyz( parm2, COM_Parse( &s ), sizeof( parm2 ) );

	if ( strlen( parm1 ) ) {
		engine->trap_S_StartBackgroundTrack( parm1, parm2, 0 );
	}
}

//----(SA)	added
/*
==============
CG_QueueMusic
==============
*/
void CG_QueueMusic( void ) {
	char    *s;
	char parm[MAX_QPATH];

	// prepare the next background track
	s = (char *)CG_ConfigString( CS_MUSIC_QUEUE );
	Q_strncpyz( parm, COM_Parse( &s ), sizeof( parm ) );

	// even if no strlen(parm).  we want to be able to clear the queue

	// TODO: \/		the values stored in here will be made accessable so
	//				it doesn't have to go through startbackgroundtrack() (which is stupid)
	engine->trap_S_StartBackgroundTrack( parm, "", -2 );  // '-2' for 'queue looping track' (QUEUED_PLAY_LOOPED)
}

//----(SA)	end


char *CG_GetMenuBuffer( const char *filename ) {
	return NULL;
}

//
// ==============================
// new hud stuff ( mission pack )
// ==============================
//
qboolean CG_Asset_Parse( int handle ) {

	return qfalse;
}

void CG_ParseMenu( const char *menuFile ) {
	
}

qboolean CG_Load_Menu( char **p ) {
	
	return qfalse;
}



void CG_LoadMenus( const char *menuFile ) {
	

}



static qboolean CG_OwnerDrawHandleKey( int ownerDraw, int flags, float *special, int key ) {
	return qfalse;
}


static int CG_FeederCount( float feederID ) {
	int i, count;
	count = 0;

	return count;
}




///////////////////////////
///////////////////////////

static clientInfo_t * CG_InfoFromScoreIndex( int index, int team, int *scoreIndex ) {
	int i, count;
	if ( cgs.gametype >= GT_TEAM ) {
		count = 0;
		for ( i = 0; i < cg.numScores; i++ ) {
			if ( cg.scores[i].team == team ) {
				if ( count == index ) {
					*scoreIndex = i;
					return &cgs.clientinfo[cg.scores[i].client];
				}
				count++;
			}
		}
	}
	*scoreIndex = index;
	return &cgs.clientinfo[ cg.scores[index].client ];
}

static const char *CG_FeederItemText( float feederID, int index, int column, qhandle_t *handle ) {


	return "";
}

static qhandle_t CG_FeederItemImage( float feederID, int index ) {
	return 0;
}

static void CG_FeederSelection( float feederID, int index ) {
	
}

static float CG_Cvar_Get( const char *cvar ) {
	char buff[128];
	memset( buff, 0, sizeof( buff ) );
	engine->trap_Cvar_VariableStringBuffer( cvar, buff, sizeof( buff ) );
	return atof( buff );
}

void CG_Text_PaintWithCursor( float x, float y, int font, float scale, vec4_t color, const char *text, int cursorPos, char cursor, int limit, int style ) {
	CG_Text_Paint( x, y, font, scale, color, text, 0, limit, style );
}

static int CG_OwnerDrawWidth( int ownerDraw, int font, float scale ) {
	
	return 0;
}

static int CG_PlayCinematic( const char *name, float x, float y, float w, float h ) {
	return engine->trap_CIN_PlayCinematic( name, x, y, w, h, CIN_loop );
}

static void CG_StopCinematic( int handle ) {
	engine->trap_CIN_StopCinematic( handle );
}

static void CG_DrawCinematic( int handle, float x, float y, float w, float h ) {
	engine->trap_CIN_SetExtents( handle, x, y, w, h );
	engine->trap_CIN_DrawCinematic( handle );
}

static void CG_RunCinematicFrame( int handle ) {
	engine->trap_CIN_RunCinematic( handle );
}



/*
==============
CG_translateString
	presumably if this gets used more extensively, it'll be modified to a hash table
==============
*/
const char *CG_translateString( const char *str ) {
	return "";
}

sfxHandle_t CG_RegisterSound(const char* name, qboolean compressed) {
	return engine->trap_S_RegisterSound(name);
}

/*
=================
CG_LoadHudMenu();

=================
*/
void CG_LoadHudMenu() {
	
}

void CG_AssetCache() {
	
}


/*
=================
CG_Init

Called after every level change or subsystem restart
Will perform callbacks to make the loading info screen update.
=================
*/
void CG_Init( int serverMessageNum, int serverCommandSequence ) {
	const char  *s;

	// clear everything
	memset( &cgs, 0, sizeof( cgs ) );
	memset( &cg, 0, sizeof( cg ) );
	memset( cg_entities, 0, sizeof( cg_entities ) );
	memset( cg_weapons, 0, sizeof( cg_weapons ) );
	memset( cg_items, 0, sizeof( cg_items ) );

	// RF, init the anim scripting
	cgs.animScriptData.soundIndex = CG_SoundScriptPrecache;
	cgs.animScriptData.playSound = CG_SoundPlayIndexedScript;

	cgs.processedSnapshotNum = serverMessageNum;
	cgs.serverCommandSequence = serverCommandSequence;

	// load a few needed things before we do any screen updates
	// (SA) using Nerve's text since they have foreign characters
	cgs.media.charsetShader     = engine->trap_R_RegisterShader( "gfx/2d/hudchars" ); //engine->trap_R_RegisterShader( "gfx/2d/bigchars" );
	// JOSEPH 4-17-00
	cgs.media.menucharsetShader = engine->trap_R_RegisterShader( "gfx/2d/hudchars" );
	// END JOSEPH
	cgs.media.whiteShader       = engine->trap_R_RegisterShader( "white" );
	cgs.media.charsetProp       = engine->trap_R_RegisterShaderNoMip( "menu/art/font1_prop.tga" );
	cgs.media.charsetPropGlow   = engine->trap_R_RegisterShaderNoMip( "menu/art/font1_prop_glo.tga" );
	cgs.media.charsetPropB      = engine->trap_R_RegisterShaderNoMip( "menu/art/font2_prop.tga" );

	CG_RegisterCvars();

	CG_InitConsoleCommands();

//	cg.weaponSelect = WP_MP40;

	// get the rendering configuration from the client system
	engine->trap_GetGlconfig( &cgs.glconfig );
	cgs.screenXScale = cgs.glconfig.vidWidth / 640.0;
	cgs.screenYScale = cgs.glconfig.vidHeight / 480.0;

	// get the gamestate from the client system
	engine->trap_GetGameState( &cgs.gameState );

	// check version
	s = CG_ConfigString( CS_GAME_VERSION );
	if ( strcmp( s, GAME_VERSION ) ) {
		CG_Error( "Client/Server game mismatch: %s/%s", GAME_VERSION, s );
	}

	s = CG_ConfigString( CS_LEVEL_START_TIME );
	cgs.levelStartTime = atoi( s );

	CG_ParseServerinfo();

	// load the new map
	CG_LoadingString( "collision map" );

	engine->trap_CM_LoadMap( cgs.mapname );

	cg.loading = qtrue;     // force players to load instead of defer

	CG_LoadingString( "sounds" );

	CG_RegisterSounds();

	CG_LoadingString( "graphics" );

	CG_RegisterGraphics();

	CG_LoadingString( "flamechunks" );

	CG_InitFlameChunks();       // RF, register and clear all flamethrower resources

	CG_LoadingString( "clients" );

	CG_RegisterClients();       // if low on memory, some clients will be deferred

	CG_AssetCache();
	CG_LoadHudMenu();      // load new hud stuff

	cg.loading = qfalse;    // future players will be deferred

	CG_InitLocalEntities();

	CG_InitMarkPolys();

	// RF, init ZombieFX
	engine->trap_RB_ZombieFXAddNewHit( -1, NULL, NULL );

	// remove the last loading update
	cg.infoScreenText[0] = 0;

	// Make sure we have update values (scores)
	CG_SetConfigValues();

	CG_StartMusic();

	cg.lightstylesInited = qfalse;

	CG_LoadingString( "" );

	CG_ShaderStateChanged();

	// RF, clear all sounds, so we dont hear anything after level load
	engine->trap_S_ClearLoopingSounds( 2 );

	// start level load music
	// too late...
//	engine->trap_S_StartBackgroundTrack( "sound/music/fla_mp03.wav", "sound/music/fla_mp03.wav", 1 );


	// NERVE - SMF
// JPW NERVE -- commented out 'cause this moved

	if ( cgs.gametype == GT_WOLF ) {
		engine->trap_Cvar_Set( "cg_drawTimer", "0" ); // jpw

	}
	// jpw
	// -NERVE - SMF
}

/*
=================
CG_Shutdown

Called before every level change or subsystem restart
=================
*/
void CG_Shutdown( void ) {

	// some mods may need to do cleanup work here,
	// like closing files or archiving session data
}

/*
=================
CG_KeyEvent
=================
*/
void CG_KeyEvent(int key, qboolean down) {
	if (!down) {
		return;
	}

	if (cg.predictedPlayerState.pm_type == PM_NORMAL || (cg.predictedPlayerState.pm_type == PM_SPECTATOR && cg.showScores == qfalse)) {
		CG_EventHandling(CGAME_EVENT_NONE);
		engine->trap_Key_SetCatcher(0);
		return;
	}
}

/*
=================
CG_MouseEvent
=================
*/
void CG_MouseEvent(int x, int y) {

}


/*
==================
CG_EventHandling
==================
 type 0 - no event handling
	  1 - team menu
	  2 - hud editor

*/
void CG_EventHandling(int type) {

}
