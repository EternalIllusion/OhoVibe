/*
 * 用来控制跳弹的网页。
 */
#pragma once
const char *HTML_CHANNEL = R"=====(
<!DOCTYPE html>
<html lang="zh">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>通道控制</title>
    <style>
        body {
            font-family: Arial, sans-serif;
        }
        .container {
            margin: 20px;
        }
        .button-group {
            margin: 10px 0;
        }
        .slider {
            width: 200px;
        }
        .button {
            margin: auto;
            padding: 5px;
            font-size: 14px;
            background-color: #f0f0f0;
            border: 1px solid #ccc;
            cursor: pointer;
        }
        .btmgroup{
            width: 100%;
            display: -webkit-flex; /* Safari */
            display: flex;
            justify-content: space-around;
        }
        .button.active {
            background-color: #4caf50;
            color: white;
        }
        .button:disabled {
            background-color: #ddd;
            cursor: not-allowed;
        }
        .log-box {
            margin-top: 20px;
            height: 150px;
            border: 1px solid #ccc;
            padding: 10px;
            overflow-y: auto;
            font-size: 12px;
            background-color: #f5f5f5;
        }
    </style>
</head>
<body>

<div class="container">
    <h2>通道控制</h2>

    <!-- 开关按钮和滑条 -->
    <div id="buttonGroup" class="button-group">
        <div class="btmgroup">
            <input type="range" id="slider" class="slider" min="0" max="100" value="0" list="tickmarks" onchange="slidervchg();" />
<datalist id="tickmarks">
  <option value="0"></option>
  <option value="25"></option>
  <option value="50"></option>
  <option value="75"></option>
  <option value="100"></option>
</datalist>
            <span id="sliderv">0%</span>
            <button class="button" onclick="setSliderValue(0)">关</button>
            <button class="button" onclick="setSliderValue(100)">满</button>
        </div>
        <hr />
        <div id="buttons">
            <!-- 11个按钮 -->
            <div class="btmgroup">
            <button id="button1" class="button" onclick="toggleButtonValue(1)">1: 0%</button>
            <button id="button2" class="button" onclick="toggleButtonValue(2)">2: 0%</button>
            <button id="button3" class="button" onclick="toggleButtonValue(3)">3: 0%</button>
            <button id="button4" class="button" onclick="toggleButtonValue(4)">4: 0%</button>
            </div>
            <hr />
            <div class="btmgroup">
            <button id="button5" class="button" onclick="toggleButtonValue(5)">5: 0%</button>
            <button id="button6" class="button" onclick="toggleButtonValue(6)">6: 0%</button>
            <button id="button7" class="button" onclick="toggleButtonValue(7)">7: 0%</button>
            </div>
            <hr />
            <div class="btmgroup">
            <button id="button8" class="button" onclick="toggleButtonValue(8)">8: 0%</button>
            <button id="button9" class="button" onclick="toggleButtonValue(9)">9: 0%</button>
            <button id="button10" class="button" onclick="toggleButtonValue(10)">10: 0%</button>
            <button id="button11" class="button" onclick="toggleButtonValue(11)">11: 0%</button>
            </div>
        </div>
    </div>
<hr />
<hr />
    <!-- 全满/全关/全设置按钮 -->
    <div class="button-group">
        <button class="button" onclick="setAllValues(100)">全满</button>
        <button class="button" onclick="setAllValues(0)">全关</button>
        <button class="button" onclick="setAllValues(slider.value)">全设置</button>
    </div>
<hr />
<a href="/swipe">切换触摸板控制</a>
<hr />
    <!-- WS连接控制 -->
    <div class="button-group">
        <button class="button" onclick="toggleWSConnection()">连接/断开 WS</button>
        <p>WS 状态: <span id="wsStatus">未连接</span></p>
    </div>
    
    <hr />
    <p>&copy;EterIll & MyZhaZha, 2025.Partial rights reserved.</p>
    
    
    <!-- 日志框,记得删 -->
    <hr />
    <h4>日志,记得隐藏</h4>
    <div class="log-box" id="logBox"></div>
</div>

<script>
    let ws;
    let wsConnected = false;

    // 设置滑条数值
    function setSliderValue(value) {
        document.getElementById('slider').value = value;
        slidervchg();
    }

    // 同步滑条到按钮数值
    function slidervchg() {
        const sliderValue = parseInt(document.getElementById('slider').value);
        document.getElementById('sliderv').innerHTML = `${sliderValue}%`;
    }

    // 更新按钮的数值
    function updateButtonValues(sliderValue) {
        const buttons = document.querySelectorAll('#buttons button');
        buttons.forEach(button => {
            const buttonValue = parseInt(button.innerText.split(":")[1]);
            if (buttonValue !== sliderValue) {
                button.innerText = button.innerText.split(":")[0] + `: ${sliderValue}%`;
                button.classList.remove('active');
            }
        });
    }

    // 切换按钮数值
    function toggleButtonValue(id) {
        const button = document.querySelector(`#button${id}`);
        let currentValue = parseInt(button.innerText.split(":")[1]);
        const sliderValue = parseInt(document.getElementById('slider').value);

        if (currentValue === sliderValue) {
            currentValue = 0;
            button.classList.remove('active');
        } else {
            currentValue = sliderValue;
            button.classList.add('active');
        }

        button.innerText = `${id}: ${currentValue}%`;
        sendWSMessage(id, currentValue);
    }

    // 设置所有按钮的值
    function setAllValues(value) {
        const buttons = document.querySelectorAll('#buttons button');
        buttons.forEach(button => {
            const id = parseInt(button.id.replace('button', ''));
            button.innerText = `${id}: ${value}%`;
            button.classList.toggle('active', value >= 1);
            
        });
        sendWSMessage(12, value);
    }

    // 发送WS消息
    function sendWSMessage(id, value) {
        const wsValue = Math.round((value / 100) * 9999);  // 将0-100%映射到0-9999
        const formattedValue = wsValue.toString().padStart(4, '0');
        const message = `${id - 1}${formattedValue}`;
        logMessage(`发送命令:${id}-${value}(${message})`);
        
        if (wsConnected && ws.readyState === WebSocket.OPEN) {
            ws.send(message);
        }else{
            logMessage(`未连接，发送失败`);
        }
    }

    // 连接或断开WS
    function toggleWSConnection() {
        if (wsConnected) {
            ws.close();
            wsConnected = false;
            document.getElementById('wsStatus').innerText = '已断开';
        } else {
            ws = new WebSocket(`ws://${window.location.hostname}:81`);
            ws.onopen = () => {
                wsConnected = true;
                document.getElementById('wsStatus').innerText = '已连接';
                logMessage("WS 连接成功");
            };
            ws.onclose = () => {
                wsConnected = false;
                document.getElementById('wsStatus').innerText = '已断开';
                logMessage("WS 连接断开");
            };
            ws.onerror = () => {
                logMessage("WS 连接错误");
            };
        }
    }

    // 打印日志
    function logMessage(message) {
        const logBox = document.getElementById('logBox');
        logBox.innerHTML += message + '<br>';
        logBox.scrollTop = logBox.scrollHeight;
    }
</script>

</body>
</html>

)=====";

const char *HTML_SWIPE = R"=====(
<!DOCTYPE html>
<html lang="zh">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>轮盘控制</title>
    <h4>注：需要触摸屏</h4>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
        }
        #wheel-container {
            position: relative;
            width: 80vw;
            height: 80vw;
            margin: 50px auto;
            border-radius: 50%;
            border: 2px solid #000;
            /*background: conic-gradient(
                #ff0000 0deg 32.73deg,
                #ff7f00 32.73deg 65.46deg,
                #ffff00 65.46deg 98.18deg,
                #00ff00 98.18deg 130.91deg,
                #0000ff 130.91deg 163.64deg,
                #4b0082 163.64deg 196.36deg,
                #9400d3 196.36deg 229.09deg,
                #ff1493 229.09deg 261.82deg,
                #ff4500 261.82deg 294.55deg,
                #2e8b57 294.55deg 327.27deg,
                #d2691e 327.27deg 360deg
            );*/
            background-color: #ccc;
            margin-bottom: 30px;
        }
        #slider-container {
            width: 100%;
            height: 30px;
            margin: 0 auto;
            background-color: #ccc;
            position: relative;
            border-radius: 15px;
        }
        #slider-bar {
            position: absolute;
            width: 20px;
            height: 30px;
            background-color: #000;
            border-radius: 50%;
            cursor: pointer;
        }
        #ws-status {
            margin: 20px 0;
        }
        #log {
            margin-top: 20px;
            height: 200px;
            overflow-y: scroll;
            border: 1px solid #ccc;
            padding: 10px;
            font-size: 12px;
            background-color: #f9f9f9;
            text-align: left;
        }
        #control-btn {
            margin: 10px;
            padding: 10px 20px;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <h3>触摸板控制</h3>

    <!-- 轮盘 -->
    <div id="wheel-container"></div>
<hr />
<a href="/">切换通道控制</a>
<hr />

    <!-- WebSocket 状态显示 -->
    <div id="ws-status">设备状态: 未连接</div>

    <!-- WebSocket 控制按钮 -->
    <button id="control-btn">连接</button>
    <hr />
    <p>&copy;EterIll & MyZhaZha, 2025.Partial rights reserved.</p>
    
    <!-- 日志显示记得删 -->
    <div id="log"></div>

    <script>
        let socket = null;
        let wscdtime = 100;
        //如果延迟还是比较大就把上面这个改大一点
        let isConnected = false;
        let wsStatusElement = document.getElementById('ws-status');
        let logElement = document.getElementById('log');
        let controlBtn = document.getElementById('control-btn');
        let wheelContainer = document.getElementById('wheel-container');

        let isTouchingWheel = false;
        let isTouchingSlider = false;
        let isWSCooldown = false;

        // WebSocket连接和控制
        controlBtn.addEventListener('click', () => {
            if (isConnected) {
                socket.close();
                isConnected = false;
                wsStatusElement.textContent = '设备状态: 已断开';
                controlBtn.textContent = '连接';
            } else {
                socket = new WebSocket(`ws://${window.location.hostname}:81`);
                socket.onopen = () => {
                    isConnected = true;
                    wsStatusElement.textContent = '设备状态: 已连接';
                    controlBtn.textContent = '断开';
                    logToConsole('WebSocket 已连接');
                };
                socket.onclose = () => {
                    isConnected = false;
                    wsStatusElement.textContent = '设备状态: 已断开';
                    controlBtn.textContent = '连接';
                    logToConsole('WebSocket 已断开');
                };
                socket.onerror = (error) => {
                    logToConsole('WebSocket 错误: ' + error);
                };
            }
        });

        // 记录日志
        function logToConsole(message) {
            let logMessage = document.createElement('div');
            logMessage.textContent = message;
            logElement.appendChild(logMessage);
            logElement.scrollTop = logElement.scrollHeight;
        }

        // 计算点击位置与中心的距离
        function getWheelValue(event) {
            let rect = wheelContainer.getBoundingClientRect();
            let centerX = rect.left + rect.width / 2;
            let centerY = rect.top + rect.height / 2;
            let touchX = event.clientX || event.changedTouches[0].clientX;
            let touchY = event.clientY || event.changedTouches[0].clientY;

            let dx = touchX - centerX;
            let dy = touchY - centerY;
            let distance = Math.sqrt(dx * dx + dy * dy);
            let radius = rect.width / 2;
            let percentage = Math.min(distance / radius, 1) * 100;
            let sector = Math.floor((Math.atan2(dy, dx) + Math.PI) / (2 * Math.PI) * 11);

            let value = Math.floor(percentage * 99.99); // 转换为0-9999的值
            return { sector, value };
        }

        // 监听轮盘点击和触摸事件
        function handleWheelTouchMove(event) {
            event.preventDefault();
            if (isTouchingWheel && !isWSCooldown) {
                let { sector, value } = getWheelValue(event);
                let command = sector.toString().padStart(2, '0') + Math.round(value).toString().padStart(4, '0');
                logToConsole(`发送指令: ${command}`);
                isWSCooldown = true;
                setTimeout(() => {isWSCooldown=false;},wscdtime);

                if (isConnected && socket.readyState === WebSocket.OPEN) {
                    socket.send(command);
                    
                } else {
                    logToConsole('未连接，发送失败');
                }
            }
        }


        // 轮盘触摸开始事件
        wheelContainer.addEventListener('touchstart', (event) => {
            isTouchingWheel = true;
            handleWheelTouchMove(event.touches[0]);
        });

        // 轮盘触摸移动事件
        wheelContainer.addEventListener('touchmove', handleWheelTouchMove);

        // 轮盘触摸结束事件
        wheelContainer.addEventListener('touchend', () => {
            if (isTouchingWheel) {
                isTouchingWheel = false;
                let command = '110000';  // 触摸结束时发送110000
                logToConsole(`发送指令: ${command}`);
                isWSCooldown = true;
                setTimeout(() => {isWSCooldown=false;},wscdtime);
                if (isConnected && socket.readyState === WebSocket.OPEN) {
                    socket.send(command);
                    isWSCooldown = true;
                    setTimeout(() => {socket.send(command);},50);
                } else {
                    logToConsole('未连接，发送失败');
                }
            }
        });

    </script>
</body>
</html>

)=====";
