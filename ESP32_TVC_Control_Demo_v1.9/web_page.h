#ifndef WEB_PAGE_H
#define WEB_PAGE_H

// HTML content for the webpage
String HTML = "<!DOCTYPE html>\
<html>\
<head>\
<style>\
  .container {\
    display: flex;\
    flex-direction: row;\
    align-items: flex-start; /* Align items at the top */\
  }\
  #plotCanvas {\
    border: 1px solid black;\
    margin-right: 20px;\
  }\
  .info-table {\
    border-collapse: collapse;\
    margin-top: 0; /* Remove default margin */\
    margin-right: 20px; /* Adjust margin between tables */\
  }\
  .info-table th, .info-table td {\
    border: 1px solid black;\
    padding: 8px;\
    vertical-align: top; /* Align content at the top */\
  }\
  .control-table {\
    border-collapse: collapse;\
    margin-top: 0; /* Remove default margin */\
  }\
  .control-table th, .control-table td {\
    border: 1px solid black;\
    padding: 8px;\
    vertical-align: top; /* Align content at the top */\
  }\
  .axis-titles {\
    display: flex;\
    justify-content: space-between;\
    margin-top: 10px;\
    position: absolute;\
    width: 100%;\
  }\
  .axis-title {\
    font-weight: bold;\
  }\
</style>\
<script>\
function toggleLed() {\
  var xhttp = new XMLHttpRequest();\
  xhttp.open(\"GET\", \"/toggle\", true);\
  xhttp.send();\
}\
function resetThumbstick() {\
  var xhttp = new XMLHttpRequest();\
  xhttp.open(\"GET\", \"/resetThumbstick\", true);\
  xhttp.send();\
}\
function CommandControl() {\
  var xhttp = new XMLHttpRequest();\
  xhttp.open(\"GET\", \"/CommandControl\", true);\
  xhttp.send();\
}\
function ZeroPointHome() {\
  var xhttp = new XMLHttpRequest();\
  xhttp.open(\"GET\", \"/ZeroPointHome\", true);\
  xhttp.send();\
}\
function updateTemperature() {\
  var xhttp = new XMLHttpRequest();\
  xhttp.onreadystatechange = function() {\
    if (this.readyState == 4 && this.status == 200) {\
      document.getElementById('temperature').innerHTML = this.responseText;\
    }\
  };\
  xhttp.open(\"GET\", \"/temperature\", true);\
  xhttp.send();\
}\
</script>\
</head>\
<body>\
<h1>ESP32 TVC Control GUI</h1>\
<div class=\"container\">\
  <table class=\"info-table\">\
    <tr>\
      <th colspan=\"2\">System Info</th>\
    </tr>\
    <tr>\
      <td>System Status:</td>\
      <td>Online</td>\
    </tr>\
    <tr>\
      <td>Version:</td>\
      <td>1.9</td>\
    </tr>\
    <tr>\
      <td>Temperature:</td>\
      <td id=\"temperature\">Loading...</td>\
    </tr>\
    <tr>\
      <td>Server X Value:</td>\
      <td id=\"serverXValue\">Loading...</td>\
    </tr>\
    <tr>\
      <td>Server Y Value:</td>\
      <td id=\"serverYValue\">Loading...</td>\
    </tr>\
    <!-- Key for the graph -->\
    <tr>\
      <td>Graph Key:</td>\
      <td>\
        Blue: TVC Thumbstick<br>\
        Green: TVC Command\
      </td>\
    </tr>\
    <tr>\
      <td colspan=\"2\" style=\"text-align: center;\"><strong>MPU6050 Data</strong></td>\
    </tr>\
    <tr>\
      <td>Accelerometer (ax, ay, az):</td>\
      <td id=\"mpuAccelData\">Loading...</td>\
    </tr>\
    <tr>\
      <td>Gyroscope (gx, gy, gz):</td>\
      <td id=\"mpuGyroData\">Loading...</td>\
    </tr>\
    <tr>\
      <td>Orientation (pitch, roll, yaw):</td>\
      <td id=\"mpuOrientData\">Loading...</td>\
    </tr>\
  </table>\
  <div style=\"position: relative;\">\
    <canvas id=\"plotCanvas\" width=\"400\" height=\"400\"></canvas>\
    <div class=\"axis-titles\">\
      <div class=\"axis-title\" style=\"position: absolute; left: 50%; bottom: -20px;\">X Axis</div>\
      <div class=\"axis-title\" style=\"position: absolute; left: -40px; top: 50%; transform: rotate(-90deg);\">Y Axis</div>\
    </div>\
  </div>\
  <table class=\"control-table\">\
    <tr>\
      <th colspan=\"4\">Control Switch States</th>\
    </tr>\
    <tr>\
      <td><button onclick=\"toggleLed()\">Toggle LED</button></td>\
      <td><button onclick=\"resetThumbstick()\">TVC Zero Lock</button></td>\
      <td><button onclick=\"CommandControl()\">TVC Command Switch</button></td>\
      <td><button onclick=\"ZeroPointHome()\">Zero Point Homing</button></td>\
    </tr>\
  </table>\
</div>\
<script>\
var canvas = document.getElementById('plotCanvas');\
var ctx = canvas.getContext('2d');\
var x1 = 0;\
var y1 = 0;\
var x2 = 0;\
var y2 = 0;\
var MPUpitch = 0;\
var MPUroll = 0;\
ctx.fillStyle = 'blue';\
function plot(x1, y1, x2, y2, x3, y3) {\
  ctx.clearRect(0, 0, canvas.width, canvas.height);\
  ctx.fillStyle = 'blue';\
  ctx.beginPath();\
  ctx.arc(x1, y1, 5, 0, 2 * Math.PI);\
  ctx.fill();\
  ctx.fillStyle = 'green';\
  ctx.beginPath();\
  ctx.arc(x2, y2, 5, 0, 2 * Math.PI);\
  ctx.fill();\
  ctx.beginPath();\
  ctx.fillStyle = 'red';\
  ctx.beginPath();\
  ctx.arc(x3, y3, 5, 0, 2 * Math.PI);\
  ctx.fill();\
  ctx.beginPath();\
  ctx.moveTo(0, canvas.height / 2);\
  ctx.lineTo(canvas.width, canvas.height / 2);\
  ctx.moveTo(canvas.width / 2, 0);\
  ctx.lineTo(canvas.width / 2, canvas.height);\
  ctx.strokeStyle = 'black';\
  ctx.stroke();\
  ctx.font = '10px Arial';\
  ctx.textAlign = 'center';\
  ctx.fillStyle = 'black';\
  for(var i = -10; i <= 10; i++) {\
    ctx.fillText(i, (i + 10) * (canvas.width / 20), canvas.height / 2 + 12);\
    ctx.fillText(-i, canvas.width / 2 - 12, (i + 10) * (canvas.height / 20) + 4);\
  }\
}\
function updatePlot(esp32_xValue, esp32_yValue, server_xValue, server_yValue, pitch, roll) {\
  var middleX = 0;\
  var middleY = 0;\
  var plotX = (((esp32_xValue/77) - middleX) / (2*10)) * canvas.width + canvas.width / 2;\
  var plotY = canvas.height - ((((esp32_yValue/77) - middleY) / (2*10)) * canvas.height + canvas.height / 2);\
  var server_plotX = ((server_xValue)/20) * canvas.width + canvas.width / 2;\
  var server_plotY = canvas.height - (((server_yValue)/20) * canvas.height + canvas.height / 2);\
  var pitchPlotX = ((pitch)/20) * canvas.width + canvas.width / 2;\
  var rollPlotY = canvas.height - (((roll)/20) * canvas.height + canvas.height / 2);\
  plot(plotX, plotY, server_plotX, server_plotY, pitchPlotX, rollPlotY);\
  document.getElementById('serverXValue').innerHTML = server_plotX.toFixed(2);\
  document.getElementById('serverYValue').innerHTML = server_plotY.toFixed(2);\
}\
setInterval(function() {\
  var xhttp = new XMLHttpRequest();\
  xhttp.onreadystatechange = function() {\
    if (this.readyState == 4 && this.status == 200) {\
      var data = this.responseText.split(',');\
       console.log(data); \
      var esp32_xValue = parseInt(data[0]);\
      var esp32_yValue = parseInt(data[1]);\
      var server_xValue = parseFloat(data[2]);\
      var server_yValue = parseFloat(data[3]);\
      var pitch = MPUpitch;\
      var roll = MPUroll;\
      updatePlot(esp32_xValue, esp32_yValue, server_xValue, server_yValue, pitch, roll);\
      console.log(pitch);\
    }\
  };\
  xhttp.open(\"GET\", \"/thumbstick\", true);\
  xhttp.send();\
}, 100);\
setInterval(function() {\
  var xhttp = new XMLHttpRequest();\
  xhttp.onreadystatechange = function() {\
    if (this.readyState == 4 && this.status == 200) {\
      var data = this.responseText.split(',');\
      console.log(data); \
      document.getElementById('mpuAccelData').innerHTML = 'ax: ' + data[0] + ', ay: ' + data[1] + ', az: ' + data[2];\
      document.getElementById('mpuGyroData').innerHTML = 'gx: ' + data[3] + ', gy: ' + data[4] + ', gz: ' + data[5];\
      document.getElementById('mpuOrientData').innerHTML = 'pitch: ' + data[6] + ', roll: ' + data[7] + ', yaw: ' + data[8];\
      MPUpitch = data[6];\
      MPUroll = data[7];\
    }\
  };\
    xhttp.open(\"GET\", \"/mpu6050\", true);\
  xhttp.send();\
}, 100);\
updateTemperature(); // Fetch and update temperature initially\
setInterval(updateTemperature, 2000); // Update temperature every 2 seconds\
</script>\
</body>\
</html>";

#endif
