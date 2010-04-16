#include "app.h"
#include "db.h"
#include "enums.h"

using namespace Slumber;
App *g_slumber_app = NULL;

App::App() : m_main_wnd(NULL)
{
#ifdef WIN32
  char tmp[MAX_PATH];

  if (!GetCurrentDirectory(MAX_PATH - 1, tmp))
    return;

  m_base_dir = tmp;
  m_tasks_mutex = NULL; //CreateMutex(0, false, NULL);
#else
  m_tasks_mutex = NULL; //(VOID)new char[1]; // fucko
#endif // WIN32

  App::set(this);
}

App::~App()
{
#ifdef WIN32
  //CloseHandle(m_tasks_mutex);
#else
  //delete [] m_tasks_mutex;
#endif // WIN32
}

App *App::get()
{
  return(g_slumber_app);
}

void App::set(App *the_app)
{
  g_slumber_app = the_app;
}

int App::run()
{
  int ret = 0;
  App *the_app = NULL;
  the_app = App::get();

  if (!the_app)
  {
    if (!(the_app = new Slumber::App()))
      return(-1);
  }

  the_app->load_settings();
  MainWnd *main_wnd = NULL;

  if (!(main_wnd = the_app->get_main_wnd()))
    return(-1);

  the_app->start_timers();
  ret = the_app->process_os_messages();
  the_app->save_settings();
  the_app->shutdown();
  delete the_app;

  return(ret);
}

int App::load_settings()
{
  WDL_String full_path = base_dir("tasks.yaml");
  MAP_LIST *tmplist = NULL;
  tmplist = DB::get_task_list(full_path.Get());

  if (!tmplist)
    return(STATUS::E_LOADING_FAILED);

  for (int i = 0; i < tmplist->GetSize(); i++)
  {
    MAP_TYPE *task = tmplist->Get(i);
    ShellAction *action = NULL;

    if (!(action = new ShellAction(task->command.c_str(), task->params.c_str())))
      break;

    action->set_title(task->title.c_str());
    action->set_id(i + 1);
    action->set_last_run_timestamp(task->last_run_at_timestamp);
    action->update_timer(task->interval_type, task->interval);
    m_actions.Add(action);
  }

  tmplist->Empty(true);
  delete tmplist;

  return(STATUS::E_OK);
}

void App::save_settings()
{
  WDL_String full_path = base_dir("tasks.yaml");
  MAP_LIST *tmplist = NULL;
  
  if (!(tmplist = new MAP_LIST()))
    return;

  for (int i = 0; i < m_actions.GetSize(); i++)
  {
    ShellAction *action = m_actions.Get(i);
    MAP_TYPE *task = new MAP_TYPE();
    task->command = action->get_path();
    task->params = action->get_params();
    task->interval = action->get_interval();
    task->interval_type = action->get_interval_type();
    task->title = action->get_title();
    task->last_run_at_timestamp = action->get_last_run_timestamp();
    tmplist->Add(task);
  }

  DB::save_task_list(full_path.Get(), tmplist);
  tmplist->Empty(true);
  delete tmplist;
  m_actions.Empty(true);
}

WDL_String App::base_dir(const char *filename)
{
  WDL_String out;
  out.Append(m_base_dir.Get());

#ifdef WIN32
  out.Append("\\");
#else
  out.Append("/");
#endif // WIN32

  out.Append(filename);

  return(out);
}

int App::process_os_messages()
{
  int ret = 0;

#ifdef WIN32
  MSG msg;

  while(GetMessage(&msg, NULL, 0, 0) > 0)
  {
//    if (TranslateAccelerator(g_ui->MainWindow(), main_accel, &msg))
//      continue;

//    if (IsDialogMessage(g_hwnd_prefs, &msg))
//      continue;

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  ret = (int)msg.wParam;
#endif // WIN32

  return(ret);
}

MainWnd *App::get_main_wnd()
{
  if (!m_main_wnd)
    m_main_wnd = MainWnd::create();
  
  return(m_main_wnd);
}

void App::shutdown()
{
  delete m_main_wnd;
}

WDL_PtrList<ShellAction> *App::get_tasks()
{
  return(&m_actions);
}

ShellAction *App::get_task_by_id(int id)
{
  for (int i = 0; i < m_actions.GetSize(); i++)
  {
    ShellAction *tmp = NULL;
    tmp = m_actions.Get(i);

    if (tmp->get_id() == id)
      return(tmp);
  }

  return(NULL);
}

void App::start_timers()
{
#ifdef WIN32
  get_main_wnd()->start_timer();
#endif // WIN32
}

void App::on_timer_tick()
{
  bool needs_ui_update = false;

  for(int i = 0; i < m_actions.GetSize(); i++)
  {
    if (m_actions.Get(i)->is_time_to_run())
    {
      m_actions.Get(i)->perform();
      needs_ui_update = true;
    }
  }

  if (needs_ui_update)
    get_main_wnd()->on_list_update();
}

void App::activate_task(Slumber::ShellAction *task)
{
  WDL_String log_data;

  if (task)
    task->get_log()->get_log_snippet(log_data);

  get_main_wnd()->display_task(task, log_data);
}

ShellAction *App::create_task()
{
  return(edit_task(NULL));
}

ShellAction * App::edit_task(ShellAction *task /* = NULL */)
{
  if (!task)
    task = new ShellAction();

  return(task);
}

void App::delete_task(ShellAction *task)
{
  for (int i = 0; i < m_actions.GetSize(); i++)
  {
    ShellAction *action = m_actions.Get(i);

    if (action == task)
    {
      m_actions.Delete(i, true);
      break;
    }
  }
}

#ifdef WIN32
HANDLE App::get_tasks_mutex()
#else
void *App::get_tasks_mutex()
#endif // WIN32
{
  return(m_tasks_mutex);
}

int App::run_task(ShellAction *task)
{
  int ret = task->perform();

  return(ret);
}