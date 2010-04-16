#include "main_wnd.h"
#include "wdlstring.h"
#include "ptrlist.h"
#include "app.h"
#include "wingui/wndsize.h"

#define SYSTRAY_UID WM_USER + 0x420
#define SYSTRAY_MSG WM_USER + 0x421
#define SLUMBER_TIMER WM_USER + 0x666
#define WM_POSTINIT WM_USER + 0x777

using namespace Slumber;

const char *RUN_TYPE_LABELS[] = {"second(s)", "minute(s)", "hour(s)", "day(s)"};

int ListView_GetCurSel(HWND ctrl)
{
  int count = ListView_GetItemCount(ctrl);

  for(int i = 0; i < count; i++)
  {
    if(ListView_GetItemState(ctrl, i, LVIS_SELECTED) & LVIS_SELECTED)
      return(i);
  }

  return(-1);
}

MainWnd::MainWnd()
{
#ifdef WIN32
  m_systray = NULL;
  m_selected_task = NULL;
  m_hwnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAIN), NULL, MainWnd::WndProc);

  if (!m_hwnd)
    return;

  SetLastError(0);
  LONG_PTR ret = SetWindowLongPtr(m_hwnd, DWL_USER, (LONG)this);

  if (!ret && GetLastError())
  {
    destroy();
    return;
  }

  SendMessage(m_hwnd, WM_INITDIALOG, 0, 0);
  m_hicon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);

  if (m_hicon)
  {
    SendMessage(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hicon);
    m_systray = new Systray(m_hwnd, SYSTRAY_UID, SYSTRAY_MSG, m_hicon);
    m_systray->setTip("Slumber");
  }

#endif // WIN32
}

MainWnd::~MainWnd()
{
#ifdef WIN32
  if (m_hicon)
  {
    DestroyIcon(m_hicon);
    delete m_systray;
  }
#endif // WIN32
}

MainWnd *MainWnd::create()
{
  MainWnd *wnd = NULL;

  if (!(wnd = new MainWnd()))
    return(NULL);

  if (!wnd->handle())
  {
    delete wnd;
    return(NULL);
  }

  return(wnd);
}

int MainWnd::destroy()
{
  DestroyWindow(m_hwnd);
  m_hwnd = NULL;

  return(0);
}

void MainWnd::show()
{
#ifdef WIN32
  ShowWindow(m_hwnd, SW_SHOW);
#endif // WIN32
}

void MainWnd::hide()
{
#ifdef WIN32
  ShowWindow(m_hwnd, SW_HIDE);
#endif // WIN32
}

void MainWnd::exit()
{
  destroy();
  PostQuitMessage(0);
}

void MainWnd::create_new_task()
{

}

void MainWnd::show_preferences()
{

}

void MainWnd::show_about()
{
  MessageBox(handle(), "Slumber - a small Windows task scheduler\r\n\r\nby Daniel Green\r\nhttp://antimac.org/slumber", "About", MB_OK);
}

#ifdef WIN32
HWND MainWnd::handle()
{
  return(m_hwnd);
}

int CALLBACK MainWnd::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static WDL_WndSizer resize;
  MainWnd *this_wnd = NULL;

  if (!(this_wnd = (MainWnd *)GetWindowLongPtr(hwnd, DWL_USER)))
    return(0);

  switch (msg)
  {
  case WM_INITDIALOG:
    {
      HWND tasklist = GetDlgItem(hwnd, IDC_TASKS);
      LVCOLUMN lvc;
      lvc.mask = LVCF_TEXT | LVCF_WIDTH;
      lvc.pszText = "Title";
      lvc.cx = 250;
      ListView_InsertColumn(tasklist, 0, &lvc);

      lvc.pszText = "Command";
      lvc.cx = 350;
      ListView_InsertColumn(tasklist, 1, &lvc);

      lvc.pszText = "Schedule";
      lvc.cx = 200;
      ListView_InsertColumn(tasklist, 2, &lvc);
      ListView_SetExtendedListViewStyle(tasklist, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

      resize.init(hwnd);
      resize.init_item(IDC_TASKS, 0.0, 0.0, 1.0, 0.0);
      resize.init_item(IDC_LOGFRAME, 0.0, 0.0, 1.0, 1.0);
      resize.init_item(IDC_JOBLOG, 0.0, 0.0, 1.0, 1.0);
      resize.init_item(IDC_WITH_SEL_LABEL, 1.0, 0.0, 1.0, 0.0);
      resize.init_item(IDC_EDIT_TASK, 1.0, 0.0, 1.0, 0.0);
      resize.init_item(IDC_DELETE_TASK, 1.0, 0.0, 1.0, 0.0);
      resize.init_item(IDC_PERFORM_TASK, 1.0, 0.0, 1.0, 0.0);
      //resize.init_item(IDC_TASKS, 0.0, 0.0, 1.0, 1.0);
      PostMessage(hwnd, WM_POSTINIT, 0, 0);
    }
  break;

  case WM_POSTINIT:
    this_wnd->on_list_update();
    App::get()->activate_task(NULL);
    this_wnd->show();
  break;

  case WM_CLOSE:
  case WM_QUIT:
    this_wnd->exit();
  break;

  case WM_SIZE:
    if (wParam != SIZE_MINIMIZED) 
      resize.onResize();
  break;

  case WM_COMMAND:
    switch (LOWORD(wParam))
    {
    case ID_FILE_EXIT:
      this_wnd->exit();
    break;

    case IDC_CREATE_TASK:
    case ID_FILE_NEWTASK:
      this_wnd->create_new_task();
    break;

    case ID_EDIT_PREFERENCES:
      this_wnd->show_preferences();
    break;

    case ID_HELP_ABOUT:
      this_wnd->show_about();
    break;

    case IDC_EDIT_TASK:
      App::get()->edit_task(this_wnd->get_selected_task());
      this_wnd->on_list_update();
    break;

    case IDC_DELETE_TASK:
      this_wnd->confirm_delete_selected();
    break;

    case IDC_PERFORM_TASK:
      App::get()->run_task(this_wnd->get_selected_task());
      this_wnd->on_list_update();
    break;
    }
  break;

  case WM_NOTIFY:
    {
      LPNMHDR l = (LPNMHDR)lParam;

      if (l->code != LVN_ITEMACTIVATE && l->code != NM_CLICK && l->code != NM_DBLCLK && l->code != NM_RCLICK)
        return(1);

      switch (l->idFrom)
      {
      case IDC_TASKS:
        {
          HWND ctrl = GetDlgItem(hwnd, IDC_TASKS);
          LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
          int task_id = 0;
          App *the_app = App::get();
          task_id = ListView_GetCurSel(ctrl) + 1;

          if (!task_id)
          {
            the_app->activate_task(NULL);
            return(1);
          }

          ShellAction *action = the_app->get_task_by_id(task_id);
          this_wnd->set_selected_task(action);

          switch (l->code)
          {
          case LVN_ITEMACTIVATE:
          case NM_CLICK:
            the_app->activate_task(action);
          break;

          case NM_DBLCLK:
            if (!action)
              return(1);

            the_app->edit_task(action);
          break;

          case NM_RCLICK:
            {
              HMENU menu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU2));
              HMENU submenu = GetSubMenu(menu, 0);
              POINT pt;
              GetCursorPos(&pt);
              int id = TrackPopupMenu(submenu, TPM_LEFTALIGN| TPM_RETURNCMD | TPM_NONOTIFY|TPM_LEFTBUTTON , pt.x, pt.y, NULL, hwnd, NULL);
              this_wnd->on_list_right_click(id);
              DestroyMenu(submenu);
              DestroyMenu(menu);

              return(1);
            }
          break;

          default:
            return(1);
          }
        }
      break;

      default:
        return(1);
      }

      return(0);
    }
  break;

  case WM_TIMER:
    if(LOWORD(wParam) == SLUMBER_TIMER)
    {
      App::get()->on_timer_tick();
      return(0);
    }
  break;
  }

  return(0);
}
#endif // WIN32

void MainWnd::on_list_update()
{
  WDL_PtrList<ShellAction> *actions = NULL;
  App *the_app = NULL;
  the_app = App::get();
  actions = the_app->get_tasks();

#ifdef WIN32
  HWND ctrl = GetDlgItem(m_hwnd, IDC_TASKS);
  int sel_id = -1;
  sel_id = ListView_GetCurSel(ctrl);
  ListView_DeleteAllItems(ctrl);

  if(!actions->GetSize())
    return;

  for(int i = 0; i < actions->GetSize(); i++)
  {
    LVITEM lvitem;
    WDL_String tmp;
    char buf[128];
    ShellAction *action = actions->Get(i);

    lvitem.mask = LVIF_TEXT | LVIF_PARAM;
    lvitem.iItem = i;
    lvitem.iSubItem = 0;
    lvitem.lParam = (LPARAM)action->get_id();
    lvitem.cchTextMax = strlen(action->get_title());
    lvitem.pszText = action->get_title();

    ListView_InsertItem(ctrl, &lvitem);
    ListView_SetItemText(ctrl, i, 1, action->get_path());

    tmp = "Every ";
    itoa(action->get_interval(), buf, 10);
    tmp.Append(buf);
    tmp.Append(" ");
    tmp.Append(RUN_TYPE_LABELS[action->get_interval_type()]);

    ListView_SetItemText(ctrl, i, 2, tmp.Get());
  }

  if (sel_id >= 0 && m_selected_task)
  {
    ShellAction *action = the_app->get_task_by_id(sel_id + 1);
    the_app->activate_task(action);
    ListView_SetItemState(ctrl, sel_id, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
  }
#endif // WIN32
}

#ifdef WIN32
void MainWnd::start_timer()
{
  SetTimer(handle(), SLUMBER_TIMER, 1000, NULL);
}
#endif // WIN32

void MainWnd::display_task(ShellAction *task, WDL_String log_data)
{
#ifdef WIN32
  HWND wnd = handle();
  HWND ctrl = GetDlgItem(wnd, IDC_JOBLOG);
  int enable = task ? 1 : 0;

  EnableWindow(ctrl, enable);
  EnableWindow(GetDlgItem(wnd, IDC_WITH_SEL_LABEL), enable);
  EnableWindow(GetDlgItem(wnd, IDC_EDIT_TASK), enable);
  EnableWindow(GetDlgItem(wnd, IDC_DELETE_TASK), enable);
  EnableWindow(GetDlgItem(wnd, IDC_PERFORM_TASK), enable);
  EnableWindow(GetDlgItem(wnd, IDC_LOGFRAME), enable);
  SetWindowText(ctrl, log_data.Get());

  if (task)
  {
    WDL_String tmp;
    tmp = "Task Log - ";
    tmp.Append(task->get_title());
    SetDlgItemText(wnd, IDC_LOGFRAME, tmp.Get());
    scroll_joblog_to_bottom();
  }
  else
  {
    SetDlgItemText(wnd, IDC_LOGFRAME, "Task Log");
  }

#endif // WIN32
}

void MainWnd::on_list_right_click(int cmd)
{
  switch (cmd)
  {
  case ID_POPUP_EDITTASK:
    App::get()->edit_task(get_selected_task());
    on_list_update();
  break;

  case ID_POPUP_DELETETASK:
    confirm_delete_selected();
  break;

  case ID_POPUP_RUN:
    App::get()->run_task(get_selected_task());
    on_list_update();
  break;
  }
}

ShellAction *MainWnd::get_selected_task()
{
  return(m_selected_task);
}

void MainWnd::set_selected_task(ShellAction *task)
{
  m_selected_task = task;
}

void MainWnd::confirm_delete_selected()
{
#ifdef WIN32
  if (MessageBox(handle(), "Are you sure you want to delete this task?", "Confirm deletion...", MB_YESNO) == IDNO)
    return;
#endif // WIN32

  App::get()->delete_task(get_selected_task());
  set_selected_task(NULL);
  on_list_update();
}

void MainWnd::scroll_joblog_to_bottom()
{
#ifdef WIN32
  HWND ctrl = GetDlgItem(handle(), IDC_JOBLOG);
  int line = SendMessage(ctrl, EM_GETFIRSTVISIBLELINE, 0, 0);
  int linecount = SendMessage(ctrl, EM_GETLINECOUNT, 0, 0);
  SendMessage(ctrl, EM_LINESCROLL, 0, (linecount - line - 12)); 
#endif // WIN32
}