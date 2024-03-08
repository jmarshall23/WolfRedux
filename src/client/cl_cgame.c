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

// cl_cgame.c  -- client system interaction with client game

#include "client.h"

#include "../game/botlib.h"

qmodule_t cgvm;
cgameExport_t* cgame;

extern botlib_export_t *botlib_export;

extern qboolean loadCamera( int camNum, const char *name );
extern void startCamera( int camNum, int time );
extern qboolean getCameraInfo( int camNum, int time, vec3_t *origin, vec3_t *angles, float *fov );

// RF, this is only used when running a local server
extern void SV_SendMoveSpeedsToGame( int entnum, char *text );
extern qboolean SV_GetModelInfo( int clientNum, char *modelName, animModelInfo_t **modelInfo );


/*
====================
CL_GetGameState
====================
*/
void CL_GetGameState( gameState_t *gs ) {
	*gs = cl.gameState;
}

/*
====================
CL_GetGlconfig
====================
*/
void CL_GetGlconfig( glconfig_t *glconfig ) {
	*glconfig = cls.glconfig;
}


/*
====================
CL_GetUserCmd
====================
*/
qboolean CL_GetUserCmd( int cmdNumber, usercmd_t *ucmd ) {
	// cmds[cmdNumber] is the last properly generated command

	// can't return anything that we haven't created yet
	if ( cmdNumber > cl.cmdNumber ) {
		Com_Error( ERR_DROP, "CL_GetUserCmd: %i >= %i", cmdNumber, cl.cmdNumber );
	}

	// the usercmd has been overwritten in the wrapping
	// buffer because it is too far out of date
	if ( cmdNumber <= cl.cmdNumber - CMD_BACKUP ) {
		return qfalse;
	}

	*ucmd = cl.cmds[ cmdNumber & CMD_MASK ];

	return qtrue;
}

int CL_GetCurrentCmdNumber( void ) {
	return cl.cmdNumber;
}


/*
====================
CL_GetParseEntityState
====================
*/
qboolean    CL_GetParseEntityState( int parseEntityNumber, entityState_t *state ) {
	// can't return anything that hasn't been parsed yet
	if ( parseEntityNumber >= cl.parseEntitiesNum ) {
		Com_Error( ERR_DROP, "CL_GetParseEntityState: %i >= %i",
				   parseEntityNumber, cl.parseEntitiesNum );
	}

	// can't return anything that has been overwritten in the circular buffer
	if ( parseEntityNumber <= cl.parseEntitiesNum - MAX_PARSE_ENTITIES ) {
		return qfalse;
	}

	*state = cl.parseEntities[ parseEntityNumber & ( MAX_PARSE_ENTITIES - 1 ) ];
	return qtrue;
}

/*
====================
CL_GetCurrentSnapshotNumber
====================
*/
void    CL_GetCurrentSnapshotNumber( int *snapshotNumber, int *serverTime ) {
	*snapshotNumber = cl.snap.messageNum;
	*serverTime = cl.snap.serverTime;
}

/*
====================
CL_GetSnapshot
====================
*/
qboolean    CL_GetSnapshot( int snapshotNumber, snapshot_t *snapshot ) {
	clSnapshot_t    *clSnap;
	int i, count;

	if ( snapshotNumber > cl.snap.messageNum ) {
		Com_Error( ERR_DROP, "CL_GetSnapshot: snapshotNumber > cl.snapshot.messageNum" );
	}

	// if the frame has fallen out of the circular buffer, we can't return it
	if ( cl.snap.messageNum - snapshotNumber >= PACKET_BACKUP ) {
		return qfalse;
	}

	// if the frame is not valid, we can't return it
	clSnap = &cl.snapshots[snapshotNumber & PACKET_MASK];
	if ( !clSnap->valid ) {
		return qfalse;
	}

	// if the entities in the frame have fallen out of their
	// circular buffer, we can't return it
	if ( cl.parseEntitiesNum - clSnap->parseEntitiesNum >= MAX_PARSE_ENTITIES ) {
		return qfalse;
	}

	// write the snapshot
	snapshot->snapFlags = clSnap->snapFlags;
	snapshot->serverCommandSequence = clSnap->serverCommandNum;
	snapshot->ping = clSnap->ping;
	snapshot->serverTime = clSnap->serverTime;
	memcpy( snapshot->areamask, clSnap->areamask, sizeof( snapshot->areamask ) );
	snapshot->ps = clSnap->ps;
	count = clSnap->numEntities;
	if ( count > MAX_ENTITIES_IN_SNAPSHOT ) {
		Com_DPrintf( "CL_GetSnapshot: truncated %i entities to %i\n", count, MAX_ENTITIES_IN_SNAPSHOT );
		count = MAX_ENTITIES_IN_SNAPSHOT;
	}
	snapshot->numEntities = count;
	for ( i = 0 ; i < count ; i++ ) {
		snapshot->entities[i] =
			cl.parseEntities[ ( clSnap->parseEntitiesNum + i ) & ( MAX_PARSE_ENTITIES - 1 ) ];
	}

	// FIXME: configstring changes and server commands!!!

	return qtrue;
}

/*
==============
CL_SetUserCmdValue
==============
*/
void CL_SetUserCmdValue( int userCmdValue, int holdableValue, float sensitivityScale, int cld ) {
	cl.cgameUserCmdValue        = userCmdValue;
	cl.cgameUserHoldableValue   = holdableValue;
	cl.cgameSensitivity         = sensitivityScale;
	cl.cgameCld                 = cld;
}

/*
==============
CL_AddCgameCommand
==============
*/
void CL_AddCgameCommand( const char *cmdName ) {
	Cmd_AddCommand( cmdName, NULL );
}

/*
==============
CL_CgameError
==============
*/
void CL_CgameError( const char *string ) {
	Com_Error( ERR_DROP, "%s", string );
}


/*
=====================
CL_ConfigstringModified
=====================
*/
void CL_ConfigstringModified( void ) {
	char        *old, *s;
	int i, index;
	char        *dup;
	gameState_t oldGs;
	int len;

	index = atoi( Cmd_Argv( 1 ) );
	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		Com_Error( ERR_DROP, "configstring > MAX_CONFIGSTRINGS" );
	}
//	s = Cmd_Argv(2);
	// get everything after "cs <num>"
	s = Cmd_ArgsFrom( 2 );

	old = cl.gameState.stringData + cl.gameState.stringOffsets[ index ];
	if ( !strcmp( old, s ) ) {
		return;     // unchanged
	}

	// build the new gameState_t
	oldGs = cl.gameState;

	memset( &cl.gameState, 0, sizeof( cl.gameState ) );

	// leave the first 0 for uninitialized strings
	cl.gameState.dataCount = 1;

	for ( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ ) {
		if ( i == index ) {
			dup = s;
		} else {
			dup = oldGs.stringData + oldGs.stringOffsets[ i ];
		}
		if ( !dup[0] ) {
			continue;       // leave with the default empty string
		}

		len = strlen( dup );

		if ( len + 1 + cl.gameState.dataCount > MAX_GAMESTATE_CHARS ) {
			Com_Error( ERR_DROP, "MAX_GAMESTATE_CHARS exceeded" );
		}

		// append it to the gameState string buffer
		cl.gameState.stringOffsets[ i ] = cl.gameState.dataCount;
		memcpy( cl.gameState.stringData + cl.gameState.dataCount, dup, len + 1 );
		cl.gameState.dataCount += len + 1;
	}

	if ( index == CS_SYSTEMINFO ) {
		// parse serverId and other cvars
		CL_SystemInfoChanged();
	}

}


/*
===================
CL_GetServerCommand

Set up argc/argv for the given command
===================
*/
qboolean CL_GetServerCommand( int serverCommandNumber ) {
	char    *s;
	char    *cmd;
	static char bigConfigString[BIG_INFO_STRING];

	// if we have irretrievably lost a reliable command, drop the connection
	if ( serverCommandNumber <= clc.serverCommandSequence - MAX_RELIABLE_COMMANDS ) {
		// when a demo record was started after the client got a whole bunch of
		// reliable commands then the client never got those first reliable commands
		if ( clc.demoplaying ) {
			return qfalse;
		}
		Com_Error( ERR_DROP, "CL_GetServerCommand: a reliable command was cycled out" );
		return qfalse;
	}

	if ( serverCommandNumber > clc.serverCommandSequence ) {
		Com_Error( ERR_DROP, "CL_GetServerCommand: requested a command not received" );
		return qfalse;
	}

	s = clc.serverCommands[ serverCommandNumber & ( MAX_RELIABLE_COMMANDS - 1 ) ];
	clc.lastExecutedServerCommand = serverCommandNumber;

	Com_DPrintf( "serverCommand: %i : %s\n", serverCommandNumber, s );

rescan:
	Cmd_TokenizeString( s );
	cmd = Cmd_Argv( 0 );

	if ( !strcmp( cmd, "disconnect" ) ) {
		Com_Error( ERR_SERVERDISCONNECT,"Server disconnected\n" );
	}

	if ( !strcmp( cmd, "bcs0" ) ) {
		Com_sprintf( bigConfigString, BIG_INFO_STRING, "cs %s \"%s", Cmd_Argv( 1 ), Cmd_Argv( 2 ) );
		return qfalse;
	}

	if ( !strcmp( cmd, "bcs1" ) ) {
		s = Cmd_Argv( 2 );
		if ( strlen( bigConfigString ) + strlen( s ) >= BIG_INFO_STRING ) {
			Com_Error( ERR_DROP, "bcs exceeded BIG_INFO_STRING" );
		}
		strcat( bigConfigString, s );
		return qfalse;
	}

	if ( !strcmp( cmd, "bcs2" ) ) {
		s = Cmd_Argv( 2 );
		if ( strlen( bigConfigString ) + strlen( s ) + 1 >= BIG_INFO_STRING ) {
			Com_Error( ERR_DROP, "bcs exceeded BIG_INFO_STRING" );
		}
		strcat( bigConfigString, s );
		strcat( bigConfigString, "\"" );
		s = bigConfigString;
		goto rescan;
	}

	if ( !strcmp( cmd, "cs" ) ) {
		CL_ConfigstringModified();
		// reparse the string, because CL_ConfigstringModified may have done another Cmd_TokenizeString()
		Cmd_TokenizeString( s );
		return qtrue;
	}

	if ( !strcmp( cmd, "map_restart" ) ) {
		// clear notify lines and outgoing commands before passing
		// the restart to the cgame
		Con_ClearNotify();
		memset( cl.cmds, 0, sizeof( cl.cmds ) );
		return qtrue;
	}

	if ( !strcmp( cmd, "popup" ) ) { // direct server to client popup request, bypassing cgame
//		trap_UI_Popup(Cmd_Argv(1));
//		if ( cls.state == CA_ACTIVE && !clc.demoplaying ) {
//			VM_Call( uivm, UI_SET_ACTIVE_MENU, UIMENU_CLIPBOARD);
//			Menus_OpenByName(Cmd_Argv(1));
//		}
		return qfalse;
	}


	// the clientLevelShot command is used during development
	// to generate 128*128 screenshots from the intermission
	// point of levels for the menu system to use
	// we pass it along to the cgame to make apropriate adjustments,
	// but we also clear the console and notify lines here
	if ( !strcmp( cmd, "clientLevelShot" ) ) {
		// don't do it if we aren't running the server locally,
		// otherwise malicious remote servers could overwrite
		// the existing thumbnails
		if ( !com_sv_running->integer ) {
			return qfalse;
		}
		// close the console
		Con_Close();
		// take a special screenshot next frame
		Cbuf_AddText( "wait ; wait ; wait ; wait ; screenshot levelshot\n" );
		return qtrue;
	}

	// we may want to put a "connect to other server" command here

	// cgame can now act on the command
	return qtrue;
}


/*
====================
CL_CM_LoadMap

Just adds default parameters that cgame doesn't need to know about
====================
*/
void CL_CM_LoadMap( const char *mapname ) {
	int checksum;

	CM_LoadMap( mapname, qtrue, &checksum );
}

/*
====================
CL_ShutdonwCGame

====================
*/
void CL_ShutdownCGame( void ) {
	cls.keyCatchers &= ~KEYCATCH_CGAME;
	cls.cgameStarted = qfalse;
	if ( !cgvm ) {
		return;
	}
	cgame->Shutdown();
	Sys_UnloadDll(cgvm);
	cgame = NULL;
	cgvm = NULL;
}

static int  FloatAsInt( float f ) {
	int temp;

	*(float *)&temp = f;

	return temp;
}


/*
====================
CL_UpdateLevelHunkUsage

  This updates the "hunkusage.dat" file with the current map and it's hunk usage count

  This is used for level loading, so we can show a percentage bar dependant on the amount
  of hunk memory allocated so far

  This will be slightly inaccurate if some settings like sound quality are changed, but these
  things should only account for a small variation (hopefully)
====================
*/
void CL_UpdateLevelHunkUsage( void ) {
	int handle;
	char *memlistfile = "hunkusage.dat";
	char *buf, *outbuf;
	char *buftrav, *outbuftrav;
	char *token;
	char outstr[256];
	int len, memusage;

	memusage = Cvar_VariableIntegerValue( "com_hunkused" ) + Cvar_VariableIntegerValue( "hunk_soundadjust" );

	len = FS_FOpenFileByMode( memlistfile, &handle, FS_READ );
	if ( len >= 0 ) { // the file exists, so read it in, strip out the current entry for this map, and save it out, so we can append the new value

		buf = (char *)Z_Malloc( len + 1 );
		memset( buf, 0, len + 1 );
		outbuf = (char *)Z_Malloc( len + 1 );
		memset( outbuf, 0, len + 1 );

		FS_Read( (void *)buf, len, handle );
		FS_FCloseFile( handle );

		// now parse the file, filtering out the current map
		buftrav = buf;
		outbuftrav = outbuf;
		outbuftrav[0] = '\0';
		while ( ( token = COM_Parse( &buftrav ) ) && token[0] ) {
			if ( !Q_strcasecmp( token, cl.mapname ) ) {
				// found a match
				token = COM_Parse( &buftrav );  // read the size
				if ( token && token[0] ) {
					if ( atoi( token ) == memusage ) {  // if it is the same, abort this process
						Z_Free( buf );
						Z_Free( outbuf );
						return;
					}
				}
			} else {    // send it to the outbuf
				Q_strcat( outbuftrav, len + 1, token );
				Q_strcat( outbuftrav, len + 1, " " );
				token = COM_Parse( &buftrav );  // read the size
				if ( token && token[0] ) {
					Q_strcat( outbuftrav, len + 1, token );
					Q_strcat( outbuftrav, len + 1, "\n" );
				} else {
					Com_Error( ERR_DROP, "hunkusage.dat file is corrupt\n" );
				}
			}
		}

#ifdef __MACOS__    //DAJ MacOS file typing
		{
			extern _MSL_IMP_EXP_C long _fcreator, _ftype;
			_ftype = 'WlfB';
			_fcreator = 'WlfS';
		}
#endif
		handle = FS_FOpenFileWrite( memlistfile );
		if ( handle < 0 ) {
			Com_Error( ERR_DROP, "cannot create %s\n", memlistfile );
		}
		// input file is parsed, now output to the new file
		len = strlen( outbuf );
		if ( FS_Write( (void *)outbuf, len, handle ) != len ) {
			Com_Error( ERR_DROP, "cannot write to %s\n", memlistfile );
		}
		FS_FCloseFile( handle );

		Z_Free( buf );
		Z_Free( outbuf );
	}
	// now append the current map to the current file
	FS_FOpenFileByMode( memlistfile, &handle, FS_APPEND );
	if ( handle < 0 ) {
		Com_Error( ERR_DROP, "cannot write to hunkusage.dat, check disk full\n" );
	}
	Com_sprintf( outstr, sizeof( outstr ), "%s %i\n", cl.mapname, memusage );
	FS_Write( outstr, strlen( outstr ), handle );
	FS_FCloseFile( handle );

	// now just open it and close it, so it gets copied to the pak dir
	len = FS_FOpenFileByMode( memlistfile, &handle, FS_READ );
	if ( len >= 0 ) {
		FS_FCloseFile( handle );
	}
}

void CL_StopCamera(int camType)
{
	if (camType == 0) {  // CAM_PRIMARY
		cl.cameraMode = qfalse;
	}
	//		stopCamera(args[1]);
}

void CL_UIClosePopup(char *temp) {
	ui->KeyEvent(K_ESCAPE, qtrue);
}

void CL_UIPopup(char* menu) {
	if (!menu)
		return;

	if (menu && !Q_stricmp(menu, "briefing")) {  //----(SA) added
		ui->SetActiveMenu(UIMENU_BRIEFING);
		return;
	}

	if (cls.state == CA_ACTIVE && !clc.demoplaying) {
		// NERVE - SMF
		if (menu && !Q_stricmp(menu, "UIMENU_WM_PICKTEAM")) {
			ui->SetActiveMenu(UIMENU_WM_PICKTEAM);
		}
		else if (menu && !Q_stricmp(menu, "UIMENU_WM_PICKPLAYER")) {
			ui->SetActiveMenu(UIMENU_WM_PICKPLAYER);
		}
		else if (menu && !Q_stricmp(menu, "UIMENU_WM_QUICKMESSAGE")) {
			ui->SetActiveMenu(UIMENU_WM_QUICKMESSAGE);
		}
		else if (menu && !Q_stricmp(menu, "UIMENU_WM_LIMBO")) {
			ui->SetActiveMenu(UIMENU_WM_LIMBO);
		}
		// -NERVE - SMF
		else if (menu && !Q_stricmp(menu, "hbook1")) {   //----(SA)
			ui->SetActiveMenu(UIMENU_BOOK1);
		}
		else if (menu && !Q_stricmp(menu, "hbook2")) { //----(SA)
			ui->SetActiveMenu(UIMENU_BOOK2);
		}
		else if (menu && !Q_stricmp(menu, "hbook3")) { //----(SA)
			ui->SetActiveMenu(UIMENU_BOOK3);
		}
		else if (menu && !Q_stricmp(menu, "pregame")) { //----(SA) added
			ui->SetActiveMenu(UIMENU_PREGAME);
		}
		else {
			ui->SetActiveMenu(UIMENU_CLIPBOARD);
		}
	}
}

void CL_CGameError(const char* fmt)
{
	Com_Error(ERR_DROP, fmt);
}

void CL_ClearLoopingSounds(int clear)
{
	S_ClearLoopingSounds(); // (SA) modified so no_pvs sounds can function
	// RF, if killall, then stop all sounds
	if (clear == 1) {
		S_ClearSounds(qtrue, qfalse);
	}
	else if (clear == 2) {
		S_ClearSounds(qtrue, qtrue);
	}
}

void    CL_S_AddLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx, int volume) {
	S_AddLoopingSound(entityNum, origin, velocity, 1250, sfx, volume);     // volume was previously removed from CG_S_ADDLOOPINGSOUND.  I added 'range'
}

void    CL_S_AddRealLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx) {
	// not in use
	//	syscall( CG_S_ADDREALLOOPINGSOUND, entityNum, origin, velocity, 1250, sfx, 255 );	//----(SA)	modified
}

/*
====================
CL_InitCGame

Should only by called by CL_StartHunkUsers
====================
*/
void CL_InitCGame( void ) {
	const char          *info;
	const char          *mapname;
	int t1, t2;
	static cgameImports_t cgi;
	
	t1 = Sys_Milliseconds();

	// put away the console
	Con_Close();

	cgi.trap_Print = Com_Printf;
	cgi.trap_Error = CL_CGameError;
	cgi.trap_Milliseconds = Sys_Milliseconds;
	cgi.trap_Cvar_Register = Cvar_Register;
	cgi.trap_Cvar_Update = Cvar_Update;
	cgi.trap_Cvar_Set = Cvar_Set;
	cgi.trap_Cvar_VariableStringBuffer = Cvar_VariableStringBuffer;
	cgi.trap_Argc = Cmd_Argc;
	cgi.trap_Argv = Cmd_ArgvBuffer;
	cgi.trap_Args = Cmd_ArgsBuffer;
	cgi.trap_FS_FOpenFile = FS_FOpenFileByMode;
	cgi.trap_FS_Read = FS_Read;
	cgi.trap_FS_Write = FS_Write;
	cgi.trap_FS_FCloseFile = FS_FCloseFile;
	cgi.trap_SendConsoleCommand = Cbuf_AddText;
	cgi.trap_AddCommand = CL_AddCgameCommand;
	cgi.trap_SendClientCommand = CL_AddReliableCommand;
	cgi.trap_UpdateScreen = SCR_UpdateScreen;
	cgi.trap_CM_LoadMap = CL_CM_LoadMap;
	cgi.trap_CM_NumInlineModels = CM_NumInlineModels;
	cgi.trap_CM_InlineModel = CM_InlineModel;
	cgi.trap_CM_TempBoxModel = CM_TempBoxModel2;
	cgi.trap_CM_TempCapsuleModel = CM_TempBoxModelCapsule;
	cgi.trap_CM_PointContents = CM_PointContents;
	cgi.trap_CM_TransformedPointContents = CM_TransformedPointContents;
	cgi.trap_CM_BoxTrace = CM_BoxTrace2;
	cgi.trap_CM_TransformedBoxTrace = CM_TransformedBoxTrace2;
	cgi.trap_CM_CapsuleTrace = CM_CapsuleTrace;
	cgi.trap_CM_TransformedCapsuleTrace = CM_TransformedCapsuleTrace;
	cgi.trap_CM_MarkFragments = re.MarkFragments;
	cgi.trap_S_StartSound = S_StartSound;
	cgi.trap_S_StartLocalSound = S_StartLocalSound;
	cgi.trap_S_ClearLoopingSounds = CL_ClearLoopingSounds;
	cgi.trap_S_AddLoopingSound = CL_S_AddLoopingSound;
	cgi.trap_S_AddRealLoopingSound = CL_S_AddRealLoopingSound;
	cgi.trap_S_StopLoopingSound = S_StopLoopingSound;
	cgi.trap_S_UpdateEntityPosition = S_UpdateEntityPosition;
	cgi.trap_S_GetVoiceAmplitude = S_GetVoiceAmplitude;
	cgi.trap_S_Respatialize = S_Respatialize;
	cgi.trap_S_RegisterSound = S_RegisterSound2;
	cgi.trap_S_StartBackgroundTrack = S_StartBackgroundTrack;
	cgi.trap_S_FadeBackgroundTrack = S_FadeStreamingSound;
	cgi.trap_S_FadeAllSound = S_FadeAllSounds;
	cgi.trap_S_StartStreamingSound = S_StartStreamingSound;
	cgi.trap_R_LoadWorldMap = re.LoadWorld;
	cgi.trap_R_RegisterModel = re.RegisterModel;
	cgi.trap_R_GetSkinModel = re.GetSkinModel;
	cgi.trap_R_GetShaderFromModel = re.GetShaderFromModel;
	cgi.trap_R_RegisterSkin = re.RegisterSkin;
	cgi.trap_R_RegisterShader = re.RegisterShader;
	cgi.trap_R_RegisterShaderNoMip = re.RegisterShaderNoMip;
	cgi.trap_R_RegisterFont = re.RegisterFont;
	cgi.trap_R_ClearScene = re.ClearScene;
	cgi.trap_R_AddRefEntityToScene = re.AddRefEntityToScene;
	cgi.trap_R_AddPolyToScene = re.AddPolyToScene;
	cgi.trap_R_AddPolysToScene = re.AddPolysToScene;
	cgi.trap_R_AddLightToScene = re.AddLightToScene;
	cgi.trap_R_AddCoronaToScene = re.AddCoronaToScene;
	cgi.trap_R_SetFog = re.SetFog;
	cgi.trap_R_RenderScene = re.RenderScene;
	cgi.trap_R_SetColor = re.SetColor;
	cgi.trap_R_DrawStretchPic = re.DrawStretchPic;
	cgi.trap_R_ModelBounds = re.ModelBounds;
	cgi.trap_R_LerpTag = re.LerpTag;
	cgi.trap_R_RemapShader = re.RemapShader;
	cgi.trap_GetGlconfig = CL_GetGlconfig;
	cgi.trap_GetGameState = CL_GetGameState;
	cgi.trap_GetCurrentSnapshotNumber = CL_GetCurrentSnapshotNumber;
	cgi.trap_GetSnapshot = CL_GetSnapshot;
	cgi.trap_GetServerCommand = CL_GetServerCommand;
	cgi.trap_GetCurrentCmdNumber = CL_GetCurrentCmdNumber;
	cgi.trap_GetUserCmd = CL_GetUserCmd;
	cgi.trap_SetUserCmdValue = CL_SetUserCmdValue;
	cgi.trap_S_StopBackgroundTrack = S_StopBackgroundTrack;
	cgi.trap_RealTime = Com_RealTime;
	cgi.trap_SnapVector = Sys_SnapVector;
	cgi.trap_SendMoveSpeedsToGame = SV_SendMoveSpeedsToGame;
	cgi.trap_CIN_PlayCinematic = CIN_PlayCinematic;
	cgi.trap_CIN_StopCinematic = CIN_StopCinematic;
	cgi.trap_CIN_RunCinematic = CIN_RunCinematic;
	cgi.trap_CIN_DrawCinematic = CIN_DrawCinematic;
	cgi.trap_CIN_SetExtents = CIN_SetExtents;
	cgi.trap_GetEntityToken = re.GetEntityToken;
	cgi.trap_UI_Popup = CL_UIPopup; // This needs to be defined as per your UI interaction code
	cgi.trap_UI_LimboChat = CL_AddToLimboChat; // Define as per your UI chat handling
	cgi.trap_UI_ClosePopup = CL_UIClosePopup; // Define for UI popup close handling
	cgi.trap_GetModelInfo = SV_GetModelInfo;
	cgi.trap_getCameraInfo = getCameraInfo; // Ensure this function is defined to match signature
	cgi.trap_Key_SetCatcher = Key_SetCatcher;
	cgi.trap_loadCamera = loadCamera; // Define as per your camera loading logic
	cgi.trap_MemoryRemaining = Hunk_MemoryRemaining;
	cgi.trap_PC_FreeSource = botlib_export->PC_FreeSourceHandle; // Adjust based on botlib exports
	cgi.trap_PC_LoadSource = botlib_export->PC_LoadSourceHandle;
	cgi.trap_PC_ReadToken = botlib_export->PC_ReadTokenHandle;
	cgi.trap_RB_ZombieFXAddNewHit = re.ZombieFXAddNewHit; // Define as per your rendering effects
	cgi.trap_R_DrawStretchPicGradient = re.DrawStretchPicGradient; // Define to match signature
	cgi.trap_startCamera = startCamera; // Define camera start logic
	cgi.trap_stopCamera = CL_StopCamera; // Define camera stop logic
	cgi.trap_S_AddRangedLoopingSound = CL_S_AddLoopingSound; // Define ranged looping sound logic
	cgi.trap_S_StartSoundEx = S_StartSoundEx; // Define extended sound start logic
	cgi.trap_S_StopStreamingSound = S_StopStreamingSound; // Define streaming sound stop logic
	cgi.trap_PC_SourceFileAndLine = botlib_export->PC_SourceFileAndLine; // Adjust based on botlib exports
	cgi.trap_S_StopBackgroundTrack = S_StopBackgroundTrack;
	cgi.trap_RealTime = Com_RealTime;
	cgi.trap_SnapVector = Sys_SnapVector;
	cgi.trap_SendMoveSpeedsToGame = SV_SendMoveSpeedsToGame;
	cgi.trap_CIN_PlayCinematic = CIN_PlayCinematic;
	cgi.trap_CIN_StopCinematic = CIN_StopCinematic;
	cgi.trap_CIN_RunCinematic = CIN_RunCinematic;
	cgi.trap_CIN_DrawCinematic = CIN_DrawCinematic;
	cgi.trap_CIN_SetExtents = CIN_SetExtents;
	cgi.trap_R_RemapShader = re.RemapShader;	
	cgi.trap_loadCamera = loadCamera;
	cgi.trap_startCamera = startCamera;
	cgi.trap_stopCamera = CL_StopCamera; // Ensure the actual function logic handles CAM_PRIMARY as in the case
	cgi.trap_getCameraInfo = getCameraInfo;
	cgi.trap_GetEntityToken = re.GetEntityToken;
	cgi.trap_GetModelInfo = SV_GetModelInfo;

	// find the current mapname
	info = cl.gameState.stringData + cl.gameState.stringOffsets[ CS_SERVERINFO ];
	mapname = Info_ValueForKey( info, "mapname" );
	Com_sprintf( cl.mapname, sizeof( cl.mapname ), "maps/%s.bsp", mapname );

	// load the dll or bytecode
	cgvm = Sys_LoadDll("cgame");
	if (!cgvm) {
		Com_Error(ERR_DROP, "VM_Create on cgame failed");
	}

	static cgameExport_t* (*vmMain)(cgameImports_t * imports);
	vmMain = Sys_GetProcAddress(cgvm, "vmMain");
	cgame = vmMain(&cgi);

	cls.state = CA_LOADING;

	// init for this gamestate
	// use the lastExecutedServerCommand instead of the serverCommandSequence
	// otherwise server commands sent just before a gamestate are dropped
	cgame->Init(clc.serverMessageSequence, clc.lastExecutedServerCommand);

	// we will send a usercmd this frame, which
	// will cause the server to send us the first snapshot
	cls.state = CA_PRIMED;

	t2 = Sys_Milliseconds();

	Com_Printf( "CL_InitCGame: %5.2f seconds\n", ( t2 - t1 ) / 1000.0 );

	// have the renderer touch all its images, so they are present
	// on the card even if the driver does deferred loading
	re.EndRegistration();

	// make sure everything is paged in
	if ( !Sys_LowPhysicalMemory() ) {
		Com_TouchMemory();
	}

	// clear anything that got printed
	Con_ClearNotify();

	// Ridah, update the memory usage file
	CL_UpdateLevelHunkUsage();
}


/*
====================
CL_GameCommand

See if the current console command is claimed by the cgame
====================
*/
qboolean CL_GameCommand( void ) {
	if ( !cgvm ) {
		return qfalse;
	}

	return cgame->ConsoleCommand();
}



/*
=====================
CL_CGameRendering
=====================
*/
void CL_CGameRendering( stereoFrame_t stereo ) {	
	cgame->DrawActiveFrame(cl.serverTime, stereo, clc.demoplaying);
}


/*
=================
CL_AdjustTimeDelta

Adjust the clients view of server time.

We attempt to have cl.serverTime exactly equal the server's view
of time plus the timeNudge, but with variable latencies over
the internet it will often need to drift a bit to match conditions.

Our ideal time would be to have the adjusted time approach, but not pass,
the very latest snapshot.

Adjustments are only made when a new snapshot arrives with a rational
latency, which keeps the adjustment process framerate independent and
prevents massive overadjustment during times of significant packet loss
or bursted delayed packets.
=================
*/

#define RESET_TIME  500

void CL_AdjustTimeDelta( void ) {
	int resetTime;
	int newDelta;
	int deltaDelta;

	cl.newSnapshots = qfalse;

	// the delta never drifts when replaying a demo
	if ( clc.demoplaying ) {
		return;
	}

	// if the current time is WAY off, just correct to the current value
	if ( com_sv_running->integer ) {
		resetTime = 100;
	} else {
		resetTime = RESET_TIME;
	}

	newDelta = cl.snap.serverTime - cls.realtime;
	deltaDelta = abs( newDelta - cl.serverTimeDelta );

	if ( deltaDelta > RESET_TIME ) {
		cl.serverTimeDelta = newDelta;
		cl.oldServerTime = cl.snap.serverTime;  // FIXME: is this a problem for cgame?
		cl.serverTime = cl.snap.serverTime;
		if ( cl_showTimeDelta->integer ) {
			Com_Printf( "<RESET> " );
		}
	} else if ( deltaDelta > 100 ) {
		// fast adjust, cut the difference in half
		if ( cl_showTimeDelta->integer ) {
			Com_Printf( "<FAST> " );
		}
		cl.serverTimeDelta = ( cl.serverTimeDelta + newDelta ) >> 1;
	} else {
		// slow drift adjust, only move 1 or 2 msec

		// if any of the frames between this and the previous snapshot
		// had to be extrapolated, nudge our sense of time back a little
		// the granularity of +1 / -2 is too high for timescale modified frametimes
		if ( com_timescale->value == 0 || com_timescale->value == 1 ) {
			if ( cl.extrapolatedSnapshot ) {
				cl.extrapolatedSnapshot = qfalse;
				cl.serverTimeDelta -= 2;
			} else {
				// otherwise, move our sense of time forward to minimize total latency
				cl.serverTimeDelta++;
			}
		}
	}

	if ( cl_showTimeDelta->integer ) {
		Com_Printf( "%i ", cl.serverTimeDelta );
	}
}


/*
==================
CL_FirstSnapshot
==================
*/
void CL_FirstSnapshot( void ) {
	// ignore snapshots that don't have entities
	if ( cl.snap.snapFlags & SNAPFLAG_NOT_ACTIVE ) {
		return;
	}
	cls.state = CA_ACTIVE;

	// set the timedelta so we are exactly on this first frame
	cl.serverTimeDelta = cl.snap.serverTime - cls.realtime;
	cl.oldServerTime = cl.snap.serverTime;

	clc.timeDemoBaseTime = cl.snap.serverTime;

	// if this is the first frame of active play,
	// execute the contents of activeAction now
	// this is to allow scripting a timedemo to start right
	// after loading
	if ( cl_activeAction->string[0] ) {
		Cbuf_AddText( cl_activeAction->string );
		Cvar_Set( "activeAction", "" );
	}

	Sys_BeginProfiling();
}

/*
==================
CL_SetCGameTime
==================
*/
void CL_SetCGameTime( void ) {
	// getting a valid frame message ends the connection process
	if ( cls.state != CA_ACTIVE ) {
		if ( cls.state != CA_PRIMED ) {
			return;
		}
		if ( clc.demoplaying ) {
			// we shouldn't get the first snapshot on the same frame
			// as the gamestate, because it causes a bad time skip
			if ( !clc.firstDemoFrameSkipped ) {
				clc.firstDemoFrameSkipped = qtrue;
				return;
			}
			CL_ReadDemoMessage();
		}
		if ( cl.newSnapshots ) {
			cl.newSnapshots = qfalse;
			CL_FirstSnapshot();
		}
		if ( cls.state != CA_ACTIVE ) {
			return;
		}
	}

	// if we have gotten to this point, cl.snap is guaranteed to be valid
	if ( !cl.snap.valid ) {
		Com_Error( ERR_DROP, "CL_SetCGameTime: !cl.snap.valid" );
	}

	// allow pause in single player
	if ( sv_paused->integer && cl_paused->integer && com_sv_running->integer ) {
		// paused
		return;
	}

	if ( cl.snap.serverTime < cl.oldFrameServerTime ) {
		// Ridah, if this is a localhost, then we are probably loading a savegame
		if ( !Q_stricmp( cls.servername, "localhost" ) ) {
			// do nothing?
			CL_FirstSnapshot();
		} else {
			Com_Error( ERR_DROP, "cl.snap.serverTime < cl.oldFrameServerTime" );
		}
	}
	cl.oldFrameServerTime = cl.snap.serverTime;


	// get our current view of time

	if ( clc.demoplaying && cl_freezeDemo->integer ) {
		// cl_freezeDemo is used to lock a demo in place for single frame advances

	} else {
		// cl_timeNudge is a user adjustable cvar that allows more
		// or less latency to be added in the interest of better
		// smoothness or better responsiveness.
		int tn;

		tn = cl_timeNudge->integer;
		if ( tn < -30 ) {
			tn = -30;
		} else if ( tn > 30 ) {
			tn = 30;
		}

		cl.serverTime = cls.realtime + cl.serverTimeDelta - tn;

		// guarantee that time will never flow backwards, even if
		// serverTimeDelta made an adjustment or cl_timeNudge was changed
		if ( cl.serverTime < cl.oldServerTime ) {
			cl.serverTime = cl.oldServerTime;
		}
		cl.oldServerTime = cl.serverTime;

		// note if we are almost past the latest frame (without timeNudge),
		// so we will try and adjust back a bit when the next snapshot arrives
		if ( cls.realtime + cl.serverTimeDelta >= cl.snap.serverTime - 5 ) {
			cl.extrapolatedSnapshot = qtrue;
		}
	}

	// if we have gotten new snapshots, drift serverTimeDelta
	// don't do this every frame, or a period of packet loss would
	// make a huge adjustment
	if ( cl.newSnapshots ) {
		CL_AdjustTimeDelta();
	}

	if ( !clc.demoplaying ) {
		return;
	}

	// if we are playing a demo back, we can just keep reading
	// messages from the demo file until the cgame definately
	// has valid snapshots to interpolate between

	// a timedemo will always use a deterministic set of time samples
	// no matter what speed machine it is run on,
	// while a normal demo may have different time samples
	// each time it is played back
	if ( cl_timedemo->integer ) {
		if ( !clc.timeDemoStart ) {
			clc.timeDemoStart = Sys_Milliseconds();
		}
		clc.timeDemoFrames++;
		cl.serverTime = clc.timeDemoBaseTime + clc.timeDemoFrames * 50;
	}

	while ( cl.serverTime >= cl.snap.serverTime ) {
		// feed another messag, which should change
		// the contents of cl.snap
		CL_ReadDemoMessage();
		if ( cls.state != CA_ACTIVE ) {
			return;     // end of demo
		}
	}

}

/*
====================
CL_GetTag
====================
*/
qboolean CL_GetTag( int clientNum, char *tagname, orientation_t *or ) {
	if ( !cgvm ) {
		return qfalse;
	}

	//return VM_Call( cgvm, CG_GET_TAG, clientNum, tagname, or );
	return cgame->GetTag(clientNum, tagname, or );
}
