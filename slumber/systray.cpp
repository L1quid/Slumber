#include "systray.h"

Systray::Systray(HWND wnd, int uid, int msg, HICON smallicon) {
  id = uid;
  hwnd = wnd;
  message = msg;
  icon = smallicon;
  int r = addIcon();
}

Systray::~Systray() {
  int r = deleteIcon();
}

void Systray::setTip(const char *_tip) {
  tip = _tip;
  tip = tip.substr(0, 64);
  setTip();
}

BOOL Systray::showBalloon(const char *title, const char *msg, DWORD icon_type, int timeout)
{
  BOOL res; 
  NOTIFYICONDATA tnid;

  if (!msg || !title)
    return(FALSE);
  
  balloon_title = title;
  balloon_msg = msg;

  tnid.cbSize = sizeof(NOTIFYICONDATA); 
  tnid.uID = id; 
  tnid.hWnd = hwnd; 
  tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_INFO;
  tnid.uCallbackMessage = message; 
  tnid.hIcon = icon;
  tnid.uTimeout = timeout * 1000;
  tnid.dwInfoFlags = icon_type;
  strncpy(tnid.szInfoTitle, title, 63);
  strncpy(tnid.szInfo, msg, 255);
  res = Shell_NotifyIcon(NIM_MODIFY, &tnid);

  return(res);
}

BOOL Systray::showLastBalloon()
{
  return(showBalloon(balloon_title.c_str(), balloon_msg.c_str()));
}

BOOL Systray::addIcon() { 
  BOOL res; 
  NOTIFYICONDATA tnid; 
 
  tnid.cbSize = sizeof(NOTIFYICONDATA); 
  tnid.uID = id; 
  tnid.hWnd = hwnd; 
  tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
  tnid.uCallbackMessage = message; 
  tnid.hIcon = icon;
  if (tip.length() > 64) {
    tip = tip.substr(0, 60);
    tip += "...";
  }
  if (tip != "") {
    //EncodedStr enc;
    //enc.convertFromUTF8(SvcStrCnv::UTF16, tip);
    //MEMCPY(tnid.szTip, enc.getEncodedBuffer(), enc.getBufferSize()); 
    strcpy(tnid.szTip, tip.c_str());
  } else {
    tnid.szTip[0] = '\0'; 
  }
 
  res = Shell_NotifyIcon(NIM_ADD, &tnid); 
 
  return res; 
} 

BOOL Systray::setTip() { 
  BOOL res; 
  NOTIFYICONDATA tnid; 
 
  tnid.cbSize = sizeof(NOTIFYICONDATA); 
  tnid.uFlags = NIF_TIP; 
  tnid.uID = id; 
	tnid.hWnd = hwnd; 
  if (tip.length() > 64) {
    tip = tip.substr(0, 60);
    tip += "...";
  }
  if (tip != "") {
    strcpy(tnid.szTip, tip.c_str());
    tnid.szTip[63] = 0;
  } else {
    tnid.szTip[0] = '\0'; 
  }
 
  res = Shell_NotifyIcon(NIM_MODIFY, &tnid); 
 
  return res; 
} 

 
BOOL Systray::deleteIcon() { 
  BOOL res; 
  NOTIFYICONDATA tnid; 
 
  tnid.cbSize = sizeof(NOTIFYICONDATA); 
  tnid.hWnd = hwnd; 
  tnid.uID = id; 
		 
  res = Shell_NotifyIcon(NIM_DELETE, &tnid); 
  return res; 
} 

