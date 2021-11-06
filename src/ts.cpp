//This is the only .cpp file that gets the #include<TaskScheduler.h>.
//Without it, the linker would not find necessary TaskScheduler's compiled code.
//
//Remember to put customization macros here as well.
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_STD_FUNCTION   // Compile with support for std::function 
#define _TASK_SCHEDULING_OPTIONS
#include <TaskScheduler.h>
#include "globals.h"
#include <vector>

// TaskScheduler - Let the runner object be a global, single instance shared between object files.
Scheduler ts;

/*
 * Add this to your sources if you want to reuse task scheduler object for your tasks
extern Scheduler ts;
*/

/*
Note that call to tasker 'ts.execute();' is made in 'embui.handle();'
make sure to add it to the loop()
 */

std::vector<Task*> *taskTrash = nullptr;    // ptr to a vector container with obsolete tasks

void GC_taskRecycle(Task *t){
    if (!taskTrash)
        taskTrash = new std::vector<Task*>(8);

    taskTrash->emplace_back(t);
}

// Dyn tasks garbage collector
void GC_taskGC(){
    if (!taskTrash || taskTrash->empty())
        return;

    size_t heapbefore = ESP.getFreeHeap();
    for(auto& _t : *taskTrash) { delete _t; }

    delete taskTrash;
    taskTrash = nullptr;
    LOG(printf_P, PSTR("UI: task garbage collect: released %d bytes\n"), ESP.getFreeHeap() - heapbefore);
}
