
void tasks_backgroundProcessesTask( void *pvParameters )
{
  for ( ;; )
  {
    // Does some background tasks - reading touches, redrawing screen, 
    // checking for idle.
    // xSemaphoreTake(xMutex, portMAX_DELAY);
    impl_runBackgroundProcesses();
    // xSemaphoreGive(xMutex);   
    taskYIELD();

  }
  vTaskDelete( NULL );
}


void tasks_startBackgroundProcessesTask(unsigned int priority)
{
  // xSemaphoreTake(xMutex, portMAX_DELAY);

    // Start the task to run the background processes
  BaseType_t returned_backgroundProcessesTask;
  returned_backgroundProcessesTask = xTaskCreatePinnedToCore(
      tasks_backgroundProcessesTask,            /* Function to implement the task */
      "tasks_backgroundProcessesTask",      /* Name of the task */
      8000,                 /* Stack size in words */
      NULL,                 /* Task input parameter */
      priority,                    /* Priority of the task */
      &backgroundProcessesTaskHandle,     /* Task handle. */
      1);

  if (returned_backgroundProcessesTask == pdPASS) {
    Serial.println("Created tasks_backgroundProcessesTask.");
  }
  else {
    Serial.println("Didn't create tasks_backgroundProcessesTask!");
  }
  // xSemaphoreGive(xMutex);  

}


void tasks_startTasks() 
{
  tasks_startBackgroundProcessesTask(3);
}