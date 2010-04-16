#ifndef _SLUMBER_MAIN_WND_H
#define _SLUMBER_MAIN_WND_H

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "resource.h"
#include "systray.h"
#include "shell_action.h"
#endif // WIN32

namespace Slumber
{
  class MainWnd
  {
  protected:
    MainWnd();

  public:
    ~MainWnd();
    static MainWnd *create();
    int destroy();
    void show();
    void hide();
    void exit();
    void create_new_task();
    void show_preferences();
    void show_about();
    void on_list_update();
    void display_task(ShellAction *task, WDL_String log_data);
    void on_list_right_click(int cmd);
    ShellAction *get_selected_task();
    void set_selected_task(ShellAction *task);
    void confirm_delete_selected();
    void scroll_joblog_to_bottom();

#ifdef WIN32
    void start_timer();
    HWND handle();
    static int CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // WIN32

  protected:
#ifdef WIN32
    HWND m_hwnd;
    HICON m_hicon;
    Systray *m_systray;
    ShellAction *m_selected_task;
#endif // WIN32
  };
}

#endif // _SLUMBER_MAIN_WND_H