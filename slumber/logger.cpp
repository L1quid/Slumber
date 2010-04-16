#include "logger.h"
#include "app.h"

using namespace Slumber;

Logger::Logger(const char *title) : m_open(false)
{
  set_title(title);
}

Logger::~Logger()
{
  if (m_open && m_output.is_open())
    end();
}

int Logger::start()
{
  if (m_output.is_open())
    return(0);

  m_output.open(m_filename.Get(), std::ios::out | std::ios::app);
  m_open = true;
  //update("Log started");

  return(0);
}

int Logger::end()
{
  if (!m_output.is_open())
    return(1);

  //update("Log ended");
  m_output << std::endl;
  m_output.close();
  m_open = false;

  return(0);
}

int Logger::update(const char *msg)
{
  if (!m_output.is_open())
    return(1);

  char buf[100];
  WDL_String out;

  int len = get_timestamp(buf);
  buf[len] = '\0';
  out.Append(buf);
  out.Append(" ");
  out.Append(msg);
  char *outmsg = NULL;
  outmsg = out.Get();
  m_output << outmsg << "\r\n";

  return(0);
}

int Logger::get_timestamp(char *buf)
{
  CHighTime time = CHighTime::GetPresentTime();
  int len = sprintf(buf, "[%d/%02d/%02d %02d:%02d:%02d]", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

  return(len);
}

void Logger::set_title(const char *title)
{
  WDL_String fn;

  m_title = title;
  fn.Append("logs");

#ifdef WIN32
  fn.Append("\\");
#else
  fn.Append("/");
#endif // WIN32

  fn.Append(title);
  fn.Append(".log");
  m_filename = App::get()->base_dir(fn.Get());
}

char *Logger::get_filename()
{
  return(m_filename.Get());
}

char *Logger::get_title()
{
  return(m_title.Get());
}

void Logger::get_log_snippet(WDL_String &logout)
{
  FILE *fp = NULL;
  const char *fn = m_filename.Get();

  if (!(fp = fopen(fn, "r")))
  {
    logout = "Unable to open log file: ";
    logout.Append(m_filename.Get());

    return;
  }

  int pos = 0;
  int len = 1024;
  char buf[1024];
  fseek(fp, 0, SEEK_END);
  pos = ftell(fp);
  
  if (pos < len)
    len = pos;

  fseek(fp, -len, SEEK_END);
  int bread = fread(buf, sizeof(char), len, fp);
  buf[bread >= len ? len : bread] = '\0';
  logout = buf;
  fclose(fp);
}