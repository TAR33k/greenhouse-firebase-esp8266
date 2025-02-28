# 🌱 Greenhouse Monitor

An automated irrigation system for greenhouses that combines hardware and software components to create an efficient IoT solution.

## 📌 Overview
The Greenhouse Monitor project utilizes the ESP8266 microcontroller to manage sensors and a water pump, while a web application serves as the user interface for monitoring and control. This system allows users to efficiently manage greenhouse conditions, ensuring optimal growth for plants.

## 🌐 Live Demo
The web application is hosted at: [Greenhouse Monitor](https://esp8266-firebase-demo-6e68a.web.app/)

## ✨ Features
### Control Interface
- **Web Application**: A user-friendly interface for managing the greenhouse system. Users can monitor real-time sensor values, view historical data through interactive graphs, and control the water pump.
- **Automatic and Manual Modes**: Switch between automatic irrigation based on soil moisture levels and manual control via the web interface.

### Sensor Monitoring
- **Temperature and Humidity**: Monitors environmental conditions using a DHT11 sensor.
- **Soil Moisture**: Measures soil moisture levels with an analog sensor.
- **Water Level**: Uses an HC-SR04 ultrasonic sensor to measure water levels in the reservoir.

### Visual Indicators
- **LED Indicators**: Four LEDs provide visual feedback on soil moisture levels, indicating critical, low, medium, and optimal states.

### Data Management
- **Firebase Integration**: Utilizes Firebase Realtime Database for two-way communication between the ESP8266 and the web application, allowing for real-time updates and historical data storage.

## 🛠️ Tech Stack
- **ESP8266 NodeMCU**: Microcontroller for managing sensors and actuators.
- **Firebase**: Realtime Database for data storage and communication.
- **HTML/CSS/JavaScript**: Technologies used for developing the web application.
- **Chart.js**: Library for rendering interactive graphs.

## 🚀 Getting Started
### Prerequisites
Ensure you have the following installed:
- **Arduino IDE**: For programming the ESP8266.
- **Firebase Account**: To set up the Realtime Database.

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/TAR33k/greenhouse-firebase-esp8266.git
   ```

2. Open the Arduino IDE and upload the ESP8266 code to your microcontroller.

3. Set up the Firebase project and configure the database URL and API key in the code.

4. Open the `index.html` file in a web browser to access the web application.

## 📝 Documentation
Comprehensive documentation is available in the `/docs` directory, including:
- **Project Description**
- **Hardware and Software Components**
- **System Functionalities**
- **FSM Diagram**

## 🔜 Future Enhancements
- Integration of weather API for external climate data.
- Advanced analytics for greenhouse performance.

## 🏫 About
This project was developed as part of the **IoT** coursework at the **Faculty of Information Technologies**.

📌 Built by:
- **Tarik Kukuljac** (IB220202)
