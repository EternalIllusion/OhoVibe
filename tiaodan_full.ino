/*
 * 小小的渣渣，大大的挑担！
 * 有种就来茶思卧～(￣▽￣～)~
 * _(:з」幺)==8 好爽♥️♥️
 * 
 * AP IP 192.168.69.1
 * 
 * @package TD_ESP32
 * @author EterIll
 * @author MyZhaZha
 * @version R_1.2.0(RELEASE)
 * @sponsor 一袋星光
 */
 
/*TODO:web控制添加自动连接ws功能*/
#include <Arduino.h>

#include <WiFi.h>
#include <Preferences.h>
#include <esp_wifi.h>  //用于删除保存的wifi
#include "SetWiFi.h"   //Web配网
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "htmlfiles.h"

//其他依赖
//#include <WebServer.h>

// 通道数量
#define NUM_CHANNELS 11

// 引脚定义，注意有些引脚无法使用pwm
const int PIN_EN[NUM_CHANNELS] = {22, 21, 35, 5, 26, 33, 16, 0, 14, 2, 13};
const int PIN_PWM[NUM_CHANNELS] = {23, 19, 34, 18, 25, 32, 17, 4, 27, 15, 12};

// 设置创建热点的密码，确保密码长度不少于8个字符
const char* ap_password = "13572468";

//启用更详尽的串口日志
const bool detailedlog = true;

//启用AP网络,用于联网后显示IP地址
const bool enableap = false;

//=====================================//

IPAddress wsapIP(192, 168, 69, 1);  //esp32-AP-IP地址

// 定义 LEDC 通道号（ESP32 支持 16 个 PWM 通道），别动！
int pwmChannels[NUM_CHANNELS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// 设置 LEDC 通道频率和分辨率
#define LEDC_FREQ 5000    // PWM频率 5kHz
#define LEDC_RESOLUTION 8 // 分辨率 8 位，0-255

/*
//强制使用程序内置密码连接
const char* fcssid = "PandoraBox";
const char* fcpwd = "myzhazha.top"; 
*/

//nvs声明，别动
String PrefSSID = "";
String PrefPassword = "";

// 处理某个通道的强度设置
void handleChannel(int channel, int intensity) {
  if (detailedlog){
    Serial.print("CH:");
    Serial.print(channel);
    Serial.print(":");
    Serial.print(intensity);
  }
  if (intensity == 0) {
    // 强度为 0000 时，PWM 0% 并且 EN 拉高
    ledcWrite(pwmChannels[channel], 0); // PWM 0%
    digitalWrite(PIN_EN[channel], LOW);
  } else {
    // 强度为 0001 到 9999 时，计算 PWM 占空比并且 EN 拉低
    // 映射强度值：强度 0001 对应 51%，9999 对应 100%。 50%以下对应驱动器刹车
    int pwmValue = mapIntensityToPWM(intensity); // 计算 PWM 占空比
    digitalWrite(PIN_EN[channel], HIGH);
    ledcWrite(pwmChannels[channel], pwmValue); // 设置 PWM 占空比
    //Serial.printf("Execute CH%d en-%d pwm-%d value-%d\n", channel, PIN_EN[channel], pwmChannels[channel], pwmValue);
  }
}

// 映射强度到 PWM 占空比的函数
int mapIntensityToPWM(int intensity) {
  if (intensity == 0) return 0;
  
  // 计算占空比：强度 0001 对应 51%，9999 对应 100%
  float dutyCycle = 51 + (float)(intensity - 1) / 9998.0 * (100 - 51);
  
  // 将占空比映射到 0 到 255 之间
  return (int)(dutyCycle * 255 / 100);
}

AsyncWebServer wsserver(80);
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket 端口 81

//初始化DNS服务器
void initAppDNS(void) {
  if (dnsServer.start(DNS_PORT, "*", apIP)) {  //判断将所有地址映射到esp的ip上是否成功
    Serial.println("start dnsserver success.");
  } else {
    Serial.println("start dnsserver failed.");
  }
}

// WebSocket 回调函数
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      //Serial.printf("[%u] Disconnected!\n", num);
      Serial.println("Disconnected!");
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        //Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        Serial.print("Connected from ");
        Serial.printf("%d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
      }
      break;
    case WStype_TEXT:
      {
        String message = String((char*)payload);
      
        // 解析消息 "<通道，二位十进制><强度，四位十进制>"
        int channel = message.substring(0, 2).toInt();
        int intensity = message.substring(2, 6).toInt();
        if (detailedlog){
          Serial.printf("[%u] WS: %d %d\n", num, channel, intensity);
        }
        // 如果通道在有效范围内（0 到 10）
        if (channel >= 0 && channel < NUM_CHANNELS) {
          handleChannel(channel, intensity);
        }
        // 如果是通道 11，表示设置所有通道的强度
        else if (channel == NUM_CHANNELS) {
          for (int i = 0; i < NUM_CHANNELS; i++) {
            handleChannel(i, intensity);
          }
        }
      }
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("ESP32 Vibe Egg.");
  Serial.println("ESP32 pin init.");
  // 初始化 PWM 输出
  for (int i = 0; i < NUM_CHANNELS; i++) {
    // 设置引脚模式
    Serial.printf("CH%d : EN-%d PWM-%d\r\n", i, PIN_EN[i], PIN_PWM[i]);
    pinMode(PIN_EN[i], OUTPUT);
    digitalWrite(PIN_EN[i], LOW);  // 默认 EN 拉高
    pinMode(PIN_PWM[i], OUTPUT);    // 设置 PWM 引脚为输出
    // 配置 PWM 通道
    ledcSetup(pwmChannels[i], LEDC_FREQ, LEDC_RESOLUTION);
    ledcAttachPin(PIN_PWM[i], pwmChannels[i]);
  }
  
  Serial.println("ESP32 WIFI init.");

  //配网部分
  PrefSSID = "PandoraBox";
  PrefPassword = "myzhazha.top";

  //首次使用自动进入配网模式,读取NVS存储空间内的ssid、password
  Preferences prefs;
  prefs.begin("wifi");
  if (prefs.isKey("ssid")) {                     //读取账号
    PrefSSID = prefs.getString("ssid", "PandoraBox");  //如果键值为空，返回"TD"
  }

  if (prefs.isKey("password")) {                         //读取密码
    PrefPassword = prefs.getString("password", "myzhazha.top");  //如果键值为空，返回你渣姨的WIFI密码
  }

  prefs.end();  //从nvs获取到wifi信息后，关闭Preferences

  Serial.print("Connecting to :");
  Serial.print(PrefSSID);
  Serial.print("->");
  Serial.println(PrefPassword);

  if (1!=0/*(PrefSSID != "TD") && (PrefPassword != "TiaoDan!")*/) {
    uint8_t i = 0;
    if (enableap){
    //WiFi.mode(WIFI_STA);  //切换为STA模式，进行入网
    //WiFi.mode(WIFI_AP);  //切换为AP模式，进行入网
      WiFi.mode(WIFI_AP_STA);
    }else{
      WiFi.mode(WIFI_AP);  //切换为AP模式，进行入网
    }
    WiFi.begin(PrefSSID.c_str(), PrefPassword.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      i++;
      Serial.print('.');
      delay(1000);
      //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  //板载led灯闪烁

      if (i > 59) {           //一分钟没连上，多半是寄了，重新配置WIFI
        WiFi.mode(WIFI_OFF);  //设置设备的工作模式为关闭模式
        Serial.print("current mode：");
        Serial.println(WiFi.getMode());  //告知用户设备当前工作模式
        //DeleteWiFi();                    //清空nvs中的wifi信息
        SetWiFi();
      }
    }

  } else {                //重新配置WIFI
    WiFi.mode(WIFI_OFF);  //设置设备的工作模式为关闭模式
    Serial.print("current mode：");
    Serial.println(WiFi.getMode());  //告知用户设备当前工作模式
    //DeleteWiFi();                    //清空nvs中的wifi信息
    SetWiFi();
  }

/*
  //强制启用硬编码的WIFI
  WiFi.mode(WIFI_AP);  //切换为AP模式，进行入网
  WiFi.begin(fcssid, fcpwd);  //鲨臂编译器
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('......');
    delay(1000);
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  //板载led灯闪烁
  }
*/   

  Serial.println();
  Serial.println("Connect ok");
  Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
  Serial.printf("PASS:%s\r\n", WiFi.psk().c_str());
  Serial.println(WiFi.localIP());

  if (enableap){ 
    WiFi.softAPConfig(wsapIP, wsapIP, IPAddress(255, 255, 255, 0)); 

    // 创建热点
    IPAddress ip = WiFi.localIP();
    String apName = "Vibe_" + ip.toString();

    if (WiFi.softAP(apName.c_str(), ap_password)) {
      Serial.print("AP Created with SSID: ");
      Serial.println(apName);
      Serial.print("AP Password: ");
      Serial.println(ap_password);
    } else {
      Serial.println("Failed to create AP");
    }
    
    Serial.println("Starting dns server.");

    initAppDNS();//启动dns服务器
  
  }
  
  //向小小的内存里狠狠插入ws服务器
  Serial.println("ESP32 wss init.");
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("ESP32 wps init.");
  
  //渲染HTML，然而没有渲染，纯发送字符串罢了（
  wsserver.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("Web Server: received a web page request");
    String html = HTML_CHANNEL;  //头文件里面，自己找
    request->send(200, "text/html", html);
  });
  
  wsserver.begin();
  
  Serial.println("Web Server ok.");

  
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("=====Init Finished=====");
}

void loop() {
  webSocket.loop();
}

