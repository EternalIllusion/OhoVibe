/*
 * 小小的渣渣，大大的挑担！
 * 有种就来茶思卧～(￣▽￣～)~
 * _(:з」幺)==8 好爽♥️♥️
 * 
 * WIFI采用硬编码，请修改代码来修改WIFI。
 * 
 * @package TD_ESP32
 * @author EterIll
 * @author MyZhaZha
 * @version R_1.0.2_TEST_FORCE_HARD_ENCODE_WIFI(TEST_RELEASE)
 * @sponsor 一袋星光
 */
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <esp_wifi.h>  //用于删除保存的wifi信息，踏马的鲨臂espressif
//#include "SetWiFi.h"   //Web配网
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "htmlfiles.h"
/*
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>
*/

//其他依赖
//#include <DNSServer.h>
//#include <WebServer.h>

/*

BLEAdvertising *pAdvertising; // BLE 广播对象
BLEServer *pServer;           // BLE 服务器

// 设置 BLE 名称为 "TD_<IP 地址>"
String bleName = "TD_";
*/
// 通道数量
#define NUM_CHANNELS 11

// 引脚定义（这里只列出通道 0 - 10 的引脚定义，通道 11 用于设置所有通道）
const int PIN_EN[NUM_CHANNELS] = {22, 21, 35, 5, 26, 33, 16, 0, 14, 2, 13};
const int PIN_PWM[NUM_CHANNELS] = {23, 19, 34, 18, 25, 32, 17, 4, 27, 15, 12};


// 定义 LEDC 通道号（ESP32 支持 16 个 PWM 通道）
int pwmChannels[NUM_CHANNELS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// 设置 LEDC 通道频率和分辨率
#define LEDC_FREQ 5000    // PWM频率 5kHz
#define LEDC_RESOLUTION 8 // 分辨率 8 位，0-255

//强制使用程序内置密码连接
const char* ssid = "PandoraBox";
const char* password = "myzhazha.top"; 


//nvs声明，别动
String PrefSSID = "";
String PrefPassword = "";

// 处理某个通道的强度设置
void handleChannel(int channel, int intensity) {
  if (intensity == 0) {
    // 强度为 0000 时，PWM 0% 并且 EN 拉高
    digitalWrite(PIN_EN[channel], HIGH);
    ledcWrite(pwmChannels[channel], 0); // PWM 0%
  } else {
    // 强度为 0001 到 9999 时，计算 PWM 占空比并且 EN 拉低
    // 映射强度值：强度 0001 对应 51%，9999 对应 100%。 50%以下对应驱动器刹车
    int pwmValue = mapIntensityToPWM(intensity); // 计算 PWM 占空比
    digitalWrite(PIN_EN[channel], LOW);
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

//bool BLEstat = true;
AsyncWebServer wsserver(80);
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket 端口 81

// WebSocket 回调函数
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        /*
        if(BLEstat){
          bleStopCast();
          BLEstat = false;
        }
        */
      }
      break;
    case WStype_TEXT:
     
      String message = String((char*)payload);
    
    // 解析消息 "<通道，二位十进制><强度，四位十进制>"
    int channel = message.substring(0, 2).toInt();
    int intensity = message.substring(2, 6).toInt();
    Serial.printf("[%u] WS: %s %s\n", num, channel, intensity);
    // 如果通道在有效范围内（0 到 10）
    if (channel >= 0 && channel < NUM_CHANNELS) {
      handleChannel(channel, intensity);
    }
    // 如果是通道 11，表示设置所有通道的强度
    else if (channel == 11) {
      for (int i = 0; i < NUM_CHANNELS; i++) {
        handleChannel(i, intensity);
      }
    }
  }
}
/*
void bleBroadCast() {
  // 启动 BLE 广播，设备将会显示在扫描列表中，但无法连接
  pAdvertising->start();
  Serial.println("Bluetooth Advertising Started - Scanable but Unconnectable");
}

void bleStopCast() {
  // 停止 BLE 广播
  pAdvertising->stop();
  Serial.println("Bluetooth Advertising Stopped");
}
*/
void setup() {
  Serial.begin(9600);
  Serial.println("ESP32 Vibe Egg.");
  Serial.println("ESP32 pin init.");
  // 初始化 PWM 输出
  for (int i = 0; i < NUM_CHANNELS; i++) {
    // 设置引脚模式
    Serial.printf("CH%d : EN-%d PWM-%d\r\n", i, PIN_EN[i], PIN_PWM[i]);
    pinMode(PIN_EN[i], OUTPUT);
    digitalWrite(PIN_EN[i], HIGH);  // 默认 EN 拉高
    pinMode(PIN_PWM[i], OUTPUT);    // 设置 PWM 引脚为输出
    // 配置 PWM 通道
    ledcSetup(pwmChannels[i], LEDC_FREQ, LEDC_RESOLUTION);
    ledcAttachPin(PIN_PWM[i], pwmChannels[i]);
  }
  
  Serial.println("ESP32 WIFI init.");
/*
  //配网部分
  PrefSSID = "TD";
  PrefPassword = "TiaoDan!";

  //首次使用自动进入配网模式,读取NVS存储空间内的ssid、password
  Preferences prefs;
  prefs.begin("wifi");
  if (prefs.isKey("ssid")) {                     //读取账号
    PrefSSID = prefs.getString("ssid", "TD");  //如果键值为空，返回"TD"
  }
  Serial.println(PrefSSID);

  if (prefs.isKey("password")) {                         //读取密码
    PrefPassword = prefs.getString("password", "TiaoDan!");  //如果键值为空，返回"挑担！"
  }
  Serial.println(PrefPassword);

  prefs.end();  //从nvs获取到wifi信息后，关闭Preferences


//  Serial.println("Connecting to :" + PrefSSID + "->" + PrefPassword);

  if ((PrefSSID != "TD") && (PrefPassword != "TiaoDan!")) {
    byte i = 0;
    
    
    
    //WiFi.mode(WIFI_STA);  //切换为STA模式，进行入网
    WiFi.mode(WIFI_AP);  //切换为AP模式，进行入网
    
    
    
    WiFi.begin(PrefSSID.c_str(), PrefPassword.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      i++;
      Serial.print('.');
      delay(1000);
      //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  //板载led灯闪烁

      if (i > 59) {           //一分钟没连上，多半是寄了，重新配置WIFI
        WiFi.mode(WIFI_OFF);  //设置设备的工作模式为关闭模式
        Serial.print("当前模式为：");
        Serial.println(WiFi.getMode());  //告知用户设备当前工作模式
        //DeleteWiFi();                    //清空nvs中的wifi信息
        SetWiFi();
      }
    }

  } else {                //重新配置WIFI
    WiFi.mode(WIFI_OFF);  //设置设备的工作模式为关闭模式
    Serial.print("当前模式为：");
    Serial.println(WiFi.getMode());  //告知用户设备当前工作模式
    //DeleteWiFi();                    //清空nvs中的wifi信息
    SetWiFi();
  }
*/

    //强制启用硬编码的WIFI
  WiFi.mode(WIFI_AP);  //切换为AP模式，进行入网
  WiFi.begin(ssid, password);  //鲨臂编译器
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('......');
    delay(1000);
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  //板载led灯闪烁
  }
   
  Serial.println();
  Serial.println("Connect ok");
  Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
  Serial.printf("PASS:%s\r\n", WiFi.psk().c_str());
  Serial.println(WiFi.localIP());
  
//TODO:这里创建一个蓝牙用来显示IP地址，或者用振动传递IP（？）
/*
    // 初始化 BLE
  BLEDevice::init(""); // 初始化 BLE
  pServer = BLEDevice::createServer();  // 创建 BLE 服务器
  pAdvertising = pServer->getAdvertising(); // 获取 BLE 广播对象
  pAdvertising->setScanResponse(false);   // 设置为不响应扫描
  
  bleName += WiFi.localIP().toString();  // 将 IP 地址添加到 BLE 名称
  // 设置蓝牙广播名称为 "TD_<IP 地址>"
  pAdvertising->setName(bleName.c_str());
  // 设置广播模式为不可连接
  pAdvertising->setAdvertiseType(BLE_GAP_ADV_TYPE_SCAN_IND); // 设置广播类型为扫描指示（即能被扫描到，但不能连接）
  bleBroadCast();
*/

  //向小小的内存里狠狠插入ws服务器
  Serial.print("ESP32 wss init.");
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.print("ESP32 wps init.");
  //渲染HTML，然而没有渲染，纯发送字符串罢了（
  wsserver.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("Web Server: received a web page request");
    String html = HTML_CHANNEL;  //头文件里面，自己找
    request->send(200, "text/html", html);
  });
  wsserver.on("/swipe", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("Web Server: received request,wan de hai ting hua.");
    String html = HTML_SWIPE;
    request->send(200, "text/html", html);
  });

  wsserver.begin();
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  webSocket.loop();
  // TO DO: 没想好要干啥
}

