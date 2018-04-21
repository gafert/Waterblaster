#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

static const char ap_ssid[] = "Waterblaster";
static const char ap_password[] = "waterfun";

static const char sta_ssid[] = "some_SSID";
static const char sta_password[] = "some_PASS";

static void action(char*);

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <meta name="viewport" content="width = device-width, user-scalable=0">
    <title>Waterblaster</title>
    <style>
        body, html {
            background-color: #36464e;
            width: 100%;
            height: 100%;
            padding: 0;
            margin: 0;
            Color: #ffffff;
            font-family: "Arial", Helvetica, Sans-Serif;
        }

        h1 {
            font-family: "Impact", Helvetica, Sans-Serif;
            font-size: 3em;
            margin: 0;
        }

        .desc {
            margin: 0 0 1.5em;
            font-size: 1em;
            font-family: "Arial", Helvetica, Sans-Serif;
            float: left;
        }

        button {
            width: 7em;
            height: 7em;
            opacity: 0.3;
            fill: #ffffff;
            background-color: transparent;
            border: none;
            text-decoration: none;
            outline: none;
        }

        .centered-wrapper {
            position: relative;
            text-align: center;
        }

        .centered-wrapper:before {
            content: "";
            position: relative;
            display: inline-block;
            width: 0;
            height: 100%;
            vertical-align: middle;
        }

        .centered-content {
            display: inline-block;
            vertical-align: middle;
        }

        .colored-button {
            opacity: 1;
        }

        #s.colored-button {
            fill: #ff6669;
        }
    </style>
    <script>

        function Command(id, element, buttonString) {
            this.id = id;
            this.element = element;
            this.button_var = false;
            this.buttonString = buttonString;
            this.active = false;
            this.timer = 0;
        }

        var websock;
        var buttons;

        function start() {
            buttons = [
                new Command('s', document.getElementById('s'), ' '),
                new Command('u', document.getElementById('u'), 'w'),
                new Command('d', document.getElementById('d'), 's'),
                new Command('l', document.getElementById('l'), 'a'),
                new Command('r', document.getElementById('r'), 'd')
            ];

            buttons.forEach(function (t) { setEvent(t.element) });

            try {
                websock = new WebSocket('ws://' + window.location.hostname + ':81/');
                websock.onopen = function (evt) {
                    console.log('websock open');
                };
                websock.onclose = function (evt) {
                    console.log('websock close');
                };
                websock.onerror = function (evt) {
                    console.log(evt);
                };
                websock.onmessage = function (evt) {
                    console.log(evt);
                    console.log(evt.data[0]);
                    console.log(evt.data[1]);
                    var id = evt.data[0];
                    var state = parseInt(evt.data[1]);
                    colorButton(id, state);
                }
            } catch (err) {
                console.log("Server not running");
            }
        }

        document.addEventListener('keydown', function (event) {
            buttons.forEach(function (t) {
                if (event.key === t.buttonString && !t.button_var) {
                    t.button_var = true;
                    handleDown(t.element);
                }
            });
        });

        document.addEventListener('keyup', function (event) {
            buttons.forEach(function (t) {
                if (event.key === t.buttonString) {
                    t.button_var = false;
                    handleUp(t.element);
                }
            });
        });

        function colorButton(id, state) {
            if (state) {
                document.getElementById(id).classList.add('colored-button');
                console.log("Added color");
            }
            else {
                document.getElementById(id).classList.remove('colored-button');
                console.log("Removed color");
            }
        }

        function setEvent(e) {
            e.addEventListener('mousedown', function () {
                handleDown(e);
            });
            e.addEventListener('mouseup', function () {
                handleUp(e);
            });
            e.addEventListener('touchstart', function () {
                handleDown(e);
            });
            e.addEventListener('touchend', function () {
                handleUp(e);
            });
        }

        // mouseup need to be monitored on a "global" element or we might miss it if
        // we move outside the original element.
        // window.addEventListener("mouseup", handleMouseUp);

        function handleDown(e) {
            console.log("Clicked");
            buttons.forEach(function (t) {
                if (t.element === e) {
                    t.active = false;
                    clearTimeout(t.timer);
                    t.timer = setTimeout(longPress.bind(this, t.element), 200);
                }
            });
        }

        function handleUp(e) {
            buttons.forEach(function (t) {
                if (t.element === e) {
                    if (t.active) {
                        try {
                            websock.send(t.id + 0);
                            t.active = false;
                        } catch (error) {
                            console.log(error);
                        }
                    }
                    clearTimeout(t.timer);
                }
            });
        }

        function longPress(e) {
            console.log("Long click");
            buttons.forEach(function (t) {
                if (t.element === e) {
                    t.active = true;
                    try {
                        websock.send(t.id + 1);
                    } catch (error) {
                        console.log(error);
                    }
                }
            });
        }
    </script>
</head>
<body class="centered-wrapper" onload="start();">
<div class="centered-content">
    <table>
        <tr>
            <td>
                <h1>Waterblaster</h1>
            </td>
        </tr>
        <tr>
            <td>
                <p class="desc">Control is Power.</p>
            </td>
        </tr>
    </table>
    <table class="centered-content">
        <tr>
            <td></td>
            <td>
                <button id="u" type="button">
                    <svg viewbox="0 0 100 100">
                        <!-- The arrow shape is simple enough that the path is hand coded -->
                        <path class="arrow" d="M 50,0 L 0,50 L 10,60 L 50,20 L 90,60 L 100,50"/>
                    </svg>
                </button>
            </td>
            <td></td>
        </tr>
        <tr>
            <td>
                <button id="l" type="button">
                    <svg viewbox="0 0 100 100">
                        <!-- The arrow shape is simple enough that the path is hand coded -->
                        <path d="M 50,0 L 60,10 L 20,50 L 60,90 L 50,100 L 0,50"/>
                    </svg>
                </button>
            </td>
            <td>
                <button id="s" type="button">
                    <svg viewbox="0 0 100 100">
                        <!-- The arrow shape is simple enough that the path is hand coded -->
                        <path d="M 50,0 L 0,50 L 50,100 L 100,50"/>
                    </svg>
                </button>
            </td>
            <td>
                <button id="r" type="button">
                    <svg viewbox="0 0 100 100">
                        <!-- The arrow shape is simple enough that the path is hand coded -->
                        <path d="M 100,50 L 50,0 L 40,10 L 80,50 L 40,90 L 50,100"/>
                    </svg>
                </button>
            </td>
        </tr>
        <tr>
            <td></td>
            <td>
                <button id="d" type="button">
                    <svg viewbox="0 0 100 100">
                        <!-- The arrow shape is simple enough that the path is hand coded -->
                        <path d="M 50,100 L 0,50 L 10,40 L 50,80 L 90,40 L 100,50"/>
                    </svg>
                </button>
            </td>
            <td></td>
        </tr>
    </table>
</div>
</body>
</html>
)rawliteral";

struct Scommands {
    char name;
    uint8_t status;
};

struct Scommands commands[5] = {0};

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SHOOT 4

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    Serial.printf("webSocketEvent(%d, %d)\r\n", num, type);
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\r\n", num);
            break;
        case WStype_CONNECTED:
        {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            // Send the current status
            for (auto &command : commands) {
                char com[2] = {command.name, command.status};
                webSocket.sendTXT(num, com, strlen(com));
            }
        }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\r\n", num, payload);
            action((char *)payload);
            // send data to all connected clients
            webSocket.broadcastTXT(payload, length);
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary length: %u\r\n", num, length);
            hexdump(payload, length);

            // echo data back to browser
            webSocket.sendBIN(num, payload, length);
            break;
        default:
            Serial.printf("Invalid WStype [%d]\r\n", type);
            break;
    }
}

void handleRoot() {
    server.send_P(200, "text/html", INDEX_HTML);
}

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

static void action(char* command) {
    Serial.println(command);
    char id = command[0];
    auto state = (uint8_t)command[1];
    for (auto &command : commands) {
        if(command.name == id) {
            command.status = state;
            char com[2] = {command.name, command.status};

            Wire.beginTransmission(8); // Send to device 8
            Wire.write(com, 2);        // Writes 2 Bytes
            Wire.endTransmission();    // Send stop


            Serial.println("Sent command over i2c");
        }
    }
}

void setup() {
    Wire.begin(0,2);                // join i2c bus with address #8

    commands[UP].name = 'u';
    commands[DOWN].name = 'd';
    commands[LEFT].name = 'l';
    commands[RIGHT].name = 'r';
    commands[SHOOT].name = 's';

    Serial.begin(115200);

    if(0){
        WiFi.mode(WIFI_STA);
        WiFi.begin(sta_ssid, sta_password); // Join network
        Serial.println();
        Serial.print("Connecting");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println();
        Serial.print("WiFi connected to ");
        Serial.println(sta_ssid);
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    } else {

        WiFi.mode(WIFI_AP);   // Set AP Mode
        WiFi.softAP(ap_ssid, ap_password);

        Serial.println();
        Serial.println();
        Serial.print("Wifi SSID: ");
        Serial.println(ap_ssid);
        Serial.print("Accesspoint address: ");
        Serial.println(WiFi.softAPIP());
    }

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);

    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
    server.handleClient();
}