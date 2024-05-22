void setup() {
  // Inicie a comunicação serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // Inicialize o pino do LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Cria o mutex
  xMutex = xSemaphoreCreateMutex();

  // Cria a fila
  xQueue = xQueueCreate(10, sizeof(Message));

  // Verifica se o mutex e a fila foram criados com sucesso
  if (xMutex == NULL || xQueue == NULL) {
    Serial.println("Erro ao criar mutex ou fila");
    while (true);
  }

  // Conecte-se ao Wi-Fi
  if (!WiFi.softAP(WIFI_SSID, WIFI_PASSWORD)) {
    log_e("Soft AP creation failed.");
    while(1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

    /*while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }*/
    if (!MDNS.begin("esp32")) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    Serial.println("started");

    // Start TCP (HTTP) server
    server.begin();
    Serial.println("TCP server started");

    // Add service to MDNS-SD 
    MDNS.addService("http", "tcp", 80);
    }
  // Crie as tarefas
  xTaskCreate(
    serverTask,           // Função da tarefa
    "Server Task",        // Nome da tarefa
    4096,                 // Tamanho da pilha
    NULL,                 // Parâmetro da tarefa
    1,                    // Prioridade da tarefa
    NULL                  // Handle da tarefa
  );

  xTaskCreate(
    blinkTask,            // Função da tarefa
    "Blink Task",         // Nome da tarefa
    1024,                 // Tamanho da pilha
    NULL,                 // Parâmetro da tarefa
    1,                    // Prioridade da tarefa
    NULL                  // Handle da tarefa
  );
}