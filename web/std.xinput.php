<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module xinput</title>
<link rel="stylesheet" type="text/css" href="css/style.css">
<script src="https://kit.fontawesome.com/f76be2b3ee.js" crossorigin="anonymous"></script>
<style>

        .container  { display: flex; flex-wrap: nowrap; flex-direction: row; margin: 0px auto; padding: 0px; }
        .left       { display: block; overflow-y: scroll; width: 500px; }
        .left-page  { margin: 10px; }
        .right      { display: block; width: 100%; }
        .right-page { max-width: 1024px; margin: 10px auto; }
        
        @media(min-width: 640px)  { .container { max-width: 640px; }}
        @media(min-width: 768px)  { .container { max-width: 768px; }}
        @media(min-width: 1024px) { .container { max-width: 1024px; }}
        @media(min-width: 1280px) { .container { max-width: 1280px; }}
        @media(min-width: 1536px) { .container { max-width: 1536px; }}
        
        @media screen and (max-width: 600px) {
            .left       { display: none; }
            .right-page { margin:  10px; }
        }

            .container { height: 100vh; }
            .right     { overflow-y: scroll; }

        body { margin: 0px; line-height: 1.3em; }
        
        .container a        { color:           DoggerBlue; }
        .container a:hover  { text-decoration: underline; }
        
        .left a     { text-decoration: none; }
        .left ul    { list-style-type: none; margin-left: -20px; }
        .left h3    { background-color: Black; color: White; padding: 6px; }
        .right h1   { margin-top: 50px; margin-bottom: 50px; }
        .right h2   { margin-top: 35px; }
        
        .blockquote-default {
            border-radius:      5px;
            border:             1px solid Orange;
            border-left:        6px solid Orange;
            background-color:   LightYellow;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-note {
            border-radius:      5px;
            border:             1px solid #ADCEDD;
            background-color:   #CDEEFD;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-tip {
            border-radius:      5px;
            border:             1px solid #BCCFBC;
            background-color:   #DCEFDC;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-warning {
            border-radius:      5px;
            border:             1px solid #DFBDB3;
            background-color:   #FFDDD3;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-attention {
            border-radius:      5px;
            border:             1px solid #DDBAB8;
            background-color:   #FDDAD8;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-default     p:first-child { margin-top: 0px; }
        .blockquote-default     p:last-child  { margin-bottom: 0px; }
        .blockquote-note        p:last-child  { margin-bottom: 0px; }
        .blockquote-tip         p:last-child  { margin-bottom: 0px; }
        .blockquote-warning     p:last-child  { margin-bottom: 0px; }
        .blockquote-attention   p:last-child  { margin-bottom: 0px; }
        .blockquote-title-block { margin-bottom:   10px; }
        .blockquote-title       { font-weight:     bold; }
        
        table.api-item            { border-collapse: separate; background-color: Black; color: White; width: 100%; margin-top: 70px; margin-right: 0px; font-size: 110%; }
        .api-item td:first-child  { width: 33%; white-space: nowrap; }
        .api-item-title-src-ref   { text-align:  right; }
        .api-item-title-src-ref a { color:       inherit; }
        .api-item-title-kind      { font-weight: normal; font-size: 80%; }
        .api-item-title-light     { font-weight: normal; }
        .api-item-title-strong    { font-weight: bold; font-size: 100%; }
        .api-additional-infos     { font-size: 90%; white-space: break-spaces; overflow-wrap: break-word; }
        
        table.table-enumeration           { border: 1px solid LightGrey; border-collapse: collapse; width: calc(100% - 40px); font-size: 90%; margin-left: 20px; margin-right: 20px; }
        .table-enumeration td             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; }
        .table-enumeration td:first-child { background-color: #f8f8f8; white-space: nowrap; }
        .table-enumeration td:last-child  { width: 100%; }
        .table-enumeration td.code-type   { background-color: #eeeeee; }
        .table-enumeration a              { text-decoration: none; color: inherit; }
        
        .code-inline            { font-size: 110%; font-family: monospace; display: inline-block; background-color: #eeeeee; padding: 2px; border-radius: 5px; border: 1px dotted #cccccc; }
        .code-block {
            background-color:   #eeeeee;
            border-radius:      5px;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
        }
        .code-block a { color: inherit; }
        
    .SCde { color: #222222; }
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
    .SInv { color: #ff0000; }
</style>
<?php include('common/end-head.php'); ?></head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="left">
<div class="left-page">
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
<div class="right-page">
<div class="blockquote-warning">
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</div>
<h1>Module xinput</h1>
<p> Wrapper for Microsoft DirectX Input. </p>
<h1>Content</h1>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XInput_XINPUT_GAMEPAD_A"><span class="api-item-title-kind">const</span> <span class="api-item-title-light">XInput.</span><span class="api-item-title-strong">Constants</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td id="XInput_XINPUT_GAMEPAD_A">XINPUT_GAMEPAD_A</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_B">XINPUT_GAMEPAD_B</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_BACK">XINPUT_GAMEPAD_BACK</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_DOWN">XINPUT_GAMEPAD_DPAD_DOWN</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_LEFT">XINPUT_GAMEPAD_DPAD_LEFT</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_RIGHT">XINPUT_GAMEPAD_DPAD_RIGHT</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_DPAD_UP">XINPUT_GAMEPAD_DPAD_UP</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_LEFT_SHOULDER">XINPUT_GAMEPAD_LEFT_SHOULDER</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_LEFT_THUMB">XINPUT_GAMEPAD_LEFT_THUMB</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_RIGHT_SHOULDER">XINPUT_GAMEPAD_RIGHT_SHOULDER</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_RIGHT_THUMB">XINPUT_GAMEPAD_RIGHT_THUMB</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_START">XINPUT_GAMEPAD_START</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_X">XINPUT_GAMEPAD_X</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XInput_XINPUT_GAMEPAD_Y">XINPUT_GAMEPAD_Y</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XInput_XINPUT_GAMEPAD"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XInput.</span><span class="api-item-title-strong">XINPUT_GAMEPAD</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>wButtons</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td>bLeftTrigger</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td>bRightTrigger</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td>sThumbLX</td>
<td class="code-type"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td>sThumbLY</td>
<td class="code-type"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td>sThumbRX</td>
<td class="code-type"><span class="STpe">s16</span></td>
<td></td>
</tr>
<tr>
<td>sThumbRY</td>
<td class="code-type"><span class="STpe">s16</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XInput_XINPUT_STATE"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XInput.</span><span class="api-item-title-strong">XINPUT_STATE</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>dwPacketNumber</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>Gamepad</td>
<td class="code-type"><span class="SCde"><span class="SCst">XInput</span>.<span class="SCst"><a href="#XInput_XINPUT_GAMEPAD">XINPUT_GAMEPAD</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XInput_XINPUT_VIBRATION"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XInput.</span><span class="api-item-title-strong">XINPUT_VIBRATION</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>wLeftMotorSpeed</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td>wRightMotorSpeed</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XInput_XInputGetState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">XInput.</span><span class="api-item-title-strong">XInputGetState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">XInputGetState</span>(dwUserIndex: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>, pState: *<span class="SCst">XInput</span>.<span class="SCst"><a href="#XInput_XINPUT_STATE">XINPUT_STATE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">DWORD</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XInput_XInputSetState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">XInput.</span><span class="api-item-title-strong">XInputSetState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xinput\src\xinput.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">XInputSetState</span>(dwUserIndex: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>, pVibration: *<span class="SCst">XInput</span>.<span class="SCst"><a href="#XInput_XINPUT_VIBRATION">XINPUT_VIBRATION</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">DWORD</span></span></code>
</div>
</div>
</div>
</div>
</body>
</html>
