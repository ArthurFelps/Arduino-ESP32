#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

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
void handleRoot();
void sendMessage(const char *msg);