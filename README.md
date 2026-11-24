# ESP32 IoT Sensor Logger and Forecasting System (PBL BVI 3114)

## System Overview
This project collects temperature and humidity data from a DHT11 sensor using an ESP32 microcontroller[cite: 4, 8]. The ESP32 sends JSON data via HTTP POST to a Google Apps Script web app, logs records into the `SensorData` sheet, generates forecasts in the `Forecasts` sheet, and visualizes results in Looker Studio.

## Forecasting Algorithm Selection
- **Selected Method**: Moving Average (SMA / EMA) time-series forecasting.
- **Why This Algorithm Was Chosen**:
  - Simple to compute and easy to trace in spreadsheet and script workflows.
  - Smooths short-term sensor noise and minor fluctuations from ambient readings.
  - Well-suited for lightweight IoT time-series data without heavy model overhead.
- **Forecast Outputs**: Generates forecasted values along with upper and lower bound trends for dashboard monitoring[cite: 8].

## Repository Contents
- `lab3.ino` — ESP32 firmware with sensor validation and HTTP POST transmission[cite: 4].
- `Code.gs` — Google Apps Script web app handler for logging and forecasting.
- `README.md` — System summary and forecasting algorithm documentation[cite: 9].