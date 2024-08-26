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
        
    html { font-family: ui-sans-serif, system-ui, "Segoe UI", Roboto, "Helvetica Neue", Arial, "Noto Sans", sans-serif; }
    body { margin: 0px; line-height: 1.3em; }     

    .container a        { color: DoggerBlue; }
    .container a:hover  { text-decoration: underline; }
    .container img      { margin: 0 auto; }
   
    .left a     { text-decoration: none; }
    .left ul    { list-style-type: none; margin-left: -20px; }
    .left h3    { background-color: Black; color: White; padding: 6px; }
    .right h1   { margin-top: 50px; margin-bottom: 50px; }
    .right h2   { margin-top: 35px; }

    .right ol li { margin-bottom: 10px; }

    .strikethrough-text { text-decoration: line-through; }
    .swag-watermark     { text-align:right; font-size: 80%; margin-top: 30px; }
    .swag-watermark a   { text-decoration: none; color: inherit; }

    .blockquote               { border-radius: 5px; border: 1px solid; margin: 20px; padding: 10px; }
    .blockquote-default       { border-color: Orange; border-left: 6px solid Orange; background-color: LightYellow; }
    .blockquote-note          { border-color: #ADCEDD; background-color: #CDEEFD; }
    .blockquote-tip           { border-color: #BCCFBC; background-color: #DCEFDC; }
    .blockquote-warning       { border-color: #DFBDB3; background-color: #FFDDD3; }
    .blockquote-attention     { border-color: #DDBAB8; background-color: #FDDAD8; }
    .blockquote-example       { border: 2px solid LightGrey; }
    .blockquote-title-block   { margin-bottom: 10px; }
    .blockquote-title         { font-weight: bold; }
    .blockquote-default       p:first-child { margin-top: 0px; }
    .blockquote-default       p:last-child  { margin-bottom: 0px; }
    .blockquote               p:last-child  { margin-bottom: 0px; }

    .description-list-title   { font-weight: bold; font-style: italic; }
    .description-list-block   { margin-left: 30px; }

    .container table          { border: 1px solid LightGrey; border-collapse: collapse; font-size: 90%; margin-left: 20px; margin-right: 20px; }
    .container td             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; }
    .container th             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; background-color: #eeeeee; }

    table.api-item            { border-collapse: separate; background-color: Black; color: White; width: 100%; margin-top: 70px; margin-right: 0px; font-size: 110%; }
    .api-item td              { font-size: revert; border: 0; }
    .api-item td:first-child  { width: 33%; white-space: nowrap; }
    .api-item-title-src-ref   { text-align:  right; }
    .api-item-title-src-ref a { color:       inherit; }
    .api-item-title-kind      { font-weight: normal; font-size: 80%; }
    .api-item-title-light     { font-weight: normal; }
    .api-item-title-strong    { font-weight: bold; font-size: 100%; }
    .api-additional-infos     { font-size: 90%; white-space: break-spaces; overflow-wrap: break-word; }

    table.table-enumeration           { width: calc(100% - 40px); }
    .table-enumeration td:first-child { background-color: #f8f8f8; white-space: nowrap; }
    .table-enumeration td:last-child  { width: 100%; }
    .table-enumeration td.code-type   { background-color: #eeeeee; }
    .table-enumeration a              { text-decoration: none; color: inherit; }

    .code-inline  { background-color: #eeeeee; border-radius: 5px; border: 1px dotted #cccccc; padding: 0px 8px; font-size: 110%; font-family: monospace; display: inline-block; }
    .code-block   { background-color: #eeeeee; border-radius: 5px; border: 1px solid LightGrey; padding: 10px; margin: 20px; white-space: pre; overflow-x: auto; }
    .code-block   { font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace; }
    .code-block a { color: inherit; }

    .blockquote-default     .code-inline    { background-color: #FFE89C; }
    .blockquote-note        .code-inline    { border-color: #9DBECD; background-color: #BDDEED; }
    .blockquote-tip         .code-inline    { border-color: #ACBFAC; background-color: #CCDFCC; }
    .blockquote-warning     .code-inline    { border-color: #CFADA3; background-color: #EFCDC3; }
    .blockquote-attention   .code-inline    { border-color: #CDAAA8; background-color: #EDCAC8; }
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
    .SBcR { color: #b5af49; }
    .SInv { color: #ff0000; }
</style>
<?php include('common/end-head.php'); ?>
</head>
<body>
<?php include('common/start-body.php'); ?>
<div class="container">
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
<h1>Module xinput</h1>
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
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">WORD</span></span></td>
<td></td>
</tr>
<tr>
<td>bLeftTrigger</td>
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">BYTE</span></span></td>
<td></td>
</tr>
<tr>
<td>bRightTrigger</td>
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">BYTE</span></span></td>
<td></td>
</tr>
<tr>
<td>sThumbLX</td>
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">SHORT</span></span></td>
<td></td>
</tr>
<tr>
<td>sThumbLY</td>
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">SHORT</span></span></td>
<td></td>
</tr>
<tr>
<td>sThumbRX</td>
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">SHORT</span></span></td>
<td></td>
</tr>
<tr>
<td>sThumbRY</td>
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">SHORT</span></span></td>
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
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">DWORD</span></span></td>
<td></td>
</tr>
<tr>
<td>Gamepad</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#XInput_XINPUT_GAMEPAD">XINPUT_GAMEPAD</a></span></span></td>
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
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">WORD</span></span></td>
<td></td>
</tr>
<tr>
<td>wRightMotorSpeed</td>
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">WORD</span></span></td>
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
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SCst">XInputGetState</span>(dwUserIndex: <span class="SCst">DWORD</span>, pState: *<span class="SCst"><a href="#XInput_XINPUT_STATE">XINPUT_STATE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">DWORD</span></span></div>
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
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SCst">XInputSetState</span>(dwUserIndex: <span class="SCst">DWORD</span>, pVibration: *<span class="SCst"><a href="#XInput_XINPUT_VIBRATION">XINPUT_VIBRATION</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">DWORD</span></span></div>
<div class="swag-watermark">
Generated on 26-08-2024 with <a href="https://swag-lang.org/index.php">swag</a> 0.38.0</div>
</div>
</div>
</div>

    <script> 
		function getOffsetTop(element, parent) {
			let offsetTop = 0;
			while (element && element != parent) {
				offsetTop += element.offsetTop;
				element = element.offsetParent;
			}
			return offsetTop;
		}	
		document.addEventListener("DOMContentLoaded", function() {
			let hash = window.location.hash;
			if (hash)
			{
				let parentScrollable = document.querySelector('.right');
				if (parentScrollable)
				{
					let targetElement = parentScrollable.querySelector(hash);
					if (targetElement)
					{
						parentScrollable.scrollTop = getOffsetTop(targetElement, parentScrollable);
					}
				}
			}
        });
    </script>
</body>
</html>
