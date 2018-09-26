
void tasks_backgroundProcessesTask( void *pvParameters )
{
  for ( ;; )
  {
    // Does some background tasks - reading touches, redrawing screen, 
    // checking for idle.
    impl_runBackgroundProcesses();
  }
  vTaskDelete( NULL );
}

void tasks_commsReaderTask( void *pvParameters )
{
  for ( ;; )
  {
    // commsRunner sets up a regular invocation of comms_checkForCommand(), which
    // checks for characters on the serial port and puts them into a buffer.
    // When the buffer is terminated, nextCommand is moved into currentCommand.
    comms_checkForCommand();
  }
  vTaskDelete( NULL );
}

void tasks_runMotorsMinimal() {
  aStepped = motorA.run();
  bStepped = motorB.run();
}


void tasks_runMotors() {
  if (backgroundRunning) {
    tasks_runMotorsMinimal();
  }

  if (millis() > (lastPeriodStartTime + 1000)) {
    lastPeriodStartTime = millis();
    (sampleBufferSlot == 2) ? sampleBufferSlot = 0 : sampleBufferSlot++;
    sampleBuffer[sampleBufferSlot] = runCounter;
    runCounter = 0L;
    steppedBuffer[sampleBufferSlot] = steppedCounter;
    steppedCounter = 0L;
    totalSamplePeriods++;
  }
  runCounter++;
  totalTriggers++;
  if (aStepped || bStepped) {
    steppedCounter++;
  }
}

void tasks_runMotorsTask( void *pvParameters )
{
  // BaseType_t xEvent;
  // const TickType_t xBlockTime = 500;
  // uint32_t ulNotifiedValue;
  for ( ;; )
  {
    /* Block to wait for a notification.  Here the RTOS task notification
    is being used as a counting semaphore.  The task's notification value
    is incremented each time the ISR calls vTaskNotifyGiveFromISR(), and
    decremented each time the RTOS task calls ulTaskNotifyTake() - so in
    effect holds a count of the number of outstanding interrupts.  The first
    parameter is set to pdFALSE, so the notification value is only decremented
    and not cleared to zero, and one deferred interrupt event is processed
    at a time.  */
    // ulNotifiedValue = ulTaskNotifyTake( pdTRUE, xBlockTime );

    // while (ulNotifiedValue > 0)
    // {
      tasks_runMotors();
    //   ulNotifiedValue--;
    // }

  }
  vTaskDelete( NULL );
}


void tasks_startBackgroundProcessesTask()
{
    // Start the task to run the background processes
  BaseType_t returned_backgroundProcessesTask;
  returned_backgroundProcessesTask = xTaskCreatePinnedToCore(
      tasks_backgroundProcessesTask,            /* Function to implement the task */
      "tasks_backgroundProcessesTask",      /* Name of the task */
      8000,                 /* Stack size in words */
      NULL,                 /* Task input parameter */
      2,                    /* Priority of the task */
      &backgroundProcessesTaskHandle,     /* Task handle. */
      1);

  if (returned_backgroundProcessesTask == pdPASS) {
    Serial.println("Created tasks_backgroundProcessesTask.");
  }
  else {
    Serial.println("Didn't create tasks_backgroundProcessesTask!");
  }
}

void tasks_startCommsReaderTask()
{

  BaseType_t returned_commsReaderTask;
  returned_commsReaderTask = xTaskCreatePinnedToCore(
      tasks_commsReaderTask,            /* Function to implement the task */
      "tasks_commsReaderTask",      /* Name of the task */
      4000,                 /* Stack size in words */
      NULL,                 /* Task input parameter */
      1,                    /* Priority of the task */
      &commsReaderTaskHandle,     /* Task handle. */
      1);

  if (returned_commsReaderTask == pdPASS) {
    Serial.println("Created tasks_commsReaderTask.");
  }
  else {
    Serial.println("Didn't create tasks_commsReaderTask!");
  }
}

void tasks_startMotorsTask()
{

  BaseType_t returned_startMotorsTask;
  returned_startMotorsTask = xTaskCreatePinnedToCore(
      tasks_runMotorsTask,            /* Function to implement the task */
      "tasks_runMotorsTask",      /* Name of the task */
      4000,                 /* Stack size in words */
      NULL,                 /* Task input parameter */
      4,                    /* Priority of the task */
      &motorsTaskHandle,     /* Task handle. */
      1);

  if (returned_startMotorsTask == pdPASS) {
    Serial.println("Created tasks_runMotorsTask.");
  }
  else {
    Serial.println("Didn't create tasks_runMotorsTask!");
  }
}

void tasks_startTasks() 
{
  tasks_startBackgroundProcessesTask();
  tasks_startCommsReaderTask();
  tasks_startMotorsTask();
}