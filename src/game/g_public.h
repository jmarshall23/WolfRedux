// Copyright (C) 1999-2000 Id Software, Inc.
//

// g_public.h -- game module information visible to server

#define GAME_API_VERSION    8

#include "bg_public.h"

// entity->svFlags
// the server does not know how to interpret most of the values
// in entityStates (level eType), so the game must explicitly flag
// special server behaviors
#define SVF_NOCLIENT            0x00000001  // don't send entity to clients, even if it has effects
#define SVF_VISDUMMY            0x00000004  // this ent is a "visibility dummy" and needs it's master to be sent to clients that can see it even if they can't see the master ent
#define SVF_BOT                 0x00000008
// Wolfenstein
#define SVF_CASTAI              0x00000010
// done.
#define SVF_BROADCAST           0x00000020  // send to all connected clients
#define SVF_PORTAL              0x00000040  // merge a second pvs at origin2 into snapshots
#define SVF_USE_CURRENT_ORIGIN  0x00000080  // entity->r.currentOrigin instead of entity->s.origin
											// for link position (missiles and movers)
// Ridah
#define SVF_NOFOOTSTEPS         0x00000100
// done.
// MrE:
#define SVF_CAPSULE             0x00000200  // use capsule for collision detection

#define SVF_VISDUMMY_MULTIPLE   0x00000400  // so that one vis dummy can add to snapshot multiple speakers

// recent id changes
#define SVF_SINGLECLIENT        0x00000800  // only send to a single client (entityShared_t->singleClient)
#define SVF_NOSERVERINFO        0x00001000  // don't send CS_SERVERINFO updates to this client
											// so that it can be updated for ping tools without
											// lagging clients
#define SVF_NOTSINGLECLIENT     0x00002000  // send entity to everyone but one client
											// (entityShared_t->singleClient)

//===============================================================


typedef struct {
	entityState_t s;                // communicated by server to clients

	qboolean linked;                // qfalse if not in any good cluster
	int linkcount;

	int svFlags;                    // SVF_NOCLIENT, SVF_BROADCAST, etc
	int singleClient;               // only send to this client when SVF_SINGLECLIENT is set

	qboolean bmodel;                // if false, assume an explicit mins / maxs bounding box
									// only set by trap_SetBrushModel
	vec3_t mins, maxs;
	int contents;                   // CONTENTS_TRIGGER, CONTENTS_SOLID, CONTENTS_BODY, etc
									// a non-solid entity should set to 0

	vec3_t absmin, absmax;          // derived from mins/maxs and origin + rotation

	// currentOrigin will be used for all collision detection and world linking.
	// it will not necessarily be the same as the trajectory evaluation for the current
	// time, because each entity must be moved one at a time after time is advanced
	// to avoid simultanious collision issues
	vec3_t currentOrigin;
	vec3_t currentAngles;

	// when a trace call is made and passEntityNum != ENTITYNUM_NONE,
	// an ent will be excluded from testing if:
	// ent->s.number == passEntityNum	(don't interact with self)
	// ent->s.ownerNum = passEntityNum	(don't interact with your own missiles)
	// entity[ent->s.ownerNum].ownerNum = passEntityNum	(don't interact with other missiles from owner)
	int ownerNum;
	int eventTime;
} entityShared_t;



// the server looks at a sharedEntity, which is the start of the game's gentity_t structure
typedef struct {
	entityState_t s;                // communicated by server to clients
	entityShared_t r;               // shared by both the server system and game
} sharedEntity_t;

//===============================================================
// Definition of gameImports_t struct
typedef struct  {
	// System Calls
	void (*trap_Printf)(const char* fmt);
	void (*trap_Error)(const char* fmt);
	void (*trap_Endgame)(void);
	int (*trap_Milliseconds)(void);
	int (*trap_Argc)(void);
	void (*trap_Argv)(int n, char* buffer, int bufferLength);

	// File System Operations
	int (*trap_FS_FOpenFile)(const char* qpath, fileHandle_t* f, fsMode_t mode);
	void (*trap_FS_Read)(void* buffer, int len, fileHandle_t f);
	int (*trap_FS_Write)(const void* buffer, int len, fileHandle_t f);
	void (*trap_FS_Rename)(const char* from, const char* to);
	void (*trap_FS_FCloseFile)(fileHandle_t f);
	void (*trap_FS_CopyFile)(char* from, char* to);
	int (*trap_FS_GetFileList)(const char* path, const char* extension, char* listbuf, int bufsize);

	// Console Commands
	void (*trap_SendConsoleCommand)(int exec_when, const char* text);

	// Cvar Operations
	void (*trap_Cvar_Register)(vmCvar_t* cvar, const char* var_name, const char* value, int flags);
	void (*trap_Cvar_Update)(vmCvar_t* cvar);
	void (*trap_Cvar_Set)(const char* var_name, const char* value);
	int (*trap_Cvar_VariableIntegerValue)(const char* var_name);
	void (*trap_Cvar_VariableStringBuffer)(const char* var_name, char* buffer, int bufsize);

	// Game Data
	void (*trap_LocateGameData)(sharedEntity_t* gEnts, int numGEntities, int sizeofGEntity_t, playerState_t* clients, int sizeofGClient);
	void (*trap_DropClient)(int clientNum, const char* reason);
	void (*trap_SendServerCommand)(int clientNum, const char* text);

	// Config Strings
	void (*trap_SetConfigstring)(int num, const char* string);
	void (*trap_GetConfigstring)(int num, char* buffer, int bufferSize);

	// User Info
	void (*trap_GetUserinfo)(int num, char* buffer, int bufferSize);
	void (*trap_SetUserinfo)(int num, const char* buffer);
	void (*trap_GetServerinfo)(char* buffer, int bufferSize);

	// Entities and Models
	void (*trap_SetBrushModel)(sharedEntity_t* ent, const char* name);
	void (*trap_LinkEntity)(sharedEntity_t* ent);
	void (*trap_UnlinkEntity)(sharedEntity_t* ent);

	// Collision and Tracing
	void (*trap_Trace)(trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask);
	void (*trap_TraceCapsule)(trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask);
	int (*trap_PointContents)(const vec3_t point, int passEntityNum);

	// Visibility and Area Portal
	qboolean(*trap_InPVS)(const vec3_t p1, const vec3_t p2);
	qboolean(*trap_InPVSIgnorePortals)(const vec3_t p1, const vec3_t p2);
	void (*trap_AdjustAreaPortalState)(sharedEntity_t* ent, qboolean open);
	qboolean(*trap_AreasConnected)(int area1, int area2);

	// More entity interactions
	int (*trap_EntitiesInBox)(const vec3_t mins, const vec3_t maxs, int* list, int maxcount);
	qboolean(*trap_EntityContact)(const vec3_t mins, const vec3_t maxs, const sharedEntity_t* ent);
	qboolean(*trap_EntityContactCapsule)(const vec3_t mins, const vec3_t maxs, const sharedEntity_t* ent);
	// Bot AI and Movement
	int (*trap_BotAllocateClient)(void);
	void (*trap_BotFreeClient)(int clientNum);
	void (*trap_GetUsercmd)(int clientNum, usercmd_t* cmd);
	qboolean(*trap_GetEntityToken)(char* buffer, int bufferSize);

	// Debugging and Development
	int (*trap_DebugPolygonCreate)(int color, int numPoints, vec3_t* points);
	void (*trap_DebugPolygonDelete)(int id);
	int (*trap_RealTime)(qtime_t* qtime);
	void (*trap_SnapVector)(float* v);
	qboolean(*trap_GetTag)(int clientNum, char* tagName, orientation_t* or );

	// BotLib API
	int (*trap_BotLibSetup)(void);
	int (*trap_BotLibShutdown)(void);
	int (*trap_BotLibVarSet)(char* var_name, char* value);
	int (*trap_BotLibVarGet)(char* var_name, char* value, int size);
	int (*trap_BotLibDefine)(char* string);
	int (*trap_BotLibStartFrame)(float time);
	int (*trap_BotLibLoadMap)(const char* mapname);
	int (*trap_BotLibUpdateEntity)(int ent, void* bue);
	int (*trap_BotLibTest)(int parm0, char* parm1, vec3_t parm2, vec3_t parm3);

	// Snapshot and Server Command
	int (*trap_BotGetSnapshotEntity)(int clientNum, int sequence);
	int (*trap_BotGetServerCommand)(int clientNum, char* message, int size);

	// User Commands
	void (*trap_BotUserCommand)(int clientNum, usercmd_t* ucmd);

	// AAS Interface
	void (*trap_AAS_EntityInfo)(int entnum, void* info);
	int (*trap_AAS_Initialized)(void);
	void (*trap_AAS_PresenceTypeBoundingBox)(int presencetype, vec3_t mins, vec3_t maxs);
	float (*trap_AAS_Time)(void);

	// AAS World Indexing
	void (*trap_AAS_SetCurrentWorld)(int index);
	int (*trap_AAS_PointAreaNum)(vec3_t point);
	int (*trap_AAS_TraceAreas)(vec3_t start, vec3_t end, int* areas, vec3_t* points, int maxareas);
	int (*trap_AAS_PointContents)(vec3_t point);
	int (*trap_AAS_NextBSPEntity)(int ent);
	int (*trap_AAS_ValueForBSPEpairKey)(int ent, char* key, char* value, int size);
	int (*trap_AAS_VectorForBSPEpairKey)(int ent, char* key, vec3_t v);
	int (*trap_AAS_FloatForBSPEpairKey)(int ent, char* key, float* value);
	int (*trap_AAS_IntForBSPEpairKey)(int ent, char* key, int* value);

	// AAS Routing
	int (*trap_AAS_AreaReachability)(int areanum);
	int (*trap_AAS_AreaTravelTimeToGoalArea)(int areanum, vec3_t origin, int goalareanum, int travelflags);
	int (*trap_AAS_Swimming)(vec3_t origin);
	int (*trap_AAS_PredictClientMovement)(void* move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize);

	// EA Functions
	void (*trap_EA_Say)(int client, char* str);
	void (*trap_EA_SayTeam)(int client, char* str);
	void (*trap_EA_Command)(int client, char* command);

	// Goal Management
	void (*trap_BotGoalName)(int number, char* name, int size);
	int (*trap_BotGetTopGoal)(int goalstate, void* goal);
	int (*trap_BotGetSecondGoal)(int goalstate, void* goal);

	// Movement
	void (*trap_BotResetMoveState)(int movestate);
	void (*trap_BotMoveToGoal)(void* result, int movestate, void* goal, int travelflags);
	int (*trap_BotMoveInDirection)(int movestate, vec3_t dir, float speed, int type);

	// Item and Weapon Management
	int (*trap_BotLoadItemWeights)(int goalstate, char* filename);
	void (*trap_BotFreeItemWeights)(int goalstate);
	void (*trap_BotInterbreedGoalFuzzyLogic)(int parent1, int parent2, int child);
	void (*trap_BotSaveGoalFuzzyLogic)(int goalstate, char* filename);
	void (*trap_BotMutateGoalFuzzyLogic)(int goalstate, float range);
	int (*trap_BotAllocGoalState)(int state);
	void (*trap_BotFreeGoalState)(int handle);

	// Movement State Management	
	void (*trap_BotResetLastAvoidReach)(int movestate);
	int (*trap_BotMovementViewTarget)(int movestate, void* goal, int travelflags, float lookahead, vec3_t target);
	int (*trap_BotPredictVisiblePosition)(vec3_t origin, int areanum, void* goal, int travelflags, vec3_t target);
	int (*trap_BotAllocMoveState)(void);
	void (*trap_BotFreeMoveState)(int handle);
	void (*trap_BotInitMoveState)(int handle, void* initmove);
	void (*trap_BotInitAvoidReach)(int handle);

	// Genetic Parents And Child Selection
	int (*trap_GeneticParentsAndChildSelection)(int numranks, float* ranks, int* parent1, int* parent2, int* child);

	// Reading and Freeing Memory from Files
	int (*trap_FS_ReadFile)(const char* qpath, void** buffer);
	void (*trap_FS_FreeFile)(void* buffer);

	void (*trap_EA_View)(int client, vec3_t viewangles);
	void (*trap_EA_GetInput)(int client, float thinktime, void* input);
	void (*trap_EA_ResetInput)(int client, void* init);
	void (*trap_EA_Crouch)(int client);
	void (*trap_EA_SelectWeapon)(int client, int weapon);
	void (*trap_EA_Jump)(int client);
	void (*trap_EA_Move)(int client, vec3_t dir, float speed);
	void (*trap_EA_Reload)(int client);
	void (*trap_EA_Attack)(int client);
	void (*trap_BotUpdateEntityItems)(void);
	void (*trap_BotResetGoalState)(int goalstate);
	void (*trap_BotResetAvoidGoals)(int goalstate);
	void (*trap_BotResetAvoidReach)(int movestate);
	void (*trap_AAS_RT_ShowRoute)(vec3_t srcpos, int srcnum, int destnum);
	qboolean(*trap_AAS_GetRouteFirstVisPos)(vec3_t srcpos, vec3_t destpos, int travelflags, vec3_t retpos);
	int (*trap_AAS_FindAttackSpotWithinRange)(int srcnum, int rangenum, int enemynum, float rangedist, int travelflags, float* outpos);
	void (*trap_EA_MoveRight)(int client);
	void (*trap_EA_MoveLeft)(int client);
	void (*trap_EA_Gesture)(int client);
	void (*trap_AAS_SetAASBlockingEntity)(vec3_t absmin, vec3_t absmax, qboolean blocking);

	void (*trap_EA_MoveForward)(int client);
	void (*trap_EA_MoveBack)(int client);
	qboolean(*trap_AAS_RT_GetHidePos)(vec3_t srcpos, int srcnum, int srcarea, vec3_t destpos, int destnum, int destarea, vec3_t returnPos);
} gameImports_t;


//
// functions exported by the game subsystem
//
typedef struct {
	int version;

	void (*G_InitGame)(int levelTime, int randomSeed, int restart);
	void (*G_ShutdownGame)(int restart);
	char *(*ClientConnect)(int clientNum, qboolean firstTime, qboolean isBot);
	void (*ClientThink)(int clientNum);
	void (*ClientUserinfoChanged)(int clientNum);
	void (*ClientDisconnect)(int clientNum);
	void (*ClientBegin)(int clientNum);
	void (*ClientCommand)(int clientNum);
	void (*G_RunFrame)(int levelTime);
	qboolean(*ConsoleCommand)(void);
	int (*BotAIStartFrame)(int time);
	qboolean(*AICast_VisibleFromPos)(vec3_t srcpos, int srcnum, vec3_t destpos, int destnum, qboolean updateVisPos);
	qboolean(*AICast_CheckAttackAtPos)(int entnum, int enemy, vec3_t pos, qboolean ducking, qboolean allowHitWorld);
	void (*G_RetrieveMoveSpeedsFromClient)(int clientNum, char* moveSpeeds);
	qboolean (*G_GetModelInfo)(int clientNum, char* modelName, animModelInfo_t** modelInfo);
} gameExport_t;

