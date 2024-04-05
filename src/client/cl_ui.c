// cl_ui.c
//

#include "client.h"

#include "../game/botlib.h"

extern botlib_export_t *botlib_export;

qmodule_t uivm;
uiExport_t* ui;

extern char cl_cdkey[34];


/*
====================
GetClientState
====================
*/
static void GetClientState( uiClientState_t *state ) {
	state->connectPacketCount = clc.connectPacketCount;
	state->connState = cls.state;
	Q_strncpyz( state->servername, cls.servername, sizeof( state->servername ) );
	Q_strncpyz( state->updateInfoString, cls.updateInfoString, sizeof( state->updateInfoString ) );
	Q_strncpyz( state->messageString, clc.serverMessage, sizeof( state->messageString ) );
	state->clientNum = cl.snap.ps.clientNum;
}

/*
====================
LAN_LoadCachedServers
====================
*/
void LAN_LoadCachedServers() {
	// TTimo: stub, this is only relevant to MP, SP kills the servercache.dat (and favorites)
	// show_bug.cgi?id=445
	/*
	  int size;
	  fileHandle_t fileIn;
	  cls.numglobalservers = cls.nummplayerservers = cls.numfavoriteservers = 0;
	  cls.numGlobalServerAddresses = 0;
	  if (FS_SV_FOpenFileRead("servercache.dat", &fileIn)) {
		  FS_Read(&cls.numglobalservers, sizeof(int), fileIn);
		  FS_Read(&cls.nummplayerservers, sizeof(int), fileIn);
		  FS_Read(&cls.numfavoriteservers, sizeof(int), fileIn);
		  FS_Read(&size, sizeof(int), fileIn);
		  if (size == sizeof(cls.globalServers) + sizeof(cls.favoriteServers) + sizeof(cls.mplayerServers)) {
			  FS_Read(&cls.globalServers, sizeof(cls.globalServers), fileIn);
			  FS_Read(&cls.mplayerServers, sizeof(cls.mplayerServers), fileIn);
			  FS_Read(&cls.favoriteServers, sizeof(cls.favoriteServers), fileIn);
		  } else {
			  cls.numglobalservers = cls.nummplayerservers = cls.numfavoriteservers = 0;
			  cls.numGlobalServerAddresses = 0;
		  }
		  FS_FCloseFile(fileIn);
	  }
	*/
}

/*
====================
LAN_SaveServersToCache
====================
*/
void LAN_SaveServersToCache() {
	// TTimo: stub, this is only relevant to MP, SP kills the servercache.dat (and favorites)
	// show_bug.cgi?id=445
	/*
	  int size;
	  fileHandle_t fileOut;
  #ifdef __MACOS__	//DAJ MacOS file typing
	  {
		  extern _MSL_IMP_EXP_C long _fcreator, _ftype;
		  _ftype = 'WlfB';
		  _fcreator = 'WlfS';
	  }
  #endif
	  fileOut = FS_SV_FOpenFileWrite("servercache.dat");
	  FS_Write(&cls.numglobalservers, sizeof(int), fileOut);
	  FS_Write(&cls.nummplayerservers, sizeof(int), fileOut);
	  FS_Write(&cls.numfavoriteservers, sizeof(int), fileOut);
	  size = sizeof(cls.globalServers) + sizeof(cls.favoriteServers) + sizeof(cls.mplayerServers);
	  FS_Write(&size, sizeof(int), fileOut);
	  FS_Write(&cls.globalServers, sizeof(cls.globalServers), fileOut);
	  FS_Write(&cls.mplayerServers, sizeof(cls.mplayerServers), fileOut);
	  FS_Write(&cls.favoriteServers, sizeof(cls.favoriteServers), fileOut);
	  FS_FCloseFile(fileOut);
	*/
}


/*
====================
LAN_ResetPings
====================
*/
static void LAN_ResetPings( int source ) {
	int count,i;
	serverInfo_t *servers = NULL;
	count = 0;

	switch ( source ) {
	case AS_LOCAL:
		servers = &cls.localServers[0];
		count = MAX_OTHER_SERVERS;
		break;
	case AS_MPLAYER:
		servers = &cls.mplayerServers[0];
		count = MAX_OTHER_SERVERS;
		break;
	case AS_GLOBAL:
		servers = &cls.globalServers[0];
		count = MAX_GLOBAL_SERVERS;
		break;
	case AS_FAVORITES:
		servers = &cls.favoriteServers[0];
		count = MAX_OTHER_SERVERS;
		break;
	}
	if ( servers ) {
		for ( i = 0; i < count; i++ ) {
			servers[i].ping = -1;
		}
	}
}

/*
====================
LAN_AddServer
====================
*/
static int LAN_AddServer( int source, const char *name, const char *address ) {
	int max, *count, i;
	netadr_t adr;
	serverInfo_t *servers = NULL;
	max = MAX_OTHER_SERVERS;
	count = 0;

	switch ( source ) {
	case AS_LOCAL:
		count = &cls.numlocalservers;
		servers = &cls.localServers[0];
		break;
	case AS_MPLAYER:
		count = &cls.nummplayerservers;
		servers = &cls.mplayerServers[0];
		break;
	case AS_GLOBAL:
		max = MAX_GLOBAL_SERVERS;
		count = &cls.numglobalservers;
		servers = &cls.globalServers[0];
		break;
	case AS_FAVORITES:
		count = &cls.numfavoriteservers;
		servers = &cls.favoriteServers[0];
		break;
	}
	if ( servers && *count < max ) {
		NET_StringToAdr( address, &adr );
		for ( i = 0; i < *count; i++ ) {
			if ( NET_CompareAdr( servers[i].adr, adr ) ) {
				break;
			}
		}
		if ( i >= *count ) {
			servers[*count].adr = adr;
			Q_strncpyz( servers[*count].hostName, name, sizeof( servers[*count].hostName ) );
			servers[*count].visible = qtrue;
			( *count )++;
			return 1;
		}
		return 0;
	}
	return -1;
}

/*
====================
LAN_RemoveServer
====================
*/
static void LAN_RemoveServer( int source, const char *addr ) {
	int *count, i;
	serverInfo_t *servers = NULL;
	count = 0;
	switch ( source ) {
	case AS_LOCAL:
		count = &cls.numlocalservers;
		servers = &cls.localServers[0];
		break;
	case AS_MPLAYER:
		count = &cls.nummplayerservers;
		servers = &cls.mplayerServers[0];
		break;
	case AS_GLOBAL:
		count = &cls.numglobalservers;
		servers = &cls.globalServers[0];
		break;
	case AS_FAVORITES:
		count = &cls.numfavoriteservers;
		servers = &cls.favoriteServers[0];
		break;
	}
	if ( servers ) {
		netadr_t comp;
		NET_StringToAdr( addr, &comp );
		for ( i = 0; i < *count; i++ ) {
			if ( NET_CompareAdr( comp, servers[i].adr ) ) {
				int j = i;
				while ( j < *count - 1 ) {
					Com_Memcpy( &servers[j], &servers[j + 1], sizeof( servers[j] ) );
					j++;
				}
				( *count )--;
				break;
			}
		}
	}
}


/*
====================
LAN_GetServerCount
====================
*/
static int LAN_GetServerCount( int source ) {
	switch ( source ) {
	case AS_LOCAL:
		return cls.numlocalservers;
		break;
	case AS_MPLAYER:
		return cls.nummplayerservers;
		break;
	case AS_GLOBAL:
		return cls.numglobalservers;
		break;
	case AS_FAVORITES:
		return cls.numfavoriteservers;
		break;
	}
	return 0;
}

/*
====================
LAN_GetLocalServerAddressString
====================
*/
static void LAN_GetServerAddressString( int source, int n, char *buf, int buflen ) {
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			Q_strncpyz( buf, NET_AdrToString( cls.localServers[n].adr ), buflen );
			return;
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			Q_strncpyz( buf, NET_AdrToString( cls.mplayerServers[n].adr ), buflen );
			return;
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			Q_strncpyz( buf, NET_AdrToString( cls.globalServers[n].adr ), buflen );
			return;
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			Q_strncpyz( buf, NET_AdrToString( cls.favoriteServers[n].adr ), buflen );
			return;
		}
		break;
	}
	buf[0] = '\0';
}

/*
====================
LAN_GetServerInfo
====================
*/
static void LAN_GetServerInfo( int source, int n, char *buf, int buflen ) {
	char info[MAX_STRING_CHARS];
	serverInfo_t *server = NULL;
	info[0] = '\0';
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.localServers[n];
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.mplayerServers[n];
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			server = &cls.globalServers[n];
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.favoriteServers[n];
		}
		break;
	}
	if ( server && buf ) {
		buf[0] = '\0';
		Info_SetValueForKey( info, "hostname", server->hostName );
		Info_SetValueForKey( info, "mapname", server->mapName );
		Info_SetValueForKey( info, "clients", va( "%i",server->clients ) );
		Info_SetValueForKey( info, "sv_maxclients", va( "%i",server->maxClients ) );
		Info_SetValueForKey( info, "ping", va( "%i",server->ping ) );
		Info_SetValueForKey( info, "minping", va( "%i",server->minPing ) );
		Info_SetValueForKey( info, "maxping", va( "%i",server->maxPing ) );
		Info_SetValueForKey( info, "game", server->game );
		Info_SetValueForKey( info, "gametype", va( "%i",server->gameType ) );
		Info_SetValueForKey( info, "nettype", va( "%i",server->netType ) );
		Info_SetValueForKey( info, "addr", NET_AdrToString( server->adr ) );
		Info_SetValueForKey( info, "sv_allowAnonymous", va( "%i", server->allowAnonymous ) );
		Q_strncpyz( buf, info, buflen );
	} else {
		if ( buf ) {
			buf[0] = '\0';
		}
	}
}

/*
====================
LAN_GetServerPing
====================
*/
static int LAN_GetServerPing( int source, int n ) {
	serverInfo_t *server = NULL;
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.localServers[n];
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.mplayerServers[n];
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			server = &cls.globalServers[n];
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.favoriteServers[n];
		}
		break;
	}
	if ( server ) {
		return server->ping;
	}
	return -1;
}

/*
====================
LAN_GetServerPtr
====================
*/
static serverInfo_t *LAN_GetServerPtr( int source, int n ) {
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return &cls.localServers[n];
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return &cls.mplayerServers[n];
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			return &cls.globalServers[n];
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return &cls.favoriteServers[n];
		}
		break;
	}
	return NULL;
}

/*
====================
LAN_CompareServers
====================
*/
static int LAN_CompareServers( int source, int sortKey, int sortDir, int s1, int s2 ) {
	int res;
	serverInfo_t *server1, *server2;

	server1 = LAN_GetServerPtr( source, s1 );
	server2 = LAN_GetServerPtr( source, s2 );
	if ( !server1 || !server2 ) {
		return 0;
	}

	res = 0;
	switch ( sortKey ) {
	case SORT_HOST:
		res = Q_stricmp( server1->hostName, server2->hostName );
		break;

	case SORT_MAP:
		res = Q_stricmp( server1->mapName, server2->mapName );
		break;
	case SORT_CLIENTS:
		if ( server1->clients < server2->clients ) {
			res = -1;
		} else if ( server1->clients > server2->clients )     {
			res = 1;
		} else {
			res = 0;
		}
		break;
	case SORT_GAME:
		if ( server1->gameType < server2->gameType ) {
			res = -1;
		} else if ( server1->gameType > server2->gameType )     {
			res = 1;
		} else {
			res = 0;
		}
		break;
	case SORT_PING:
		if ( server1->ping < server2->ping ) {
			res = -1;
		} else if ( server1->ping > server2->ping )     {
			res = 1;
		} else {
			res = 0;
		}
		break;
	}

	if ( sortDir ) {
		if ( res < 0 ) {
			return 1;
		}
		if ( res > 0 ) {
			return -1;
		}
		return 0;
	}
	return res;
}

/*
====================
LAN_GetPingQueueCount
====================
*/
static int LAN_GetPingQueueCount( void ) {
	return ( CL_GetPingQueueCount() );
}

/*
====================
LAN_ClearPing
====================
*/
static void LAN_ClearPing( int n ) {
	CL_ClearPing( n );
}

/*
====================
LAN_GetPing
====================
*/
static void LAN_GetPing( int n, char *buf, int buflen, int *pingtime ) {
	CL_GetPing( n, buf, buflen, pingtime );
}

/*
====================
LAN_GetPingInfo
====================
*/
static void LAN_GetPingInfo( int n, char *buf, int buflen ) {
	CL_GetPingInfo( n, buf, buflen );
}

/*
====================
LAN_MarkServerVisible
====================
*/
static void LAN_MarkServerVisible( int source, int n, qboolean visible ) {
	if ( n == -1 ) {
		int count = MAX_OTHER_SERVERS;
		serverInfo_t *server = NULL;
		switch ( source ) {
		case AS_LOCAL:
			server = &cls.localServers[0];
			break;
		case AS_MPLAYER:
			server = &cls.mplayerServers[0];
			break;
		case AS_GLOBAL:
			server = &cls.globalServers[0];
			count = MAX_GLOBAL_SERVERS;
			break;
		case AS_FAVORITES:
			server = &cls.favoriteServers[0];
			break;
		}
		if ( server ) {
			for ( n = 0; n < count; n++ ) {
				server[n].visible = visible;
			}
		}

	} else {
		switch ( source ) {
		case AS_LOCAL:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				cls.localServers[n].visible = visible;
			}
			break;
		case AS_MPLAYER:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				cls.mplayerServers[n].visible = visible;
			}
			break;
		case AS_GLOBAL:
			if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
				cls.globalServers[n].visible = visible;
			}
			break;
		case AS_FAVORITES:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				cls.favoriteServers[n].visible = visible;
			}
			break;
		}
	}
}


/*
=======================
LAN_ServerIsVisible
=======================
*/
static int LAN_ServerIsVisible( int source, int n ) {
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return cls.localServers[n].visible;
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return cls.mplayerServers[n].visible;
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			return cls.globalServers[n].visible;
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return cls.favoriteServers[n].visible;
		}
		break;
	}
	return qfalse;
}

/*
=======================
LAN_UpdateVisiblePings
=======================
*/
qboolean LAN_UpdateVisiblePings( int source ) {
	return CL_UpdateVisiblePings_f( source );
}

/*
====================
LAN_GetServerStatus
====================
*/
int LAN_GetServerStatus( char *serverAddress, char *serverStatus, int maxLen ) {
	return CL_ServerStatus( serverAddress, serverStatus, maxLen );
}

/*
====================
CL_GetGlConfig
====================
*/
static void CL_GetGlconfig( glconfig_t *config ) {
	*config = cls.glconfig;
}

/*
====================
GetClipboardData
====================
*/
static void GetClipboardData( char *buf, int buflen ) {
	char    *cbd;

	cbd = Sys_GetClipboardData();

	if ( !cbd ) {
		*buf = 0;
		return;
	}

	Q_strncpyz( buf, cbd, buflen );

	Z_Free( cbd );
}

/*
====================
Key_KeynumToStringBuf
====================
*/
static void Key_KeynumToStringBuf( int keynum, char *buf, int buflen ) {
	Q_strncpyz( buf, Key_KeynumToString( keynum, qtrue ), buflen );
}

/*
====================
Key_GetBindingBuf
====================
*/
static void Key_GetBindingBuf( int keynum, char *buf, int buflen ) {
	char    *value;

	value = Key_GetBinding( keynum );
	if ( value ) {
		Q_strncpyz( buf, value, buflen );
	} else {
		*buf = 0;
	}
}

/*
====================
Key_GetCatcher
====================
*/
int Key_GetCatcher( void ) {
	return 0;
}

/*
====================
Ket_SetCatcher
====================
*/
void Key_SetCatcher( int catcher ) {
	//cls.keyCatchers = catcher;
}


/*
====================
CLUI_GetCDKey
====================
*/
static void CLUI_GetCDKey( char *buf, int buflen ) {
	cvar_t  *fs;
	fs = Cvar_Get( "fs_game", "", CVAR_INIT | CVAR_SYSTEMINFO );
	if ( UI_usesUniqueCDKey() && fs && fs->string[0] != 0 ) {
		memcpy( buf, &cl_cdkey[16], 16 );
		buf[16] = 0;
	} else {
		memcpy( buf, cl_cdkey, 16 );
		buf[16] = 0;
	}
}


/*
====================
CLUI_SetCDKey
====================
*/
static void CLUI_SetCDKey( char *buf ) {
	cvar_t  *fs;
	fs = Cvar_Get( "fs_game", "", CVAR_INIT | CVAR_SYSTEMINFO );
	if ( UI_usesUniqueCDKey() && fs && fs->string[0] != 0 ) {
		memcpy( &cl_cdkey[16], buf, 16 );
		cl_cdkey[32] = 0;
		// set the flag so the fle will be written at the next opportunity
		cvar_modifiedFlags |= CVAR_ARCHIVE;
	} else {
		memcpy( cl_cdkey, buf, 16 );
		// set the flag so the fle will be written at the next opportunity
		cvar_modifiedFlags |= CVAR_ARCHIVE;
	}
}


/*
====================
GetConfigString
====================
*/
static int GetConfigString( int index, char *buf, int size ) {
	int offset;

	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		return qfalse;
	}

	offset = cl.gameState.stringOffsets[index];
	if ( !offset ) {
		if ( size ) {
			buf[0] = 0;
		}
		return qfalse;
	}

	Q_strncpyz( buf, cl.gameState.stringData + offset, size );

	return qtrue;
}

/*
====================
CL_ShutdownUI
====================
*/
void CL_ShutdownUI( void ) {
	//cls.keyCatchers &= ~KEYCATCH_UI;
	cls.uiStarted = qfalse;
	if ( !uivm ) {
		return;
	}
	ui->Shutdown();
	Sys_UnloadDll(uivm);
	ui = NULL;
	uivm = NULL;
}

void CL_UiError(const char* fmt)
{
	Com_Error(ERR_DROP, fmt);
}

/*
====================
CL_InitUI
====================
*/

void CL_InitUI( void ) {
	static uiImports_t uiImports;

	uiImports.trap_Print = Com_Printf;
	uiImports.trap_Error = CL_UiError;
	uiImports.trap_Milliseconds = Sys_Milliseconds;
	uiImports.trap_Cvar_Register = Cvar_Register;
	uiImports.trap_Cvar_Update = Cvar_Update;
	uiImports.trap_Cvar_Set = Cvar_Set;
	uiImports.trap_Cvar_VariableValue = Cvar_VariableValue;
	uiImports.trap_Cvar_VariableStringBuffer = Cvar_VariableStringBuffer;
	uiImports.trap_Cvar_SetValue = Cvar_SetValue;
	uiImports.trap_Cvar_Reset = Cvar_Reset;
	uiImports.trap_Cvar_Create = Cvar_Get;
	uiImports.trap_Cvar_InfoStringBuffer = Cvar_InfoStringBuffer;
	uiImports.trap_Argc = Cmd_Argc;
	uiImports.trap_Argv = Cmd_ArgvBuffer;
	uiImports.trap_Cmd_ExecuteText = Cbuf_ExecuteText;
	uiImports.trap_FS_FOpenFile = FS_FOpenFileByMode;
	uiImports.trap_FS_Read = FS_Read;
	uiImports.trap_FS_Seek = FS_Seek;
	uiImports.trap_FS_Write = FS_Write;
	uiImports.trap_FS_FCloseFile = FS_FCloseFile;
	uiImports.trap_FS_GetFileList = FS_GetFileList;
	uiImports.trap_FS_Delete = FS_Delete;
	uiImports.trap_R_RegisterModel = re.RegisterModel;
	uiImports.trap_R_RegisterSkin = re.RegisterSkin;
	uiImports.trap_R_RegisterFont = re.RegisterFont;
	uiImports.trap_R_RegisterShaderNoMip = re.RegisterShaderNoMip;
	uiImports.trap_R_ClearScene = re.ClearScene;
	uiImports.trap_R_AddRefEntityToScene = re.AddRefEntityToScene;
	uiImports.trap_R_AddPolyToScene = re.AddPolyToScene;
	uiImports.trap_R_AddLightToScene = re.AddLightToScene;
	uiImports.trap_R_AddCoronaToScene = re.AddCoronaToScene;
	uiImports.trap_R_RenderScene = re.RenderScene;
	uiImports.trap_R_SetColor = re.SetColor;
	uiImports.trap_R_DrawStretchPic = re.DrawStretchPic;
	uiImports.trap_R_ModelBounds = re.ModelBounds;
	uiImports.trap_CM_LerpTag = re.LerpTag;
	uiImports.trap_S_StartLocalSound = S_StartLocalSound;
	uiImports.trap_S_RegisterSound = S_RegisterSound;
	uiImports.trap_S_FadeBackgroundTrack = S_FadeStreamingSound;
	uiImports.trap_S_FadeAllSound = S_FadeAllSounds;
	uiImports.trap_Key_KeynumToStringBuf = Key_KeynumToStringBuf;
	uiImports.trap_Key_GetBindingBuf = Key_GetBindingBuf;
	uiImports.trap_Key_SetBinding = Key_SetBinding;
	uiImports.trap_Key_IsDown = Key_IsDown;
	uiImports.trap_Key_GetOverstrikeMode = Key_GetOverstrikeMode;
	uiImports.trap_Key_SetOverstrikeMode = Key_SetOverstrikeMode;
	uiImports.trap_Key_ClearStates = Key_ClearStates;
	uiImports.trap_Key_GetCatcher = Key_GetCatcher;
	uiImports.trap_Key_SetCatcher = Key_SetCatcher;
	uiImports.trap_GetClipboardData = GetClipboardData;
	uiImports.trap_GetClientState = GetClientState;
	uiImports.trap_GetGlconfig = CL_GetGlconfig;
	uiImports.trap_GetConfigString = GetConfigString;
	//uiImports.trap_LAN_GetLocalServerCount = LAN_GetLocalServerCount;
	//uiImports.trap_LAN_GetLocalServerAddressString = LAN_GetLocalServerAddressString;
	//uiImports.trap_LAN_GetGlobalServerCount = LAN_GetGlobalServerCount;
	//uiImports.trap_LAN_GetGlobalServerAddressString = LAN_GetGlobalServerAddressString;
	uiImports.trap_LAN_GetPingQueueCount = LAN_GetPingQueueCount;
	uiImports.trap_LAN_ClearPing = LAN_ClearPing;
	uiImports.trap_LAN_GetPing = LAN_GetPing;
	uiImports.trap_LAN_GetPingInfo = LAN_GetPingInfo;
	uiImports.trap_LAN_MarkServerVisible = LAN_MarkServerVisible;
	uiImports.trap_LAN_UpdateVisiblePings = LAN_UpdateVisiblePings;
	uiImports.trap_LAN_AddServer = LAN_AddServer;
	uiImports.trap_LAN_RemoveServer = LAN_RemoveServer;
	uiImports.trap_LAN_GetServerCount = LAN_GetServerCount;
	uiImports.trap_LAN_GetServerAddressString = LAN_GetServerAddressString;
	uiImports.trap_LAN_GetServerInfo = LAN_GetServerInfo;
	uiImports.trap_LAN_GetServerPing = LAN_GetServerPing;
	uiImports.trap_LAN_ServerIsVisible = LAN_ServerIsVisible;
	uiImports.trap_LAN_ServerStatus = LAN_GetServerStatus;
	uiImports.trap_LAN_SaveCachedServers = LAN_SaveServersToCache;
	uiImports.trap_LAN_LoadCachedServers = LAN_LoadCachedServers;
	uiImports.trap_LAN_ResetPings = LAN_ResetPings;
	uiImports.trap_LAN_CompareServers = LAN_CompareServers;
	uiImports.trap_MemoryRemaining = Hunk_MemoryRemaining;
	uiImports.trap_GetCDKey = CLUI_GetCDKey;
	uiImports.trap_SetCDKey = CLUI_SetCDKey;
	uiImports.trap_PC_AddGlobalDefine = botlib_export->PC_AddGlobalDefine;
	uiImports.trap_PC_LoadSource = botlib_export->PC_LoadSourceHandle;
	uiImports.trap_PC_FreeSource = botlib_export->PC_FreeSourceHandle;
	uiImports.trap_PC_ReadToken = botlib_export->PC_ReadTokenHandle;
	uiImports.trap_PC_SourceFileAndLine = botlib_export->PC_SourceFileAndLine;
	uiImports.trap_CIN_PlayCinematic = CIN_PlayCinematic;
	uiImports.trap_CIN_StopCinematic = CIN_StopCinematic;
	uiImports.trap_CIN_RunCinematic = CIN_RunCinematic;
	uiImports.trap_CIN_DrawCinematic = CIN_DrawCinematic;
	uiImports.trap_CIN_SetExtents = CIN_SetExtents;
	uiImports.trap_R_RemapShader = re.RemapShader;
	uiImports.trap_VerifyCDKey = CL_CDKeyValidate;
	uiImports.trap_GetLimboString = CL_GetLimboString;
	uiImports.trap_RealTime = Com_RealTime;
	uiImports.trap_S_StartBackgroundTrack = S_StartBackgroundTrack;
	uiImports.trap_S_StopBackgroundTrack = S_StopBackgroundTrack;
	uiImports.trap_UpdateScreen = SCR_UpdateScreen;

	// load the dll or bytecode
	uivm = Sys_LoadDll("ui");
	if (!uivm) {
		Com_Error(ERR_FATAL, "VM_Create on ui failed");
	}

	static uiExport_t* (*vmMain)(uiImports_t * imports);
	vmMain = Sys_GetProcAddress(uivm, "vmMain");
	ui = vmMain(&uiImports);
	if (ui->version != UI_API_VERSION) {
		Com_Error(ERR_FATAL, "UI API version incorrect!\n");
	}


	ui->Init((cls.state >= CA_AUTHORIZING && cls.state < CA_ACTIVE));
}


qboolean UI_usesUniqueCDKey() {
	return qfalse;
}

/*
====================
UI_GameCommand

See if the current console command is claimed by the ui
====================
*/
qboolean UI_GameCommand( void ) {
	if ( !uivm ) {
		return qfalse;
	}

	return ui->ConsoleCommand(cls.realtime); 
}
