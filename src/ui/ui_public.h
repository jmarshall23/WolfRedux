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

#ifndef __UI_PUBLIC_H__
#define __UI_PUBLIC_H__

#define UI_API_VERSION  4001

typedef struct {
	connstate_t connState;
	int connectPacketCount;
	int clientNum;
	char servername[MAX_STRING_CHARS];
	char updateInfoString[MAX_STRING_CHARS];
	char messageString[MAX_STRING_CHARS];
} uiClientState_t;

typedef struct  {
	void (*trap_Print)(const char* string);
	void (*trap_Error)(const char* string);
	int (*trap_Milliseconds)(void);
	void (*trap_Cvar_Register)(vmCvar_t* cvar, const char* var_name, const char* value, int flags);
	void (*trap_Cvar_Update)(vmCvar_t* cvar);
	void (*trap_Cvar_Set)(const char* var_name, const char* value);
	float (*trap_Cvar_VariableValue)(const char* var_name);
	void (*trap_Cvar_VariableStringBuffer)(const char* var_name, char* buffer, int bufsize);
	void (*trap_Cvar_SetValue)(const char* var_name, float value);
	void (*trap_Cvar_Reset)(const char* name);
	void (*trap_Cvar_Create)(const char* var_name, const char* var_value, int flags);
	void (*trap_Cvar_InfoStringBuffer)(int bit, char* buffer, int bufsize);
	int (*trap_Argc)(void);
	void (*trap_Argv)(int n, char* buffer, int bufferLength);
	void (*trap_Cmd_ExecuteText)(int exec_when, const char* text);
	int (*trap_FS_FOpenFile)(const char* qpath, fileHandle_t* f, fsMode_t mode);
	void (*trap_FS_Read)(void* buffer, int len, fileHandle_t f);
	void (*trap_FS_Seek)(fileHandle_t f, long offset, int origin);
	void (*trap_FS_Write)(const void* buffer, int len, fileHandle_t f);
	void (*trap_FS_FCloseFile)(fileHandle_t f);
	int (*trap_FS_GetFileList)(const char* path, const char* extension, char* listbuf, int bufsize);
	int (*trap_FS_Delete)(const char* filename);
	qhandle_t(*trap_R_RegisterModel)(const char* name);
	qhandle_t(*trap_R_RegisterSkin)(const char* name);
	void (*trap_R_RegisterFont)(const char* fontName, int pointSize, fontInfo_t* font);
	qhandle_t(*trap_R_RegisterShaderNoMip)(const char* name);
	void (*trap_R_ClearScene)(void);
	void (*trap_R_AddRefEntityToScene)(const refEntity_t* re);
	void (*trap_R_AddPolyToScene)(qhandle_t hShader, int numVerts, const polyVert_t* verts);
	void (*trap_R_AddLightToScene)(const vec3_t org, float intensity, float r, float g, float b, int overdraw);
	void (*trap_R_AddCoronaToScene)(const vec3_t org, float r, float g, float b, float scale, int id, int flags);
	void (*trap_R_RenderScene)(const refdef_t* fd);
	void (*trap_R_SetColor)(const float* rgba);
	void (*trap_R_DrawStretchPic)(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader);
	void (*trap_R_ModelBounds)(clipHandle_t model, vec3_t mins, vec3_t maxs);
	int (*trap_CM_LerpTag)(orientation_t* tag, const refEntity_t* refent, const char* tagName, int startIndex);
	void (*trap_S_StartLocalSound)(sfxHandle_t sfx, int channelNum);
	sfxHandle_t(*trap_S_RegisterSound)(const char* sample);
	void (*trap_S_FadeBackgroundTrack)(float targetvol, int time, int num);
	void (*trap_S_FadeAllSound)(float targetvol, int time);
	void (*trap_Key_KeynumToStringBuf)(int keynum, char* buf, int buflen);
	void (*trap_Key_GetBindingBuf)(int keynum, char* buf, int buflen);
	void (*trap_Key_SetBinding)(int keynum, const char* binding);
	qboolean(*trap_Key_IsDown)(int keynum);
	qboolean(*trap_Key_GetOverstrikeMode)(void);
	void (*trap_Key_SetOverstrikeMode)(qboolean state);
	void (*trap_Key_ClearStates)(void);
	int (*trap_Key_GetCatcher)(void);
	void (*trap_Key_SetCatcher)(int catcher);
	void (*trap_GetClipboardData)(char* buf, int bufsize);
	void (*trap_GetClientState)(uiClientState_t* state);
	void (*trap_GetGlconfig)(glconfig_t* glconfig);
	int (*trap_GetConfigString)(int index, char* buff, int buffsize);
	int (*trap_LAN_GetLocalServerCount)(void);
	void (*trap_LAN_GetLocalServerAddressString)(int n, char* buf, int buflen);

	// LAN Server Discovery
	//int (*trap_LAN_GetLocalServerCount)(void);
	//void (*trap_LAN_GetLocalServerAddressString)(int n, char* buf, int buflen);
	int (*trap_LAN_GetGlobalServerCount)(void);
	void (*trap_LAN_GetGlobalServerAddressString)(int n, char* buf, int buflen);
	int (*trap_LAN_GetPingQueueCount)(void);
	void (*trap_LAN_ClearPing)(int n);
	void (*trap_LAN_GetPing)(int n, char* buf, int buflen, int* pingtime);
	void (*trap_LAN_GetPingInfo)(int n, char* buf, int buflen);
	qboolean(*trap_LAN_UpdateVisiblePings)(int source);
	int (*trap_LAN_AddServer)(int source, const char* name, const char* addr);
	void (*trap_LAN_RemoveServer)(int source, const char* addr);
	int (*trap_LAN_GetServerCount)(int source);
	void (*trap_LAN_GetServerAddressString)(int source, int n, char* buf, int buflen);
	void (*trap_LAN_GetServerInfo)(int source, int n, char* buf, int buflen);
	int (*trap_LAN_GetServerPing)(int source, int n);
	int (*trap_LAN_ServerIsVisible)(int source, int n);
	int (*trap_LAN_ServerStatus)(const char* serverAddress, char* serverStatus, int maxLen);
	void (*trap_LAN_SaveCachedServers)(void);
	void (*trap_LAN_LoadCachedServers)(void);
	void (*trap_LAN_MarkServerVisible)(int source, int n, qboolean visible);
	void (*trap_LAN_ResetPings)(int n);
	int (*trap_LAN_CompareServers)(int source, int sortKey, int sortDir, int s1, int s2);

	// Memory Management
	int (*trap_MemoryRemaining)(void);

	// CD Key
	void (*trap_GetCDKey)(char* buf, int buflen);
	void (*trap_SetCDKey)(char* buf);

	// Config Strings
	//int (*trap_GetConfigString)(int index, char* buff, int buffsize);

	// PC Scripting
	int (*trap_PC_AddGlobalDefine)(char* define);
	int (*trap_PC_LoadSource)(const char* filename);
	int (*trap_PC_FreeSource)(int handle);
	int (*trap_PC_ReadToken)(int handle, pc_token_t* pc_token);
	int (*trap_PC_SourceFileAndLine)(int handle, char* filename, int* line);

	// Cinematics
	int (*trap_CIN_PlayCinematic)(const char* arg0, int xpos, int ypos, int width, int height, int bits);
	e_status(*trap_CIN_StopCinematic)(int handle);
	e_status(*trap_CIN_RunCinematic)(int handle);
	void (*trap_CIN_DrawCinematic)(int handle);
	void (*trap_CIN_SetExtents)(int handle, int x, int y, int w, int h);

	// Shader Remap
	void (*trap_R_RemapShader)(const char* oldShader, const char* newShader, const char* timeOffset);

	// CD Key Verification
	qboolean(*trap_VerifyCDKey)(const char* key, const char* chksum);

	// Limbo String
	qboolean(*trap_GetLimboString)(int index, char* buf);

	// Real Time
	int (*trap_RealTime)(qtime_t* qtime);

	void (*trap_S_StartBackgroundTrack)(const char* intro, const char* loop, int fadeupTime);
	void (*trap_S_StopBackgroundTrack)(void);
	void (*trap_UpdateScreen)(void);
} uiImports_t;

typedef enum {
	UIMENU_NONE,
	UIMENU_MAIN,
	UIMENU_INGAME,
	UIMENU_NEED_CD,
	UIMENU_ENDGAME, //----(SA)	added
	UIMENU_BAD_CD_KEY,
	UIMENU_TEAM,
	UIMENU_PREGAME, //----(SA)	added
	UIMENU_POSTGAME,
	UIMENU_NOTEBOOK,
	UIMENU_CLIPBOARD,
	UIMENU_HELP,
	UIMENU_BOOK1,           //----(SA)	added
	UIMENU_BOOK2,           //----(SA)	added
	UIMENU_BOOK3,           //----(SA)	added
	UIMENU_WM_PICKTEAM,     // NERVE - SMF - for multiplayer only
	UIMENU_WM_PICKPLAYER,   // NERVE - SMF - for multiplayer only
	UIMENU_WM_QUICKMESSAGE, // NERVE - SMF
	UIMENU_WM_LIMBO,        // NERVE - SMF
	UIMENU_BRIEFING         //----(SA)	added
} uiMenuCommand_t;

#define SORT_HOST           0
#define SORT_MAP            1
#define SORT_CLIENTS        2
#define SORT_GAME           3
#define SORT_PING           4

#define SORT_SAVENAME       0
#define SORT_SAVETIME       1

typedef struct {
	int		version;

	void	(*Init)(qboolean);
	void	(*Shutdown)(void);
	void	(*KeyEvent)(int key, qboolean isDown);
	void	(*MouseEvent)(int dx, int dy);
	void	(*Refresh)(int time);
	qboolean(*IsFullscreen)( void );
	void	(*SetActiveMenu)( uiMenuCommand_t menu );
	uiMenuCommand_t (*GetActiveMenu)( void );
	qboolean(*ConsoleCommand)(int realTime);
	void	(*DrawConnectScreen)( qboolean overlay );
} uiExport_t;

#endif
