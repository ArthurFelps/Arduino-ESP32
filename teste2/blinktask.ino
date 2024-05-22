// Tarefa para piscar o LED
void blinkTask(void * pvParameters) {
  for(;;){
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(500);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(500);

    // Envie uma mensagem para a fila
    sendMessage("LED blinked");
  }
}