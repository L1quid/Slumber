#ifndef __SYSTRAY_H
#define __SYSTRAY_H

#define _WIN32_IE 0x0501

#include <windows.h>
#include <CommCtrl.h>
#include <shellapi.h>
#include <string>

class Systray {
public: 
  Systray(HWND wnd, int uid, int msg, HICON smallicon);
  ~Systray();
  void setTip(const char *tip);
  BOOL showBalloon(const char *title, const char *msg, DWORD icon_type = 0x00000001, int timeout = 10);
  BOOL showLastBalloon();
  BOOL refreshIcon() { return(addIcon()); }
private:
  BOOL addIcon();
  BOOL deleteIcon();
  BOOL setTip();
  int id, message;
  HWND hwnd;
  HICON icon;
  std::string tip;
  std::string balloon_title, balloon_msg;
};


#endif
