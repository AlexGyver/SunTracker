#pragma once

const char GP_LIGHT[] PROGMEM =
"<style type='text/css'>\n"
"body{font-family:sans-serif;margin-top:15px;}\n"
"hr{width:95%;margin-bottom:10px;}\n"
"h2{margin:8px 0;}\n"
"span{font-size:20px;}\n"
"label{white-space:nowrap;font-size:20px;margin:0 5px;}\n"
".chartBlock{border-radius:15px;margin:10px;width:90%;max-width:1000px;height:500px}\n"
".grid{display:flex;justify-content:space-between;}\n"
"#grid .block{width:100%;margin:10px;}\n"
".block{max-width:400px;border-radius:10px;background-color:#f2f2f2;padding:15px 11px;margin:20px 10px;box-shadow:#aaa 0px 0px 10px;}\n"
".blockTab{padding-top:0px;}\n"
".blockHeader{font-size:20px;color:#fff;border-radius:10px 10px 0px 0px;background-color:#4caf50;padding:6px 0px;margin:0 -11px 10px -11px;}\n"
".thinBlock{background:none;border:2px solid #3c4048;box-shadow:none;}\n"
".thinText{padding:0px 10px;background:#ffffff;color:#3c4048;}\n"
".thinTab{background:none;border:none;color:#9c9ea1;margin-top:-17px;margin-bottom:5px;display:block;}\n"
".inliner{display:flex;flex-direction:row;align-content:center;flex-wrap:nowrap;align-items:center;}"
"input[type=number],input[type=text],input[type=password],input[type=date],input[type=time],input[type=color],input[type=checkbox],select,textarea\n"
"{border-radius:8px;padding:3px 10px;vertical-align:middle;position:relative;margin:6px 4px;font-size:20px;height:40px;cursor:pointer;width:90%;max-width:200px;border:1px solid #ccc;}\n"
"input[type=date],input[type=time]{width:180px;}\n"
"input[type=color]{width:80px;height:40px;}\n"
"input[type=checkbox]{width:27px;height:27px;margin-bottom:10px;}\n"
"input[type=number],input[type=text],input[type=password],select,textarea{display:inline-block;border:1px solid #ccc;border-radius:8px;box-sizing:border-box;cursor:auto;}\n"
"input[type=submit],input[type=button]{height:55px;font-size:28px;width:90%;max-width:300px;margin:8px 5px;background-color:#4CAF50;border:none;border-radius:8px;color:white;cursor:pointer;padding:0px 10px;}\n"
"input[type=submit]:hover,input[type=button]:hover{filter:brightness(0.95);}\n"
"input[type=button]{display:flex;align-items:center;justify-content:center;}\n"
"select{width:auto}\n"
".led{margin:9px 10px;}\n"
".led:after{width:20px;height:20px;border-radius:10px;top:-4px;left:-3px;position:relative;content:'';display:inline-block;background-color:#333;box-shadow:inset 0px 3px 0px 0px #fff5}\n"
".led.red:checked:after{background-color:#d00;box-shadow:inset 0px 3px #fff7,0px 0px 10px 1px #f00b;}\n"
".led.green:checked:after{background-color:#3c0;box-shadow:inset 0px 3px #fff7,0px 0px 10px 1px #4d08;}\n"
".ledn{margin:9px 10px;}\n"
".ledn:after{width:20px;height:20px;border-radius:10px;top:-4px;left:-3px;position:relative;content:'';display:inline-block;background-color:#d00;box-shadow: inset 0px 3px #fff7,0px 0px 10px 1px #f00b;}\n"
".ledn:checked:after{background-color:#3c0;box-shadow:inset 0px 3px #fff7,0px 0px 10px 1px #4d08;}\n"
".miniButton{min-height:40px;height:auto!important;font-size:20px!important;width:auto!important;}\n"
".switch{margin:10px 0 5px;position:relative;display:inline-block;width:60px;height:34px}\n"
".switch input{opacity:0;width:0;height:0}\n"
".slider{border-radius:34px;position:absolute;cursor:pointer;top:-5px;left:0;right:0;bottom:5px;background-color:#ccc;-webkit-transition:.1s;transition:.1s}\n"
".slider:before{border-radius:50%;position:absolute;content:'';height:26px;width:26px;left:4px;bottom:4px;background-color:#fff;-webkit-transition:.1s;transition:.1s}\n"
"input:checked+.slider{background-color:#37a93c}\n"
"input:checked+.slider:before{-webkit-transform:translateX(26px);-ms-transform:translateX(26px);transform:translateX(26px)}\n"
"output{display:inline-block;font-size:14px;padding:4px 5px;border-radius:4px;color:#fff;min-width:30px;background:#37a93c;margin-right:5px;}\n"
"input[type=range]{-webkit-appearance:none;width:70%;margin:12px 10px;height:12px;background:#fff;border:1px #b5b5b5 solid;border-radius:5px;background-repeat:no-repeat;cursor:pointer;padding:0px;background-image:linear-gradient(#37a93c,#37a93c);}\n"
"input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;height:15px;width:15px;}\n"
"input[type=range]:hover{filter:brightness(0.93);}\n"
".display{display:inline-block;font-size:18px;padding:3px 5px;border-radius:4px;color:#fff;background:#37a93c;margin:2px 3px;}\n"
"#ubtn{width:50px !important;height:40px;font-size:25px;margin:8px 5px;background-color:#4CAF50;border:none;border-radius:8px;color:white;cursor:pointer;text-align:center;vertical-align:middle;}\n"
"#ubtnclr{height:0px;width:0px;overflow:hidden;}\n"
".navtab>ul{display:flex;align-items:stretch;justify-content:center;list-style-type:none;margin:15px 0;padding:0;overflow:hidden;border-radius:5px;background:#37a93c;}\n"
".navtab>ul>li{display:flex;align-items:center;cursor:pointer;color:white;padding:8px 12px;}\n"
".navtab>ul>li:hover{background-color:#0001;}\n"
".navblock{display:none;padding:0;}\n"
"i{padding:5px 7px}\n"
"img.colorpick-eyedropper-input-trigger{display:none;}\n"
"input::-webkit-outer-spin-button,input::-webkit-inner-spin-button{-webkit-appearance:none;margin:0;}\n"
".spinner{display:flex;align-items:center;justify-content:center;}\n"
".spinBtn{height:40px!important;width:40px!important;font-size:35px!important;}\n"
"#spinner input[type=number]{width:80px}\n"
"</style>\n";

const char GP_DARK[] PROGMEM =
"<style type='text/css'>\n"
"#blockBack input[type=password]{background:#13161a}\n"
"#blockBack input[type=text]{background:#13161a}\n"
"#blockBack input[type=date]{background:#13161a}\n"
"#blockBack input[type=time]{background:#13161a}\n"
"#blockBack input[type=number]{background:#13161a}\n"
"#blockBack input[type=range]{background:#13161a;background-repeat:no-repeat;}\n"
"#blockBack select{background:#13161a}\n"
"#blockBack .slider{background:#13161a}\n"
"#blockBack input:checked+.slider{background-color:#37a93c}\n"
"body{font-family:sans-serif;background:#13161a;color:#bbb;margin-top:15px;}\n"
"hr{width:95%;margin-bottom:10px;border:none;border-top:2px solid #394048;}\n"
"h2{margin:8px 0;}\n"
"span{font-size:20px;}\n"
"label{white-space:nowrap;font-size:20px;margin:0 5px;}\n"
".chartBlock{border-radius:15px;margin:10px;width:90%;max-width:1000px;height:500px}\n"
".grid{display:flex;justify-content:space-between;}\n"
"#grid .block{width:100%;margin:10px;}\n"
".block{max-width:400px;border-radius:10px;background-color:#2a2d35;padding:15px 11px;margin:20px 10px;box-shadow:#111 0px 0px 20px;}\n"
".blockTab{padding-top:0px;}\n"
".blockHeader{color:#000;font-size:20px;border-radius:10px 10px 0px 0px;background-color:#4caf50;padding:6px 0px;margin:0px -11px 10px -11px;}\n"
".thinBlock{background:none;border:2px solid #3c4048;}\n"
".thinText{padding:0px 10px;background:#13161a;}\n"
".thinTab{background:none;border:none;color:#9c9ea1;margin-top:-17px;margin-bottom:5px;display:block;}\n"
".inliner{display:flex;flex-direction:row;align-content:center;flex-wrap:nowrap;align-items:center;}"
"input[type=number],input[type=text],input[type=password],input[type=date],input[type=time],input[type=color],input[type=checkbox],select,textarea\n"
"{border-radius:8px;padding:3px 10px;color:#bbb;border:none;background-color:#2a2d35;vertical-align:middle;position:relative;margin:6px 4px;font-size:20px;height:40px;cursor:pointer;width:90%;max-width:200px;}\n"
"input[type=date],input[type=time]{width:180px;}\n"
"input[type=color]{width:80px;height:40px;}\n"
"input[type=checkbox]{width:27px;height:27px;margin-bottom:10px;}\n"
"input[type=number],input[type=text],input[type=password],select,textarea{display:inline-block;border-radius:8px;box-sizing:border-box;cursor:auto;}\n"
"input[type=submit],input[type=button]{height:55px;font-size:28px;width:90%;max-width:300px;margin:8px 5px;background-color:#4CAF50;border:none;border-radius:8px;color:white;cursor:pointer;}\n"
"input[type=submit]:hover,input[type=button]:hover{filter:brightness(0.95 );}\n"
"input[type=button]{display:flex;align-items:center;justify-content:center;}\n"
"input:focus,select,textarea:focus{outline:none;}"
"select{width:auto}\n"
".led{margin:9px 10px;}\n"
".led:after{width:20px;height:20px;border-radius:10px;top:-4px;left:-3px;position:relative;content:'';display:inline-block;background-color:#333;box-shadow:inset 0px 3px 0px 0px #fff5}\n"
".led:checked:after{box-shadow:0px 0px 10px 4px;}\n"
".led.red:checked:after{background-color:#d00;box-shadow:inset 0px 3px 0px 0px #fff7,0px 0px 10px 4px #f00b;}\n"
".led.green:checked:after{background-color:#3c0;box-shadow:inset 0px 3px 0px 0px #fff7,0px 0px 10px 4px #4d08;}\n"
".ledn{margin:9px 10px;}\n"
".ledn:after{width:20px;height:20px;border-radius:10px;top:-4px;left:-3px;position:relative;content:'';display:inline-block;background-color:#d00;box-shadow:inset 0px 3px 0px 0px #fff7,0px 0px 10px 2px #f00;}\n"
".ledn:checked:after{background-color: #3c0;box-shadow:inset 0px 3px 0px 0px #fff7,0px 0px 10px 2px #4d0;}\n"
".miniButton{min-height:40px;height:auto!important;font-size:20px!important;width:auto!important;}\n"
".switch{margin:10px 0 5px;position:relative;display:inline-block;width:60px;height:34px}\n"
".switch input{opacity:0;width:0;height:0}\n"
".slider{border-radius:34px;position:absolute;cursor:pointer;top:-5px;left:0;right:0;bottom:5px;background-color:#54545485;-webkit-transition:.1s;transition:.1s}\n"
".slider:before{border-radius:50%;position:absolute;content:'';height:26px;width:26px;left:4px;bottom:4px;background-color:#fff;-webkit-transition:.1s;transition:.1s}\n"
"input:checked+.slider{background-color:#37a93c}\n"
"input:checked+.slider:before{-webkit-transform:translateX(26px);-ms-transform:translateX(26px);transform:translateX(26px)}\n"
"output{display:inline-block;font-size:14px;padding:4px 5px;border-radius:4px;color:#fff;min-width:30px;background:#37a93c;margin-right:5px;}\n"
"input[type=range]{-webkit-appearance:none;width:70%;margin:12px 10px;height:12px;background:#2a2d35;border-radius:5px;background-repeat:no-repeat;cursor:pointer;padding:0px;background-image:linear-gradient(#37a93c,#37a93c);}\n"
"input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;height:15px;width:15px;}\n"
"input[type=range]:hover{filter:brightness(0.93);}\n"
".display{display:inline-block;font-size:18px;padding:3px 5px;border-radius:4px;color:#fff;background:#37a93c;margin:2px 3px;}\n"
"#ubtn{width:50px !important;height:40px;font-size:25px;margin:8px 5px;background-color:#4CAF50;border:none;border-radius:8px;color:white;cursor:pointer;text-align:center;vertical-align:middle;}\n"
"#ubtnclr{height:0px;width:0px;overflow:hidden;}\n"
".navtab>ul{display:flex;align-items:stretch;justify-content:center;list-style-type:none;margin:15px 0;padding:0;overflow:hidden;border-radius:5px;background:#37a93c;}\n"
".navtab>ul>li{display:flex;align-items:center;cursor:pointer;color:white;padding:8px 12px;}\n"
".navtab>ul>li:hover{background-color:#0001;}\n"
".navblock{display:none;padding:0;}\n"
"i{padding:5px 7px}\n"
"img.colorpick-eyedropper-input-trigger{display:none;}\n"
"input::-webkit-outer-spin-button,input::-webkit-inner-spin-button{-webkit-appearance:none;margin:0;}\n"
".spinner{display:flex;align-items:center;justify-content:center;}\n"
".spinBtn{height:40px!important;width:40px!important;font-size:35px!important;}\n"
"#spinner input[type=number]{width:80px}\n"
"</style>\n";