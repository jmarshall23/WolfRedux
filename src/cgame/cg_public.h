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

struct clientGameImports_t {
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
	void (*trap_S_ClearLoopingSounds)(qboolean killall);
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
	void (*trap_R_AddCoronaToScene)(const vec3_t org, float r, float g, float b, float scale, int id, qboolean visible);
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
	void (*trap_SetUserCmdValue)(int stateValue, float sensitivityScale, int fpSel, int cgvm);
	void (*trap_SetClientLerpOrigin)(float x, float y, float z);
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
	void (*trap_UI_Popup)(const char* arg0);
	void (*trap_UI_LimboChat)(const char* arg0);
	void (*trap_UI_ClosePopup)(const char* arg0);
	qboolean(*trap_GetModelInfo)(int clientNum, char* modelName, animModelInfo_t** modelInfo);
};

/*
==================================================================

functions exported to the main executable

==================================================================
*/

typedef enum {
	CG_INIT,
//	void CG_Init( int serverMessageNum, int serverCommandSequence )
	// called when the level loads or when the renderer is restarted
	// all media should be registered at this time
	// cgame will display loading status by calling SCR_Update, which
	// will call CG_DrawInformation during the loading process
	// reliableCommandSequence will be 0 on fresh loads, but higher for
	// demos, tourney restarts, or vid_restarts

	CG_SHUTDOWN,
//	void (*CG_Shutdown)( void );
	// oportunity to flush and close any open files

	CG_CONSOLE_COMMAND,
//	qboolean (*CG_ConsoleCommand)( void );
	// a console command has been issued locally that is not recognized by the
	// main game system.
	// use Cmd_Argc() / Cmd_Argv() to read the command, return qfalse if the
	// command is not known to the game

	CG_DRAW_ACTIVE_FRAME,
//	void (*CG_DrawActiveFrame)( int serverTime, stereoFrame_t stereoView, qboolean demoPlayback );
	// Generates and draws a game scene and status information at the given time.
	// If demoPlayback is set, local movement prediction will not be enabled

	CG_CROSSHAIR_PLAYER,
//	int (*CG_CrosshairPlayer)( void );

	CG_LAST_ATTACKER,
//	int (*CG_LastAttacker)( void );

	CG_KEY_EVENT,
//	void	(*CG_KeyEvent)( int key, qboolean down );

	CG_MOUSE_EVENT,
//	void	(*CG_MouseEvent)( int dx, int dy );
	CG_EVENT_HANDLING,
//	void (*CG_EventHandling)(int type);

	CG_GET_TAG,
//	qboolean CG_GetTag( int clientNum, char *tagname, orientation_t *or );

	MAX_CGAME_EXPORT

} cgameExport_t;

//----------------------------------------------
