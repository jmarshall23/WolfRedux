// cg_public.h
//

#pragma once

#include "../game/bg_public.h"
#include "tr_types.h"

#define CMD_BACKUP          64
#define CMD_MASK            ( CMD_BACKUP - 1 )
// allow a lot of command backups for very fast systems
// multiple commands may be combined into a single packet, so this
// needs to be larger than PACKET_BACKUP


#define MAX_ENTITIES_IN_SNAPSHOT    256

// snapshots are a view of the server at a given time

// Snapshots are generated at regular time intervals by the server,
// but they may not be sent if a client's rate level is exceeded, or
// they may be dropped by the network.
typedef struct {
	int snapFlags;                      // SNAPFLAG_RATE_DELAYED, etc
	int ping;

	int serverTime;                 // server time the message is valid for (in msec)

	byte areamask[MAX_MAP_AREA_BYTES];                  // portalarea visibility bits

	playerState_t ps;                       // complete information about the current player at this time

	int numEntities;                        // all of the entities that need to be presented
	entityState_t entities[MAX_ENTITIES_IN_SNAPSHOT];   // at the time of this snapshot

	int numServerCommands;                  // text based server commands to execute when this
	int serverCommandSequence;              // snapshot becomes current
} snapshot_t;

enum {
	CGAME_EVENT_NONE,
	CGAME_EVENT_TEAMMENU,
	CGAME_EVENT_SCOREBOARD,
	CGAME_EVENT_EDITHUD
};


/*
==================================================================

functions imported from the main executable

==================================================================
*/

#define CGAME_IMPORT_API_VERSION    2001

typedef struct  {
	// System Calls
	void (*trap_Print)(const char* fmt);
	void (*trap_Error)(const char* fmt);
	int (*trap_Milliseconds)(void);
	void (*trap_Cvar_Register)(vmCvar_t* vmCvar, const char* varName, const char* defaultValue, int flags);
	void (*trap_Cvar_Update)(vmCvar_t* vmCvar);
	void (*trap_Cvar_Set)(const char* var_name, const char* value);
	void (*trap_Cvar_VariableStringBuffer)(const char* var_name, char* buffer, int bufsize);
	int (*trap_Argc)(void);
	void (*trap_Argv)(int n, char* buffer, int bufferLength);
	void (*trap_Args)(char* buffer, int bufferLength);
	int (*trap_FS_FOpenFile)(const char* qpath, fileHandle_t* f, fsMode_t mode);
	void (*trap_FS_Read)(void* buffer, int len, fileHandle_t f);
	void (*trap_FS_Write)(const void* buffer, int len, fileHandle_t f);
	void (*trap_FS_FCloseFile)(fileHandle_t f);
	void (*trap_SendConsoleCommand)(const char* text);
	void (*trap_AddCommand)(const char* cmdName);
	void (*trap_SendClientCommand)(const char* s);
	void (*trap_UpdateScreen)(void);
	void (*trap_CM_LoadMap)(const char* mapname);
	int (*trap_CM_NumInlineModels)(void);
	clipHandle_t(*trap_CM_InlineModel)(int index);
	clipHandle_t(*trap_CM_TempBoxModel)(const vec3_t mins, const vec3_t maxs);
	clipHandle_t(*trap_CM_TempCapsuleModel)(const vec3_t mins, const vec3_t maxs);
	int (*trap_CM_PointContents)(const vec3_t p, clipHandle_t model);
	int (*trap_CM_TransformedPointContents)(const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles);
	void (*trap_CM_BoxTrace)(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask);
	void (*trap_CM_TransformedBoxTrace)(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles);
	void (*trap_CM_CapsuleTrace)(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask);
	void (*trap_CM_TransformedCapsuleTrace)(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles);
	int (*trap_CM_MarkFragments)(int numPoints, const vec3_t* points, const vec3_t projection, int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t* fragmentBuffer);
	void (*trap_S_StartSound)(vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx);
	void (*trap_S_StartLocalSound)(sfxHandle_t sfx, int channelNum);
	void (*trap_S_ClearLoopingSounds)(int killall);
	void (*trap_S_AddLoopingSound)(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx, int volume);
	void (*trap_S_AddRealLoopingSound)(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx);
	void (*trap_S_StopLoopingSound)(int entityNum);
	void (*trap_S_UpdateEntityPosition)(int entityNum, const vec3_t origin);
	int (*trap_S_GetVoiceAmplitude)(int entityNum);
	void (*trap_S_Respatialize)(int entityNum, const vec3_t origin, vec3_t axis[3], int inwater);
	sfxHandle_t(*trap_S_RegisterSound)(const char* sample);
	void (*trap_S_StartBackgroundTrack)(const char* intro, const char* loop, int fadeupTime);
	void (*trap_S_FadeBackgroundTrack)(float targetvol, int time, int num);
	void (*trap_S_FadeAllSound)(float targetvol, int time);
	void (*trap_S_StartStreamingSound)(const char* intro, const char* loop, int entnum, int channel, int attenuation);
	void (*trap_R_LoadWorldMap)(const char* mapname);
	qhandle_t(*trap_R_RegisterModel)(const char* name);
	qboolean(*trap_R_GetSkinModel)(qhandle_t skinid, const char* type, char* name);
	qhandle_t(*trap_R_GetShaderFromModel)(qhandle_t modelid, int surfnum, int withlightmap);
	qhandle_t(*trap_R_RegisterSkin)(const char* name);
	qhandle_t(*trap_R_RegisterShader)(const char* name);
	qhandle_t(*trap_R_RegisterShaderNoMip)(const char* name);
	void (*trap_R_RegisterFont)(const char* fontName, int pointSize, fontInfo_t* font);
	void (*trap_R_ClearScene)(void);
	void (*trap_R_AddRefEntityToScene)(const refEntity_t* re);
	void (*trap_R_AddPolyToScene)(qhandle_t hShader, int numVerts, const polyVert_t* verts);
	void (*trap_R_AddPolysToScene)(qhandle_t hShader, int numVerts, const polyVert_t* verts, int numPolys);
	void (*trap_R_AddLightToScene)(const vec3_t org, float intensity, float r, float g, float b, int overdraw);
	void (*trap_R_AddCoronaToScene)(const vec3_t org, float r, float g, float b, float scale, int id, int visible);
	void (*trap_R_SetFog)(int fogvar, int var1, int var2, float r, float g, float b, float density);
	void (*trap_R_RenderScene)(const refdef_t* fd);
	void (*trap_R_SetColor)(const float* rgba);
	void (*trap_R_DrawStretchPic)(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader);
	void (*trap_R_ModelBounds)(clipHandle_t model, vec3_t mins, vec3_t maxs);
	int (*trap_R_LerpTag)(orientation_t* tag, const refEntity_t* refent, const char* tagName, int startIndex);
	void (*trap_R_RemapShader)(const char* oldShader, const char* newShader, const char* timeOffset);
	void (*trap_GetGlconfig)(glconfig_t* glconfig);
	void (*trap_GetGameState)(gameState_t* gamestate);
	void (*trap_GetCurrentSnapshotNumber)(int* snapshotNumber, int* serverTime);
	qboolean(*trap_GetSnapshot)(int snapshotNumber, snapshot_t* snapshot);
	qboolean(*trap_GetServerCommand)(int serverCommandNumber);
	int (*trap_GetCurrentCmdNumber)(void);
	qboolean(*trap_GetUserCmd)(int cmdNumber, usercmd_t* ucmd);
	void (*trap_SetUserCmdValue)(int userCmdValue, int holdableValue, float sensitivityScale, int cld);
	void (*trap_S_StopBackgroundTrack)(void);
	int (*trap_RealTime)(qtime_t* qtime);
	void (*trap_SnapVector)(float* v);
	void (*trap_SendMoveSpeedsToGame)(int entnum, char* movespeeds);
	int (*trap_CIN_PlayCinematic)(const char* arg0, int xpos, int ypos, int width, int height, int bits);
	e_status(*trap_CIN_StopCinematic)(int handle);
	e_status(*trap_CIN_RunCinematic)(int handle);
	void (*trap_CIN_DrawCinematic)(int handle);
	void (*trap_CIN_SetExtents)(int handle, int x, int y, int w, int h);
	qboolean(*trap_GetEntityToken)(char* buffer, int bufferSize);
	void (*trap_UI_Popup)(char* arg0);
	void (*trap_UI_LimboChat)(char* arg0);
	void (*trap_UI_ClosePopup)(char* arg0);
	qboolean(*trap_GetModelInfo)(int clientNum, char* modelName, animModelInfo_t** modelInfo);
	qboolean (*trap_getCameraInfo)(int camNum, int time, vec3_t* origin, vec3_t* angles, float* fov);
	void (*trap_Key_SetCatcher)(int catcher);
	qboolean (*trap_loadCamera)(int, const char* name);
	int (*trap_MemoryRemaining)(void);
	int (*trap_PC_FreeSource)(int handle);
	int (*trap_PC_LoadSource)(const char* filename);
	int (*trap_PC_ReadToken)(int source, pc_token_t* token);
	void (*trap_RB_ZombieFXAddNewHit)(int entityNum, const vec3_t hitPos, const vec3_t hitDir);
	void (*trap_R_DrawStretchPicGradient)(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader, const float* gradientColor, int gradientType);
	void (*trap_startCamera)(int spine, int time);
	void (*trap_stopCamera)(int);
	void (*trap_S_AddRangedLoopingSound)(int entityNum, const vec3_t origin, const vec3_t velocity, int range, sfxHandle_t sfx);
	void (*trap_S_StartSoundEx)(vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx, int flags);
	void (*trap_S_StopStreamingSound)(int entityNum);
	int (*trap_PC_SourceFileAndLine)(int handle, char* filename, int* line);
} cgameImports_t;

/*
==================================================================

functions exported to the main executable

==================================================================
*/

typedef struct {
	int apiVersion;

	void (*Init)(int serverMessageNum, int serverCommandSequence);
	void (*Shutdown)( void );
	qboolean (*ConsoleCommand)( void );
	void (*DrawActiveFrame)( int serverTime, stereoFrame_t stereoView, qboolean demoPlayback );
	int (*CrosshairPlayer)( void );
	int (*LastAttacker)( void );
	void	(*KeyEvent)( int key, qboolean down );
	void	(*MouseEvent)( int dx, int dy );
	void (*EventHandling)(int type);
	qboolean (*GetTag)( int clientNum, char *tagname, orientation_t *or );
} cgameExport_t;

//----------------------------------------------
