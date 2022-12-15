//coded by AsrofurRizqi
//testngoding1.0

#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h> 
#include <TimeLib.h>
#include <WiFiClient.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String token = "";
int load1,load2,load3;
long long tx = 0,rx =0;
long long lasttx,lastrx;
const int button = 16;
int state = 0;
const char *ssid     = "KURO_2G"; //change wifi ssid
const char *password = "WifiPewds"; //change wifi password
const char *server = "http://192.168.1.1/ubus"; //your openwrt ip
const char *reqtoken = "{\"jsonrpc\":\"2.0\",\"id\":\"1\",\"method\":\"call\",\"params\":\[\"00000000000000000000000000000000\",\"session\",\"login\",{\"username\": \"kuro\",\"password\": \"kuro\"}\]}"; //change ubus user and password(kuro/kuro) with your ubus user
String reqcpuinfo = "{\"jsonrpc\":\"2.0\",\"id\":\"1\",\"method\":\"call\",\"params\":\[\"token\",\"luci\",\"getCPUInfo\",{}\]}";
String reqsysinfo = "{\"jsonrpc\":\"2.0\",\"id\":\"1\",\"method\":\"call\",\"params\":\[\"token\",\"system\",\"info\",{}\]}";
String reqifaceinfo = "{\"jsonrpc\":\"2.0\",\"id\":\"1\",\"method\":\"call\",\"params\":\[\"token\",\"network.device\",\"status\",{\"name\":\"br-lan\"}\]}";
String reqcpuusage = "{\"jsonrpc\":\"2.0\",\"id\":\"1\",\"method\":\"call\",\"params\":\[\"token\",\"luci\",\"getCPUUsage\",{}\]}";
String filexec = "{\"jsonrpc\":\"2.0\",\"id\":\"1\",\"method\":\"call\",\"params\":\[\"token\",\"file\",\"exec\",{\"command\":\"reboot\",\"params\": []}\]}";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
HTTPClient http;
WiFiClient client;
int last_second;

void getToken(){
  DynamicJsonBuffer jsonBuffer;
  String payload = "{}";
  http.begin(client, server);
  http.addHeader("Content-Type", "application/json");
  int httpRes = http.POST(reqtoken);
  if (httpRes == 200) {
    Serial.print("Response token: ");
    Serial.println(httpRes);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpRes);
  }
  JsonObject& root = jsonBuffer.parseObject(payload);
  JsonObject& result_1 = root["result"][1];
  const char* ubus_rpc_session = result_1["ubus_rpc_session"];
  // Free resources
  http.end();
  token = ubus_rpc_session;
  reqcpuinfo.replace("token",token);
  reqsysinfo.replace("token",token);
  reqifaceinfo.replace("token",token);
  reqcpuusage.replace("token",token);
  filexec.replace("token",token);
  jsonBuffer.clear();
  return;
}
String getCPUTemp(){
  DynamicJsonBuffer jsonBuffer1;
  String payload = "{}";
  String temp = "";
  http.begin(client, server);
  http.addHeader("Content-Type", "application/json");
  int httpRes = http.POST(reqcpuinfo);
  if (httpRes == 200) {
    Serial.print("Response1: ");
    Serial.println(httpRes);
    payload = http.getString();
    JsonObject& root1 = jsonBuffer1.parseObject(payload);
    if(root1["error"]["code"] == -32002){
      getToken();
    }else{
      String info = root1["result"][1]["cpuinfo"];
      int indexdata = info.indexOf("°C") - 4;
      temp = info.substring(indexdata,indexdata+4);
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpRes);
  }
  // Free resources
  jsonBuffer1.clear();
  http.end();
  return temp;
}
String getCPUUsage(){
  DynamicJsonBuffer jsonBuffer4;
  String payload = "{}";
  String info;
  http.begin(client, server);
  http.addHeader("Content-Type", "application/json");
  int httpRes = http.POST(reqcpuusage);
  if (httpRes == 200) {
    Serial.print("Response2: ");
    Serial.println(httpRes);
    payload = http.getString();
    JsonObject& root1 = jsonBuffer4.parseObject(payload);
    if(root1["error"]["code"] == -32002){
      getToken();
    }else{
      String data = root1["result"][1]["cpuusage"];
      info = data;     
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpRes);
  }
  // Free resources
  jsonBuffer4.clear();
  http.end();
  return info;
}
void getSYSInfo(){
  DynamicJsonBuffer jsonBuffer2;
  String payload = "{}";
  http.begin(client, server);
  http.addHeader("Content-Type", "application/json");
  int httpRes = http.POST(reqsysinfo);
  if (httpRes == 200) {
    Serial.print("Response3: ");
    Serial.println(httpRes);
    payload = http.getString();
    JsonObject& root1 = jsonBuffer2.parseObject(payload);
    if(root1["error"]["code"] == -32002){
      getToken();
    }else{
      JsonObject& data = root1["result"][1];
      JsonArray& data1  = data["load"];
      load1 = data1[0];
      load2 = data1[1];
      load3 = data1[2];
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpRes);
  }
  // Free resources
  jsonBuffer2.clear();
  http.end();
  return;
}
void getIfaceData(){
  DynamicJsonBuffer jsonBuffer3;
  String payload = "{}";
  http.begin(client, server);
  http.addHeader("Content-Type", "application/json");
  int httpRes = http.POST(reqifaceinfo);
  if (httpRes == 200) {
    Serial.print("Response4: ");
    Serial.println(httpRes);
    payload = http.getString();
    JsonObject& root1 = jsonBuffer3.parseObject(payload);
    if(root1["error"]["code"] == -32002){
      getToken();
    }else{
      JsonObject& data = root1["result"][1]["statistics"];
      long long data1 = data["tx_bytes"];
      long long data2 = data["rx_bytes"];
      lasttx = data1 - tx;
      lastrx = data2 - rx;
      tx = data1;
      rx = data2;
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpRes);
  }
  // Free resources
  jsonBuffer3.clear();
  http.end();
  return;
}
void buttonCommand(){
  DynamicJsonBuffer jsonBuffer5;
  String payload = "{}";
  http.begin(client, server);
  http.addHeader("Content-Type", "application/json");
  int httpRes = http.POST(filexec);
  if (httpRes == 200) {
    Serial.print("Response5: ");
    Serial.println(httpRes);
    payload = http.getString();
    JsonObject& root1 = jsonBuffer5.parseObject(payload);
    if(root1["error"]["code"] == -32002){
      getToken();
    }
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpRes);
  }
  // Free resources
  jsonBuffer5.clear();
  http.end();
  return;
}
String FormatBytes(double bytes){
  double datas = bytes;
  String orders[] = { "B", "KB", "MB", "GB", "TB" };
  int order = 0;
  while (datas >= 1024 && order < sizeof(orders) - 1) {
    order++;
    datas = datas/1024;
  }
  String result = String(datas) + orders[order];
  return result;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  timeClient.begin();
  pinMode(button, INPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15, 1);
  display.println("Senin 19/9/2022");
  
  display.setTextSize(2);
  display.setCursor(5, 10);
  display.println("Display ON");
  
  display.display();
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    //time ntp
    if(!timeClient.update()){
      timeClient.forceUpdate();
    }
    //cpu temp & usage
    String temp = "CPU: " + getCPUTemp() +"C | "+getCPUUsage();
    Serial.println(temp);

    //cpu load
    getSYSInfo();
    double load4 = load1 / 65536.0;
    double load5 = load2 / 65536.0;
    double load6 = load3 / 65536.0;

    //interface bandwitdh        
    getIfaceData();
    String pemakaian = "LAN:"+ FormatBytes(tx+rx) + "|" + FormatBytes(lasttx)+"s";
    unsigned long rawTime = timeClient.getEpochTime() + 25200;

    //button state
    state = digitalRead(button);
    if(state == HIGH){
        Serial.println("Button Pressed");
        buttonCommand();
    }

    if (rawTime != last_second){
    time_t t = rawTime;
  
    int jam = hour(t);
    String jamStr = jam < 10 ? "0" + String(jam) : String(jam);
    int menit = minute(t);
    String menitStr = menit < 10 ? "0" + String(menit) : String(menit);
    int detik = second(t);
    String detikStr = detik < 10 ? "0" + String(detik) : String(detik);
          
    String hari;
    switch (weekday(t)){
      case 1 :
        hari = "Minggu";
        break;
      case 2 :
        hari = "Senin";
        break;
      case 3 :
        hari = "Selasa";
        break;
      case 4 :
        hari = "Rabu";
        break;
      case 5 :
        hari = "Kamis";
        break;
      case 6 :
        hari = "Jumat";
        break;
      case 7 :
        hari = "Sabtu";
        break;
      }
    int tgl = day(t);
    String tglStr = tgl < 10 ? "0" + String(tgl) : String(tgl);
    int bln = month(t);
    String blnStr = bln < 10 ? "0" + String(bln) : String(bln);
    int thn = year(t);
    String thnStr = String(thn);

    String tanggal = hari + " " + tglStr + "/" + blnStr + "/" + thnStr;
    String waktu = jamStr + ":" + menitStr + ":" + detikStr;

    String load = "LOAD: " + String(load4) + "|" + String(load5) + "|" + String(load6);
    
    Serial.println(waktu);
    display.clearDisplay();
  
    display.setTextSize(1);
    display.setCursor(15, 1);
    display.println(tanggal);
    
    display.setTextSize(2);
    display.setCursor(15, 10);
    display.println(waktu);

    display.setTextSize(1);
    display.setCursor(15, 27);
    display.println(temp);

    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println(load);
    
    display.setTextSize(1);
    display.setCursor(0, 55);
    display.println(pemakaian);    

    display.display(); 
    delay(500);
    last_second = rawTime;
    }
  }else {
    Serial.println("Wifi Not Connected");   
  }
}

