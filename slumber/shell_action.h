#ifndef _SLUMBER_SHELL_ACTION_H
#define _SLUMBER_SHELL_ACTION_H

#include "timed_action.h"
#include "../../wdl/wdlstring.h"

namespace Slumber
{
  class ShellAction : public TimedAction
  {
  public:
    ShellAction(const char *path = NULL, const char *params = NULL);
    virtual ~ShellAction();

    int perform();

    void set_path(const char *path);
    void set_params(const char *pms);

    char *get_path();
    char *get_params();
    int get_expanded_params(char *buf, int max_len);

  private:
    WDL_String m_path, m_params;
  };
}

#endif // _SLUMBER_SHELL_ACTION_H