
/*
  ==========================================================
  HOLOSHUTTER V1.1 - PHOSPHOR EDITION
  Arduino Nano RP2040 Connect
  SG90 Servo - D9
  ==========================================================

  WiFi AP:
    SSID: HoloShutter-RP2040
    PASS: HoloShutter530
    URL:  http://192.168.4.1

  Servo:
    CLOSED = 0 degrees
    OPEN   = 68 degrees

  Exposure:
    Opening settle = 700 ms
    Maximum        = 210 seconds

  API:
    GET /
    GET /api/status
    GET /api/open
    GET /api/close
    GET /api/start?ms=120000
    GET /api/cancel

  Libraries:
    WiFiNINA
    Servo
  ==========================================================
*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <Servo.h>
#include <string.h>

// ==========================================================
// CONFIGURATION
// ==========================================================

const char WIFI_SSID[] = "HoloShutter-RP2040";
const char WIFI_PASSWORD[] = "HoloShutter530";

const uint8_t SERVO_PIN = 9;

const uint8_t OPEN_ANGLE = 68;
const uint8_t CLOSED_ANGLE = 0;

const unsigned long OPEN_SETTLE_MS = 700UL;
const unsigned long MAX_EXPOSURE_MS = 210000UL;

WiFiServer server(80);
Servo shutterServo;

// ==========================================================
// SYSTEM STATE
// ==========================================================

bool shutterIsOpen = false;
bool exposurePending = false;
bool exposureActive = false;

unsigned long exposureDuration = 0;
unsigned long exposurePendingTime = 0;
unsigned long exposureStartTime = 0;

// ==========================================================
// PHOSPHOR USER INTERFACE
// ==========================================================

const char INDEX_HTML[] = R"HOLO(
<!DOCTYPE html>
<html lang="en">
<head>

<meta charset="UTF-8">
<meta name="viewport"
content="width=device-width, initial-scale=1, maximum-scale=1">

<meta name="theme-color" content="#080e0b">

<title>HOLOSHUTTER</title>

<style>

:root {
  --bg: #080e0b;
  --panel: #101d15;
  --border: #28583b;
  --green: #83ffad;
  --green2: #35d875;
  --dim: #639b76;
  --red: #ff6666;
}

* {
  box-sizing: border-box;
}

body {
  margin: 0;
  padding: 20px 12px 35px;
  background: var(--bg);
  color: var(--green);
  font-family: Arial, Helvetica, sans-serif;
}

.container {
  width: 100%;
  max-width: 480px;
  margin: auto;
}

header {
  text-align: center;
  padding: 12px 0 24px;
}

.logo {
  font-size: 29px;
  font-weight: 900;
  letter-spacing: 2px;
  color: var(--green);
  text-shadow: 0 0 14px rgba(83,255,140,.35);
}

.subtitle {
  margin-top: 8px;
  color: var(--dim);
  font-size: 12px;
  letter-spacing: 2px;
}

.panel {
  background: var(--panel);
  border: 1px solid var(--border);
  border-radius: 14px;
  padding: 17px;
  margin-bottom: 14px;
  box-shadow: 0 0 16px rgba(30,255,100,.035);
}

.section-title {
  color: #a4dfb5;
  font-size: 13px;
  font-weight: 800;
  letter-spacing: 1.5px;
  margin-bottom: 15px;
}

.status-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 8px;
}

.status-pill {
  padding: 8px 11px;
  border: 1px solid #367b4e;
  border-radius: 7px;
  background: #12241a;
  color: var(--green);
  font-size: 11px;
  font-weight: bold;
  white-space: nowrap;
}

.status-pill.offline {
  border-color: #853d3d;
  color: var(--red);
}

.status-pill.active {
  border-color: var(--green2);
  box-shadow: 0 0 10px rgba(53,216,117,.2);
}

.countdown {
  text-align: center;
  font-family: "Courier New", monospace;
  font-size: 46px;
  font-weight: bold;
  letter-spacing: 2px;
  padding: 14px 0 7px;
  color: var(--green);
  text-shadow: 0 0 15px rgba(83,255,140,.4);
  font-variant-numeric: tabular-nums;
}

.detail {
  text-align: center;
  color: var(--dim);
  font-size: 12px;
  min-height: 17px;
}

button {
  display: block;
  width: 100%;
  border: none;
  border-radius: 9px;
  padding: 15px 10px;
  margin: 9px 0;
  font-size: 14px;
  font-weight: 900;
  letter-spacing: .5px;
  cursor: pointer;
  -webkit-appearance: none;
}

button:active {
  transform: scale(.985);
}

button:disabled {
  opacity: .4;
  cursor: not-allowed;
}

.open-button {
  background: #14874c;
  color: #e1ffeb;
  box-shadow: 0 0 12px rgba(30,220,100,.15);
}

.close-button {
  background: #253d2e;
  border: 1px solid #438054;
  color: #a0ffba;
}

.execute-button {
  background: #2cda73;
  color: #07170c;
  padding: 18px 10px;
  margin-top: 17px;
  box-shadow: 0 0 15px rgba(44,218,115,.2);
}

.cancel-button {
  background: #421f22;
  border: 1px solid #873d43;
  color: #ff9999;
}

.time-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 12px;
}

label {
  display: block;
  color: var(--dim);
  font-size: 11px;
  font-weight: bold;
  letter-spacing: 1px;
  margin-bottom: 8px;
}

input {
  width: 100%;
  background: #080f0a;
  border: 1px solid #326543;
  border-radius: 8px;
  padding: 14px 6px;
  color: var(--green);
  font-family: "Courier New", monospace;
  font-size: 25px;
  font-weight: bold;
  text-align: center;
  outline: none;
  -webkit-appearance: none;
}

input:focus {
  border-color: var(--green2);
  box-shadow: 0 0 12px rgba(53,216,117,.2);
}

.message {
  color: var(--green);
  text-align: center;
  font-size: 11px;
  min-height: 17px;
  margin-top: 10px;
}

.helper {
  color: var(--dim);
  text-align: center;
  font-size: 10px;
  line-height: 1.6;
  margin-top: 12px;
}

.footer {
  color: #416c4d;
  text-align: center;
  font-size: 10px;
  letter-spacing: 1.5px;
  margin-top: 23px;
}

[hidden] {
  display: none !important;
}

</style>
</head>

<body>

<div class="container">

<header>
  <div class="logo">HOLOSHUTTER</div>
  <div class="subtitle">NANO RP2040 CONNECT</div>
</header>

<!-- STATUS -->

<section class="panel">

  <div class="status-row">

    <div class="section-title">
      SHUTTER STATUS
    </div>

    <div id="connection"
         class="status-pill offline">
      CONNECTING...
    </div>

  </div>

  <div class="countdown" id="countdown">
    00:00
  </div>

  <div class="detail" id="detail">
    Waiting for controller
  </div>

</section>

<!-- MANUAL CONTROL -->

<section class="panel">

  <div class="section-title">
    MANUAL CONTROL
  </div>

  <button class="open-button"
          id="openButton"
          type="button"
          onclick="manualOpen()">
    OPEN SHUTTER
  </button>

  <button class="close-button"
          id="closeButton"
          type="button"
          onclick="manualClose()">
    CLOSE SHUTTER
  </button>

</section>

<!-- CUSTOM EXPOSURE -->

<section class="panel">

  <div class="section-title">
    CUSTOM EXPOSURE
  </div>

  <div class="time-grid">

    <div>
      <label for="minutes">MINUTES</label>

      <input
        type="number"
        id="minutes"
        min="0"
        max="3"
        step="1"
        value="2"
        inputmode="numeric">
    </div>

    <div>
      <label for="seconds">SECONDS</label>

      <input
        type="number"
        id="seconds"
        min="0"
        max="59"
        step="1"
        value="0"
        inputmode="numeric">
    </div>

  </div>

  <button class="execute-button"
          id="executeButton"
          type="button"
          onclick="executeExposure()">
    EXECUTE EXPOSURE
  </button>

  <button class="cancel-button"
          id="cancelButton"
          type="button"
          onclick="cancelExposure()"
          hidden>
    CANCEL EXPOSURE
  </button>

  <div class="helper">
    SHUTTER OPENS → SETTLES → EXPOSURE → AUTOMATIC CLOSE
    <br>
    MAXIMUM EXPOSURE: 03:30
  </div>

  <div class="message" id="message"></div>

</section>

<div class="footer">
  HOLOSHUTTER SYSTEMS // PHOSPHOR EDITION
</div>

</div>

<script>

"use strict";

const el = id => document.getElementById(id);

let connected = false;
let running = false;
let pollInProgress = false;
let commandInProgress = false;

let lastStatus = null;
let lastRemaining = 0;
let lastStatusTime = 0;

function formatTime(ms) {

  const total = Math.max(0, Math.ceil(ms / 1000));

  const minutes = Math.floor(total / 60);
  const seconds = total % 60;

  return String(minutes).padStart(2, "0") +
         ":" +
         String(seconds).padStart(2, "0");
}

function message(text) {
  el("message").textContent = text;
}

function updateDisplay(data) {

  connected = true;

  running = data.running;
  lastStatus = data.status;

  const pill = el("connection");

  pill.textContent = "CONNECTED";
  pill.className = "status-pill" +
    (running ? " active" : "");

  el("countdown").textContent =
    formatTime(data.remaining);

  el("detail").textContent = data.detail;

  el("openButton").disabled =
    commandInProgress || running;

  el("closeButton").disabled =
    commandInProgress;

  el("executeButton").disabled =
    commandInProgress || running;

  el("cancelButton").hidden = !running;

  if (running) {
    el("executeButton").textContent = "EXPOSURE RUNNING";
  } else {
    el("executeButton").textContent = "EXECUTE EXPOSURE";
  }

  lastRemaining = data.remaining;
  lastStatusTime = Date.now();
}

function showOffline() {

  connected = false;

  el("connection").textContent = "OFFLINE";
  el("connection").className = "status-pill offline";

  el("detail").textContent =
    "Controller not responding";

  el("openButton").disabled = true;
  el("closeButton").disabled = true;
  el("executeButton").disabled = true;
  el("cancelButton").hidden = true;
}

async function request(url) {

  const response = await fetch(url, {
    method: "GET",
    cache: "no-store"
  });

  if (!response.ok) {
    throw new Error("HTTP " + response.status);
  }

  return response;
}

async function pollStatus() {

  if (pollInProgress) return;

  pollInProgress = true;

  try {

    const response = await request(
      "/api/status?t=" + Date.now()
    );

    const data = await response.json();

    if (typeof data.running !== "boolean" ||
        typeof data.remaining !== "number") {
      throw new Error("Invalid status response");
    }

    updateDisplay(data);

  } catch (error) {

    showOffline();

  } finally {

    pollInProgress = false;

  }
}

async function sendCommand(url, successMessage) {

  if (commandInProgress) return;

  commandInProgress = true;

  el("openButton").disabled = true;
  el("closeButton").disabled = true;
  el("executeButton").disabled = true;

  try {

    await request(url);

    message(successMessage);

    await pollStatus();

  } catch (error) {

    message("COMMAND FAILED - CHECK CONNECTION");
    showOffline();

  } finally {

    commandInProgress = false;

    if (connected) {
      el("openButton").disabled = running;
      el("closeButton").disabled = false;
      el("executeButton").disabled = running;
    }
  }
}

function manualOpen() {

  sendCommand(
    "/api/open",
    "Shutter opened."
  );
}

function manualClose() {

  sendCommand(
    "/api/close",
    "Shutter closed."
  );
}

function cancelExposure() {

  sendCommand(
    "/api/cancel",
    "Exposure cancelled."
  );
}

async function executeExposure() {

  const minutes = Number(el("minutes").value);
  const seconds = Number(el("seconds").value);

  if (!Number.isInteger(minutes) ||
      !Number.isInteger(seconds) ||
      minutes < 0 ||
      minutes > 3 ||
      seconds < 0 ||
      seconds > 59) {

    message("PLEASE ENTER A VALID TIME");
    return;
  }

  const totalSeconds = minutes * 60 + seconds;

  const duration = totalSeconds * 1000;

  if (duration < 1000) {

    message("MINIMUM EXPOSURE IS 1 SECOND");
    return;
  }

  if (duration > 210000) {

    message("MAXIMUM EXPOSURE IS 3 MIN 30 SEC");
    return;
  }

  await sendCommand(
    "/api/start?ms=" + duration,
    "Exposure command accepted."
  );
}

// Update countdown locally between controller polls.
// Controller remains authoritative.

function animateCountdown() {

  if (running && connected) {

    const elapsed = Date.now() - lastStatusTime;

    const displayRemaining =
      Math.max(0, lastRemaining - elapsed);

    el("countdown").textContent =
      formatTime(displayRemaining);
  }

}

el("minutes").addEventListener("input", () => {
  message("");
});

el("seconds").addEventListener("input", () => {
  message("");
});

setInterval(pollStatus, 500);
setInterval(animateCountdown, 100);

pollStatus();

</script>

</body>
</html>
)HOLO";

// ==========================================================
// HTTP RESPONSE HELPERS
// ==========================================================

void sendHeaders(
  WiFiClient &client,
  int code,
  const char *status,
  const char *contentType,
  size_t contentLength
) {

  client.print("HTTP/1.1 ");
  client.print(code);
  client.print(" ");
  client.println(status);

  client.print("Content-Type: ");
  client.println(contentType);

  client.print("Content-Length: ");
  client.println(contentLength);

  client.println("Cache-Control: no-store, no-cache, must-revalidate");
  client.println("Pragma: no-cache");
  client.println("Connection: close");
  client.println();

}

// Send the page in small chunks.
// Check every write so partial writes do not silently truncate HTML.

bool writePage(WiFiClient &client) {

  const size_t pageLength = sizeof(INDEX_HTML) - 1;
  const size_t chunkSize = 128;

  size_t sent = 0;

  while (sent < pageLength && client.connected()) {

    size_t amount = pageLength - sent;

    if (amount > chunkSize) {
      amount = chunkSize;
    }

    size_t written = client.write(
      (const uint8_t *)(INDEX_HTML + sent),
      amount
    );

    if (written == 0) {
      delay(2);
      continue;
    }

    sent += written;
  }

  return sent == pageLength;
}

void sendMainPage(WiFiClient &client) {

  sendHeaders(
    client,
    200,
    "OK",
    "text/html; charset=utf-8",
    sizeof(INDEX_HTML) - 1
  );

  writePage(client);
}

void sendJson(
  WiFiClient &client,
  const char *json
) {

  size_t length = strlen(json);

  sendHeaders(
    client,
    200,
    "OK",
    "application/json; charset=utf-8",
    length
  );

  client.write((const uint8_t *)json, length);
}

void sendError(
  WiFiClient &client,
  int code,
  const char *status,
  const char *messageText
) {

  size_t length = strlen(messageText);

  sendHeaders(
    client,
    code,
    status,
    "text/plain; charset=utf-8",
    length
  );

  client.write(
    (const uint8_t *)messageText,
    length
  );
}

void sendNoContent(WiFiClient &client) {

  client.println("HTTP/1.1 204 No Content");
  client.println("Cache-Control: no-store");
  client.println("Connection: close");
  client.println();
}

// ==========================================================
// SERVO CONTROL
// ==========================================================

void cancelTimer() {

  exposurePending = false;
  exposureActive = false;

  exposureDuration = 0;

}

void openShutter() {

  cancelTimer();

  shutterServo.write(OPEN_ANGLE);

  shutterIsOpen = true;

  Serial.println("SHUTTER OPEN");

}

void closeShutter() {

  cancelTimer();

  shutterServo.write(CLOSED_ANGLE);

  shutterIsOpen = false;

  Serial.println("SHUTTER CLOSED");

}

// ==========================================================
// EXPOSURE CONTROL
// ==========================================================

bool startExposure(unsigned long duration) {

  if (duration < 1000UL ||
      duration > MAX_EXPOSURE_MS) {

    return false;
  }

  cancelTimer();

  exposureDuration = duration;

  shutterServo.write(OPEN_ANGLE);

  shutterIsOpen = true;

  exposurePending = true;
  exposurePendingTime = millis();

  Serial.print("EXPOSURE REQUEST: ");
  Serial.print(duration);
  Serial.println(" ms");

  return true;
}

void updateExposure() {

  unsigned long now = millis();

  if (exposurePending &&
      now - exposurePendingTime >= OPEN_SETTLE_MS) {

    exposurePending = false;
    exposureActive = true;

    exposureStartTime = now;

    Serial.println("EXPOSURE TIMER STARTED");
  }

  if (exposureActive &&
      now - exposureStartTime >= exposureDuration) {

    Serial.println("EXPOSURE COMPLETE");

    closeShutter();
  }
}

// ==========================================================
// STATUS JSON
// ==========================================================

void buildStatus(char *buffer, size_t bufferSize) {

  const char *status;
  const char *detail;

  unsigned long remaining = 0;

  if (exposurePending) {

    status = "opening";
    detail = "Shutter settling";

    remaining = exposureDuration;
  }

  else if (exposureActive) {

    status = "exposing";
    detail = "Exposure in progress";

    unsigned long elapsed =
      millis() - exposureStartTime;

    if (elapsed < exposureDuration) {
      remaining = exposureDuration - elapsed;
    }
  }

  else if (shutterIsOpen) {

    status = "open";
    detail = "Shutter open";
  }

  else {

    status = "closed";
    detail = "Shutter closed";
  }

  snprintf(
    buffer,
    bufferSize,
    "{\"connected\":true,\"status\":\"%s\","
    "\"running\":%s,\"remaining\":%lu,"
    "\"shutterOpen\":%s,\"detail\":\"%s\"}",
    status,
    (exposurePending || exposureActive) ? "true" : "false",
    remaining,
    shutterIsOpen ? "true" : "false",
    detail
  );
}

// ==========================================================
// HTTP REQUEST HANDLER
// ==========================================================

void handleClient() {

  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  client.setTimeout(1000);

  // Read first HTTP request line.

  String requestLine = client.readStringUntil('\n');
  requestLine.trim();

  if (requestLine.length() == 0) {
    client.stop();
    return;
  }

  // Read and discard request headers.

  unsigned long headerStart = millis();

  while (client.connected() &&
         millis() - headerStart < 1000UL) {

    String line = client.readStringUntil('\n');

    if (line == "\r" || line.length() == 0) {
      break;
    }
  }

  Serial.print("HTTP: ");
  Serial.println(requestLine);

  // Extract URL.

  int firstSpace = requestLine.indexOf(' ');
  int secondSpace = requestLine.indexOf(' ', firstSpace + 1);

  if (firstSpace < 0 || secondSpace < 0) {

    sendError(client, 400, "Bad Request", "Invalid request");
    delay(5);
    client.stop();
    return;
  }

  String url = requestLine.substring(
    firstSpace + 1,
    secondSpace
  );

  String path = url;

  int queryPosition = path.indexOf('?');

  if (queryPosition >= 0) {
    path.remove(queryPosition);
  }

  // ========================================================
  // ROUTES
  // ========================================================

  if (path == "/") {

    sendMainPage(client);
  }

  else if (path == "/api/status") {

    char json[256];

    buildStatus(json, sizeof(json));

    sendJson(client, json);
  }

  else if (path == "/api/open") {

    openShutter();

    sendNoContent(client);
  }

  else if (path == "/api/close" ||
           path == "/api/cancel") {

    closeShutter();

    sendNoContent(client);
  }

  else if (path == "/api/start") {

    int parameterPosition = url.indexOf("ms=");

    if (parameterPosition < 0) {

      sendError(
        client,
        400,
        "Bad Request",
        "Missing exposure duration"
      );
    }

    else {

      parameterPosition += 3;

      String value = url.substring(parameterPosition);

      int ampersand = value.indexOf('&');

      if (ampersand >= 0) {
        value.remove(ampersand);
      }

      bool valid = value.length() > 0;

      for (unsigned int i = 0; i < value.length(); i++) {

        if (!isDigit(value[i])) {
          valid = false;
          break;
        }
      }

      if (!valid) {

        sendError(
          client,
          400,
          "Bad Request",
          "Invalid exposure duration"
        );
      }

      else {

        unsigned long duration = strtoul(
          value.c_str(),
          NULL,
          10
        );

        if (startExposure(duration)) {
          sendNoContent(client);
        }

        else {

          sendError(
            client,
            400,
            "Bad Request",
            "Exposure must be 1000 to 210000 ms"
          );
        }
      }
    }
  }

  else {

    sendError(
      client,
      404,
      "Not Found",
      "Unknown endpoint"
    );
  }

  delay(5);
  client.stop();
}

// ==========================================================
// WIFI STARTUP
// ==========================================================

void startAccessPoint() {

  Serial.println("Starting WiFi access point...");

  int status = WiFi.beginAP(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  if (status != WL_AP_LISTENING &&
      status != WL_AP_CONNECTED) {

    Serial.println("WiFi AP FAILED");

    while (true) {
      delay(1000);
    }
  }

  delay(1500);

  Serial.print("IP ADDRESS: ");
  Serial.println(WiFi.localIP());

  server.begin();

  Serial.println("HTTP SERVER READY");
}

// ==========================================================
// SETUP
// ==========================================================

void setup() {

  Serial.begin(115200);

  delay(1200);

  Serial.println();
  Serial.println("================================");
  Serial.println(" HOLOSHUTTER PHOSPHOR V1.1");
  Serial.println("================================");

  // Initialize shutter in closed position.

  shutterServo.attach(SERVO_PIN);

  shutterServo.write(CLOSED_ANGLE);

  shutterIsOpen = false;

  Serial.println("SERVO INITIALIZED");

  startAccessPoint();

  Serial.println();
  Serial.println("CONNECT TO:");
  Serial.println(WIFI_SSID);

  Serial.println("OPEN:");
  Serial.println("http://192.168.4.1");

}

// ==========================================================
// MAIN LOOP
// ==========================================================

void loop() {

  // Always service exposure timer.

  updateExposure();

  // Process incoming browser request.

  handleClient();

  // Keep timer responsive.

  updateExposure();

  delay(2);
}
