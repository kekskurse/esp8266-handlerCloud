#include "HandlerCloud.h"

ESP8266WebServer server(80);
WiFiClient wifiClient;
PubSubClient client(wifiClient);

String _topics = "";
char* _hostname = "";

bool _internetConnection = false;

HandlerCloud::HandlerCloud() {
  //_debug = false;
}

void HandlerCloud::setupHostname(char* hostname) {
  debugMessage("Set Hostname to: ");
  debugMessage(hostname);
  _hostname = hostname;
}

void HandlerCloud::setupDebug(int baudRate) {
  Serial.begin(baudRate);
  _debug = true;
  debugMessage("Enabled Debug");
}

//Wifi Stuff
void HandlerCloud::setupWifi() {
  String s = getConfig("wifi_ssid");
  char ssid[s.length()*2];
  s.toCharArray(ssid, s.length()*2);
  s = getConfig("wifi_pw");
  char password[s.length()*2];
  s.toCharArray(password, s.length()*2);

  WiFi.hostname(_hostname);
  debugMessage("###Connect To WiFi###");
  if (String(ssid) != "")
  {
    debugMessage("Connect to " + String(ssid));

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
      i++;
      delay(500);
      debugMessage(".", false);
      if (i > 25) {
        break;
      }
      if (i > 25) {
        break;
      }
    }

    debugMessage("");
    if(WiFi.status() == WL_CONNECTED)
    {
      _internetConnection = true;
      debugMessage("Connectet to WiFi and get IP: " + WiFi.localIP().toString());
    }
    else
    {
      debugMessage("Connection failed, open AP");
       openWiFiAccessPoint();
    }

  } else {
    openWiFiAccessPoint();
  }
}

void HandlerCloud::openWiFiAccessPoint() {
  debugMessage("Open WiFi Access Point");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("HandlerCloudConfig", "setupsetup");
    debugMessage("SSID: HandlerCloudConfig");
    debugMessage("PASS: setupsetup");
    debugMessage("IP  : " + WiFi.softAPIP().toString());
}

void HandlerCloud::checkWifi() {
  if (WiFi.status() != WL_CONNECTED && _internetConnection == true)
  {
    debugMessage("Reconnect to WiFi");
    setupWifi();
  }
}

// Webpage
void HandlerCloud::setupWebserver() {
  String setup_ssid = getConfig("wifi_ssid");
  String setup_wifipw = getConfig("wifi_pw");
  String setup_mqttuser = getConfig("mqtt_user");
  String setup_mqttpass = getConfig("mqtt_pw");
  String setup_mqttserver = getConfig("mqtt_server");
  server.on("/", [setup_ssid, setup_wifipw, setup_mqttuser, setup_mqttpass, setup_mqttserver]() {
    String html = "<html><head> <title>Handler Cloud ESP Config</title> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\"> <link href=\"style.min.css\" rel=\"stylesheet\"></head><body> <div class=\"container\"> <form method=\"POST\" action=\"/save\"> <div class=\"row\"> <div class=\"col-md-12\"> <h1>Handler Cloud ESP Config Interface</h1> <p>Setup the WiFi and MQTT for your ESP8266 Device</p> </div> </div> <div class=\"row\"> <div class=\"col-md-6\"> <h1>WiFi Config</h1> <label>SSID</label> <input name=\"wifi_ssid\" class=\"form-control\" value=\"";
    html += setup_ssid;
    html += "\" placeholder=\"SSID\"> <label>Password</label> <input name=\"wifi_pw\" class=\"form-control\" value=\"";
    html += setup_wifipw;
    html += "\" type=\"password\" placeholder=\"Password\"> </div> <div class=\"col-md-6\"> <h1>MQTT Config</h1> <label>Username</label> <input name=\"mqtt_user\" class=\"form-control\" value=\"";
    html += setup_mqttuser;
    html += "\" placeholder=\"Username\"> <label>Password</label> <input name=\"mqtt_pass\" class=\"form-control\" value=\"";
    html += setup_mqttpass;
    html += "\" type=\"password\" placeholder=\"Password\"> <label>Server</label> <input name=\"mqtt_server\" class=\"form-control\" value=\"";
    html += setup_mqttserver;
    html += "\" placeholder=\"Server\"> </div> </div> <div class=\"row\" style=\"margin-top:20px;\"> <div class=\"col-md-12\"> <input type=\"submit\" class=\"btn btn-success\" value=\"Save\" style=\"width:100%;\"> <br><br> <center>Software from <a href=\"http://www.kekskurse.de/\">S&ouml;ren</a>. Questions via Mail hello@kekskurse.de or Twitter <a href=\"https://twitter.com/soerenposchmann\">@soerenposchmann</a>.</center> </div> </div> </form> </div></body>";
    server.send(200, "text/html", html);
    });

  server.on("/save", []() {
    //debugMessage("Save WiFi and MQTT Config");
    String wifi_ssid = server.arg("wifi_ssid");
    String wifi_pw = server.arg("wifi_pw");
    String mqtt_user = server.arg("mqtt_user");
    String mqtt_pass = server.arg("mqtt_pass");
    String mqtt_server = server.arg("mqtt_server");
    String eeromConfigString = String(wifi_ssid.length()) + ";" + String(wifi_pw.length()) + ";" + String(mqtt_user.length()) + ";" + String(mqtt_pass.length()) + ";" + String(mqtt_server.length()) + ";" + wifi_ssid + wifi_pw + mqtt_user + mqtt_pass + mqtt_server;
    //debugMessage("Config String: "+eeromConfigString);
    EEPROM.begin(512);
    for (int i = 0; i < eeromConfigString.length(); ++i)
    {
      EEPROM.write(i, eeromConfigString[i]);
    }
    EEPROM.commit();
    //debugMessage("Saved to EEPROM");
    server.send(200, "text/html", "Saved: " + String(wifi_ssid));
  });
  server.on("/style.min.css", []() {
    server.send(200, "text/css", "@import url(https://fonts.googleapis.com/css?family=Lato:300,400,700);.btn,.form-control{background-image:none;padding:8px 16px;font-size:15px;line-height:1.42857143}body{background-color:#2b3e50;color:#ebebeb;font-family:Lato,'Helvetica Neue',Helvetica,Arial,sans-serif}.container{width:100%;margin-right:auto;margin-left:auto}.row{display:block;margin-left:-15px;margin-right:-15px;box-sizing:border-box}.form-control{display:block;width:100%;height:39px;color:#2b3e50;background-color:#fff;border:1px solid transparent;border-radius:0;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075);box-shadow:inset 0 1px 1px rgba(0,0,0,.075);-webkit-transition:border-color ease-in-out .15s,-webkit-box-shadow ease-in-out .15s;-o-transition:border-color ease-in-out .15s,box-shadow ease-in-out .15s;transition:border-color ease-in-out .15s,box-shadow ease-in-out .15s}.btn{display:inline-block;margin-bottom:0;font-weight:400;text-align:center;vertical-align:middle;-ms-touch-action:manipulation;touch-action:manipulation;cursor:pointer;border:1px solid transparent;white-space:nowrap;border-radius:0;-webkit-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none}.btn-success.focus,.btn-success:focus,.btn-success:hover{color:#fff;background-color:#449d44;border-color:transparent}.btn-success{color:#fff;background-color:#5cb85c;border-color:transparent}a{color:#df691a;text-decoration:none}label{font-size:12px;font-weight:700;line-height:17px}.col-md-12,.col-md-6{width:100%;float:left;padding:20px;display:block;box-sizing:border-box}@media (min-width:768px){.container{width:750px}}@media (min-width:992px){.col-md-12,.col-md-6{float:left;padding:10px;display:block;box-sizing:border-box}.container{width:970px}.col-md-6{width:50%}.col-md-12{width:100%}}@media (min-width:1200px){.container{width:1170px}}");
  });
  server.begin();
}

void HandlerCloud::handleClient() {
  server.handleClient();
}

// MQTT
void HandlerCloud::setupMQTT() {
  if (_internetConnection)
  {
    String s = getConfig("mqtt_user");
    char username[s.length()*2];
    s.toCharArray(username, s.length()*2);
    s = getConfig("mqtt_pw");
    char password[s.length()*2];
    s.toCharArray(password, s.length()*2);
    s = getConfig("mqtt_server");
    char server[s.length()*2];
    s.toCharArray(server, s.length()*2);

    String clientName = getClientID();
    debugMessage("Client Name: " + clientName);

    client.setServer("mqtt.handler.cloud", 1883);

    if (client.connect((char*) clientName.c_str(), username, password)) {
      debugMessage("Connectetion to MQTT Successful");
      String topics = String(_topics);
      int nextDemitter = topics.indexOf(";");
      while (nextDemitter != -1) {
        String topicName = topics.substring(0, nextDemitter);
        topics = topics.substring(nextDemitter + 1);
        nextDemitter = topics.indexOf(";");
        char topicChar[topicName.length()*2];
        topicName.toCharArray(topicChar, topicName.length()*2);
        client.subscribe(topicChar);
        debugMessage("Subscribe topic: " + topicName);
      }
      if (topics != "")
      {
        char topicChar[topics.length()*2];
        topics.toCharArray(topicChar, topics.length()*2);
        client.subscribe(topicChar);
        debugMessage("Subscribe topic: " + topics);
      }
    } else {
      debugMessage("Connection to MQTT Failed");
    }
  }
  else
  {
    debugMessage("No internet connection, abort MQTT Connection!");
  }
}
void HandlerCloud::checkMQTT() {
  if (client.connected() != true && _internetConnection == true)
  {
    debugMessage("Reconnect to MQTT Server");
    setupMQTT();
  }
}

bool HandlerCloud::addTopic(char* topic) {
  debugMessage("Add Topic: " + String(topic));
  if (_topics != "") {
    _topics = _topics + ";";
  }
  _topics = _topics + topic;
}
void HandlerCloud::setCallback(MQTT_CALLBACK_SIGNATURE) {
  debugMessage("Set MQTT Callback");
  client.setCallback(callback);
}

void HandlerCloud::handleMQTT() {
  client.loop();
}

void HandlerCloud::publishMessage(char* topic, char* message) {
  client.publish(topic, message);
}

//Externe Helper functions
void HandlerCloud::setupAll(bool debug, int baudRate) {
  if (debug == true) {
    setupDebug(baudRate);
  }
  setupWifi();
  setupWebserver();
  setupMQTT();
}

void HandlerCloud::checkAll() {
  checkWifi();
  checkMQTT();
}

//Interne Helper functions
void HandlerCloud::debugMessage(char* message, bool newline) {
  if (_debug == true)
  {
    if (newline == true)
    {
      Serial.println(message);
    } else {
      Serial.print(message);
    }
  }
}

void HandlerCloud::debugMessage(String message, bool newline) {
  if (_debug == true)
  {
    if (newline == true)
    {
      Serial.println(message);
    } else {
      Serial.print(message);
    }
  }
}

String HandlerCloud::macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
  }
  return result;
}
String HandlerCloud::getConfig(char* configName) {
  delay(100);
  EEPROM.begin(512);
  debugMessage("Read Config from EEPROM");
  int countDemitter = 0;
  bool readContent = true;
  int i = 0;
  String tmp = "";

  int lenght_wifi_ssid = 0;
  int lenght_wifi_pw = 0;
  int lenght_mqtt_user = 0;
  int lenght_mqtt_pw = 0;
  int lenght_mqtt_server = 0;

  while (readContent) {
    char value = EEPROM.read(i);
    if (String(value) == ";")
    {
      if (countDemitter == 0) {
        lenght_wifi_ssid = tmp.toInt();
      }
      else if (countDemitter == 1) {
        lenght_wifi_pw = tmp.toInt();
      }
      else if (countDemitter == 2) {
        lenght_mqtt_user = tmp.toInt();
      }
      else if (countDemitter == 3) {
        lenght_mqtt_pw = tmp.toInt();
      }
      else if (countDemitter == 4) {
        lenght_mqtt_server = tmp.toInt();
      }
      tmp = "";
      countDemitter++;
    }
    else {
      tmp = tmp + String(value);
    }
    if (countDemitter >= 5)
    {
      readContent = false;
    }
    i++;
    if (i > 10 and countDemitter <= 0)
    {
      readContent = false;
    }
  }

  if(configName=="wifi_ssid")
  {
    return redEERromString(i, lenght_wifi_ssid);
  }
  else
  {
    i = i + lenght_wifi_ssid;
  }
  if(configName=="wifi_pw")
  {
    return redEERromString(i, lenght_wifi_pw);
  }
  else
  {
    i = i + lenght_wifi_pw;
  }
  if(configName=="mqtt_user")
  {
    return redEERromString(i, lenght_mqtt_user);
  }
  else
  {
    i = i + lenght_mqtt_user;
  }
  if(configName=="mqtt_pw")
  {
    return redEERromString(i, lenght_mqtt_pw);
  }
  else
  {
    i = i + lenght_mqtt_pw;
  }
  if(configName=="mqtt_server")
  {
    return redEERromString(i, lenght_mqtt_server);
  }
  else
  {
    i = i + lenght_mqtt_server;
  }
  String s = "";
  return s;


}
String HandlerCloud::redEERromString(int start, int lenght) {
  String tmp = "";
  for (int i = 0; i < lenght; i++) {
    char c = EEPROM.read(start + i);
    tmp = tmp + String(c);
  }
  return tmp;
}
String HandlerCloud::getClientID() {
  String clientName;
    clientName += "esp8266-";
    uint8_t mac[6];
    WiFi.macAddress(mac);
    clientName += macToStr(mac);
    clientName += "-";
    clientName += String(micros() & 0xff, 16);
    debugMessage("Client ID: "+clientName);
    return clientName;
}
