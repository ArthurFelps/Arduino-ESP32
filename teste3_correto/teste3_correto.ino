#include <WiFi.h>
#include <WebServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <ESPmDNS.h>

// Defina o nome e a senha da rede Wi-Fi
#define WIFI_SSID "ESP"
#define WIFI_PASSWORD "12345678"

// Defina o pino do LED (normalmente o LED embutido está no GPIO 2)
#define LED_BUILTIN 2

// Variáveis globais
WebServer server(80);
SemaphoreHandle_t xMutex;
QueueHandle_t xQueue;

// Estrutura para mensagens da fila
typedef struct {
  int id;
  char message[20];
} Message;

// Prototipos das funções
void serverTask(void * pvParameters);
void blinkTask(void * pvParameters);
void processMessagesTask(void * pvParameters);
void handleRoot();
void sendMessage(const char *msg);

void setup() {
  // Inicie a comunicação serial
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // Inicialize o pino do LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Crie o mutex
  xMutex = xSemaphoreCreateMutex();

  // Crie a fila
  xQueue = xQueueCreate(10, sizeof(Message));

  // Verifique se o mutex e a fila foram criados com sucesso
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

  // Configure o servidor web
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Servidor HTTP iniciado");

  // Inicialize o MDNS
  if (!MDNS.begin("esp32")) {
    Serial.println("Error setting up MDNS responder!");
    while (true) {
      delay(1000);
    }
  }
  Serial.println("MDNS responder started");
  MDNS.addService("http", "tcp", 80);

  // Crie as tarefas
  xTaskCreate(
  serverTask, 
  "Server Task",
   4096, 
   NULL,
    1, 
    NULL);
  xTaskCreate(
  blinkTask,
  "Blink Task",
  1024,
  NULL,
  0,
  NULL);
  xTaskCreate(
  processMessagesTask,
  "Process Messages Task",
  2048,
  NULL,
  1,
  NULL);
}

void loop() {
  // Não faz nada no loop
}

// Tarefa para o servidor web
void serverTask(void * pvParameters) {
  for(;;){
    server.handleClient();
    delay(1); // Evite o watchdog reset
  }
}

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

// Tarefa para processar mensagens da fila
void processMessagesTask(void * pvParameters) {
  Message receivedMessage;
  while (true) {
    if (xQueueReceive(xQueue, &receivedMessage, portMAX_DELAY) == pdPASS) {
      Serial.print("Mensagem recebida: ");
      Serial.println(receivedMessage.message);
    }
  }
}

// Manipulador da página raiz
void handleRoot() {
  // Proteja o acesso à variável global usando o mutex
  if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
    server.send(200, "text/plain", "Ola, ESP32!");

    // Libere o mutex após o acesso
    xSemaphoreGive(xMutex);
  }
}

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
