// Tarefa para piscar o LED
void blinkTask(void * pvParameters) {
  for(;;){
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Envie uma mensagem para a fila
    sendMessage("LED blinked");
  }
}