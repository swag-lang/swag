<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>

        .container {
            display:        flex;
            flex-wrap:      nowrap;
            flex-direction: row;
            margin:         0px auto;
            padding:        0px;
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
            .left {
                display:    block;
                overflow-y: scroll;
                width:      600px;
                height:     100vh;
            }
            .leftpage {
                margin:    10px;
            }
            .right {
                overflow-y: scroll;
                height:     100vh;
            }
            @media only screen and (max-width: 600px) {
                td {
                    display: block;
                    width:   100%;
                }
            }
            @media screen and (max-width: 600px) {
                .left {
                    display: none;
                }
            }
        body {
            margin:         0px;
            line-height:    1.3em;
            font-family:    Segoe UI;
        }
        .container blockquote {
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            margin-right:       50px;
            padding:            10px;
        }
        .container a {
            text-decoration: none;
            color:           DoggerBlue;
        }
        .container a:hover {
            text-decoration: underline;
        }
        .container a.src {
            font-size:          90%;
            color:              LightGrey;
        }
        .container table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .container td.enumeration {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              30%;
        }
        .container td.tdname {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              20%;
            background-color:   #f8f8f8;
        }
        .container td.tdtype {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              auto;
        }
        .container td:last-child {
            width:              auto;
        }
        .left ul {
            list-style-type:    none;
            margin-left:        -30px;
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
        .left h2 {
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
        table.item {
            background-color:   Black;
            color:              White;
            width:              100%;
            margin-top:         70px;
            margin-right:       0px;
            padding:            4px;
            font-size:          110%;
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
        .container pre {
            background-color:   #eeeeee;
            border:             1px solid LightGrey;
            padding:            10px;
            margin-left:        20px;
            margin-right:       20px;
        }    .SyntaxCode      { color: #7f7f7f; }
    .SyntaxComment   { color: #71a35b; }
    .SyntaxCompiler  { color: #7f7f7f; }
    .SyntaxFunction  { color: #ff6a00; }
    .SyntaxConstant  { color: #3173cd; }
    .SyntaxIntrinsic { color: #b4b44a; }
    .SyntaxType      { color: #3bc3a7; }
    .SyntaxKeyword   { color: #3186cd; }
    .SyntaxLogic     { color: #b040be; }
    .SyntaxNumber    { color: #74a35b; }
    .SyntaxString    { color: #bb6643; }
    .SyntaxAttribute { color: #7f7f7f; }
</style>
</head>
<body>
<div class="container">
<div class="left">
<div class="leftpage">
<h1>Module xinput</h1>
<h2>Structs</h2>
<h3></h3>
<ul>
<li><a href="#XInput_XINPUT_GAMEPAD">XINPUT_GAMEPAD</a></li>
<li><a href="#XInput_XINPUT_STATE">XINPUT_STATE</a></li>
<li><a href="#XInput_XINPUT_VIBRATION">XINPUT_VIBRATION</a></li>
</ul>
<h2>Constants</h2>
<h3></h3>
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
<h2>Functions</h2>
<h3></h3>
<ul>
<li><a href="#XInput_XInputGetState">XInput.XInputGetState</a></li>
<li><a href="#XInput_XInputSetState">XInput.XInputSetState</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Overview</h1>
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
<td id="XInput_XINPUT_GAMEPAD_A" class="tdname">
XINPUT_GAMEPAD_A</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_B" class="tdname">
XINPUT_GAMEPAD_B</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_BACK" class="tdname">
XINPUT_GAMEPAD_BACK</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_DOWN" class="tdname">
XINPUT_GAMEPAD_DPAD_DOWN</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_LEFT" class="tdname">
XINPUT_GAMEPAD_DPAD_LEFT</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_RIGHT" class="tdname">
XINPUT_GAMEPAD_DPAD_RIGHT</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_UP" class="tdname">
XINPUT_GAMEPAD_DPAD_UP</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_LEFT_SHOULDER" class="tdname">
XINPUT_GAMEPAD_LEFT_SHOULDER</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_LEFT_THUMB" class="tdname">
XINPUT_GAMEPAD_LEFT_THUMB</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_RIGHT_SHOULDER" class="tdname">
XINPUT_GAMEPAD_RIGHT_SHOULDER</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_RIGHT_THUMB" class="tdname">
XINPUT_GAMEPAD_RIGHT_THUMB</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_START" class="tdname">
XINPUT_GAMEPAD_START</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_X" class="tdname">
XINPUT_GAMEPAD_X</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_Y" class="tdname">
XINPUT_GAMEPAD_Y</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
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
<td class="tdname">
wButtons</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bLeftTrigger</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bRightTrigger</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sThumbLX</td>
<td class="tdtype">
Win32.SHORT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sThumbLY</td>
<td class="tdtype">
Win32.SHORT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sThumbRX</td>
<td class="tdtype">
Win32.SHORT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sThumbRY</td>
<td class="tdtype">
Win32.SHORT</td>
<td class="enumeration">
</td>
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
<td class="tdname">
dwPacketNumber</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Gamepad</td>
<td class="tdtype">
<a href="#XInput_XINPUT_GAMEPAD">XInput.XINPUT_GAMEPAD</a></td>
<td class="enumeration">
</td>
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
<td class="tdname">
wLeftMotorSpeed</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wRightMotorSpeed</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">XInputGetState</span><span class="SyntaxCode">(dwUserIndex: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, pState: *</span><span class="SyntaxConstant">XInput</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#XInput_XINPUT_STATE">XINPUT_STATE</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">XInputSetState</span><span class="SyntaxCode">(dwUserIndex: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, pVibration: *</span><span class="SyntaxConstant">XInput</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#XInput_XINPUT_VIBRATION">XINPUT_VIBRATION</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
</pre>
</div>
</div>
</div>
</body>
</html>
