#include "stdafx.h"
#include "app.hpp"
#include "system.hpp"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{

	LOG_MGR.break_on_error(true);

	if (!App::instance().init(hInstance)) {
		return 1;
	}

	App::instance().run();

	if (!App::instance().close()) {
		return 1;
	}

  return 0;
}
