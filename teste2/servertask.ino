// Tarefa para o servidor web
void serverTask(void * pvParameters) {
  for(;;){
    server.handleClient();
    delay(1); // Evite o watchdog reset
  }
}
