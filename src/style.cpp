const char* style_css = R"rawliteral(
  html {
    font-family: sans-serif;
    font-weight: normal;
    font-size: 11pt;
    margin: 0;
    text-align: center;
  }


  body {
    font-family: sans-serif;
    font-size: 14px;
    line-height: 1.4;  /* oppure 1.2 o 1.0 o 0.8  - 1.0 é uguale al font */
  }


  legend {
    text-shadow: 2px 1px grey;
    text-align: right;
    font-size: 24px;
  }

  h1 {
    text-align: left;
    font-size: 11pt;
    /* line-height: 1.0 */
    line-height: 1.4
    padding: 4px;
    /* padding: 1px; */
  }

  select,
  input[type="button"],
  input[type="submit"],
  input[type="text"] {
    border-radius: 10px;
    border: 1px solid #ccc;
    font-size: 11pt;
    text-align: center;
    background-color: white;
  }

  select {
    padding: 0.5px;
    cursor: pointer;
  }

  select:hover {
    background-color: #e0e0e0;
  }

  select:disabled {
    background-color: #ddd;
    cursor: not-allowed;
    opacity: 0.6;
  }

  input[type="button"],
  input[type="submit"] {
    padding: 4px 10px; /* dimensioni pulsante */
    background-color: #f0f0f0;
    cursor: pointer;
  }


  #beacons_timing:disabled {
    cursor: not-allowed;
  }


 input:disabled {
    /* background-color: #ddd; */
    /* cursor: not-allowed; */
    /* opacity: 0.6; */
  }


  input[type="button"]:disabled,
  input[type="submit"]:disabled {
    background-color: #ddd;
    color: #666;
    cursor: not-allowed;
    opacity: 0.6;
    border: 1px solid #aaa;
  }


  input[type="submit"]:hover,
  input[type="button"]:hover {
    background-color: #e0e0e0;
  }

  input[type="text"]:hover {
    background-color: #e0e0e0;
  }


  input[type="text"] {
    /* padding: 1px 4px; */
    padding: 0.5px 2px;
    width: auto;  /* o un valore es: 80px */
    font-size: 11pt;
  }


  .value-box {
    border-radius: 10px;
    border: 1px solid #ccc;
    padding: 0.5px 2px;
    font-size: 11pt;
  }


.switch input:disabled + .slider {
  background-color: #aaa;
  cursor: not-allowed;
  opacity: 0.5;
}

.switch input:disabled + .slider:before {
  background-color: #eee;
}



 .switch:disabled {
    /* background-color: #ddd; */
    cursor: not-allowed;
    opacity: 0.6;
  }

  .switch {
    position: relative;
    display: inline-block;
    width: 43px;
    height: 16px;
  }

  .switch input {
    opacity: 0;
    width: 0;
    height: 0;
  }

  .slider {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: #ccc;
    border-radius: 34px;
    transition: 0.4s;
  }

  .slider:before {
    content: "";
    position: absolute;
    height: 10px;
    width: 10px;
    left: 4px;
    bottom: 4px;
    background-color: #fff;
    border-radius: 50%;
    transition: 0.4s;
  }

  input:checked + .slider {
    background-color: #2196F3;
  }

  input:checked + .slider:before {
    transform: translateX(24px);
  }

  fieldset {
    font-size: 11pt;
    width: 350px;
    border: 2px solid #22a6b3;
    border-radius: 12px;
    box-shadow: 0 0 10px #22a6b3;
    justify-content: center;
  }

  .grid-container1,
  .grid-container4 {
    display: grid;
    max-width: 550px;
    width: 350px;
    margin: 0 auto;
    text-align: center;
    justify-items: center;
    gap: 1px;
  }

  .grid-item input[type="text"],
  .grid-container1 input[type="text"] {
    width: 320px;
    padding: 1px;
    font-size: 11pt;
    text-align: center;
    border-radius: 10px;
    border: 1px solid #ccc;
    /* background-color: #f9f9f9; */
  }

  .grid-container1 {
    grid-template-columns: 1fr;
  }

  .grid-container4 {
    grid-template-columns: 1fr 1fr 1fr 1fr;
  }
  .grid-container4 input[type="text"] {
    width: 80px;
  }

  .grid-item {
    padding: 1px;
  }

  .data-input {
    font-size: 10pt;
  }

  .status-label {
    padding: 8px 12px;
    border-radius: 6px;
    font-weight: bold;
    font-family: sans-serif;
    font-size: 14px;
    display: inline-block;
  }

  .online {
    background-color: #007e33;
    color: #fff;
  }

  .offline {
    background-color: #cc0000;
    color: #fff;
  }


  .fieldset-wrapper {
    position: relative;
    display: inline-block;
  }

  .image-top-right {
    position: absolute;
    top: 5px;
    right: 5px;
    width: 100px; /* Puoi adattare dimensioni */
    height: auto;
  }
    
.image-top-center {
  position: absolute;
  top: 5px;
  left: 50%;
  transform: translateX(-50%);
  width: 40px;
  height: auto;
}



)rawliteral";
