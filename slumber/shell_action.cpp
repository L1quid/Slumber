#include "shell_action.h"

using namespace Slumber;

ShellAction::ShellAction(const char *path, const char *params) : TimedAction()
{
  m_path = path;
  m_params = params;
}

ShellAction::~ShellAction()
{
}

int ShellAction::perform()
{
  m_log->start();
  to_log("Job started");

  char dir[MAX_PATH];
  strcpy_s(dir, MAX_PATH, m_path.Get());

  int i = m_path.GetLength() - 1;
  int ret = 1;

  while(dir[i] != '\\' && dir[i] != '//')
    dir[i--] = '\0';

#ifdef WIN32
  char params[1024];
  memset(params, '\0', 1024);
  int written = get_expanded_params(params, 1023);

  ret = (int)ShellExecute(NULL, NULL, m_path.Get(), params, dir, SW_SHOWNORMAL);
  WDL_String errmsg = "Error performing task: ";

  switch (ret)
  {
  case 0:
    errmsg.Append("The operating system is out of memory or resources.");
  break;

  case ERROR_FILE_NOT_FOUND:
    errmsg.Append("The specified file was not found.");
  break;

  case ERROR_PATH_NOT_FOUND:
    errmsg.Append("The specified path was not found.");
  break;

  case ERROR_BAD_FORMAT:
    errmsg.Append("The .exe file is invalid (non-Microsoft Win32 .exe or error in .exe image).");
  break;

  case SE_ERR_ACCESSDENIED:
    errmsg.Append("The operating system denied access to the specified file.");
  break;

  case SE_ERR_ASSOCINCOMPLETE:
    errmsg.Append("The file name association is incomplete or invalid.");
  break;

  case SE_ERR_DDEBUSY:
    errmsg.Append("The Dynamic Data Exchange (DDE) transaction could not be completed because other DDE transactions were being processed.");
  break;

  case SE_ERR_DDEFAIL:
    errmsg.Append("The DDE transaction failed.");
  break;

  case SE_ERR_DDETIMEOUT:
    errmsg.Append("The DDE transaction could not be completed because the request timed out.");
  break;

  case SE_ERR_DLLNOTFOUND:
    errmsg.Append("The specified DLL was not found.");
  break;

  case SE_ERR_NOASSOC:
    errmsg.Append("There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.");
  break;

  case SE_ERR_OOM:
    errmsg.Append("There was not enough memory to complete the operation.");
  break;

  case SE_ERR_SHARE:
    errmsg.Append("A sharing violation occurred.");
  break;

  default:
    errmsg = "Task performed successfully.";
    ret = 0;
  break;
  }

  to_log(errmsg.Get());
#endif // WIN32

  TimedAction::perform();
  m_log->end();

  return(ret);
}

void ShellAction::set_path(const char *path)
{
  m_path = path;
}

void ShellAction::set_params(const char *pms)
{
  m_params = pms;
}

char *ShellAction::get_path()
{
  return(m_path.Get());
}

char *ShellAction::get_params()
{
  return(m_params.Get());
}

int ShellAction::get_expanded_params(char *buf, int max_len)
{
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  return(strftime(buf, max_len, m_params.Get(), timeinfo));
}