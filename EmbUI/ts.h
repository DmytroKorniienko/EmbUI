// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#pragma once
// Task Scheduler lib   https://github.com/arkhipenko/TaskScheduler
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_STD_FUNCTION   // Compile with support for std::function.
#define _TASK_SCHEDULING_OPTIONS
#include <TaskSchedulerDeclarations.h>

#define TASK_RECYCLE embui.taskRecycle(ts.getCurrentTask())