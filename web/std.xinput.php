<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module xinput</title>
<link rel="stylesheet" type="text/css" href="css/style.css">
<style>

        .container {
            display:        flex;
            flex-wrap:      nowrap;
            flex-direction: row;
            margin:         0px auto;
            padding:        0px;
        }
        .left {
            display:    block;
            overflow-y: scroll;
            width:      500px;
        }
        .leftpage {
            margin:     10px;
        }
        .right {
            display:    block;
            width:      100%;
        }
        .rightpage {
            max-width:  1024px;
            margin:     10px auto;
        }
        @media(min-width: 640px) {
            .container {
                max-width: 640px;
            }
        }
        @media(min-width: 768px) {
            .container {
                max-width: 768px;
            }
        }
        @media(min-width: 1024px) {
            .container {
                max-width: 1024px;
            }
        }
        @media(min-width: 1280px) {
            .container {
                max-width: 1280px;
            }
        }
        @media(min-width: 1536px) {
            .container {
                max-width: 1536px;
            }
        }
        @media screen and (max-width: 600px) {
            .left {
                display: none;
            }
            .rightpage {
                margin:  10px;
            }
        }

            .container {
                height:     100vh;
            }
            .right {
                overflow-y: scroll;
            }

        body {
            margin:         0px;
            line-height:    1.3em;
            font-family:    Segoe UI;
        }
        .container blockquote {
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            margin:             20px;
            margin-left:        50px;
            margin-right:       50px;
            padding:            10px;
        }
        .container a {
            text-decoration: none;
            color:           DoggerBlue;
        }
        .precode a {
            text-decoration: revert;
            color:           inherit;
        }
        .codetype a {
            text-decoration: revert;
            color:           inherit;
        }
        .container a:hover {
            text-decoration: underline;
        }
        table.item {
            border-collapse:    separate;
            background-color:   Black;
            color:              White;
            width:              100%;
            margin-top:         70px;
            margin-right:       0px;
            font-size:          110%;
        }
        .item td:first-child {
            width:              33%;
            white-space:        nowrap;
        }
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .enumeration td {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            min-width:          100px;
        }
        .enumeration td:first-child {
            background-color:   #f8f8f8;
            white-space:        nowrap;
        }
        .codetype {
            background-color:   #eeeeee;
        }
        .container td:last-child {
            width:              100%;
        }
        .left ul {
            list-style-type:    none;
            margin-left:        -20px;
        }
        .titletype {
            font-weight:        normal;
            font-size:          80%;
        }
        .titlelight {
            font-weight:        normal;
        }
        .titlestrong {
            font-weight:        bold;
            font-size:          100%;
        }
        .left h3 {
            background-color:   Black;
            color:              White;
            padding:            6px;
        }
        .right h1 {
            margin-top:         50px;
            margin-bottom:      50px;
        }
        .right h2 {
            margin-top:         35px;
        }
        .srcref {
            text-align:         right;
        }
        .incode {
            background-color:   #eeeeee;
            padding:            2px;
            border: 1px dotted  #cccccc;
        }
        .tdname .incode {
            background-color:   revert;
            padding:            2px;
            border:             revert;
        }
        .addinfos {
            font-size:          90%;
            white-space:        break-spaces;
            overflow-wrap:      break-word;
        }
        .precode {
            background-color:   #eeeeee;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
        }
    .SCde { color: #7f7f7f; }
    .SCmt { color: #71a35b; }
    .SCmp { color: #7f7f7f; }
    .SFct { color: #ff6a00; }
    .SCst { color: #3bc3a7; }
    .SItr { color: #b4b44a; }
    .STpe { color: #ed9a11; }
    .SKwd { color: #3186cd; }
    .SLgc { color: #b040be; }
    .SNum { color: #74a35b; }
    .SStr { color: #bb6643; }
    .SAtr { color: #7f7f7f; }
</style>
<?php include('common/end-head.php'); ?></head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="left">
<div class="leftpage">
<h2>Table of Contents</h2>
<h3>Structs</h3>
<h4></h4>
<ul>
<li><a href="#XInput_XINPUT_GAMEPAD">XINPUT_GAMEPAD</a></li>
<li><a href="#XInput_XINPUT_STATE">XINPUT_STATE</a></li>
<li><a href="#XInput_XINPUT_VIBRATION">XINPUT_VIBRATION</a></li>
</ul>
<h3>Constants</h3>
<h4></h4>
<ul>
<li><a href="#XInput_XINPUT_GAMEPAD_A">XINPUT_GAMEPAD_A</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_B">XINPUT_GAMEPAD_B</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_BACK">XINPUT_GAMEPAD_BACK</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_DPAD_DOWN">XINPUT_GAMEPAD_DPAD_DOWN</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_DPAD_LEFT">XINPUT_GAMEPAD_DPAD_LEFT</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_DPAD_RIGHT">XINPUT_GAMEPAD_DPAD_RIGHT</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_DPAD_UP">XINPUT_GAMEPAD_DPAD_UP</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_LEFT_SHOULDER">XINPUT_GAMEPAD_LEFT_SHOULDER</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_LEFT_THUMB">XINPUT_GAMEPAD_LEFT_THUMB</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_RIGHT_SHOULDER">XINPUT_GAMEPAD_RIGHT_SHOULDER</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_RIGHT_THUMB">XINPUT_GAMEPAD_RIGHT_THUMB</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_START">XINPUT_GAMEPAD_START</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_X">XINPUT_GAMEPAD_X</a></li>
<li><a href="#XInput_XINPUT_GAMEPAD_Y">XINPUT_GAMEPAD_Y</a></li>
</ul>
<h3>Functions</h3>
<h4></h4>
<ul>
<li><a href="#XInput_XInputGetState">XInput.XInputGetState</a></li>
<li><a href="#XInput_XInputSetState">XInput.XInputSetState</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</blockquote>
<h1>Module xinput</h1>
<p>Wrapper for Microsoft DirectX Input. </p>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XInput_XINPUT_GAMEPAD_A"><span class="titletype">const</span> <span class="titlelight">XInput.</span><span class="titlestrong">Constants</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="XInput_XINPUT_GAMEPAD_A" class="codetype"><span class="SCst">XINPUT_GAMEPAD_A</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_B" class="codetype"><span class="SCst">XINPUT_GAMEPAD_B</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_BACK" class="codetype"><span class="SCst">XINPUT_GAMEPAD_BACK</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_DOWN" class="codetype"><span class="SCst">XINPUT_GAMEPAD_DPAD_DOWN</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_LEFT" class="codetype"><span class="SCst">XINPUT_GAMEPAD_DPAD_LEFT</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_RIGHT" class="codetype"><span class="SCst">XINPUT_GAMEPAD_DPAD_RIGHT</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_UP" class="codetype"><span class="SCst">XINPUT_GAMEPAD_DPAD_UP</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_LEFT_SHOULDER" class="codetype"><span class="SCst">XINPUT_GAMEPAD_LEFT_SHOULDER</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_LEFT_THUMB" class="codetype"><span class="SCst">XINPUT_GAMEPAD_LEFT_THUMB</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_RIGHT_SHOULDER" class="codetype"><span class="SCst">XINPUT_GAMEPAD_RIGHT_SHOULDER</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_RIGHT_THUMB" class="codetype"><span class="SCst">XINPUT_GAMEPAD_RIGHT_THUMB</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_START" class="codetype"><span class="SCst">XINPUT_GAMEPAD_START</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_X" class="codetype"><span class="SCst">XINPUT_GAMEPAD_X</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_Y" class="codetype"><span class="SCst">XINPUT_GAMEPAD_Y</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XInput_XINPUT_GAMEPAD"><span class="titletype">struct</span> <span class="titlelight">XInput.</span><span class="titlestrong">XINPUT_GAMEPAD</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">wButtons</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bLeftTrigger</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bRightTrigger</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">sThumbLX</span></td>
<td class="codetype"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">sThumbLY</span></td>
<td class="codetype"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">sThumbRX</span></td>
<td class="codetype"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">sThumbRY</span></td>
<td class="codetype"><span class="STpe">s16</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XInput_XINPUT_STATE"><span class="titletype">struct</span> <span class="titlelight">XInput.</span><span class="titlestrong">XINPUT_STATE</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">dwPacketNumber</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Gamepad</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">XInput</span>.<span class="SCst"><a href="#XInput_XINPUT_GAMEPAD">XINPUT_GAMEPAD</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XInput_XINPUT_VIBRATION"><span class="titletype">struct</span> <span class="titlelight">XInput.</span><span class="titlestrong">XINPUT_VIBRATION</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">wLeftMotorSpeed</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wRightMotorSpeed</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XInput_XInputGetState"><span class="titletype">func</span> <span class="titlelight">XInput.</span><span class="titlestrong">XInputGetState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">XInputGetState</span>(dwUserIndex: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>, pState: *<span class="SCst">XInput</span>.<span class="SCst"><a href="#XInput_XINPUT_STATE">XINPUT_STATE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">DWORD</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XInput_XInputSetState"><span class="titletype">func</span> <span class="titlelight">XInput.</span><span class="titlestrong">XInputSetState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">XInputSetState</span>(dwUserIndex: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>, pVibration: *<span class="SCst">XInput</span>.<span class="SCst"><a href="#XInput_XINPUT_VIBRATION">XINPUT_VIBRATION</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">DWORD</span></span></code>
</div>
</div>
</div>
</div>
</body>
</html>
