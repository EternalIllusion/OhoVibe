#pragma once
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <Preferences.h>

/*******************************************************
*******************************************************/
//配网页面代码
const String page_html1 = R"(<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1.0,maximum-scale=1.0,minimum-scale=1.0,user-scalable=no"><title>OhoVibe配网</title><style type="text/css">*{transition:all .3s ease}body{height:100%;display:flex;justify-content:center}h2{text-align:center;color:#000000;line-height:2.2}.dowebok{background-color:#f9f9f9;border:1px solid #bbb;width:90%;height:auto;margin:auto;border-radius:10px;overflow:hidden;display:flex;justify-content:center}.form-item{position:relative;width:360px;margin:0 auto;padding-bottom:20px}.form-item input{width:80%;height:48px;padding-left:10px;border:1px solid #000000;border-radius:25px;font-size:18px;color:#000000;background-color:transparent;outline:none}.send_button{width:64%;height:50px;border:0;border-radius:25px;font-size:18px;color:#000;outline:none;cursor:pointer;background-color:#000000}.dowebok::placeholder{font-size:18px;line-height:1.4;color:#000000}</style></head><body><div class="dowebok"><form style="text-align: center;padding-top: 20px" name="input" action="/" method="POST"><h2>WIFI配置</h2><div class="form-item"><input id="username" type="text" name="ssid" autocomplete="off" placeholder="WiFi名称" list="data-list"><datalist id="data-list">)";
//Web配网模式下用于储存扫描到的WiFi ID
String scanNetworksID = "";
//配网页面代码2
const String page_html2 = R"(</datalist></div><div class="form-item"><input id="password" type="password" name="password" autocomplete="off" placeholder="WiFi密码"></div><div class="form-item"><div><input id="send_button" type="submit" value="保存并连接"></div></div><div class="form-item"><div class="user_text"><br /></div></div></form></div></body></html>)";

const char* AP_NAME = "ESP32TiaoDan";  //Web配网模式下的AP-wifi名字

//暂时存储wifi账号密码
char sta_ssid[32] = { 0 };      //定义配网得到的WIFI名长度(最大32字节)
char sta_password[32] = { 0 };  //定义配网得到的WIFI密码长度(最大32字节)

bool setWiFi_Flag = false;

const byte DNS_PORT = 53;        //DNS端口号
IPAddress apIP(192, 168, 69, 1);  //esp32-AP-IP地址

DNSServer dnsServer;   //创建dnsServer实例
WebServer server(80);  //创建WebServer

void handleRoot() {  //访问主页回调函数
  Serial.println("handleRoot");
  server.send(200, "text/html", page_html1 + scanNetworksID + page_html2);  //scanNetWprksID是扫描到的wifi,可在网络选择框中下拉选择
}

void handleRootPost() {  //Post回调函数获取wifi账号与密码
  String wifiid = "";
  String wifipass = "";
  Serial.println("handleRootPost");

  //读取网页返回的账号
  if (server.hasArg("ssid")) {  //判断是否有账号参数
    Serial.print("got ssid:");
    strcpy(sta_ssid, server.arg("ssid").c_str());  //将账号参数拷贝到sta_ssid中
    Serial.println(sta_ssid);
  } else {  //没有参数
    Serial.println("error, not found ssid");
    server.send(200, "text/html", "<meta charset='UTF-8'>提示:请输入WiFi名称");  //返回错误页面
    return;
  }

  //读取网页返回的密码
  if (server.hasArg("password")) {
    Serial.print("got password:");
    strcpy(sta_password, server.arg("password").c_str());
    Serial.println(sta_password);
  } else {
    Serial.println("error, not found password");
    server.send(200, "text/html", "<meta charset='UTF-8'>提示:请输入WiFi密码");
    return;
  }

  //写入WIFI信息
  wifiid = sta_ssid;
  wifipass = sta_password;

  Preferences prefs;
  prefs.begin("wifi");
  prefs.putString("ssid", wifiid);        //写入账号
  prefs.putString("password", wifipass);  //写入密码
  prefs.end();

  server.send(200, "text/html", "<meta charset='UTF-8'><h1>保存成功,ESP32重启中...</h1>");  //返回保存成功页面
  delay(2000);

  ESP.restart();  //重启ESP32
}

//删除保存的wifi信息
void DeleteWiFi() {
  Serial.println("\r\n正在清除NVS保存的信息...");

  Preferences prefs;
  prefs.begin("wifi", false);           //为false才能删除键值
  Serial.println(prefs.freeEntries());  //查询清除前的剩余空间
  prefs.remove("ssid");                 //删除当前命名空间中键名为"ssid"的元素
  prefs.remove("password");             //删除当前命名空间中键名为"ssid"的元素
  prefs.clear();
  delay(1000);
  Serial.println(prefs.freeEntries());  //查询清除后的剩余空间
  prefs.end();

  esp_wifi_restore();  //删除保存的wifi信息

  Serial.println("\r\n已清除WIFI信息,准备重置...");
}


//初始化AP模式
void initSoftAP(void) {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  Serial.println(WiFi.softAPIP());
  Serial.print("LocalIP: ");
  Serial.println(WiFi.localIP());
  if (WiFi.softAP(AP_NAME)) {
    Serial.println("ESP32 SoftAP is right");
  }
}

//初始化WebServer
void initWebServer(void) {
  //server.on("/",handleRoot);
  //上面那行必须以下面这种格式去写否则无法强制门户
  server.on("/", HTTP_GET, handleRoot);       //设置主页回调函数
  server.onNotFound(handleRoot);              //设置无法响应的http请求的回调函数
  server.on("/", HTTP_POST, handleRootPost);  //设置Post请求回调函数,获取WIFI账号和密码
  server.begin();                             //启动WebServer
  Serial.println("WebServer started!");
}

//初始化DNS服务器
void initDNS(void) {
  if (dnsServer.start(DNS_PORT, "*", apIP)) {  //判断将所有地址映射到esp的ip上是否成功
    Serial.println("start dnsserver success.");
  } else {
    Serial.println("start dnsserver failed.");
  }
}

//扫描附近的WiFi，为了显示在配网界面
bool scanWiFi(void) {
  Serial.println("scan start--------->");
  //扫描附近WiFi
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no WIFI found");
    scanNetworksID += "<option>no WIFI found</option>";
    return false;
  } else {
    Serial.print(n);
    Serial.println(" WIFI found");
    for (int i = 0; i < n; ++i) {
      //Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      scanNetworksID += "<option>" + WiFi.SSID(i) + "</option>";
    }
    Serial.println(scanNetworksID);
    return true;
  }
}

//配置wifi
void SetWiFi() {
  scanNetworksID = "";
  initSoftAP();
  initWebServer();
  initDNS();
  scanWiFi();
  while (setWiFi_Flag == false) {
    server.handleClient();
    dnsServer.processNextRequest();
    if (WiFi.status() == WL_CONNECTED) {
      server.stop();
      setWiFi_Flag = true;
    }
  }
}
