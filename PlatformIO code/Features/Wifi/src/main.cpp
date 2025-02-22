#include <main.h>

// DHTTYPE: Specifies the DHT sensor model (DHT11 in this case)
DHT dht(DHTPIN, DHTTYPE);

// Add after WiFi credentials
const char* hostname = "ESP32-Weather-Station";

// Web Server and WebSocket Setup
// AsyncWebServer handles HTTP requests asynchronously
// WebSocket enables real-time bidirectional communication
AsyncWebServer server(80);  // Server runs on port 80 (HTTP default)
AsyncWebSocket ws("/ws");   // WebSocket endpoint at /ws

// FreeRTOS Task Handle
// Used to manage the sensor reading task
TaskHandle_t sensorTaskHandle = NULL;

// WebSocket Message Handler
// Processes incoming WebSocket messages from clients
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        // Handle incoming WebSocket messages if needed
    }
}

// WebSocket Event Handler
// Manages different WebSocket events (connect, disconnect, data, etc.)
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

// Sensor Reading Task (runs on Core 1)
// Continuously reads temperature and humidity data and broadcasts to connected clients
void sensorTask(void *parameter) {
    // Create a JSON document to store sensor readings
    StaticJsonDocument<200> doc;
    
    while (1) {
        // Read sensor values
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();
        
        // Only send valid readings
        if (!isnan(temperature) && !isnan(humidity)) {
            // Pack readings into JSON format
            doc["temperature"] = temperature;
            doc["humidity"] = humidity;
            doc["hostname"] = hostname;
            doc["ip"] = WiFi.localIP().toString();
            
            // Convert to string and broadcast to all clients
            String jsonString;
            serializeJson(doc, jsonString);
            ws.textAll(jsonString);
        }
        
        // Wait for 2 seconds before next reading
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// Setup Function
// Initializes all components and starts the system
void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    
    // Start the DHT sensor
    dht.begin();
    
    // Set hostname before WiFi connection
    WiFi.setHostname(hostname);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println(WiFi.localIP());  // Print the IP address
    
    // WebSocket Setup
    ws.onEvent(onEvent);           // Register event handler
    server.addHandler(&ws);        // Add WebSocket handler to server
    
    // Web Server Route Configuration
    // Serve the main webpage
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });
    
    // Start the web server
    server.begin();
    
    // Create Sensor Reading Task on Core 1
    // Stack size: 4096 bytes
    // Priority: 1 (low)
    xTaskCreatePinnedToCore(
        sensorTask,
        "SensorTask",
        4096,
        NULL,
        1,
        &sensorTaskHandle,
        1  // Run on Core 1
    );
}

// Main Loop
// Handles WebSocket client cleanup
void loop() {
    // Remove disconnected clients
    ws.cleanupClients();
    
    // Small delay to prevent watchdog timer issues
    vTaskDelay(pdMS_TO_TICKS(1000));
}