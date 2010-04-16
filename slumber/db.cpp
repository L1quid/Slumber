#include <string>
#include <iostream>
#include <fstream>

#include "db.h"
#include "enums.h"

using namespace Slumber;

void operator >> (const YAML::Node& node, TASKSTRUCT& task)
{
  node["title"] >> task.title;
  node["command"] >> task.command;
  node["params"] >> task.params;
  node["interval"] >> task.interval;
  std::string tmp;
  node["interval_type"] >> tmp;

  if (tmp == "seconds")
    task.interval_type = 0;
  else if (tmp == "minutes")
    task.interval_type = 1;
  else if (tmp == "hours")
    task.interval_type = 2;
  else if (tmp == "days")
    task.interval_type = 3;
  else
    task.interval_type = 0;

  node["last_run_at"] >> task.last_run_at;

  if (!strcmp(task.last_run_at.c_str(), ""))
    task.last_run_at_timestamp = CHighTime::GetPresentTime(); // what is the best default?
  else
  {
    // parse date string, bitch
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    sscanf(task.last_run_at.c_str(), "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

    if (!month)
      task.last_run_at_timestamp = CHighTime::GetPresentTime();
    else
      task.last_run_at_timestamp = CHighTime(year, month, day, hour, minute, second);
  }
}

int DB::save_task_list(const char *filename, MAP_LIST *tasks)
{
  const char *type_labels[] = {"seconds", "minutes", "hours", "days"};
  std::ofstream ofs(filename);

  if (!ofs.is_open())
    return(STATUS::E_LOADING_FAILED);

  ofs << "# slumber - your tasks (" << tasks->GetSize() << " of 'em)" << std::endl << std::endl;

  for (int i = 0; i < tasks->GetSize(); i++)
  {
    MAP_TYPE *task = tasks->Get(i);
    char buf[100];
    task->last_run_at_timestamp.Format(buf, 100, "%Y\/%m\/%d\ %H\:%M\:%S");
    ofs << "- title: " << task->title << std::endl;
    ofs << "  command: " << task->command << std::endl;
    ofs << "  params: " << task->params << std::endl;
    ofs << "  interval: " << task->interval << std::endl;
    ofs << "  interval_type: " << type_labels[task->interval_type] << std::endl;
    ofs << "  last_run_at: " << buf << std::endl << std::endl;
  }

  ofs.close();

  return(STATUS::E_OK);
}

MAP_LIST *DB::get_task_list(const char *filename)
{
  std::ifstream ifs;

  try
  {
    ifs.open(filename);
  }
  catch (...)
  {
    return(NULL);
  }

  MAP_LIST *out = NULL;

  if (!(out = new MAP_LIST()))
    return(NULL);

  try
  {
    YAML::Parser parser(ifs);
    YAML::Node doc;
    parser.GetNextDocument(doc);

    for (unsigned int i = 0; i < doc.size(); i++)
    {
      MAP_TYPE *obj = NULL;
      obj = new MAP_TYPE();
      doc[i] >> *obj;
      out->Add(obj);
    }
  }
  catch (...)
  {
    ifs.close();
    delete out;
    return(NULL);
  }

  ifs.close();

  return(out);
}