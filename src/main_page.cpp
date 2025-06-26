#include <pgmspace.h>


const char* index_html = R"rawliteral(
<!DOCTYPE html>
  <html>


    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="stylesheet" href="/style.css">
      <link rel="icon" href="http://iw1cgw.altervista.org/favicon.ico" type="image/x-icon">
    </head>


    <script>
      window.onload = function() {
        fetch("/dash_value", { method: "POST" })
        .then(response => response.json())
        .then(data => {
          document.getElementById("igate_toggle").checked = (data.igate === "ok");
          document.getElementById("digi_toggle").checked = (data.digi === "ok");
          document.getElementById("meteo_toggle").checked = (data.meteo === "ok");
          document.getElementById("igate").value = data.igate;
          document.getElementById("digi").value = data.digi;
          document.getElementById("meteo").value = data.meteo;
          document.getElementById("meteo_mode").value = data.meteo_mode;
          document.getElementById("BM_sensor_status").value = data.BM_sensor_status;  
          document.getElementById("fw").value = "release: " + data.fw;
          document.getElementById("beacons_switch").value = data.beacons_switch;
          document.getElementById("call").textContent = data.call + " - LoRa system";
          document.getElementById("aprs_view_call").value = data.aprs_view_call;
          document.getElementById("INA226_status").value = data.INA226_status;
          document.getElementById("battery_view").textContent = data.battery_view;
          const input1 = document.getElementById("voltage_dashboard"); 
          input1.style.display = (data.battery_view === "ok") ? "block" : "none";
          const input2 = document.getElementById("aux_voltage"); 
          input2.style.display = (data.INA226_status === "ok") ? "block" : "none";
          const input3 = document.getElementById("current_server_aprsis"); 
          input3.style.display = (data.igate === "ok" || data.meteo_mode === "ip" && data.meteo === "ok" ) ? "block" : "none";
          const input4 = document.getElementById("view_Zambretti"); 
          input4.style.display = ( data.Zambretti === "ok" ) ? "block" : "none";
          const fieldset = document.getElementById("fieldsetMeteoStatus");
          fieldset.style.display = (data.BM_sensor_status === "ok") ? "block" : "none";
          const beaconStatus = data.beacons_switch; // "ok" o "ko"
          const button = document.getElementById("beaconButton");
          button.disabled = (data.beacons_switch.trim() !== "ok");
          button.disabled = (data.meteo === "ko" && data.igate === "ko" && data.digi === "ko" || data.beacons_switch === "ko");
          if (data.BM_sensor_status === "ko") {
            meteo_toggle.disabled = true;
          } else {
            meteo_toggle.disabled = false;
          }
          if (data.meteo === "ko" || data.BM_sensor_status === "ko") {
            meteo_mode.disabled = true;
          } else {
            meteo_mode.disabled = false;
          }
          const label = document.getElementById("stationStatus");
          if (data.BM_sensor_status && data.BM_sensor_status.trim().toLowerCase() === "ok") {
            label.textContent = "Meteostation online";
            label.className = "status-label online";
          } else {
            label.textContent = "Meteostation offline";
            label.className = "status-label offline";
          }
        })
        .catch(error => {
          console.error("Errore fetch:", error);
          const label = document.getElementById("stationStatus");
          label.textContent = "Meteostation offline";
          label.className = "status-label offline";
        });
      }
    </script>


    <!-- CAMPO NASCOSTO COLLEGATO -->
    <input type="hidden" id="beacons_switch" name="beacons_switch">
    <input type="hidden" id="INA226_status" name="INA226_status">
    <input type="hidden" id="battery_view" name="battery_view">
    <input type="hidden" id="battPercent" name="battPercent">
    <input type="hidden" id="aux_current" name="aux_current">
    <input type="hidden" id="aprs_view_call" name="aprs_view_call">


    <script>
      function update_igate_Hidden(checked) {
        document.getElementById("igate").value = checked ? "ok" : "ko";
      }
      function update_digi_Hidden(checked) {
        document.getElementById("digi").value = checked ? "ok" : "ko";
      }
      function update_meteo_Hidden(checked) {
        document.getElementById("meteo").value = checked ? "ok" : "ko";
      }
    </script>

    <script>
      function updateStatus() {
        fetch("/dash_meteo_value", { method: "POST" })
          .then(response => response.json())
          .then(data => {
            document.getElementById('TempC').value = data.TempC + " °C";
            document.getElementById('Press').value = data.Press + " hPa";
            document.getElementById('Hum').value = data.Hum + " %";
            document.getElementById('DewPoint').value = data.DewPoint;
            document.getElementById('myip').value = "ip: " + data.myip;
            document.getElementById('battPercent').value = data.battPercent + " %";
            document.getElementById('voltage_dashboard').value = "TTGO battery: " + data.voltage_dashboard + " Volt - " + data.battPercent + " %";
            document.getElementById('aux_current').value = data.aux_current;
            document.getElementById('aux_voltage').value = "AUX: " + data.aux_voltage + " Volt / " + data.aux_current + " Amp.";
            document.getElementById('current_server_aprsis').value = "server APRS: " + data.current_server_aprsis;
            document.getElementById('pressureTrend').value = "pressure trend: " + data.pressureTrend;
            document.getElementById('forecast').value = "forecast: " + data.forecast;
            document.getElementById('lastRX').value = data.lastRX;
            document.getElementById('dxRX').value = data.dxRX; 
          })
      }      
      updateStatus();                    // chiamata iniziale
      setInterval(updateStatus, 10000);  // ogni 10 secondi
    </script>   


    <body>
      <form action="/dash_save" method="POST">
      <title>LoRa system by iw1cgw</title>


      <fieldset>
        <div 
          id="call" 
          class="status-label online" 
          style="font-size: 16px; text-align: center; cursor: pointer;">
          Vedi posizione su APRS.fi
        </div>
      </fieldset>


      <script>
        document.getElementById("call").onclick = function () {
          const callsign = document.getElementById("aprs_view_call").value;
          if (callsign) {
            const encodedCall = encodeURIComponent("a/" + callsign);
            const url = `https://aprs.fi/#!z=7&call=${encodedCall}&timerange=3600&tail=3600/`;
            window.open(url, '_blank');
          } else {
            alert("Nominativo non disponibile.");
          }
        };
      </script>


      <fieldset>
      <div class="grid-container1">
        <div class="grid-item"></div>
        <div class="grid-item">
          <input type="text"
                id="fw"
                value="..."
                readonly
                style="cursor: pointer;"
                onclick="window.open('https://iw1cgw.wordpress.com/lora-aprs-system-news/', '_blank')">
        </div>
        <div class="grid-item"><input type="text" id="voltage_dashboard"     disabled></div>
        <div class="grid-item"><input type="text" id="aux_voltage"           disabled></div>
        <div class="grid-item"><input type="text" id="myip"                  disabled></div>
        <div class="grid-item"><input type="text" id="current_server_aprsis" disabled></div>
      </div>
      </fieldset>


      <fieldset>
      <h1>
        iGate       <!-- SWITCH VISIBILE -->
                    <label class="switch">
                    <input type="checkbox" id="igate_toggle" onchange="update_igate_Hidden(this.checked)">
                    <span class="slider"></span>
                    </label>
                    <!-- CAMPO NASCOSTO COLLEGATO -->
                    <input type="hidden" id="igate" name="igate">
        Digipeater  <!-- SWITCH VISIBILE -->
                    <label class="switch">
                    <input type="checkbox" id="digi_toggle" onchange="update_digi_Hidden(this.checked)">
                    <span class="slider"></span>
                    </label>
                    <!-- CAMPO NASCOSTO COLLEGATO -->
                    <input type="hidden" id="digi" name="digi"><br>
        Meteo       <!-- SWITCH VISIBILE -->
                    <label class="switch">
                    <input type="checkbox" id="meteo_toggle" onchange="update_meteo_Hidden(this.checked)">
                    <span class="slider"></span>
                    </label>
                    <!-- CAMPO NASCOSTO COLLEGATO -->
                    <input type="hidden" id="meteo" name="meteo">     
                    <select name="meteo_mode" id="meteo_mode">
                    <option value="rf">via RF</option>
                    <option value="ip">via IP</option>
                    <option value="rf_sleep">via RF deep sleep mode</option>
                    </select>
        <input type="submit" value="apply"     style="float: right;">
      </h1>
      </fieldset>
   

      <fieldset> <legend>LoRa RX/DX</legend>
      <div class="grid-container1">
        <div class="grid-item">Last RX  <input type="text" id="lastRX" style="width: 285px; color: blue; font-size: 8pt;" disabled></div>
        <div class="grid-item">Last DX  <input type="text" id="dxRX"   style="width: 285px; color: red; font-size: 8pt;" disabled></div>
        <div class="grid-item"> </div>
      </div>
        <input type="button" value="log"          style="float: right;" onclick="window.location.href='/mh'" >
        <input type="button" value="send beacons" style="float: left;"  id="beaconButton" onclick="window.location.href='/beacon'">
      </fieldset>


      <fieldset id="fieldsetMeteoStatus">
        <br>
        <span id="stationStatus" class="status-label">Checking status...</span>
        <!-- CAMPO NASCOSTO COLLEGATO -->
        <input type="hidden" id="BM_sensor_status" name="BM_sensor_status">
        <br><br>
        <div class="grid-container4" id="grid">
          <div class="grid-item">Temp</div>
          <div class="grid-item">Hum</div>
          <div class="grid-item">DewPoint</div>
          <div class="grid-item">Press</div>
          <div class="grid-item"><input type="text" id="TempC"  disabled></div>
          <div class="grid-item"><input type="text" id="Hum"    disabled></div>
          <div class="grid-item"><input type="text" id="DewPoint"  disabled></div>
          <div class="grid-item"><input type="text" id="Press"  disabled></div>
        </div>  
        <div class="grid-container1">
        <h1>
          <div class="grid-item"><input type="text" id="pressureTrend" disabled></div>
          <div class="grid-item" id="view_Zambretti"><input type="text" id="forecast" disabled></div>
        </h1>
        </div> 
      </fieldset>


      <fieldset>
      <h1>
        <input type="button" value="setup" style="float: right;"    onclick="window.location.href='/setup'" >
        <input type="button" value="manual" style="float: left;"    onclick="window.open('https://iw1cgw.wordpress.com/lora-aprs-system/', '_blank')">
      </h1>
      </fieldset>


      </form 
    </body>
  </html>
)rawliteral"; // by iw1cgw 26/06/2025