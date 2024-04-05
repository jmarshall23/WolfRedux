// ui_local.h
//

#ifndef __UI_LOCAL_H__
#define __UI_LOCAL_H__

#include "../game/q_shared.h"
#include "../cgame/tr_types.h"
#include "ui_public.h"

class idUserInterfaceSystem {
public:
	uiMenuCommand_t			currentMenu = UIMENU_NONE;
};

#endif
