<div align="center">

  <h1 style="font-size: 3em; margin-bottom: 0;">
    🌱
    <br>
    <font color="#4CAF50" style="font-weight: bold;">Greenhouse Monitor</font>
  </h1>

  <h3 style="font-weight: normal; margin-top: 0;">An IoT-Powered Automated Irrigation System</h3>

  A complete IoT solution combining an ESP8266 microcontroller, sensors, and a web-based dashboard to automate and monitor greenhouse conditions.

  <p>
    <img alt="Hardware" src="https://img.shields.io/badge/Hardware-ESP8266-E73525.svg?style=for-the-badge&logo=espressif"/>
    <img alt="Backend" src="https://img.shields.io/badge/Backend-Firebase-FFCA28.svg?style=for-the-badge&logo=firebase"/>
    <img alt="Frontend" src="https://img.shields.io/badge/Frontend-JavaScript-F7DF1E.svg?style=for-the-badge&logo=javascript"/>
  </p>

   <a href="https://esp8266-firebase-demo-6e68a.web.app/" target="_blank">
      <img alt="Live Demo" src="https://img.shields.io/badge/Live_Demo-View_Dashboard-brightgreen.svg?style=for-the-badge"/>
   </a>
</div>

---

## About Greenhouse Monitor

**Greenhouse Monitor** is an end-to-end IoT project designed to automate plant care within a greenhouse. The system leverages an **ESP8266** microcontroller to collect real-time data from environmental sensors and control a water pump. All data is synchronized with a **Firebase Realtime Database**, which powers a web-based dashboard, allowing for remote monitoring and control from any device.

### Key Features

| Category              | Core Functionality                                                                                             |
| --------------------- | -------------------------------------------------------------------------------------------------------------- |
| **Remote Control**    | A responsive web dashboard to monitor live sensor data and manually toggle the water pump on or off.         |
| **Automation**        | An "Automatic Mode" that triggers the irrigation system based on predefined soil moisture thresholds.          |
| **Data Visualization**| Interactive charts powered by **Chart.js** display historical sensor data, helping track environmental trends. |
| **Real-Time Sync**    | **Firebase** ensures seamless, two-way communication between the hardware and the web dashboard.                |
| **Hardware Feedback** | Onboard LEDs provide an immediate visual status of soil moisture levels (Critical, Low, Medium, Optimal).      |
| **Sensor Suite**      | Monitors key metrics: soil moisture, water reservoir level, ambient temperature, and humidity.                 |

---

## How It Works

The system operates on a simple yet effective data flow, enabling real-time interaction between the physical hardware and the cloud-based user interface.

1.  **Data Collection:** The ESP8266 continuously reads data from the DHT11 (temperature/humidity), soil moisture, and ultrasonic (water level) sensors.
2.  **Cloud Sync:** This data is pushed to the Firebase Realtime Database in a structured JSON format.
3.  **Dashboard Display:** The web application listens for changes in the Firebase database and instantly updates the dashboard with the latest sensor readings and historical graphs.
4.  **User Control:** When a user toggles the pump or changes the mode on the web app, the command is written to Firebase.
5.  **Hardware Action:** The ESP8266 listens for specific data changes in Firebase and activates or deactivates the water pump relay accordingly.

---

## Tech Stack

The project integrates hardware, cloud services, and frontend technologies to create a cohesive system.

| Component         | Technology / Tool                                     | Purpose                                                              |
| ----------------- | ----------------------------------------------------- | -------------------------------------------------------------------- |
| **Microcontroller** | **ESP8266 NodeMCU**                                   | The brain of the system; connects to WiFi, reads sensors, controls pump. |
| **Sensors**       | **DHT11**, **Analog Soil Moisture**, **HC-SR04**        | Collects environmental and system data.                              |
| **Backend & DB**  | **Firebase Realtime Database**                        | Provides a cloud-hosted NoSQL database for real-time data sync.      |
| **Frontend**      | **HTML**, **CSS**, **JavaScript**             | Builds the structure, style, and logic of the user dashboard.        |
| **Charting**      | **Chart.js**                                          | Renders interactive and responsive charts for historical data.       |
| **IDE**           | **Arduino IDE**                                       | Used to program and upload the firmware to the ESP8266.              |

---

## Getting Started

Follow these steps to set up the project on your own hardware.

### Prerequisites

*   An **ESP8266 NodeMCU** board and the required sensors/actuators.
*   A **Firebase Account** with a new Realtime Database project created.
*   **[Arduino IDE](https://www.arduino.cc/en/software)** installed and configured for the ESP8266 board.

### Setup Instructions

**1. Clone the Repository**

```bash
git clone https://github.com/TAR33k/greenhouse-firebase-esp8266.git
```

**2. Configure Firebase**

1.  In your Firebase project, go to **Realtime Database** and create a database.
2.  Under **Project Settings**, find your **Web API Key** and **Database URL**.
3.  In the Arduino code (`.ino` file), replace the placeholder values for `API_KEY` and `DATABASE_URL` with your credentials. Also, enter your WiFi SSID and password.
4.  In the Web code (`index.html` file), replace the placeholder values for `firebaseConfig`.

**3. Hardware & Firmware**

1.  Assemble the hardware.
2.  Open the `.ino` file in the Arduino IDE, install any required libraries (e.g., Firebase Arduino, DHT sensor library).
3.  Upload the code to your ESP8266 board.

**4. Launch the Web App**

Simply open the `index.html` file in your web browser. The dashboard will automatically connect to your Firebase instance and display data once the ESP8266 is online.

---

## Project Information

-   **Documentation:** Comprehensive documentation, including a detailed **Project Description**, **FSM Diagram**, and component lists, is available in the `/Docs` directory.

-   **Future Enhancements:**
    -   Integration of a weather API to make smarter irrigation decisions.
    -   Advanced analytics and pattern recognition for plant health.

### About This Project

This project was developed as part of the **Internet of Things (IoT)** coursework at the **Faculty of Information Technologies, "Džemal Bijedić" University of Mostar**.
