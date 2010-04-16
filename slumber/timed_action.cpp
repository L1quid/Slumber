#include "timed_action.h"

using namespace Slumber;

TimedAction::TimedAction()
{
  m_last_run = CHighTime::GetPresentTime();
  m_timespan = CHighTimeSpan(0, 0, 0, 0);
  m_id = 0;
  m_log = NULL;
}

TimedAction::~TimedAction()
{
  if (m_log)
  {
    delete m_log;
    m_log = NULL;
  }
}

bool TimedAction::is_time_to_run()
{
  CHighTime now = CHighTime::GetPresentTime();
  bool ret = now >= (m_last_run + m_timespan);

  return(ret);
}

int TimedAction::update_timer(int run_type, int interval)
{
  m_interval_type = run_type;
  
  switch (m_interval_type)
  {
  case RUN_TYPE::SECOND:
    m_timespan = CHighTimeSpan(0, 0, 0, interval, 0, 0, 0);
  break;

  case RUN_TYPE::MINUTE:
    m_timespan = CHighTimeSpan(0, 0, interval, 0, 0, 0, 0);
  break;

  case RUN_TYPE::HOUR:
    m_timespan = CHighTimeSpan(0, interval, 0, 0, 0, 0, 0);
  break;

  case RUN_TYPE::DAY:
    m_timespan = CHighTimeSpan(interval, 0, 0, 0, 0, 0, 0);
  break;

  default:
    return(1);
  }
  
  m_interval = interval;

  return(0);
}

int TimedAction::perform()
{
  m_last_run = CHighTime::GetPresentTime();

  return(0);
}

int TimedAction::get_interval()
{
  return(m_interval);
}

int TimedAction::get_interval_type()
{
  return(m_interval_type);
}

void TimedAction::set_title(const char *title)
{
  if (!m_log)
    m_log = new Logger(title);
  else
  {
#ifdef WIN32
    WDL_String old_fn = m_log->get_filename();
    m_log->set_title(title);
    WDL_String new_fn = m_log->get_filename();

    MoveFile(old_fn.Get(), new_fn.Get());
#else
    m_log->set_title(title);
#endif // WIN32
  }

  m_title = title;
}

char *TimedAction::get_title()
{
  return(m_title.Get());
}

void TimedAction::set_id(int id)
{
  m_id = id;
}

int TimedAction::get_id()
{
  return(m_id);
}

CHighTime TimedAction::get_last_run_timestamp()
{
  return(m_last_run);
}

void TimedAction::set_last_run_timestamp(CHighTime timestamp)
{
  m_last_run = timestamp;
}

void TimedAction::to_log(const char *msg)
{
  m_log->update(msg);
}

Logger *TimedAction::get_log()
{
  return(m_log);
}