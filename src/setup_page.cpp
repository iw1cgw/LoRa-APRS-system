#include <pgmspace.h>

const char* setup_html = R"rawliteral(
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
      fetch("/valori", { method: "POST" })
      .then(response => response.json())
      .then(data => {
        document.getElementById("igate_call").value = data.igate_call;
        document.getElementById("igate_latlong").value = data.igate_latlong;
        document.getElementById("igate_info").value = data.igate_info;
        document.getElementById("igate_beacon_arf_toggle").checked = (data.igate_beacon_arf === "ok");
        document.getElementById("igate_beacon_arf").value = data.igate_beacon_arf;
        document.getElementById("beacons_switch_toggle").checked = (data.beacons_switch === "ok");
        document.getElementById("beacons_switch").value = data.beacons_switch;
        document.getElementById("beacons_timing").value = data.beacons_timing;
        document.getElementById("digi_radius").value = data.digi_radius;
        document.getElementById("digi_route").value = data.digi_route;
        document.getElementById("digi_denied").value = data.digi_denied;
        document.getElementById("meteo_call").value = data.meteo_call;
        document.getElementById("meteo_latlong").value = data.meteo_latlong;
        document.getElementById("meteo_info").value = data.meteo_info;
        document.getElementById("igate_backup_toggle").checked = (data.igate_backup === "ok");
        document.getElementById("igate_backup").value = data.igate_backup;
        document.getElementById("meteo_altitude").value = data.meteo_altitude;
        document.getElementById("drift_temperature").value = data.drift_temperature;
        document.getElementById("drift_pressure").value = data.drift_pressure;
        document.getElementById("Zambretti_toggle").checked = (data.Zambretti === "ok");
        document.getElementById("Zambretti").value = data.Zambretti;
        document.getElementById("Zambretti_enabled").value = data.Zambretti_enabled;
        document.getElementById("frequency").value = data.frequency;
        document.getElementById("power").value = data.power;
        document.getElementById("modulation").value = data.modulation;
        document.getElementById("WiFi1_ssid").value = data.WiFi1_ssid;
        document.getElementById("WiFi1_pwd").value = data.WiFi1_pwd;
        document.getElementById("WiFi2_ssid").value = data.WiFi2_ssid;
        document.getElementById("WiFi2_pwd").value = data.WiFi2_pwd;
        document.getElementById("access_point_pwd").value = data.access_point_pwd;
        document.getElementById("access_point_asd_toggle").checked = (data.access_point_asd === "ok");
        document.getElementById("access_point_asd").value = data.access_point_asd;
        document.getElementById("aprs_server").value = data.aprs_server;
        document.getElementById("aprs_passcode").value = data.aprs_passcode;
        document.getElementById("aprs_sh_toggle").checked = (data.aprs_sh === "ok");
        document.getElementById("aprs_sh").value = data.aprs_sh;
        document.getElementById("wunder_switch_toggle").checked = (data.wunder_switch === "ok");
        document.getElementById("wunder_switch").value = data.wunder_switch;
        document.getElementById("wunder_id").value = data.wunder_id;
        document.getElementById("wunder_key").value = data.wunder_key;
        document.getElementById("password").value = data.password;
        document.getElementById("gmt_zone").value = data.gmt_zone;
        document.getElementById("drift_battery").value = data.drift_battery;
        document.getElementById("battery_view_toggle").checked = (data.battery_view === "ok");
        document.getElementById("battery_view").value = data.battery_view;
        document.getElementById("telemetry_toggle").checked = (data.telemetry === "ok");
        document.getElementById("telemetry").value = data.telemetry;
        document.getElementById("oled_toggle").checked = (data.oled === "ok");
        document.getElementById("oled").value = data.oled;
        document.getElementById("INA226_status").value = data.INA226_status;
        const container = document.getElementById("INA226_switch_container"); // costruzione di un container per far apparire/scomparire il toggle
        if (data.INA226_status === "ok") {
          container.style.display = "inline-block";  // o "block" o "inline-block"
        } else {
          container.style.display = "none";
        }
        document.getElementById("INA226_scale_toggle").checked = (data.INA226_scale === "10");
        document.getElementById("INA226_scale").value = data.INA226_scale;
        if (data.beacons_switch === "ko") {
          igate_beacon_arf_toggle.disabled = true;
          beacons_timing.disabled = true;
          /* beacons_timing.style.cursor = "auto"; // oppure "text" */
          } else {
            igate_beacon_arf_toggle.disabled = false;
            beacons_timing.disabled = false;
            /* beacons_timing.style.cursor = "not-allowed"; */
          }
        if (data.Zambretti_enabled === "ko") Zambretti_toggle.disabled = true;
          else Zambretti_toggle.disabled = false;
        });
      };
    </script>


    <script>
      function update_igate_backup_Hidden(checked) {
        document.getElementById("igate_backup").value = checked ? "ok" : "ko";
      }
      function update_beacons_switch_Hidden(checked) {
        document.getElementById("beacons_switch").value = checked ? "ok" : "ko";
      }
      function update_igate_beacon_arf_Hidden(checked) {
        document.getElementById("igate_beacon_arf").value = checked ? "ok" : "ko";
      }
      function update_Zambretti_Hidden(checked) {
        document.getElementById("Zambretti").value = checked ? "ok" : "ko";
      }
      function update_access_point_asd_Hidden(checked) {
        document.getElementById("access_point_asd").value = checked ? "ok" : "ko";
      }
      function update_aprs_sh_Hidden(checked) {
        document.getElementById("aprs_sh").value = checked ? "ok" : "ko";
      }
      function update_battery_view_Hidden(checked) {
        document.getElementById("battery_view").value = checked ? "ok" : "ko";
      }  
      function update_INA226_scale_Hidden(checked) {
        document.getElementById("INA226_scale").value = checked ? "10" : "01";
      }
      function update_wunder_switch_Hidden(checked) {
        document.getElementById("wunder_switch").value = checked ? "ok" : "ko";
      }
      function update_telemetry_Hidden(checked) {
        document.getElementById("telemetry").value = checked ? "ok" : "ko";
      }
      function update_oled_Hidden(checked) {
        document.getElementById("oled").value = checked ? "ok" : "ko";
      }
    </script>


    <body>
      <title>S E T U P</title>
      <form action="/salva" method="POST">
        
      <fieldset> <legend> iGate - digipeater </legend>
      <h1>
        igate call                  <input type="text" id="igate_call"    name="igate_call"    minlength="4"  style="width: 100px;" ><br>
        igate latitude longitude    <input type="text" id="igate_latlong" name="igate_latlong" minlength="14" style="width: 165px;"><br>
        igate info                  <input type="text" id="igate_info"    name="igate_info"    maxlength="40" style="width: 260px;"><br>
        igate backup                <!-- SWITCH VISIBILE -->
                                    <label class="switch">
                                    <input type="checkbox" id="igate_backup_toggle" onchange="update_igate_backup_Hidden(this.checked)">
                                    <span class="slider"></span>
                                    </label>
                                    <!-- CAMPO NASCOSTO COLLEGATO -->
                                    <input type="hidden" id="igate_backup" name="igate_backup"> <br>
        digipeater route only for   <input type="text" id="digi_route"    name="digi_route"   style="width: 125px;"> <br>
        digipeater route denied for <input type="text" id="digi_denied"   name="digi_denied"  style="width: 125px;"> <br>
        digipeater radius           <input type="text" id="digi_radius"   name="digi_radius"  style="width: 75px;">  
        <input type="submit" value="apply"     style="float: right;">
      </h1>
      </fieldset>


      <fieldset><legend> meteo </legend>
      <h1>
        meteo call               <input type="text" id="meteo_call"        name="meteo_call"    minlength="4"  style="width: 100px;" ><br>
        meteo latitude longitude <input type="text" id="meteo_latlong"     name="meteo_latlong" minlength="14" style="width: 165px;"> <br>
        meteo info               <input type="text" id="meteo_info"        name="meteo_info"    maxlength="40" style="width: 260px;"> <br>
        meteo altitude           <input type="text" id="meteo_altitude"    name="meteo_altitude"    style="width:  75px;"> <br>
        drift temperature        <input type="text" id="drift_temperature" name="drift_temperature" style="width:  75px;"> <br>
        drift pressure           <input type="text" id="drift_pressure"    name="drift_pressure"    style="width:  75px;"> <br>
        forecast by Zambretti    <!-- SWITCH VISIBILE -->
                                <label class="switch">
                                <input type="checkbox" id="Zambretti_toggle" onchange="update_Zambretti_Hidden(this.checked)">
                                <span class="slider"></span>
                                </label>
                                <!-- CAMPO NASCOSTO COLLEGATO -->
                                <input type="hidden" id="Zambretti" name="Zambretti"> <br>
                                <input type="hidden" id="Zambretti_enabled" name="Zambretti_enabled">
        WUnder iD                <input type="text" id="wunder_id"        name="wunder_id"        style="width: 150px;"> <br>
        WUnder station key       <input type="text" id="wunder_key"       name="wunder_key"       style="width: 150px;"> <br>
        WUnder                   <!-- SWITCH VISIBILE -->
                                <label class="switch">
                                <input type="checkbox" id="wunder_switch_toggle" onchange="update_wunder_switch_Hidden(this.checked)">
                                <span class="slider"></span>
                                </label>
                                <!-- CAMPO NASCOSTO COLLEGATO -->
                                <input type="hidden" id="wunder_switch" name="wunder_switch">
        <input type="submit" value="apply"     style="float: right;">
      </h1>
      </fieldset>


      <fieldset><legend> beacons </legend>
      <h1>
        beacons on/off           <!-- SWITCH VISIBILE -->
                                <label class="switch">
                                <input type="checkbox" id="beacons_switch_toggle" onchange="update_beacons_switch_Hidden(this.checked)">
                                <span class="slider"></span>
                                </label>
                                <!-- CAMPO NASCOSTO COLLEGATO -->
                                <input type="hidden" id="beacons_switch" name="beacons_switch"> <br>
        beacons timing           <input type="text" id="beacons_timing" name="beacons_timing"  style="width: 75px;"> <br>  
        igate beacon also in RF  <!-- SWITCH VISIBILE -->
                                <label class="switch">
                                <input type="checkbox" id="igate_beacon_arf_toggle" onchange="update_igate_beacon_arf_Hidden(this.checked)">
                                <span class="slider"></span>
                                </label>
                                <!-- CAMPO NASCOSTO COLLEGATO -->
                                <input type="hidden" id="igate_beacon_arf" name="igate_beacon_arf">
        <input type="submit" value="apply"     style="float: right;">
      </h1>
      </fieldset>


      <fieldset><legend> radio </legend>
      <h1>
        modulation               <select name="modulation" id="modulation"><br>
                                <option value="300">BW 125KHz CR 4:5 SF 12 (300bps)</option>
                                <option value="1200">BW 125KHz CR 4:7 SF 9 (1200bps)</option>
                                </select>
        frequency MHz            <input type="text" id="frequency"  name="frequency"  style="width: 75px;"> <br>
        power                    <input type="text" id="power"      name="power"      style="width: 75px;">
        <input type="submit" value="apply"     style="float: right;">
      </h1>
      </fieldset>


      <fieldset><legend> ip network </legend>
      <h1>
        WiFi 1 SSiD              <input type="text" id="WiFi1_ssid"       name="WiFi1_ssid" minlength="8"      style="width: 250px;"> <br>
        WiFi 1 pwd               <input type="text" id="WiFi1_pwd"        name="WiFi1_pwd"  minlength="8"      style="width: 250px;"> <br>
        WiFi 2 SSiD              <input type="text" id="WiFi2_ssid"       name="WiFi2_ssid" minlength="8"      style="width: 250px;"> <br>
        WiFi 2 pwd               <input type="text" id="WiFi2_pwd"        name="WiFi2_pwd"  minlength="8"      style="width: 250px;"> <br>
        AP pwd                   <input type="text" id="access_point_pwd" name="access_point_pwd" minlength="8" style="width: 200px;"> <br>
        AP auto shutdown         <!-- SWITCH VISIBILE -->
                                <label class="switch">
                                <input type="checkbox" id="access_point_asd_toggle" onchange="update_access_point_asd_Hidden(this.checked)">
                                <span class="slider"></span>
                                </label>
                                <!-- CAMPO NASCOSTO COLLEGATO -->
                                <input type="hidden" id="access_point_asd" name="access_point_asd"> <br>
        APRS server              <input type="text" id="aprs_server"      name="aprs_server"      style="width: 200px;"> <br>
        APRS passcode            <input type="text" id="aprs_passcode"    name="aprs_passcode"    style="width:  75px;"> <br>
        APRS server hide         <!-- SWITCH VISIBILE -->
                                <label class="switch">
                                <input type="checkbox" id="aprs_sh_toggle" onchange="update_aprs_sh_Hidden(this.checked)">
                                <span class="slider"></span>
                                </label>
                                <!-- CAMPO NASCOSTO COLLEGATO -->
                                <input type="hidden" id="aprs_sh" name="aprs_sh">
        <input type="submit" value="apply"     style="float: right;">
      </h1>
      </fieldset>


      <fieldset><legend> system </legend>
      <h1>
        password                 <input type="text" id="password"       name="password"       style="width: 200px;"> <br>
        GMT zone                 <input type="text" id="gmt_zone"       name="gmt_zone"       style="width: 75px;"> <br>
        TTGO drift battery       <input type="text" id="drift_battery"  name="drift_battery"  style="width: 75px;"> <br>
        TTGO battery view        <!-- SWITCH VISIBILE -->
                                <label class="switch">
                                <input type="checkbox" id="battery_view_toggle" onchange="update_battery_view_Hidden(this.checked)">
                                <span class="slider"></span>
                                </label>
                                <!-- CAMPO NASCOSTO COLLEGATO -->
                                <input type="hidden" id="battery_view" name="battery_view"><br>
        Telemetry                <!-- SWITCH VISIBILE -->
                                <label class="switch">
                                <input type="checkbox" id="telemetry_toggle" onchange="update_telemetry_Hidden(this.checked)">
                                <span class="slider"></span>
                                </label>
                                <!-- CAMPO NASCOSTO COLLEGATO -->
                                <input type="hidden" id="telemetry" name="telemetry">
                                <!-- SWITCH VISIBILE -->
                                <span id="INA226_switch_container">
        INA226 scale x10         <label class="switch">
                                <input type="checkbox" id="INA226_scale_toggle" onchange="update_INA226_scale_Hidden(this.checked)"> 
                                <span class="slider"></span> 
                                </label> 
                                </span>
                                <!-- CAMPI NASCOSTI COLLEGATI -->
                                <input type="hidden" id="INA226_scale" name="INA226_scale"> <br>
                                <input type="hidden" id="INA226_status" name="INA226_status">
        OLED display             <!-- SWITCH VISIBILE -->
                                <label class="switch">
                                <input type="checkbox" id="oled_toggle" onchange="update_oled_Hidden(this.checked)">
                                <span class="slider"></span>
                                </label>
                                <!-- CAMPO NASCOSTO COLLEGATO -->
                                <input type="hidden" id="oled" name="oled">
        <input type="submit" value="apply"     style="float: right;">
      </h1>
      </fieldset>


      <fieldset>
      <h1>
        <input type="button" value="dashboard" style="float: right;"    onclick="window.location.href='/'" >
        <input type="button" value="reboot" style="float: left;"    onclick="window.location.href='/reboot'" >
      </h1>
      </fieldset>

      </form 
    </body>
  </html>
)rawliteral"; // by iw1cgw 26/06/2025