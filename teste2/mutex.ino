// Manipulador da página raiz
void handleRoot() {
  // Proteja o acesso à variável global usando o mutex
  if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
    server.send(200, "text/plain", "Ola, ESP32!");

    // Libere o mutex após o acesso
    xSemaphoreGive(xMutex);
  }
}
