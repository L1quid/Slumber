#ifdef WIN32
#include <windows.h>
#include <Objbase.h>
#include <commctrl.h>
#endif // WIN32

#include "app.h"

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *szCmdLine, int iCmdShow)
{
  LoadLibrary(TEXT("Riched20.dll"));
  InitCommonControls();
  CoInitialize(NULL);

  return(Slumber::App::run());
}
#endif // WIN32