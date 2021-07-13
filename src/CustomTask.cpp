#ifdef ESP32

#include <esp_log.h>
#include "CustomTask.h"
#include <string>

static const char TAG[] = "CustomTask";

//CustomTask *customTask = NULL;

void CustomTask::halt(const char *msg, CustomTask *customTask) {
  if (customTask)
    delete customTask;
  ESP_LOGE(TAG, "%s\r\n", msg);
  Serial.println(msg);
  Serial.flush();
  esp_deep_sleep_start();
}

CustomTask::CustomTask(const char *name, uint32_t stack, uint8_t priority, core_t core) :
_taskName(name),_taskData(nullptr),_stackSize(stack),_priority(priority),_coreId(core),_task(nullptr)
{
  BaseType_t result;

  _flags = xEventGroupCreate();
  if (! _flags) {
    _task = NULL;
    ESP_LOGE(TAG, "Error creating RTOS event group!\r\n");
    return;
  }
  if (core < CORE_ANY)
    result = xTaskCreatePinnedToCore((TaskFunction_t)&CustomTask::taskHandler, name, stack, this, priority, &_task, core);
  else
    result = xTaskCreate((TaskFunction_t)&CustomTask::taskHandler, name, stack, this, priority, &_task);
  if (result != pdPASS) {
    _task = NULL;
    vEventGroupDelete(_flags);
    ESP_LOGE(TAG, "Error creating RTOS CustomTask!\r\n");
  }
}

bool CustomTask::isRunning() {
  if (_task) {
    return eTaskGetState(_task) == eRunning;
  }
  return false;
}

void CustomTask::pause() {
  if (_task) {
    vTaskSuspend(_task);
  }
}

void CustomTask::resume() {
  if (_task) {
    vTaskResume(_task);
  }
}

void CustomTask::destroy() {
  if (_task) {
    xEventGroupSetBits(_flags, FLAG_DESTROYING);
    xEventGroupWaitBits(_flags, FLAG_DESTROYED, pdFALSE, pdTRUE, portMAX_DELAY);
    vEventGroupDelete(_flags);
    _task = NULL;
  }
}

void CustomTask::notify(uint32_t value) {
  if (_task) {
    xTaskNotify(_task, value, eSetValueWithOverwrite);
  }
}

void CustomTask::lock() {
  portENTER_CRITICAL(&_mutex);
}

void CustomTask::unlock() {
  portEXIT_CRITICAL(&_mutex);
}

void CustomTask::taskHandler(void* pTaskInstance) {
	CustomTask* pTask = (CustomTask*) pTaskInstance;

	ESP_LOGD(TAG, ">> taskHandler: taskName=%s\r\n", pTask->_taskName.c_str());
  pTask->setup();
  while (! (xEventGroupWaitBits(pTask->_flags, FLAG_DESTROYING, pdFALSE, pdTRUE, 0) & FLAG_DESTROYING)) {
    pTask->loop();
  }
  pTask->cleanup();
  xEventGroupSetBits(pTask->_flags, FLAG_DESTROYED);
  vTaskDelete(pTask->_task);
  ESP_LOGD(TAG, "<< taskHandler: taskName=%s\r\n", pTask->_taskName.c_str());
}

/**
 * @brief Suspend the task for the specified milliseconds.
 * @param [in] ms The delay time in milliseconds.
 */
/* static */ void CustomTask::delay(int ms) {
	::vTaskDelay(ms / portTICK_PERIOD_MS);
} // delay

/**
 * @brief Stop the task.
 */
void CustomTask::stop() {
	if (_task == nullptr) return;
	xTaskHandle temp = _task;
	_task = nullptr;
	::vTaskDelete(temp);
} // stop

/**
 * @brief Set the stack size of the task.
 *
 * @param [in] stackSize The size of the stack for the task.
 */
void CustomTask::setStackSize(uint16_t stackSize) {
	_stackSize = stackSize;
} // setStackSize

/**
 * @brief Set the priority of the task.
 *
 * @param [in] priority The priority for the task.
 */
void CustomTask::setPriority(uint8_t priority) {
	_priority = priority;
} // setPriority

/**
 * @brief Set the name of the task.
 *
 * @param [in] name The name for the task.
 */
void CustomTask::setName(const char *name) {
	_taskName = name;
} // setName

/**
 * @brief Set the core number the task has to be executed on.
 * If the core number is not set, tskNO_AFFINITY will be used
 *
 * @param [in] coreId The id of the core.
 */
void CustomTask::setCore(BaseType_t coreId) {
	_coreId = coreId;
}

/**
 * @brief Start an instance of the task.
 *
 * @param [in] taskData Data to be passed into the task.
 */
void CustomTask::start(void* taskData) {
	if (_task != nullptr) {
		ESP_LOGW(TAG, "Task::start - There might be a task already running!\r\n");
	}
	_taskData = taskData;
	::xTaskCreatePinnedToCore((TaskFunction_t)&CustomTask::taskHandler, _taskName.c_str(), _stackSize, this, _priority, &_task, _coreId);
} // start

portMUX_TYPE CustomTask::_mutex = portMUX_INITIALIZER_UNLOCKED;

#endif
