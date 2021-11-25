#ifndef _EMBUI_CUSTOMTASK_H
#define _EMBUI_CUSTOMTASK_H

#ifdef ESP32

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include <HardwareSerial.h>
#include <string>

class CustomTask {
public:
  enum core_t : uint32_t { CORE_0 = 0, CORE_1, CORE_ANY = tskNO_AFFINITY };

  CustomTask(const char *name = "CustomTask", uint32_t stack = 2048, uint8_t priority = 1, core_t core = CORE_ANY);
  virtual ~CustomTask() {
    destroy();
  }

  static void halt(const char *msg, CustomTask *customTask=nullptr);
  operator bool() {
    return _task != NULL;
  }

  bool isRunning();
  void pause();
  void resume();
  void destroy();
  void notify(uint32_t value);

  static void lock();
  static void unlock();

protected:
  enum flag_t : uint8_t { FLAG_DESTROYING = 1, FLAG_DESTROYED = 2, FLAG_USER = 4 };

  virtual void setup() {};
  virtual void loop() = 0;
  virtual void cleanup() {};

  static void taskHandler(void* pTaskInstance);
  static void delay(int ms);
  void stop();
	void setStackSize(uint16_t stackSize);
	void setPriority(uint8_t priority);
	void setName(const char *name);
	void setCore(BaseType_t coreId);
  void start(void* taskData = nullptr);

  std::string _taskName;
  void*       _taskData;
	uint16_t    _stackSize;
	uint8_t     _priority;
	BaseType_t  _coreId;
  TaskHandle_t _task;
  static portMUX_TYPE _mutex;
  EventGroupHandle_t _flags;
};

#endif

#endif