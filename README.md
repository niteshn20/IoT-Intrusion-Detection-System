# IoT-Intrusion-Detection-System
Cybersecurity &amp; Machine Learning

Project Overview
  This project presents a lightweight, real-time Intrusion Detection System (IDS) designed for IoT networks using embedded hardware. The system leverages an ESP32 microcontroller for network traffic monitoring and on-device machine learning-based anomaly detection. An ESP8266 module is used to simulate IoT node traffic.
  The objective is to implement a cost-effective, edge-based security solution capable of detecting abnormal traffic patterns such as flooding attacks and unusual bitrate spikes without relying on cloud infrastructure.

Problem Statement
  IoT networks are increasingly vulnerable to cyber threats including denial-of-service attacks, packet flooding, and unauthorized access. Traditional intrusion detection systems are often cloud-dependent, computationally intensive, and unsuitable for deployment on low-power embedded devices.
  There is a need for a lightweight, real-time IDS capable of operating directly on microcontrollers with minimal latency and infrastructure requirements.

Proposed Solution
  This project implements an edge-based IDS using:
  ESP32 for packet monitoring and ML inference
  ESP8266 as IoT traffic generator
  Hybrid machine learning model for classification
  Telegram API for real-time alert notification
  The system monitors live network traffic, extracts relevant features, classifies traffic as normal or malicious, and sends alerts when abnormal behavior is detected.

System Architecture
1. Network Configuration
  ESP32 connects to a WiFi network or operates as a hotspot.
  ESP8266 connects as an IoT node generating normal or simulated attack traffic.

2. Traffic Monitoring
  ESP32 operates in promiscuous mode.
  Captures packet count and bitrate metrics.
  Extracts network-level statistical features.

3. Machine Learning Inference
  A hybrid ML model (e.g., logistic regression combined with threshold validation) is trained offline.
  Model parameters are embedded into ESP32 firmware.
  Real-time classification into:
  Normal Traffic
  Attack Traffic

4. Alert Mechanism
  When malicious traffic is detected:
  ESP32 triggers a Telegram notification.
  User receives immediate remote alert.
  Machine Learning Model
  The model is trained using traffic-related features such as:
  Packet rate
  Bitrate
  Traffic burst frequency
  Statistical deviation from baseline behavior
  Training is performed in Python using Scikit-learn. The trained model coefficients are converted into a lightweight format compatible with embedded C/C++ implementation on ESP32.
  The model performs real-time inference directly on the microcontroller without cloud dependency.
