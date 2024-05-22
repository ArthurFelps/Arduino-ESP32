// Função para enviar mensagens para a fila
void sendMessage(const char *msg) {
  Message message;
  message.id = 1;
  strncpy(message.message, msg, sizeof(message.message) - 1);
  message.message[sizeof(message.message) - 1] = '\0';

  if (xQueueSend(xQueue, &message, portMAX_DELAY) != pdPASS) {
    Serial.println("Erro ao enviar mensagem para a fila");
  }
}

// Função para processar mensagens da fila (pode ser chamada em uma tarefa separada, se necessário)
void processMessages() {
  Message receivedMessage;
  while (true) {
    if (xQueueReceive(xQueue, &receivedMessage, portMAX_DELAY) == pdPASS) {
      Serial.print("Mensagem recebida: ");
      Serial.println(receivedMessage.message);
    }
  }
}