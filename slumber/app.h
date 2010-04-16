#ifndef _SLUMBER_APP_H
#define _SLUMBER_APP_H

#include <windows.h>
#include "../../wdl/ptrlist.h"
#include "main_wnd.h"
#include "timed_action.h"
#include "shell_action.h"

namespace Slumber
{
  // singleton
  class App
  {
  protected:
    App();

  public:
    ~App();

    static App *get();
    static void set(App *the_app);
    static int run();
    int load_settings();
    void save_settings();
    WDL_String base_dir(const char *filename);
    int process_os_messages();
    MainWnd *get_main_wnd();
    void shutdown();
    WDL_PtrList<ShellAction> *get_tasks();
    ShellAction *get_task_by_id(int id);
    void start_timers();
    void on_timer_tick();
    void activate_task(ShellAction *task);
    ShellAction *create_task();
    ShellAction * edit_task(ShellAction *task = NULL);
    void delete_task(ShellAction *task);
    int run_task(ShellAction *task);


#ifdef WIN32
    HANDLE App::get_tasks_mutex();
#else
    void *App::get_tasks_mutex();
#endif // WIN32

  protected:
    MainWnd *m_main_wnd;
    WDL_PtrList<ShellAction> m_actions;
    WDL_String m_base_dir;

#ifdef _WIN32
    HANDLE m_tasks_mutex;
#else
    void *m_tasks_mutex;
#endif
  };
}


#endif // _SLUMBER_APP_H