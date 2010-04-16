#ifndef _SLUMBER_LOGGER_H
#define _SLUMBER_LOGGER_H

#include <iostream>
#include <fstream>
#include "wdlstring.h"

namespace Slumber
{
  class Logger
  {
  public:
    Logger(const char *title);
    ~Logger();

    int start();
    int end();
    int update(const char *msg);
    int get_timestamp(char *buf);
    void set_title(const char *title);
    char *get_filename();
    char *get_title();
    void get_log_snippet(WDL_String &logout);

  protected:
    WDL_String m_title, m_filename;
    bool m_open;
    std::ofstream m_output;
  };
}

#endif // _SLUMBER_LOGGER_H