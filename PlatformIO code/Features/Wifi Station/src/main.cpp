/**
 * ESP32-S3 WiFi Access Point with FreeRTOS
 * 
 * This implementation creates a WiFi Access Point with a TCP server
 * and provides visual feedback through an LED for client connections.
 * The system uses FreeRTOS tasks distributed across both cores for
 * efficient operation.
 */

#include "main.h"

// Network configuration
IPAddress local_IP(192, 168, 4, 100);  // Static IP for the AP
IPAddress gateway(192, 168, 4, 100);    // Gateway (same as local_IP for AP mode)
IPAddress subnet(255, 255, 255, 0);     // Subnet mask
WiFiServer server;                       // TCP server instance
                 // GPIO pin for RED LED

// FreeRTOS task handles for task management
TaskHandle_t wifiAPTaskHandle = NULL;    // Handle for WiFi AP configuration task
TaskHandle_t serverTaskHandle = NULL;    // Handle for TCP server task
TaskHandle_t ledTaskHandle = NULL;       // Handle for LED control task

// Global state variables
static bool clientConnected = false;     // Flag to track client connection status

/**
 * LED Control Task (Runs on Core 1)
 * Handles the LED blinking based on client connection status
 * @param parameter - Task parameters (unused)
 */
void ledTask(void *parameter) {
    bool ledState = false;
    while(1) {
        if (clientConnected) {
            ledState = !ledState;  // Toggle LED state
            digitalWrite(LED_RED, ledState);
            vTaskDelay(pdMS_TO_TICKS(500));
        } else {
            digitalWrite(LED_RED, LOW);  // LED off when no client
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

/**
 * WiFi Access Point Configuration Task (Runs on Core 0)
 * Sets up the WiFi AP and initializes network configuration
 * Self-terminates after successful setup
 * @param parameter - Task parameters (unused)
 */
void wifiAPTask(void *parameter) {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    while (!WiFi.softAP(ssid, password)) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    Serial.println("AP Success!");
    Serial.println("====================");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
    WiFi.softAPsetHostname("myHostName");
    Serial.print("HostName: ");
    Serial.println(WiFi.softAPgetHostname());
    Serial.print("MAC Address: ");
    Serial.println(WiFi.softAPmacAddress());
    Serial.println("====================");
    
    server.begin(8888);
    Serial.println("Server started on port 8888");

    // Signal server task to start
    xTaskNotify(serverTaskHandle, 1, eSetBits);
    
    // Task is done, delete itself
    vTaskDelete(NULL);
}

/**
 * TCP Server Task (Runs on Core 0)
 * Handles client connections and data communication
 * Requires notification from WiFi AP task before starting
 * @param parameter - Task parameters (unused)
 */
void serverTask(void *parameter) {
    // Wait for WiFi AP to be ready
    uint32_t notification;
    if (xTaskNotifyWait(0, 0, &notification, portMAX_DELAY) != pdTRUE) {
        vTaskDelete(NULL);
    }

    while(1) {
        WiFiClient client = server.available();
        if (client) {
            Serial.println("[Client connected]");
            clientConnected = true;  // Set flag when client connects
            
            while (client.connected()) {
                if (client.available()) {
                    String readBuff = client.readString();
                    client.print("Received: " + readBuff);
                    Serial.println("Received: " + readBuff);
                }
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            clientConnected = false;  // Clear flag when client disconnects
            Serial.println("[Client disconnected]");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * System initialization
 * Sets up hardware and creates FreeRTOS tasks
 * Task Priority: WiFi AP (2) > Server (1) = LED (1)
 */
void setup() {
    // Initialize hardware
    pinMode(LED_RED, OUTPUT);
    digitalWrite(LED_RED, LOW);
    Serial.begin(115200);
    Serial.println();
    delay(1000);  // Allow system to stabilize

    // Create tasks with core assignments:
    // LED Task -> Core 1 (UI/Indication)
    // Server and WiFi Tasks -> Core 0 (Network Operations)
    xTaskCreatePinnedToCore(ledTask, "LED Task", 2048, NULL, 1, &ledTaskHandle, 1);
    xTaskCreatePinnedToCore(serverTask, "Server Task", 4096, NULL, 1, &serverTaskHandle, 0);
    xTaskCreatePinnedToCore(wifiAPTask, "WiFi AP Task", 4096, NULL, 2, &wifiAPTaskHandle, 0);
}

/**
 * Main loop (unused in this implementation)
 * All functionality is handled by FreeRTOS tasks
 */
void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));  // Prevent watchdog trigger
}
