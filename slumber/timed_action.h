#ifndef _SLUMBER_TIMED_ACTION_H
#define _SLUMBER_TIMED_ACTION_H

#include "../../hightime.h"
#include "wdlstring.h"
#include "logger.h"

namespace Slumber
{
  typedef enum tagRUN_TYPE
  {
    SECOND,
    MINUTE,
    HOUR,
    DAY
  } RUN_TYPE;

  class TimedAction
  {
  protected:
    TimedAction();

  public:
    virtual ~TimedAction();
    bool is_time_to_run();
    int update_timer(int run_type, int interval);
    virtual int perform();
    int get_interval();
    int get_interval_type();
    void set_title(const char *title);
    char *get_title();
    void set_id(int id);
    int get_id();
    CHighTime get_last_run_timestamp();
    void set_last_run_timestamp(CHighTime timestamp);
    void to_log(const char *msg);
    Logger *get_log();

  protected:
    CHighTime m_last_run;
    CHighTimeSpan m_timespan;
    int m_interval;
    int m_interval_type;
    int m_id;
    WDL_String m_title;
    Logger *m_log;
  };
}


#endif // _SLUMBER_TIMED_ACTION_H