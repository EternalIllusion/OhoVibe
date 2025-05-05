<img src="https://raw.githubusercontent.com/EternalIllusion/OhoVibe/refs/heads/main/banner.png"/>
<div align = center>
  <h1>OhoVibe</h1>
  <img alt="version" src="https://img.shields.io/badge/Channels-9-8A2BE2">
  <img alt="version" src="https://img.shields.io/badge/Very-Powerful-8A2BE2">
</div>

A simple controller program for MyZhaZha's vibrate massager.

> [!WARNING]
> Access to this repository is not recommended for individuals under the age of 18, as it may cause irreversible harm.    
> This is not a treating product and will not help in treating process.

Written in arduino and based on ESP32.

EasyEDA Project: Under review.

Demo video: Under review.

 <img alt="version" src="https://img.shields.io/badge/Version-1.2.8-brightgreen">

Online controller: [click here](https://eterill.xyz/OhoVibe/)

Android APP: [download](https://raw.githubusercontent.com/EternalIllusion/OhoVibe/refs/heads/app/app.apk)

## Build Dependencies

You need to install **ESP32** Boards and **following libraries**.

```text
ArduinoHttpClient
ArduinoWebsockets
Async TCP
ESP Async WebServer
```

## Build & Flash

Since you've installed ESP32 boards, you can simply select your board, compile and flash. 

For MyZhaZha's PCB or you have no idea which board you should select, simply select **ESP32 Dev Module**. *(If you've installed many version of ESP32 boards library, I suggest the Official ones.)*

Some boards may stuck while flashing and inform you that you should perform power **SYNC**.You should make sure that your board is properly connected to your computer *(not simply connected to provide power)* , and hold **BOOT(FLASH)** and tap once **RST(EN)** before releasing boot.You may need to use extra wires to do that.If you still don't know how to deal with that, you can **ask chatGPT or deepseek**. 

## Healthy Warnings

> [!WARNING]
> You should properly use the thing and careful with potential risks.
> 
> This product contains **remote control** feature, please don't expose your ip on the web.
> 
> **THIS IS NOT A TREATING PRODUCT AND WILL NOT HELP IN TREATING PROCESS.**
> 
> We do not suggest use this if you're uncomfortable with it.
> 
> **WE WONT TAKE THE RESPONSIBILITY FOR YOUR HEALTH ISSUES OR MISUSES.**

## WLAN Configuration

The program will automatically connect to the wifi written in the program. If failed, the program will start a wifi configuration wizard.

The following steps will tell you how to configure through the wizard.

Firstly, Connect to the wifi named `"OhoVibe_Config"` , Open your web browser and enter the URL: `192.168.69.1`.

> If a password is required to connect the wifi, please use `12345678` or `13572468` .

Secondly, When the page is ready, **select** the wifi you wanted to connect and **enter the password** for the wifi.

Then, Click on the submit button and wait until the ESP32 restarts. After restarted, use a device with wifi scan ability to scan the wifi, you will find a wifi named `Vibe_{IP}`, the `{IP}` is the IP address of ESP32. Cut the power of ESP32 to cancel the IP display.Now you can use the app controller or webpage controller.

The IP address you should fill in the controller is:
```text
{ip}:{port}
//default port is 81.
//example: 192.168.69.1:81
```

> [!WARNING]
> This product contains **remote control** feature, please don't expose your ip on the web.

## Using FRP for Cross-LAN Control

 1. Prerequisites
 
Before we can use FRP to establish a cross - LAN connection, we need to have the following:

- A public - network server (VPS or cloud server, A dedicated frp server is also OK ) with a static IP address. This server will act as the FRP service side (server - end).

- The device, which is located in the local area network and will act as the FRP client - side (client - end).

- Download the FRP program. You can get it from the official GitHub repository.

### 2. Configure the FRP Client

#### 2.1 Download and Extract

Download the appropriate version of FRP for your server's operating system. For example, if it's a Linux server, you can use the following commands:

```bash
wget https://github.com/fatedier/frp/releases/download/v0.61.0/frp_0.61.0_linux_amd64.tar.gz
tar -zxvf frp_0.61.0_linux_amd64.tar.gz
cd frp_0.61.0_linux_amd64
```

#### 2.2 Edit the Configuration File

Open the `frpc.ini` file and configure it as follows:

```ini
serverAddr = "11.4.5.14"
serverPort = 1145
auth.token = "sodaiyo"
[[proxies]]
name = "ohovibe"
type = "tcp"
localIP = "192.168.69.1"
localPort = 81
remotePort = 81
```

- `serverPort`: The port on which the FRP service - side listens for connections from the client - side. The default is 7000.

- `remotePort`: The port on which the FRP service - side listens for connections from the pubic network.

- `token`: A secure token used for authentication between the client and the server. You should set a strong and unique token.

- `localIP`: The IP of the device.

- `localPort`: The Port for device web control.

#### 2.3 Start the FRP Service

Start the FRP service using the following command:

```bash
./frpc -c frpc.ini
```

If you want the service to run in the background in linux, you can use the `nohup` command:

```bash
nohup ./frpc -c frpc.ini &
```

Suppose the ESP32 has a web service running on port 80. On the device that wants to access the ESP32, we can enter the following URL in the browser:
```
ws://your_server_public_ip:remote_port
```
This way, the request will be forwarded through the FRP tunnel to the ESP32's local IP address and port, allowing us to access the ESP32's web service.

### Troubleshooting
- Program Fails: Please leave the COM log in the issues and properly describe the problem.    
- Connection Failure: If the ESP32 fails to connect to the FRP server - end, check the following:
    - Ensure that the public IP address of the server - end is correct.
    - Check if the `bind_port` on the server - end is open and not blocked by the firewall.
    - Verify that the token on the client and server sides is the same.
- Slow Connection: If the connection is slow, consider adjusting the `token` authentication method or increasing the network bandwidth of the server - end.
