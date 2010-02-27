#include "stdafx.h"
#include "app.hpp"
#include "system.hpp"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
  App app(hInstance);
  app.init();
  app.run();
  return 0;
}
