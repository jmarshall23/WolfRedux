// ui_syscalls.c
//

#include "ui_local.h"

uiImports_t *engine;

void _UI_Init(qboolean);
void _UI_Shutdown(void);
void _UI_KeyEvent(int key, qboolean down);
void _UI_MouseEvent(int dx, int dy);
void _UI_Refresh(int realtime);
qboolean _UI_IsFullscreen(void);

uiExport_t *vmMain(uiImports_t *uiImports) {
	// Save uiImports to the global variable
	engine = uiImports;

	// Initialize uiExports and assign its functions
	static uiExport_t uiExports;
	uiExports.version = UI_API_VERSION;
	uiExports.Init = _UI_Init;
	uiExports.Shutdown = _UI_Shutdown;

	uiExports.KeyEvent = _UI_KeyEvent;
	uiExports.MouseEvent = _UI_MouseEvent;
	uiExports.Refresh = _UI_Refresh;
	uiExports.IsFullscreen = _UI_IsFullscreen;
	uiExports.SetActiveMenu = _UI_SetActiveMenu;
	uiExports.GetActiveMenu = _UI_GetActiveMenu; // Casting to match the signature in the struct
	uiExports.ConsoleCommand = UI_ConsoleCommand;
	uiExports.DrawConnectScreen = UI_DrawConnectScreen;

	// Return the populated uiExports
	return &uiExports;
}