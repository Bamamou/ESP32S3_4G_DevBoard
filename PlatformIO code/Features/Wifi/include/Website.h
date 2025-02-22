#ifndef __WEBSITE_H__
#define __WEBSITE_H__

#include <Arduino.h>
// HTML content with modern design
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html data-theme="light">
<head>
    <title>ESP32 Sensor Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css">
    <style>
        :root[data-theme="light"] {
            --bg-color: #f0f2f5;
            --container-bg: white;
            --text-color: #333;
            --shadow-color: rgba(0,0,0,0.1);
        }
        
        :root[data-theme="dark"] {
            --bg-color: #1a1a1a;
            --container-bg: #2d2d2d;
            --text-color: #ffffff;
            --shadow-color: rgba(0,0,0,0.3);
        }

        body {
            font-family: Arial, sans-serif;
            background-color: var(--bg-color);
            margin: 0;
            padding: 20px;
            transition: background-color 0.3s ease;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background: var(--container-bg);
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px var(--shadow-color);
        }
        .card {
            background: linear-gradient(135deg, #6e8efb, #4a6cf7);
            color: white;
            padding: 20px;
            border-radius: 8px;
            margin: 10px 0;
            text-align: center;
        }
        .temperature-card {
            background: linear-gradient(135deg, var(--temp-color-1, #6e8efb), var(--temp-color-2, #4a6cf7));
            color: white;
            padding: 20px;
            border-radius: 8px;
            margin: 10px 0;
            text-align: center;
            transition: background 0.5s ease;
        }
        .value {
            font-size: 2em;
            font-weight: bold;
            margin: 10px 0;
        }
        h1 {
            color: var(--text-color);
            text-align: center;
        }
        .time-card {
            background: linear-gradient(135deg, #43cea2, #185a9d);
            color: white;
            padding: 20px;
            border-radius: 8px;
            margin: 10px 0;
            text-align: center;
        }
        .datetime {
            font-size: 1.5em;
            margin: 5px 0;
        }
        .weather-card {
            background: linear-gradient(135deg, #FF8C00, #FFA500);
            color: white;
            padding: 20px;
            border-radius: 8px;
            margin: 10px 0;
            text-align: center;
        }
        .error {
            color: #ff0000;
            font-size: 0.8em;
            margin-top: 5px;
        }

        /* Theme Switch styles */
        .theme-switch {
            position: fixed;
            top: 20px;
            right: 20px;
            display: flex;
            align-items: center;
            background: var(--container-bg);
            padding: 10px;
            border-radius: 20px;
            box-shadow: 0 2px 5px var(--shadow-color);
            z-index: 1000;
        }
        .theme-switch label {
            margin-right: 10px;
            color: var(--text-color);
            font-size: 1.2em;
        }
        .theme-switch .fa-sun {
            color: #ffd700;
        }
        .theme-switch .fa-moon {
            color: #5c6bc0;
        }
        .switch {
            position: relative;
            display: inline-block;
            width: 60px;
            height: 34px;
        }
        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }
        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            transition: .4s;
            border-radius: 34px;
        }
        .slider:before {
            position: absolute;
            content: "";
            height: 26px;
            width: 26px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
            border-radius: 50%;
        }
        input:checked + .slider {
            background-color: #2196F3;
        }
        input:checked + .slider:before {
            transform: translateX(26px);
        }
        .device-info {
            background-color: #f0f0f0;
            padding: 10px;
            border-radius: 5px;
            margin: 10px 0;
        }
    </style>
</head>
<body>
    <div class="theme-switch">
        <label>
            <i class="fas fa-sun"></i>
            /
            <i class="fas fa-moon"></i>
        </label>
        <label class="switch">
            <input type="checkbox" id="themeSwitch">
            <span class="slider"></span>
        </label>
    </div>
    <div class="container">
        <h1>ESP32 Sensor Dashboard</h1>
        <div class="device-info">
            <h3>Device Information</h3>
            <p>Hostname: <span id="hostname">-</span></p>
            <p>IP Address: <span id="ipaddress">-</span></p>
        </div>
        <div class="time-card">
            <h2>Date & Time</h2>
            <div class="datetime" id="datetime">--:--:--</div>
        </div>
        <div class="temperature-card" id="temp-card">
            <h2>Temperature</h2>
            <div class="value" id="temperature">--°C</div>
        </div>
        <div class="card">
            <h2>Humidity</h2>
            <div class="value" id="humidity">--%</div>
        </div>
        <div class="weather-card">
            <h2>Weather Condition</h2>
            <div class="value" id="weather">--</div>
        </div>
    </div>
    <script>
        // Add these functions at the beginning of your script section
        function initTheme() {
            const darkMode = localStorage.getItem('darkMode') === 'true';
            document.documentElement.setAttribute('data-theme', darkMode ? 'dark' : 'light');
            document.getElementById('themeSwitch').checked = darkMode;
        }

        function toggleTheme() {
            const darkMode = document.documentElement.getAttribute('data-theme') === 'light';
            document.documentElement.setAttribute('data-theme', darkMode ? 'dark' : 'light');
            localStorage.setItem('darkMode', darkMode);
        }

        var gateway = `ws://${window.location.hostname}/ws`;
        var websocket;
        
        function initWebSocket() {
            websocket = new WebSocket(gateway);
            websocket.onopen = onOpen;
            websocket.onclose = onClose;
            websocket.onmessage = onMessage;
        }
        
        function onOpen(event) {
            console.log('Connection opened');
        }
        
        function onClose(event) {
            console.log('Connection closed');
            setTimeout(initWebSocket, 2000);
        }
        
        function getWeatherDescription(temperature) {
            if (temperature <= 0) return "Freezing";
            if (temperature <= 10) return "Very Cold";
            if (temperature <= 20) return "Cool";
            if (temperature <= 25) return "Pleasant";
            if (temperature <= 30) return "Warm";
            if (temperature <= 35) return "Hot";
            return "Very Hot";
        }

        function getTemperatureColors(temperature) {
            if (temperature <= 0) return ['#00ffff', '#0000ff'];     // Ice blue
            if (temperature <= 10) return ['#4169E1', '#0000CD'];    // Royal blue
            if (temperature <= 20) return ['#32CD32', '#228B22'];    // Green
            if (temperature <= 25) return ['#6e8efb', '#4a6cf7'];    // Default blue
            if (temperature <= 30) return ['#FFA500', '#FF8C00'];    // Orange
            if (temperature <= 35) return ['#FF4500', '#FF0000'];    // Red-Orange
            return ['#FF0000', '#8B0000'];                          // Deep Red
        }

        function onMessage(event) {
            try {
                var data = JSON.parse(event.data);
                document.getElementById('temperature').innerHTML = data.temperature.toFixed(1) + '&deg;C';
                document.getElementById('humidity').innerHTML = data.humidity.toFixed(1) + '%';
                document.getElementById('weather').innerHTML = getWeatherDescription(data.temperature);
                document.getElementById('hostname').innerHTML = data.hostname;
                document.getElementById('ipaddress').innerHTML = data.ip;
                
                // Update temperature card colors
                const [color1, color2] = getTemperatureColors(data.temperature);
                const tempCard = document.getElementById('temp-card');
                tempCard.style.setProperty('--temp-color-1', color1);
                tempCard.style.setProperty('--temp-color-2', color2);

                // Clear any previous error messages
                Array.from(document.getElementsByClassName('error')).forEach(el => el.remove());
            } catch (e) {
                console.error('Error parsing sensor data:', e);
                
                // Add error message to container
                const errorDiv = document.createElement('div');
                errorDiv.className = 'error';
                errorDiv.innerHTML = 'Error reading sensor data. Please check connection.';
                document.querySelector('.container').appendChild(errorDiv);
            }
        }
        
        window.addEventListener('load', onLoad);
        
        function updateDateTime() {
            const now = new Date();
            const dateTimeString = now.toLocaleString();
            document.getElementById('datetime').innerHTML = dateTimeString;
        }

        function onLoad(event) {
            initWebSocket();
            updateDateTime();
            setInterval(updateDateTime, 1000);
            initTheme();
            document.getElementById('themeSwitch').addEventListener('change', toggleTheme);
        }
    </script>
</body>
</html>


    )rawliteral";

    
#endif