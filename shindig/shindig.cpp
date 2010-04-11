#include "stdafx.h"
#include "app.hpp"
#include "system.hpp"

bool failure()
{
  return false;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{

  RETURN_ON_FAIL_BOOL(failure(), ErrorPredicate<bool>, LOG_ERROR_LN, "tjing tjong: %d", 52);

	if (!App::instance().init(hInstance)) {
		return 1;
	}

	App::instance().run();

	if (!App::instance().close()) {
		return 1;
	}

  return 0;
}
