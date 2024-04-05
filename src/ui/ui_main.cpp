// ui_main.cpp
//

#include "ui_local.h"

uiImports_t* engine;
idUserInterfaceSystem ui;


/*
====================
UI_Init
====================
*/
void UI_Init(qboolean) {

}

/*
====================
UI_Shutdown
====================
*/
void UI_Shutdown(void) {

}

/*
====================
UI_KeyEvent
====================
*/
void UI_KeyEvent(int key, qboolean isDown) {

}

/*
====================
UI_MouseEvent
====================
*/
void UI_MouseEvent(int dx, int dy) {

}

/*
====================
UI_Refresh
====================
*/
void UI_Refresh(int time) {

}

/*
====================
UI_IsFullscreen
====================
*/
qboolean UI_IsFullscreen(void) {
	return qfalse; // Assuming qfalse is defined appropriately.
}

/*
====================
UI_SetActiveMenu
====================
*/
void UI_SetActiveMenu(uiMenuCommand_t menu) {
	ui.currentMenu = menu;
}

/*
====================
UI_GetActiveMenu
====================
*/
uiMenuCommand_t UI_GetActiveMenu(void) {
	return UIMENU_NONE;
}

/*
====================
UI_ConsoleCommand
====================
*/
qboolean UI_ConsoleCommand(int realTime) {
	return qfalse; // Assuming qfalse is defined appropriately.
}

/*
====================
UI_DrawConnectScreen
====================
*/
void UI_DrawConnectScreen(qboolean overlay) {

}

/*
====================
UI_HasInputControl
====================
*/
qboolean UI_HasInputControl(void) {
	return ui.currentMenu != UIMENU_NONE;
}


uiExport_t* vmMain(uiImports_t* uiImports) {
	// Save uiImports to the global variable
	engine = uiImports;

	// Initialize uiExports and assign its functions
	static uiExport_t uiExports;
	uiExports.version = UI_API_VERSION;
	uiExports.Init = UI_Init;
	uiExports.Shutdown = UI_Shutdown;

	uiExports.KeyEvent = UI_KeyEvent;
	uiExports.MouseEvent = UI_MouseEvent;
	uiExports.Refresh = UI_Refresh;
	uiExports.IsFullscreen = UI_IsFullscreen;
	uiExports.SetActiveMenu = UI_SetActiveMenu;
	uiExports.GetActiveMenu = UI_GetActiveMenu; // Casting to match the signature in the struct
	uiExports.ConsoleCommand = UI_ConsoleCommand;
	uiExports.DrawConnectScreen = UI_DrawConnectScreen;
	uiExports.HasInputControl = UI_HasInputControl;

	// Return the populated uiExports
	return &uiExports;
}