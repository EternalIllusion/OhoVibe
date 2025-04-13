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
        * {
            transition: all 0.3s ease;
        }
        
        body {
            font-family: Arial, sans-serif;
            background-color: #EFF1F0;
        }
        
        .container {
            margin: 20px;
            background-color: #ffffff;
            border-radius: 30px;
            padding: 20px;
        }
        
        .button-group {
            margin: 10px 0;
        }
        
        .slider {
            width: 200px;
        }
        .sli-c {
            display: flex;
            justify-content:center;
            width:70%;
        }
        .sli-cc {
            width:70%;
        }
        
        #rndsw {
            display: inline-block;
            flex: none;
            width:100%;
        }
        
        input[type="range"] {
            -webkit-appearance: none;
            appearance: none;
            margin: auto;
            outline: 0;
            background-color: transparent;
            width: 200px;
            width: 100%;
        }
        input[type="range"]::-webkit-slider-runnable-track {
            height: 4px;
            background: #eee;
        }
        input[type="range"]::-webkit-slider-container {
            height: 20px;
            overflow: hidden;
        }
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background-color: #27A1DC;
            border: 1px solid transparent;
            margin-top: -8px;
            border-image: linear-gradient(to right,#00D2FF,#27A1DC) 0 fill / 8 20 8 0 / 0px 0px 0 200px;
        }
        .btmgroup{
            width: 100%;
            display: -webkit-flex;
            display: flex;
            justify-content: center;
        }
        
        .btngroup{
            width: 100%;
            display: -webkit-flex;
            display: flex;
            justify-content: space-between;
        }
        
        input[type="text"] {
          background-color: #ffffff;
          border: none;
          border-bottom: 2px solid #4a90e2;
          padding: 8px 12px;
          font-size: 16px;
          transition: border-color 0.3s ease, box-shadow 0.3s ease;
          width: 100%;
        }
        
        input[type="text"]:focus {
          border-bottom: 2px solid #66ccff;
          outline: none;
        }
        
        input[type="text"]:hover {
          border-bottom: 2px solid #66ccff;
        }
        
        button {
            margin: auto;
            padding: 5px;
            font-size: 14px;
            background-color: #EDEDEC;
            color: #EDEDEC;
            border: 2px solid #B2B2B2;
            border-radius: 7px;
            cursor: pointer;
            transition: all 0.3s ease;
                
        }
            
        button.active {
            border: 2px solid #357EC2;
            outline: none;
            background-color: #27A1DC;
            color: #27A1DC;
        }
        
        button:active {
            background-color: #66ccff;
            color: #66ccff;
            border: 2px solid #ffffff; 
            box-shadow: 0 1px 3px rgba(0, 0, 0, 0.3);
        }
        .mode {
            width: 30%;
            border: none;
            border-radius: 0;
            background-color: #fff;
            border-top: #66ccff 1px solid;
            color: #000;
            margin: auto;
        }
        .mode.active {
            border: none;
            border-radius: 0;
            background-color: #B4D6FF;
            border-top: #27A1DC 2px solid;
            color: #000;
        }
        .menu {
            border: none;
            background-color: #fff;
            border-bottom: #66ccff 1px solid;
            border-top: #ddd 1px solid;
            color: #000;
            margin: auto;
        }
        .menu:active,.menu:hover {
            border: none;
            border-bottom: #27A1DC 2px solid;
            color: #000;
        }
        .copy {
            color: #000;
            margin: auto;
        }
        .button {
            width: 18vw;
            height: 18vw;
            margin: 2vw;
            opacity: 0.5;
        }
        #svc {
            border: none;
            background-color:inherit;
            box-shadow: none;
            padding: 8px 0;
            color: #000;
        }
        #src {
            border: none;
            background-color:inherit;
            box-shadow: none;
            padding: 8px 0;
            color: #000;
        }
        
        .log-box {
            margin-top: 0px;
            height: 150px;
            border: 1px solid #ccc;
            border-radius: 3px;
            padding: 10px;
            overflow-y: auto;
            font-size: 12px;
            background-color: #f5f5f5;
            
        }
        #wheel-container {
            position: relative;
            width: 70vw;
            height: 70vw;
            margin: 50px auto;
            border-radius: 50%;
            border: 2px solid #000;
            background-color: #eff1f0;
            margin-bottom: 30px
        }
        .ctrlgrp{
            height:auto;
            opacity:1;
            overflow:hidden;
        }
        .hidd {
            height: 0;
            opacity: 0; 
            padding: 0;
            margin: 0;
        }
        .hidden {
            display: none;
        }
        .log-title {
            display: flex;
            justify-content: space-between;
            width: 100%;
            padding: 0;
            margin: 0;
        }
        .log-title * {
            margin-top: 0;
            margin-bottom: 0;
        }
        .log-title h4 {
            margin: auto;
            margin-left: 0;
        }
        #logger {
            margin-right: 0;
        }
        .random-section {
            overflow: hidden;
        }
        
    </style>
</head>

<body>
    <div class="container">

        <h2>通道控制</h2>
        <!-- 开关按钮和滑条 -->
        <div id="buttonGroup" class="button-group">
            <div id="swiper" class="ctrlgrp hidd">
                <div id="wheel-container"></div>
                <div style="height:5vw;"></div>
            </div>
            <div id="normal" class="ctrlgrp">
                <div class="btmgroup">
                    <div class="sli-c">
                    <input type="range" id="slider" class="slider" min="0" max="100" value="0" oninput="sliderpchg();" onchange="slidervchg();" />
                    </div>
                    <button id="svc" onclick="svc();"><span id="sliderv">00%</span></button>
                    <button class="menu" onclick="setSliderValue(0)">○</button>
                    <button class="menu" onclick="setSliderValue(100)">●</button>
                </div>
                

                <hr />

                <div id="buttons">
                    <!-- 9个按钮 -->
                                        <div class="btmgroup">
                        <button id="button1" class="button" onclick="toggleButtonValue(1)">0: 0%</button>
                        <button id="button2" class="button" onclick="toggleButtonValue(2)">0: 0%</button>
                        <button id="button3" class="button" onclick="toggleButtonValue(3)">0: 0%</button>
                    </div>
                    <div class="btmgroup">
                        <button id="button4" class="button" onclick="toggleButtonValue(4)">0: 0%</button>

                        <button id="button5" class="button" onclick="toggleButtonValue(5)">0: 0%</button>
                        <button id="button6" class="button" onclick="toggleButtonValue(6)">0: 0%</button>
                    </div>
                    <div class="btmgroup">
                        <button id="button7" class="button" onclick="toggleButtonValue(7)">0: 0%</button>
                        <button id="button8" class="button" onclick="toggleButtonValue(8)">0: 0%</button>
                        <button id="button9" class="button" onclick="toggleButtonValue(9)">0: 0%</button>
                    </div>
                </div>
            </div>
        <div class="random-section btmgroup hidd" id="rndss">
            <div class="sli-cc"><ui-range min="50" max="5000" value="500,1000" multiple="" id="rndsw"></ui-range></div>
            <button id="src"><span id="rndv">0.50-1.00s</span></button>
        </div>
        </div>
        <!-- 全满/全关/全设置按钮 -->
        <div class="btngroup">
            <button class="mode" id="touching" onclick="updatemode(3);">触摸模式</button>
            <button class="mode" id="random" onclick="updatemode(1);">随机模式</button>
            <button class="mode" id="powerful" onclick="updatemode(2);">强力模式</button>
        </div>
        <a id="lnk" style="display:none;"></a>
        <hr />
        <!-- WS连接控制 -->
        <div>
<div class="btmgroup">
<p id="wsst">设备连接状态: <span id="wsStatus">未连接</span></p>
                <button class="menu r" onclick="toggleWSConnection()">✦</button>
                <button class="menu r" onclick="shareurl()">⌘</button>
            </div>
        </div>
        <hr />
        <div class="log-title">
        <h4>日志</h4>
        <button class="menu" id="logger" onclick="togglelog()">▼</button>
        </div>
        <div class="log-box hidd" id="logBox"></div>
    </div>
    <script>
        
        /* 
         * OhoVibe Controller
         * by EterIll
         * All rights of code reserved.
         * LICENCE CC.BY.SA
         * Do not steal this to csdn!
         */
        
        // 模式控制器
        let mode=0;
        /* 0=normal
        1=random
        2=powerful
        3=touch
        */
        
        // 切换模式
        function updatemode(modec){
            modec=parseInt(modec);
            let mran = document.getElementById('random');
            let mpow = document.getElementById('powerful');
            let mtch = document.getElementById('touching');
            let cnorm = document.getElementById('normal');
            let cswip = document.getElementById('swiper');
            let rndss = document.getElementById('rndss');
            if(modec < 4){
                if(modec===mode){//退出模式
                    if(modec===2){
                        updateButtonValues(0);
                    }
                    mode=0;
                }else{
                    mode=modec;
                }
                if(mode === 1){
                    randomToggle();
                }
                if(mode === 2){
                    updateButtonValues(parseInt(document.getElementById('slider').value));
                }
            }
            mran.classList.toggle('active', mode === 1);
            mpow.classList.toggle('active', mode === 2);
            mtch.classList.toggle('active', mode === 3);
            cswip.classList.toggle('hidd', mode !== 3);
            cnorm.classList.toggle('hidd', mode === 3);
            rndss.classList.toggle('hidd', mode !== 1);
            
        }
        	
		let isTouchingWheel = false;
		let isWSCooldown = false;
		let rndcda = 500;
		let rndcdb = 1000;
		// 触摸板发送间隔
		let wscdtime = 100;
		// 如果控制延迟还是比较大就把上面这个改大一点
		
		// WebSockets实例
		let ws;
		
		// 自动获取URL参数
		let wsConnected = false;
		window.onload = function(){
		    document.getElementById('rndsw').addEventListener('input', srcvchg);
		    toggleWSConnection();
		    }
		    
		// 切换日志显示
		function togglelog(){
		    document.getElementById('logBox').classList.toggle('hidd');
		}
		
		// 分享控制权
		function shareurl(){
		    let sip = `${window.location.hostname}`;
		    let spp = 81;
		    if(!spp){spp=81;}
		    if(confirm(`请将以下内容复制后粘贴给你想要分享控制权的对象（需要在同一网络下）：
https://eterill.xyz/OhoVibe/?ip=${sip}&port=${spp}
点击确定一键复制`)){
		        copyToClip(`https://eterill.xyz/OhoVibe/?ip=${sip}&port=${spp}`);
		    }
		}
		
		// 随机模式
		function randomToggle() {
            if (mode !== 1) {
                return;
            }
            const randomId = Math.floor(Math.random() * 9) + 1;
            toggleButtonValue(randomId,document.getElementById('slider').value );
            // 随机间隔
            const delay = Math.floor(Math.random() * (rndcdb - rndcda + 1)) + rndcda;
            setTimeout(() => {
                if (mode !== 1) {
                    return;
                }
                toggleButtonValue(randomId, 0);
                randomToggle();
            }, delay);
        }
        
		// 设置滑条数值
		function setSliderValue(value) {
		    document.getElementById('slider').value = value;
		    slidervchg();
		}
		
		// 设置确切百分比
		function svc(){
		    let value = prompt('请输入数字：',document.getElementById('slider').value.toString());
		    value = parseInt(value);
		    if(isNaN(value)){return;}
		    if(value>100){value = 100;}
		    if(value<0){value=0;}
		    setSliderValue(value);
		}
		
		// 设置随机模式范围
		function srcvchg() {
		    let dom = document.getElementById('rndsw');
		    let domv = document.getElementById('rndv');
		    let val = dom.value.split(/,\s*|\s+/);
		    let intvala = parseInt(val[0]);
		    let intvalb = parseInt(val[1]);
		    rndcda = intvala;
		    rndcdb = intvalb;
		    intvala = (intvala/1000).toFixed(2);
		    intvalb = (intvalb/1000).toFixed(2);
		    domv.innerHTML = `${intvala}-${intvalb}s`;
		}
		
		
		
		// 同步滑条数值
		function slidervchg() {
		    const sliderValue = parseInt(document.getElementById('slider').value);
		    document.getElementById('sliderv').innerHTML = `${sliderValue.toString().padStart(2,0)}%`;
		    if(mode ===2) {
		        updateButtonValues(sliderValue)
		    }
		}
		
		// 同步滑条预览数值
		function sliderpchg() {
		    document.getElementById('sliderv').innerHTML = `${parseInt(document.getElementById('slider').value).toString().padStart(2,0)}%`;
		}
		
		// 更新按钮的数值
		function updateButtonValues(sliderValue) {
		    const buttons = document.querySelectorAll('#buttons button');
		    let actives = parseInt(sliderValue) >= 1;
		    buttons.forEach(button => {
		        const buttonValue = parseInt(button.innerText.split(":")[1]);
		        if (buttonValue !== sliderValue) {
		            button.innerText = button.innerText.split(":")[0] + `: ${sliderValue}%`;
		            button.classList.toggle('active', actives);
		            
		        }
		        button.style.opacity = ((sliderValue/100)*0.5+0.5).toString();
		    });
		    sendWSMessage(10, sliderValue);
		}
		
		// 切换按钮数值
		function toggleButtonValue(id,val) {
		    const button = document.querySelector(`#button${id}`);
		    let currentValue = parseInt(button.innerText.split(":")[1]);
		    let sliderValue = parseInt(document.getElementById('slider').value);
            if (val !== undefined && val !== null) {
                sliderValue = val;
            }
		
		    if (currentValue === sliderValue) {
		        currentValue = 0;
		    } else {
		        currentValue = sliderValue;
		    }
		
		    if (currentValue < 1) {
		        button.classList.remove('active');
		    } else {
		        button.classList.add('active');
		    }
		
		    button.innerText = `0: ${currentValue}%`;
		    button.style.opacity = ((currentValue/100)*0.5+0.5).toString();
		    sendWSMessage(id, currentValue);
		}
		
		// 复制内容到粘贴板
		function copyToClip(content, message) {
		    var aux = document.createElement("input"); 
		    aux.setAttribute("value", content); 
		    document.body.appendChild(aux); 
		    aux.select();
		    document.execCommand("copy"); 
		    document.body.removeChild(aux);
		    if (message == null) {
		        alert("复制成功");
		    } else{
		        alert(message);
		    }
		}
		
		// 发送WS消息
		function sendWSMessage(id, value) {
		    const wsValue = Math.round((value / 100) * 9999);  // 将0-100%映射到0-9999
		    const formattedValue = wsValue.toString().padStart(4, '0');
		    let iid = id -1;
		    iid = iid.toString().padStart(2, '0');
		    const message = `${iid}${formattedValue}`;
		    logMessage(`发送命令(${message}).`);
		    //logMessage(`发送:${parseInt(id)===10?"全部":id}-${value}`);
		    
		    if (wsConnected && ws.readyState === WebSocket.OPEN) {
		    logMessage(`发送成功:${parseInt(id)===10?"全部":id}-${value}`);
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
		        document.getElementById('wsStatus').innerText = '连接中';
		        logMessage("发起连接......");
		        ws = new WebSocket("ws://"+`${window.location.hostname}`+":81");
		        ws.onopen = () => {
		            wsConnected = true;
		            
		            logMessage("连接成功");
		        };
		        ws.onclose = () => {
		            wsConnected = false;
		            document.getElementById('wsStatus').innerText = '已断开';
		            logMessage("连接断开");
		        };
		        ws.onerror = () => {
		            logMessage("连接错误");
		        };
		    }
		}
		
		// 打印日志
		function logMessage(message) {
		    const logBox = document.getElementById('logBox');
		    logBox.innerHTML += message + '<br>';
		    logBox.scrollTop = logBox.scrollHeight;
		}
		
		let wheelContainer = document.getElementById('wheel-container');
		
		// 轮盘位置转换分区信号
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
		    let sector = Math.floor((Math.atan2(dy, dx) + Math.PI) / (2 * Math.PI) * 9);
		    let value = Math.floor(percentage); // 转换为0-9999的值
		    return { sector, value };
		}
		
		// 监听轮盘点击和触摸事件
		function handleWheelTouchMove(event) {
		    event.preventDefault();
		    if (isTouchingWheel && !isWSCooldown) {
		        let { sector, value } = getWheelValue(event);
		        sendWSMessage(sector+1, value);
		        isWSCooldown = true;
		        setTimeout(() => {isWSCooldown=false;},wscdtime);
		
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
		        sendWSMessage(10,0);
		        sendWSMessage(10,0);
		        isWSCooldown = true;
		        setTimeout(() => {isWSCooldown=false;},wscdtime);
		        
		    }
		});		
		
		/*
		 * Range selector widget
		 * by Zhangxinxu | modified by EterIll
		 * https://www.zhangxinxu.com/study/202102/two-range-input-demo.php
		 */
		
		class uiRange extends HTMLElement {
            constructor () {
                super();
            }
            static get style () {
                return `<style>
        :host{--ui-range-track-hegiht:4px;--ui-range-thumb-size:16px;--ui-gray:#eee;--ui-blue:#00D2FF;display:inline-block;position:relative}:host([multiple]){pointer-events:none}[type="range"]{display:block;-webkit-appearance:none;appearance:none;margin:0;outline:0;background:none;width:-webkit-fill-available;width:fill-available;width:fill}[type="range"] + [type="range"]{position:absolute;left:0;top:0;bottom:0;margin:auto}[type="range"]::-webkit-slider-runnable-track{display:flex;align-items:center;height:var(--ui-range-track-hegiht);border-radius:var(--ui-range-track-hegiht);background:linear-gradient(to right,var(--ui-gray) calc(1% * var(--from,0)),var(--ui-blue) calc(1% * var(--from,0)) calc(1% * var(--to,100)),var(--ui-gray) 0%)}[type="range"]::-webkit-slider-thumb{-webkit-appearance:none;appearance:none;pointer-events:auto;width:var(--ui-range-thumb-size);height:var(--ui-range-thumb-size);border-radius:50%;background-color:#fff;box-shadow:0 1px 3px 1px rgba(0,0,0,.25);transition:border-color .15s,background-color .15s;cursor:pointer;margin-top:calc((var(--ui-range-thumb-size) - var(--ui-range-track-hegiht)) * -0.5)}[type="range"]::-webkit-slider-thumb:active{background-color:var(--ui-light,#f7f9fa);box-shadow:0 0 1px 1px rgba(0,0,0,.25)}[type="range"] + [type="range"]::-webkit-slider-runnable-track{background:none}[type="range"]{width:-moz-available}input[type=range]::-moz-range-track{display:flex;align-items:center;height:var(--ui-range-track-hegiht);border-radius:var(--ui-range-track-hegiht);background:linear-gradient(to right,var(--ui-gray) calc(1% * var(--from,0)),var(--ui-blue) calc(1% * var(--from,0)) calc(1% * var(--to,100)),var(--ui-gray) 0%)}input[type=range]::-moz-range-thumb{-webkit-appearance:none;appearance:none;pointer-events:auto;width:var(--ui-range-thumb-size);height:var(--ui-range-thumb-size);border-radius:50%;background-color:#fff;box-shadow:0 1px 3px 1px rgba(0,0,0,.25);transition:border-color .15s,background-color .15s;cursor:pointer;margin-top:calc((var(--ui-range-thumb-size) - var(--ui-range-track-hegiht)) * -0.5)}[type="range"]::-moz-range-thumb:active{background-color:var(--ui-light,#f7f9fa);box-shadow:0 0 1px 1px rgba(0,0,0,.25)}[type="range"] + [type="range"]::-moz-range-track{background:none}
        </style>`;
            }
            static get observedAttributes () {
                return ['max', 'min', 'step', 'value'];
            }
            get value () {
                return this.getAttribute('value');
            }
            set value (val) {
                this.setAttribute('value', val);
            }
            get min () {
                return this.getAttribute('min') || '0';
            }
            set min (val) {
                this.setAttribute('min', val);
            }
            get max () {
                return this.getAttribute('max') || '100';
            }
            set max (val) {
                this.setAttribute('max', val);
            }
            get step () {
                return this.getAttribute('step') || '1';
            }
            set step (val) {
                this.setAttribute('step', val);
            }
        
            get multiple () {
                return this.hasAttribute('multiple');
            }
            set multiple (val) {
                this.toggleAttribute('multiple', val);
            }
        
            connectedCallback () {
                this.create();
            }
            attributeChangedCallback (name, oldValue, newValue) {
                // 略，非重点
                this.render();
            }
            create () {
                // Shadow DOM元素
                let shadow = this.attachShadow({
                    mode: 'open'
                });
        
                // 样式
                shadow.innerHTML = uiRange.style;
        
                // 赋值处理
                let value = this.value || '';
                let arrValue = value.split(/,\s*|\s+/);
        
                if (this.multiple && arrValue.length === 1) {
                    arrValue[1] = arrValue[0];
                }
                arrValue.forEach((val, index) => {
                    let range = document.createElement('input');
                    range.type = 'range';
                    // 默认属性
                    ['max', 'min', 'step'].forEach(attr => {
                        if (this.hasAttribute(attr)) {
                            range[attr] = this[attr];
                        }
                    });
                    // 赋值处理
                    if (val) {
                        range.value = val;
                    }
        
                    // 事件处理
                    range.addEventListener('input', (event) => {
                        this.dispatchEvent(new CustomEvent('input'));
                    });
        
                    // 放在Shadow DOM中
                    shadow.append(range);
                    // 暴露给自定义元素
                    this['range' + index] = range;
                });
                
                // 事件
                this.events();
            }
        
            events () {
                this.addEventListener('input', _ => {
                    // value值变化
                    let value0 = this.range0.value;
                    this.value = value0;
                    if (this.multiple) {
                        let value1 = this.range1.value;
                        this.value = [value0, value1].map(val => {
                            return Number(val);
                        }).sort(function (a, b) {
                            return a - b;
                        }).join();
                    }
                });
            }
        
            render () {
                let value = this.value;
                let arrValue = value.split(/,\s*|\s+/);
                // 百分比值确定
                let min = this.min;
                let max = this.max;
                let distance = max - min;
                let from = 0;
                let to = 100;
        
                if (this.multiple) {
                    from = 100 * (arrValue[0] - min) / distance;            
                }
                to = 100 * ((arrValue[1] || arrValue[0]) - min) / distance;
        
                this.style.setProperty('--from', from);
                this.style.setProperty('--to', to);
            }
        }
        if (!customElements.get('ui-range')) {
            customElements.define('ui-range', uiRange);
        }
    </script>
    <p style="font-size: 8px;" align="center">&copy;<a href="eterill.xyz" style="color:inherit;text-decoration:none;">EterIll</a> & MyZhaZha, 2025.Partial rights reserved.</p>
</body>

</html>
)=====";