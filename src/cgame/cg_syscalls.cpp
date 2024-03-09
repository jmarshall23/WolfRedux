// cg_syscalls.c
//

#include "cg_local.h"

cgameImports_t* engine;

void CG_Init(int serverMessageNum, int serverCommandSequence);
void CG_Shutdown(void);

cgameExport_t *vmMain(cgameImports_t* imports) {
	static cgameExport_t exports;

	engine = imports;

	exports.apiVersion = CGAME_IMPORT_API_VERSION;
	exports.Init = CG_Init;
	exports.Shutdown = CG_Shutdown;
	exports.ConsoleCommand = CG_ConsoleCommand;
	exports.DrawActiveFrame = CG_DrawActiveFrame;
	exports.CrosshairPlayer = CG_CrosshairPlayer;
	exports.LastAttacker = CG_LastAttacker;
	exports.KeyEvent = CG_KeyEvent;
	exports.MouseEvent = CG_MouseEvent;
	exports.EventHandling = CG_EventHandling;
	exports.GetTag = CG_GetTag;

	return &exports;
}