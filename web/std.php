<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Swag</title>
<link rel="icon" type="image/x-icon" href="favicon.ico">
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

    .right hr   { margin-top: 50px; margin-bottom: 50px; }

    .right h1 { font-size: 2em; }
    .right h2 { font-size: 1.75em; }
    .right h3 { font-size: 1.5em; }

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

</head>
<body>
<?php include('common/start-body.php'); ?>
<div class="container">
<div class="right">
<div class="right-page">
<p>This is the list of all modules that come with the compiler. As they are always in sync, they are considered as <b>standard</b>. They are all part of the same workspace <b>std</b>. </p>
<p>You can find that workspace locally in <span class="code-inline">bin/std</span>, or <a href="https://github.com/swag-lang/swag/tree/master/bin/std">here</a> on GitHub. </p>
<h1 id="">Modules </h1>
<table class="table-markdown">
<tr><td> <a href="std.core.php">std.core</a>         </td><td> Main core module, the base of everything else</td></tr>
<tr><td> <a href="std.pixel.php">std.pixel</a>       </td><td> An image and a 2D painting module</td></tr>
<tr><td> <a href="std.gui.php">std.gui</a>           </td><td> A user interface module (windows, widgets...)</td></tr>
<tr><td> <a href="std.audio.php">std.audio</a>       </td><td> An audio module to decode and play sounds</td></tr>
<tr><td> <a href="std.libc.php">std.libc</a>         </td><td> Libc wrapper</td></tr>
</table>
<h1 id="">Wrappers </h1>
<p>Those other modules are just wrappers to external libraries. </p>
<table class="table-markdown">
<tr><td> <a href="std.ogl.php">std.ogl</a>           </td><td> Opengl wrapper</td></tr>
<tr><td> <a href="std.freetype.php">std.freetype</a> </td><td> Freetype wrapper</td></tr>
<tr><td> <a href="std.win32.php">std.win32</a>       </td><td> Windows <span class="code-inline">win32</span> wrapper (kernel32, user32...)</td></tr>
<tr><td> <a href="std.gdi32.php">std.gdi32</a>       </td><td> Windows <span class="code-inline">gdi32</span> wrapper</td></tr>
<tr><td> <a href="std.xinput.php">std.xinput</a>     </td><td> Windows 'direct X input' wrapper</td></tr>
<tr><td> <a href="std.xaudio2.php">std.xaudio2</a>   </td><td> <span class="code-inline">xaudio2</span> wrapper</td></tr>
</table>
<div class="swag-watermark">
Generated on 01-05-2025 with <a href="https://swag-lang.org/index.php">swag</a> 0.44.0</div>
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
