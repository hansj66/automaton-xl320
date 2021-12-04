/*
*  Copyright [2020] [Hans Jørgen Grimstad]
*  
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*  
*      http://www.apache.org/licenses/LICENSE-2.0
*  
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
#include <Arduino.h>

String config_page = R"TEMPLATE(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    body {
      text-align: center;
      font-family: "Trebuchet MS", Arial;
      margin-left:auto;
      margin-right:auto;
      height: 100%;
      width: 100%;
    }
    .slider {
      width: 300px;
    }
  </style>
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
</head>
<body onload="FlashLoad()">
  <center>
        <h1>Automaton XL-320</h1>
        <h2>PID</h2>
        <a href="https://www.TimeExpander.com">Time Expander</a>
    </center>

    <hr>
    <p align="left">PID parameters and set point can be stored in and later be read back from non volatile flash partition.</p>
    <button onclick="FlashSave()">Save</button>
    <button onclick="FlashLoad()">Load</button>
    <hr>

    <table id="parametertable" style="width:100%">
      <tr>
        <th></th>
        <th>Adjust</th>
        <th>Value</th>
      </tr>
    </table>  
    <hr>
    <button onclick="GreetHumanOverlord()">Greet human overlord</button>
    <button onclick="Boogie()">Boogie</button>
  
  

  <script>
    function createControls() 
    {
      var table = document.getElementById("parametertable");
      var rowCount = table.rows.length;

      // Set point
      var row = table.insertRow(rowCount);
      row.insertCell(0).innerHTML= '<h3 align="left">Set</h3>';
      row.insertCell(1).innerHTML= '<input type="range" min="-100" max="100" class="slider" id="setPointSlider"/>';
      row.insertCell(2).innerHTML= '<p><span id="setPointSliderValue"></span></p>';
      // Derivative gain
      row = table.insertRow(rowCount);
      row.insertCell(0).innerHTML= '<h3 align="left">D</h3>';
      row.insertCell(1).innerHTML= '<input type="range" min="0" max="500" class="slider" id="derivativeGainSlider"/>';
      row.insertCell(2).innerHTML= '<p><span id="derivativeGainSliderValue"></span></p>';

      // Integral gain
      row = table.insertRow(rowCount);
      row.insertCell(0).innerHTML= '<h3 align="left">I</h3>';
      row.insertCell(1).innerHTML= '<input type="range" min="0" max="2000" class="slider" id="integralGainSlider"/>';
      row.insertCell(2).innerHTML= '<p><span id="integralGainSliderValue"></span></p>';

      // Proportional gain
      row = table.insertRow(rowCount);
      row.insertCell(0).innerHTML= '<h3 align="left">P</h3>';
      row.insertCell(1).innerHTML= '<input type="range" min="0" max="500" class="slider" id="proportionalGainSlider"/>';
      row.insertCell(2).innerHTML= '<p><span id="proportionalGainSliderValue"></span></p>';
    }

    function setupSliderListener(sliderName, sliderValueName, param, scale, offset)
    {
      var i = document.getElementById(sliderName);
      var o = document.getElementById(sliderValueName);
      o.innerHTML = i.value * scale + offset;
      i.addEventListener('input', function() {
        var num = i.value * scale + offset;
        num = num.toFixed(2);
        o.innerHTML = num;
        }, true);

        i.addEventListener('change', function() {
          $.get(param + i.value);    
          {Connection: close};
        }, true);
    }

    // Main code
    createControls();

    document.getElementById("derivativeGainSlider").value = 0;
    document.getElementById("integralGainSlider").value = 0;
    document.getElementById("proportionalGainSlider").value = 0;

    setupSliderListener('setPointSlider', 'setPointSliderValue', '/param?setpoint=', 0.04,90)
    setupSliderListener('proportionalGainSlider', 'proportionalGainSliderValue', '/param?P=', 1,0)
    setupSliderListener('integralGainSlider', 'integralGainSliderValue', '/param?I=', 1,0)
    setupSliderListener('derivativeGainSlider', 'derivativeGainSliderValue', '/param?D=', 0.05,0)


    $.ajaxSetup({timeout:1000});

 
    function FlashSave()
    {
      var setPoint = document.getElementById("setPointSlider").value;
      var P = document.getElementById("proportionalGainSlider").value;
      var I = document.getElementById("integralGainSlider").value;
      var D = document.getElementById("derivativeGainSlider").value;
      $.get("/save?setpoint=" + setPoint + "&P=" + P + "&I=" + I + "&D=" +D);
      {Connection: close};
    }

    function FlashLoad()
    {
      var pidParamName = "PIDParams";
      $.get("/load?param=" + pidParamName.value, function(data, status) 
      {
        var paramValue = data.split(",");
        document.getElementById("setPointSlider").value = paramValue[3];
        document.getElementById("proportionalGainSlider").value = paramValue[0];
        document.getElementById("integralGainSlider").value = paramValue[1];
        document.getElementById("derivativeGainSlider").value = paramValue[2];

        document.getElementById("setPointSliderValue").innerHTML = parseFloat(paramValue[3]) + 90.0;
        document.getElementById("proportionalGainSliderValue").innerHTML = paramValue[0];
        document.getElementById("integralGainSliderValue").innerHTML = paramValue[1];
        document.getElementById("derivativeGainSliderValue").innerHTML = paramValue[2];
      });
      {Connection: close};
    }

    function GreetHumanOverlord()
    {
      $.get("/greet");
      {Connection: close};
    }

    function Boogie()
    {
      $.get("/boogie");
      {Connection: close};
    }


    </script>
</body>
</html>

)TEMPLATE";

