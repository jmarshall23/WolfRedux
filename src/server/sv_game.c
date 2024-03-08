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

// sv_game.c -- interface to the game dll

#include "server.h"

#include "../game/botlib.h"

botlib_export_t *botlib_export;
qmodule_t  gvm = NULL;                // game virtual machine // bk001212 init
gameExport_t* game;

void SV_GameError( const char *string ) {
	Com_Error( ERR_DROP, "%s", string );
}

void SV_GamePrint( const char *string ) {
	Com_Printf( "%s", string );
}

// these functions must be used instead of pointer arithmetic, because
// the game allocates gentities with private information after the server shared part
int SV_NumForGentity( sharedEntity_t *ent ) {
	int num;

	num = ( (byte *)ent - (byte *)sv.gentities ) / sv.gentitySize;

	return num;
}

sharedEntity_t *SV_GentityNum( int num ) {
	sharedEntity_t *ent;

	ent = ( sharedEntity_t * )( (byte *)sv.gentities + sv.gentitySize * ( num ) );

	return ent;
}

playerState_t *SV_GameClientNum( int num ) {
	playerState_t   *ps;

	ps = ( playerState_t * )( (byte *)sv.gameClients + sv.gameClientSize * ( num ) );

	return ps;
}

svEntity_t  *SV_SvEntityForGentity( sharedEntity_t *gEnt ) {
	if ( !gEnt || gEnt->s.number < 0 || gEnt->s.number >= MAX_GENTITIES ) {
		Com_Error( ERR_DROP, "SV_SvEntityForGentity: bad gEnt" );
	}
	return &sv.svEntities[ gEnt->s.number ];
}

sharedEntity_t *SV_GEntityForSvEntity( svEntity_t *svEnt ) {
	int num;

	num = svEnt - sv.svEntities;
	return SV_GentityNum( num );
}

/*
===============
SV_GameSendServerCommand

Sends a command string to a client
===============
*/
void SV_GameSendServerCommand( int clientNum, const char *text ) {
	if ( clientNum == -1 ) {
		SV_SendServerCommand( NULL, "%s", text );
	} else {
		if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
			return;
		}
		SV_SendServerCommand( svs.clients + clientNum, "%s", text );
	}
}


/*
===============
SV_GameDropClient

Disconnects the client with a message
===============
*/
void SV_GameDropClient( int clientNum, const char *reason ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		return;
	}
	SV_DropClient( svs.clients + clientNum, reason );
}


/*
=================
SV_SetBrushModel

sets mins and maxs for inline bmodels
=================
*/
void SV_SetBrushModel( sharedEntity_t *ent, const char *name ) {
	clipHandle_t h;
	vec3_t mins, maxs;

	if ( !name ) {
		Com_Error( ERR_DROP, "SV_SetBrushModel: NULL" );
	}

	if ( name[0] != '*' ) {
		Com_Error( ERR_DROP, "SV_SetBrushModel: %s isn't a brush model", name );
	}


	ent->s.modelindex = atoi( name + 1 );

	h = CM_InlineModel( ent->s.modelindex );
	CM_ModelBounds( h, mins, maxs );
	VectorCopy( mins, ent->r.mins );
	VectorCopy( maxs, ent->r.maxs );
	ent->r.bmodel = qtrue;

	ent->r.contents = -1;       // we don't know exactly what is in the brushes

	SV_LinkEntity( ent );       // FIXME: remove
}



/*
=================
SV_inPVS

Also checks portalareas so that doors block sight
=================
*/
qboolean SV_inPVS( const vec3_t p1, const vec3_t p2 ) {
	int leafnum;
	int cluster;
	int area1, area2;
	byte    *mask;

	leafnum = CM_PointLeafnum( p1 );
	cluster = CM_LeafCluster( leafnum );
	area1 = CM_LeafArea( leafnum );
	mask = CM_ClusterPVS( cluster );

	leafnum = CM_PointLeafnum( p2 );
	cluster = CM_LeafCluster( leafnum );
	area2 = CM_LeafArea( leafnum );
	if ( mask && ( !( mask[cluster >> 3] & ( 1 << ( cluster & 7 ) ) ) ) ) {
		return qfalse;
	}
	if ( !CM_AreasConnected( area1, area2 ) ) {
		return qfalse;      // a door blocks sight
	}
	return qtrue;
}


/*
=================
SV_inPVSIgnorePortals

Does NOT check portalareas
=================
*/
qboolean SV_inPVSIgnorePortals( const vec3_t p1, const vec3_t p2 ) {
	int leafnum;
	int cluster;
	int area1, area2;
	byte    *mask;

	leafnum = CM_PointLeafnum( p1 );
	cluster = CM_LeafCluster( leafnum );
	area1 = CM_LeafArea( leafnum );
	mask = CM_ClusterPVS( cluster );

	leafnum = CM_PointLeafnum( p2 );
	cluster = CM_LeafCluster( leafnum );
	area2 = CM_LeafArea( leafnum );

	if ( mask && ( !( mask[cluster >> 3] & ( 1 << ( cluster & 7 ) ) ) ) ) {
		return qfalse;
	}

	return qtrue;
}


/*
========================
SV_AdjustAreaPortalState
========================
*/
void SV_AdjustAreaPortalState( sharedEntity_t *ent, qboolean open ) {
	svEntity_t  *svEnt;

	svEnt = SV_SvEntityForGentity( ent );
	if ( svEnt->areanum2 == -1 ) {
		return;
	}
	CM_AdjustAreaPortalState( svEnt->areanum, svEnt->areanum2, open );
}


/*
==================
SV_GameAreaEntities
==================
*/
qboolean    SV_EntityContact( const vec3_t mins, const vec3_t maxs, const sharedEntity_t *gEnt, const int capsule ) {
	const float *origin, *angles;
	clipHandle_t ch;
	trace_t trace;

	// check for exact collision
	origin = gEnt->r.currentOrigin;
	angles = gEnt->r.currentAngles;

	ch = SV_ClipHandleForEntity( gEnt );
	CM_TransformedBoxTrace( &trace, vec3_origin, vec3_origin, mins, maxs,
							ch, -1, origin, angles, capsule );

	return trace.startsolid;
}

qboolean    SV_EntityContact2(const vec3_t mins, const vec3_t maxs, const sharedEntity_t* gEnt) {
	return SV_EntityContact(mins, maxs, gEnt, qfalse);
}

qboolean    SV_EntityContactCapsule(const vec3_t mins, const vec3_t maxs, const sharedEntity_t* gEnt) {
	return SV_EntityContact(mins, maxs, gEnt, qtrue);
}

/*
===============
SV_GetServerinfo

===============
*/
void SV_GetServerinfo( char *buffer, int bufferSize ) {
	if ( bufferSize < 1 ) {
		Com_Error( ERR_DROP, "SV_GetServerinfo: bufferSize == %i", bufferSize );
	}
	Q_strncpyz( buffer, Cvar_InfoString( CVAR_SERVERINFO ), bufferSize );
}

/*
===============
SV_LocateGameData

===============
*/
void SV_LocateGameData( sharedEntity_t *gEnts, int numGEntities, int sizeofGEntity_t,
						playerState_t *clients, int sizeofGameClient ) {
	sv.gentities = gEnts;
	sv.gentitySize = sizeofGEntity_t;
	sv.num_entities = numGEntities;

	sv.gameClients = clients;
	sv.gameClientSize = sizeofGameClient;
}


/*
===============
SV_GetUsercmd

===============
*/
void SV_GetUsercmd( int clientNum, usercmd_t *cmd ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		Com_Error( ERR_DROP, "SV_GetUsercmd: bad clientNum:%i", clientNum );
	}
	*cmd = svs.clients[clientNum].lastUsercmd;
}

//==============================================

static int  FloatAsInt( float f ) {
	int temp;

	*(float *)&temp = f;

	return temp;
}

void SV_EndGame(void)
{
	Com_Error(ERR_ENDGAME, "endgame");
}

qboolean SV_GetEntityToken(char* dest, int size)
{
	const char* s;

	s = COM_Parse(&sv.entityParsePoint);
	Q_strncpyz(dest, s, size);
	if (!sv.entityParsePoint && !s[0]) {
		return qfalse;
	}
	else {
		return qtrue;
	}

}

void SV_InitGameImports() {
	
}

/*
===============
SV_ShutdownGameProgs

Called every time a map changes
===============
*/
void SV_ShutdownGameProgs( void ) {
	if ( !gvm ) {
		return;
	}
	
	game->G_ShutdownGame(qfalse);
	Sys_UnloadDll(gvm);
	gvm = NULL;
}

/*
==================
SV_InitGameVM

Called for both a full init and a restart
==================
*/
static void SV_InitGameVM( qboolean restart ) {
	int i;

	// start the entity parsing at the beginning
	sv.entityParsePoint = CM_EntityString();

	// use the current msec count for a random seed
	// init for this gamestate
	game->G_InitGame(svs.time, Com_Milliseconds(), restart );

	// clear all gentity pointers that might still be set from
	// a previous level
	for ( i = 0 ; i < sv_maxclients->integer ; i++ ) {
		svs.clients[i].gentity = NULL;
	}
}



/*
===================
SV_RestartGameProgs

Called on a map_restart, but not on a normal map change
===================
*/
void SV_RestartGameProgs( void ) {
	if ( !gvm ) {
		return;
	}
	//VM_Call( gvm, GAME_SHUTDOWN, qtrue );
	game->G_ShutdownGame(qtrue);

	// do a restart instead of a free
	//gvm = VM_Restart( gvm );
	if ( !gvm ) { // bk001212 - as done below
		Com_Error( ERR_FATAL, "VM_Restart on game failed" );
	}

	SV_InitGameVM( qtrue );
}

/*
===============
SV_GameErrorDrop
===============
*/
void SV_GameErrorDrop(const char* fmt)
{
	Com_Error(ERR_DROP, fmt);
}

void SV_UserCommand(int client, usercmd_t* cmd) {
	SV_ClientThink(&svs.clients[client], cmd);
}

/*
===============
SV_InitGameProgs

Called on a normal map change, not on a map_restart
===============
*/
void SV_InitGameProgs( void ) {
	cvar_t* var;
	static gameImports_t gi;

	//FIXME these are temp while I make bots run in vm
	extern int bot_enable;

	var = Cvar_Get("bot_enable", "1", CVAR_LATCH);
	if (var) {
		bot_enable = var->integer;
	}
	else {
		bot_enable = 0;
	}

	gi.trap_Printf = Com_Printf;
	gi.trap_Error = SV_GameErrorDrop;
	gi.trap_Endgame = SV_EndGame;
	gi.trap_Milliseconds = Sys_Milliseconds;
	gi.trap_Argc = Cmd_Argc;
	gi.trap_Argv = Cmd_ArgvBuffer;

	// File System Operations
	gi.trap_FS_FOpenFile = FS_FOpenFileByMode;
	gi.trap_FS_Read = FS_Read;
	gi.trap_FS_Write = FS_Write;
	gi.trap_FS_Rename = FS_Rename;
	gi.trap_FS_FCloseFile = FS_FCloseFile;
	gi.trap_FS_CopyFile = FS_CopyFileOS; // Note: Check actual implementation name
	gi.trap_FS_GetFileList = FS_GetFileList;

	// Console Commands
	gi.trap_SendConsoleCommand = Cbuf_ExecuteText;

	// Cvar Operations
	gi.trap_Cvar_Register = Cvar_Register;
	gi.trap_Cvar_Update = Cvar_Update;
	gi.trap_Cvar_Set = Cvar_Set;
	gi.trap_Cvar_VariableIntegerValue = Cvar_VariableIntegerValue;
	gi.trap_Cvar_VariableStringBuffer = Cvar_VariableStringBuffer;

	// Continuing from gi.trap_Cvar_VariableStringBuffer
	gi.trap_Cvar_VariableStringBuffer = Cvar_VariableStringBuffer;

	// Game Data
	gi.trap_LocateGameData = SV_LocateGameData;
	gi.trap_DropClient = SV_GameDropClient;
	gi.trap_SendServerCommand = SV_GameSendServerCommand;

	// Config Strings
	gi.trap_SetConfigstring = SV_SetConfigstring;
	gi.trap_GetConfigstring = SV_GetConfigstring;

	// User Info
	gi.trap_GetUserinfo = SV_GetUserinfo;
	gi.trap_SetUserinfo = SV_SetUserinfo;
	gi.trap_GetServerinfo = SV_GetServerinfo;

	// Entities and Models
	gi.trap_SetBrushModel = SV_SetBrushModel;
	gi.trap_LinkEntity = SV_LinkEntity;
	gi.trap_UnlinkEntity = SV_UnlinkEntity;

	// Collision and Tracing
	gi.trap_Trace = SV_Trace2;
	gi.trap_TraceCapsule = SV_TraceCapsule;
	gi.trap_PointContents = SV_PointContents;

	// Visibility and Area Portal
	gi.trap_InPVS = SV_inPVS;
	gi.trap_InPVSIgnorePortals = SV_inPVSIgnorePortals;
	gi.trap_AdjustAreaPortalState = SV_AdjustAreaPortalState;
	gi.trap_AreasConnected = CM_AreasConnected;

	// More entity interactions
	gi.trap_EntitiesInBox = SV_AreaEntities;
	gi.trap_EntityContact = SV_EntityContact2;
	gi.trap_EntityContactCapsule = SV_EntityContactCapsule;

	// Bot AI and Movement
	gi.trap_BotAllocateClient = SV_BotAllocateClient;
	gi.trap_BotFreeClient = SV_BotFreeClient;
	gi.trap_GetUsercmd = SV_GetUsercmd;
	gi.trap_GetEntityToken = SV_GetEntityToken;

	// Debugging and Development
	gi.trap_DebugPolygonCreate = BotImport_DebugPolygonCreate;
	gi.trap_DebugPolygonDelete = BotImport_DebugPolygonDelete;
	gi.trap_RealTime = Com_RealTime;
	gi.trap_SnapVector = Sys_SnapVector;
	gi.trap_GetTag = SV_GetTag;

	// BotLib API
	gi.trap_BotLibSetup = botlib_export->BotLibSetup;
	gi.trap_BotLibShutdown = botlib_export->BotLibShutdown;
	gi.trap_BotLibVarSet = botlib_export->BotLibVarSet;
	gi.trap_BotLibVarGet = botlib_export->BotLibVarGet;
	gi.trap_BotLibDefine = botlib_export->PC_AddGlobalDefine;
	gi.trap_BotLibStartFrame = botlib_export->BotLibStartFrame;
	gi.trap_BotLibLoadMap = botlib_export->BotLibLoadMap;
	gi.trap_BotLibUpdateEntity = botlib_export->BotLibUpdateEntity;
	gi.trap_BotLibTest = botlib_export->Test;

	// Snapshot and Server Command
	gi.trap_BotGetSnapshotEntity = SV_BotGetSnapshotEntity;
	gi.trap_BotGetServerCommand = SV_BotGetConsoleMessage;

	// User Commands
	gi.trap_BotUserCommand = SV_UserCommand;

	// AAS Interface
	gi.trap_AAS_EntityInfo = botlib_export->aas.AAS_EntityInfo;
	gi.trap_AAS_Initialized = botlib_export->aas.AAS_Initialized;
	gi.trap_AAS_PresenceTypeBoundingBox = botlib_export->aas.AAS_PresenceTypeBoundingBox;
	gi.trap_AAS_Time = botlib_export->aas.AAS_Time;

	// AAS World Indexing
	gi.trap_AAS_SetCurrentWorld = botlib_export->aas.AAS_SetCurrentWorld;
	gi.trap_AAS_PointAreaNum = botlib_export->aas.AAS_PointAreaNum;
	gi.trap_AAS_TraceAreas = botlib_export->aas.AAS_TraceAreas;
	gi.trap_AAS_PointContents = botlib_export->aas.AAS_PointContents;
	gi.trap_AAS_NextBSPEntity = botlib_export->aas.AAS_NextBSPEntity;
	gi.trap_AAS_ValueForBSPEpairKey = botlib_export->aas.AAS_ValueForBSPEpairKey;
	gi.trap_AAS_VectorForBSPEpairKey = botlib_export->aas.AAS_VectorForBSPEpairKey;
	gi.trap_AAS_FloatForBSPEpairKey = botlib_export->aas.AAS_FloatForBSPEpairKey;
	gi.trap_AAS_IntForBSPEpairKey = botlib_export->aas.AAS_IntForBSPEpairKey;

	// AAS Routing
	gi.trap_AAS_AreaReachability = botlib_export->aas.AAS_AreaReachability;
	gi.trap_AAS_AreaTravelTimeToGoalArea = botlib_export->aas.AAS_AreaTravelTimeToGoalArea;
	gi.trap_AAS_Swimming = botlib_export->aas.AAS_Swimming;
	gi.trap_AAS_PredictClientMovement = botlib_export->aas.AAS_PredictClientMovement;

	// EA Functions
	gi.trap_EA_Say = botlib_export->ea.EA_Say;
	gi.trap_EA_SayTeam = botlib_export->ea.EA_SayTeam;
	gi.trap_EA_Command = botlib_export->ea.EA_Command;

	// Goal Management
	gi.trap_BotGoalName = botlib_export->ai.BotGoalName;
	gi.trap_BotGetTopGoal = botlib_export->ai.BotGetTopGoal;
	gi.trap_BotGetSecondGoal = botlib_export->ai.BotGetSecondGoal;

	// Movement
	gi.trap_BotResetMoveState = botlib_export->ai.BotResetMoveState;
	gi.trap_BotMoveToGoal = botlib_export->ai.BotMoveToGoal;
	gi.trap_BotMoveInDirection = botlib_export->ai.BotMoveInDirection;

	// Item and Weapon Management
	gi.trap_BotLoadItemWeights = botlib_export->ai.BotLoadItemWeights;
	gi.trap_BotFreeItemWeights = botlib_export->ai.BotFreeItemWeights;
	gi.trap_BotInterbreedGoalFuzzyLogic = botlib_export->ai.BotInterbreedGoalFuzzyLogic;
	gi.trap_BotSaveGoalFuzzyLogic = botlib_export->ai.BotSaveGoalFuzzyLogic;
	gi.trap_BotMutateGoalFuzzyLogic = botlib_export->ai.BotMutateGoalFuzzyLogic;
	gi.trap_BotAllocGoalState = botlib_export->ai.BotAllocGoalState;
	gi.trap_BotFreeGoalState = botlib_export->ai.BotFreeGoalState;

	// Movement State Management
	gi.trap_BotResetLastAvoidReach = botlib_export->ai.BotResetLastAvoidReach;
	gi.trap_BotMovementViewTarget = botlib_export->ai.BotMovementViewTarget;
	gi.trap_BotPredictVisiblePosition = botlib_export->ai.BotPredictVisiblePosition;
	gi.trap_BotAllocMoveState = botlib_export->ai.BotAllocMoveState;
	gi.trap_BotFreeMoveState = botlib_export->ai.BotFreeMoveState;
	gi.trap_BotInitMoveState = botlib_export->ai.BotInitMoveState;
	gi.trap_BotInitAvoidReach = botlib_export->ai.BotInitAvoidReach;

	// Reading and Freeing Memory from Files
	gi.trap_FS_ReadFile = FS_ReadFile;
	gi.trap_FS_FreeFile = FS_FreeFile;

	// Genetic Parents And Child Selection
	gi.trap_GeneticParentsAndChildSelection = botlib_export->ai.GeneticParentsAndChildSelection;

	// EA Extended Actions
	gi.trap_EA_View = botlib_export->ea.EA_View;
	gi.trap_EA_GetInput = botlib_export->ea.EA_GetInput;
	gi.trap_EA_ResetInput = botlib_export->ea.EA_ResetInput;
	gi.trap_EA_Crouch = botlib_export->ea.EA_Crouch;
	gi.trap_EA_MoveForward = botlib_export->ea.EA_MoveForward;
	gi.trap_EA_MoveBack = botlib_export->ea.EA_MoveBack;
	gi.trap_EA_MoveLeft = botlib_export->ea.EA_MoveLeft;
	gi.trap_EA_MoveRight = botlib_export->ea.EA_MoveRight;
	gi.trap_EA_Attack = botlib_export->ea.EA_Attack;
	gi.trap_EA_Reload = botlib_export->ea.EA_Reload;
	gi.trap_EA_Jump = botlib_export->ea.EA_Jump;
	gi.trap_EA_Move = botlib_export->ea.EA_Move;
	gi.trap_EA_SelectWeapon = botlib_export->ea.EA_SelectWeapon;
	gi.trap_EA_Gesture = botlib_export->ea.EA_Gesture;
	//gi.trap_EA_MoveUp = EA_MoveUp;
	//gi.trap_EA_MoveDown = EA_MoveDown;

	// AAS Routing and Visibility
	gi.trap_AAS_RT_ShowRoute = botlib_export->aas.AAS_RT_ShowRoute;
	gi.trap_AAS_GetRouteFirstVisPos = botlib_export->aas.AAS_GetRouteFirstVisPos;
	gi.trap_AAS_FindAttackSpotWithinRange = botlib_export->aas.AAS_FindAttackSpotWithinRange;
	gi.trap_AAS_SetAASBlockingEntity = botlib_export->aas.AAS_SetAASBlockingEntity;
	gi.trap_AAS_RT_GetHidePos = botlib_export->aas.AAS_RT_GetHidePos;
	gi.trap_AAS_RT_GetHidePos = botlib_export->aas.AAS_RT_GetHidePos;

	// Reading and Freeing Memory from Files
	gi.trap_FS_ReadFile = FS_ReadFile;
	gi.trap_FS_FreeFile = FS_FreeFile;

	// AAS Interface Extended
	gi.trap_AAS_EntityInfo = botlib_export->aas.AAS_EntityInfo;
	gi.trap_AAS_Initialized = botlib_export->aas.AAS_Initialized;
	gi.trap_AAS_Time = botlib_export->aas.AAS_Time;
	gi.trap_AAS_PointAreaNum = botlib_export->aas.AAS_PointAreaNum;
	gi.trap_AAS_PointContents = botlib_export->aas.AAS_PointContents;
	gi.trap_AAS_NextBSPEntity = botlib_export->aas.AAS_NextBSPEntity;
	gi.trap_AAS_ValueForBSPEpairKey = botlib_export->aas.AAS_ValueForBSPEpairKey;
	gi.trap_AAS_VectorForBSPEpairKey = botlib_export->aas.AAS_VectorForBSPEpairKey;
	gi.trap_AAS_FloatForBSPEpairKey = botlib_export->aas.AAS_FloatForBSPEpairKey;
	gi.trap_AAS_IntForBSPEpairKey = botlib_export->aas.AAS_IntForBSPEpairKey;
	gi.trap_AAS_AreaReachability = botlib_export->aas.AAS_AreaReachability;
	gi.trap_AAS_AreaTravelTimeToGoalArea = botlib_export->aas.AAS_AreaTravelTimeToGoalArea;
	gi.trap_AAS_Swimming = botlib_export->aas.AAS_Swimming;
	gi.trap_AAS_PredictClientMovement = botlib_export->aas.AAS_PredictClientMovement;

	// Movement
	gi.trap_BotResetMoveState = botlib_export->ai.BotResetMoveState;
	gi.trap_BotMoveToGoal = botlib_export->ai.BotMoveToGoal;
	gi.trap_BotMoveInDirection = botlib_export->ai.BotMoveInDirection;
	gi.trap_BotResetLastAvoidReach = botlib_export->ai.BotResetLastAvoidReach;
	gi.trap_BotResetAvoidReach = botlib_export->ai.BotResetAvoidReach;

	// Goal Management
	gi.trap_BotGoalName = botlib_export->ai.BotGoalName;
	gi.trap_BotGetTopGoal = botlib_export->ai.BotGetTopGoal;
	gi.trap_BotGetSecondGoal = botlib_export->ai.BotGetSecondGoal;
	//gi.trap_BotChooseLTGItem = BotChooseLTGItem;
	//gi.trap_BotChooseNBGItem = BotChooseNBGItem;
	//gi.trap_BotTouchingGoal = BotTouchingGoal;
	//gi.trap_BotItemGoalInVisButNotVisible = BotItemGoalInVisButNotVisible;
	//gi.trap_BotGetNextCampSpotGoal = BotGetNextCampSpotGoal;
	//gi.trap_BotGetMapLocationGoal = BotGetMapLocationGoal;

	// Item and Weapon Management
	gi.trap_BotLoadItemWeights = botlib_export->ai.BotLoadItemWeights;
	gi.trap_BotFreeItemWeights = botlib_export->ai.BotFreeItemWeights;
	gi.trap_BotInterbreedGoalFuzzyLogic = botlib_export->ai.BotInterbreedGoalFuzzyLogic;
	gi.trap_BotSaveGoalFuzzyLogic = botlib_export->ai.BotSaveGoalFuzzyLogic;
	gi.trap_BotMutateGoalFuzzyLogic = botlib_export->ai.BotMutateGoalFuzzyLogic;
	gi.trap_BotAllocGoalState = botlib_export->ai.BotAllocGoalState;
	gi.trap_BotFreeGoalState = botlib_export->ai.BotFreeGoalState;

	// Movement State Management
	gi.trap_BotMovementViewTarget = botlib_export->ai.BotMovementViewTarget;
	gi.trap_BotPredictVisiblePosition = botlib_export->ai.BotPredictVisiblePosition;
	gi.trap_BotAllocMoveState = botlib_export->ai.BotAllocMoveState;
	gi.trap_BotFreeMoveState = botlib_export->ai.BotFreeMoveState;
	gi.trap_BotInitMoveState = botlib_export->ai.BotInitMoveState;
	gi.trap_BotInitAvoidReach = botlib_export->ai.BotInitAvoidReach;

	// Genetic Parents And Child Selection
	gi.trap_GeneticParentsAndChildSelection = botlib_export->ai.GeneticParentsAndChildSelection;

	// load the dll or bytecode
	gvm = Sys_LoadDll("qagame");
	if (!gvm) {
		Com_Error(ERR_FATAL, "VM_Create on game failed");
	}

	static gameExport_t* (*vmMain)(gameImports_t * imports);
	vmMain = Sys_GetProcAddress(gvm, "vmMain");
	game = vmMain(&gi);

	SV_InitGameVM(qfalse);
}


/*
====================
SV_GameCommand

See if the current console command is claimed by the game
====================
*/
qboolean SV_GameCommand( void ) {
	if ( sv.state != SS_GAME ) {
		return qfalse;
	}

	//return VM_Call( gvm, GAME_CONSOLE_COMMAND );
	return game->ConsoleCommand();
}

/*
====================
SV_SendMoveSpeedsToGame
====================
*/
void SV_SendMoveSpeedsToGame( int entnum, char *text ) {
	if ( !gvm ) {
		return;
	}
	//VM_Call( gvm, GAME_RETRIEVE_MOVESPEEDS_FROM_CLIENT, entnum, text );
	game->G_RetrieveMoveSpeedsFromClient(entnum, text);
}

/*
====================
SV_GetTag

  return qfalse if unable to retrieve tag information for this client
====================
*/
extern qboolean CL_GetTag( int clientNum, char *tagname, orientation_t *or );

qboolean SV_GetTag( int clientNum, char *tagname, orientation_t *or ) {
	if ( com_dedicated->integer ) {
		return qfalse;
	}

	return CL_GetTag( clientNum, tagname, or );
}

/*
===================
SV_GetModelInfo

  request this modelinfo from the game
===================
*/
qboolean SV_GetModelInfo( int clientNum, char *modelName, animModelInfo_t **modelInfo ) {
	//return VM_Call( gvm, GAME_GETMODELINFO, clientNum, modelName, modelInfo );
	return game->G_GetModelInfo(clientNum, modelName, modelInfo);
}