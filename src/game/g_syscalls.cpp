// Copyright (C) 1999-2000 Id Software, Inc.
//
#include "g_local.h"

gameImports_t* engine;

void G_InitGame(int levelTime, int randomSeed, int restart);
void G_RunFrame(int levelTime);
qboolean AICast_CheckAttackAtPos(int entnum, int enemy, vec3_t pos, qboolean ducking, qboolean allowHitWorld);
void G_RetrieveMoveSpeedsFromClient(int entnum, char* text);
void G_ShutdownGame(int restart);

// Function to create and return a populated gameExport_t struct
gameExport_t *vmMain(gameImports_t *imports) {
	static gameExport_t exports;

	engine = imports;

	// Map the function pointers to their implementations
	exports.version = GAME_API_VERSION;
	exports.G_InitGame = G_InitGame;
	exports.G_ShutdownGame = G_ShutdownGame;
	exports.ClientConnect = ClientConnect;
	exports.ClientThink = ClientThink;
	exports.ClientUserinfoChanged = ClientUserinfoChanged;
	exports.ClientDisconnect = ClientDisconnect;
	exports.ClientBegin = ClientBegin;
	exports.ClientCommand = ClientCommand;
	exports.G_RunFrame = G_RunFrame;
	exports.ConsoleCommand = ConsoleCommand;
	exports.BotAIStartFrame = BotAIStartFrame;
	exports.AICast_VisibleFromPos = AICast_VisibleFromPos;
	exports.AICast_CheckAttackAtPos = AICast_CheckAttackAtPos;
	exports.G_RetrieveMoveSpeedsFromClient = G_RetrieveMoveSpeedsFromClient;
	exports.G_GetModelInfo = G_GetModelInfo;

	return &exports;
}