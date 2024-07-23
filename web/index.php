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
<p style="white-space: break-spaces"><div style="background-color:Black; border-radius: 5px;" align="center">
    <p style="padding-top:30px;"> <img src="imgs/swag_logo.png" width=50%> </p>
    <p style="color:white;font-size:40px;line-height:1.3em;"> <b>SWAG</b> Programming Language </p>
    <div style="margin-top:50px; padding-bottom:20px; color:White; font-size:20px;">
        <div class="round-button"><a href="getting-started.php" style="color:#F7F900; text-decoration:none;">Getting Started</a></div>
        <div class="round-button"><a href="https://github.com/swag-lang/swag/releases" style="color:#F7F900; text-decoration:none;">Download the Compiler</a> </div>
    </div>
</div>

<div style="display:flex;flex-wrap:wrap;margin-bottom:30px;">
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>A sophisticated toy</h2>
        <p>Swag is a <b>system</b> programming language made for fun because, let's be honest, C++ is now an horrible and ugly beast !</p>
        This is my third compiler (the other ones were developed for AAA <b>game engines</b>), but that one is by far the most advanced.
    </div>
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Native or interpreted</h2>
        <p>The Swag compiler can generate fast <b>native</b> code with a custom x64 backend or with LLVM.
        It can also act as an interpreter for a <b>scripting</b> language.</p>
        Imagine C++, but where everything could be <i>constexpr</i>.
    </div>
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Modern</h2>
        <p>Swag has full <b>compile-time</b> evaluation, <b>type reflection</b> at both runtime and compile time, <b>meta programming</b>, <b>generics</b>, a powerful <b>macro system</b>...</p>
        Enjoy.
    </div>
</div>

<div align="center">
    <div class="round-button">
        <a href="language.php" class="no-decoration">Documentation</a>
    </div>
    <div class="round-button">
        <a href="swag-as-script.php" class="no-decoration">Scripting</a>
    </div>
    <div class="round-button">
        <a href="std.php">Standard Modules</a>
    </div>
</div></p>
<p style="white-space: break-spaces"><div align="center">
    <p>To take a look at an explained implementation of the <b>Flappy Bird</b> game in one single little script file.</p>
    <div class="round-button">
        <a href="flappy.php" class="no-decoration">Flappy Bird</a>
    </div>
</div></p>
<div class="blockquote blockquote-warning">
<div class="blockquote-title-block"><i class="fa fa-exclamation-triangle"></i>  <span class="blockquote-title">Warning</span></div><p> We haven't officially launched anything yet! Everything, even this website, is still a <b>work in progress</b>. So, the rules of the game can change anytime until we hit version 1.0.0. Let's keep the fun going! </p>
</div>
<div class="code-block"><span class="SCde"><span class="SCmt">// The 'IsSet' generic struct is generated as a mirror of the user input struct.</span>
<span class="SCmt">// Each field has the same name as the original one, but with a 'bool' type.</span>
<span class="SCmt">// It will indicate, after the command line parsing, that the corresponding argument has been</span>
<span class="SCmt">// specified or not by the user.</span>
<span class="SKwd">struct</span>(<span class="SCst">T</span>) <span class="SCst">IsSet</span>
{
    <span class="SFct">#ast</span>
    {
        <span class="SKwd">var</span> str = <span class="SCst">StrConv</span>.<span class="SCst">StringBuilder</span>{}
        <span class="SKwd">let</span> typeof = <span class="SItr">@typeof</span>(<span class="SCst">T</span>)
        <span class="SLgc">visit</span> f: typeof.fields
            str.<span class="SFct">appendFormat</span>(<span class="SStr">"%: bool\n"</span>, f.name)
        <span class="SLgc">return</span> str.<span class="SFct">toString</span>()
    }
}</span></div>
<div class="code-block"><span class="SCde"><span class="SFct">#test</span>
{
    <span class="SKwd">const</span> <span class="SCst">N</span> = <span class="SNum">4</span>
    <span class="SKwd">const</span> <span class="SCst">PowerOfTwo</span>: [<span class="SCst">N</span>] <span class="STpe">s32</span> = <span class="SFct">#run</span>
        {
            <span class="SKwd">var</span> arr: [<span class="SCst">N</span>] <span class="STpe">s32</span>
            <span class="SLgc">loop</span> i: arr
                arr[i] = <span class="SNum">1</span> &lt;&lt; <span class="SKwd">cast</span>(<span class="STpe">u32</span>) i
            <span class="SLgc">return</span> arr
        }

    <span class="SCmp">#assert</span> <span class="SCst">PowerOfTwo</span>[<span class="SNum">0</span>] == <span class="SNum">1</span>
    <span class="SCmp">#assert</span> <span class="SCst">PowerOfTwo</span>[<span class="SNum">1</span>] == <span class="SNum">2</span>
    <span class="SCmp">#assert</span> <span class="SCst">PowerOfTwo</span>[<span class="SNum">2</span>] == <span class="SNum">4</span>
    <span class="SCmp">#assert</span> <span class="SCst">PowerOfTwo</span>[<span class="SNum">3</span>] == <span class="SNum">8</span>
}</span></div>
<p>Swag is <b>open source</b> and released under the <a href="https://github.com/swag-lang/swag/blob/master/LICENCE">MIT license</a>. You will find the compiler source code on <a href="https://github.com/swag-lang/swag">GitHub</a>. You can also visit the <a href="https://www.youtube.com/channel/UC9dkBu1nNfJDxUML7r7QH1Q">YouTube</a> channel to see some little coding sessions. </p>
<div class="blockquote blockquote-warning">
<div class="blockquote-title-block"><i class="fa fa-exclamation-triangle"></i>  <span class="blockquote-title">Warning</span></div><p> The language evolves, and those videos can show out of date syntax. But the corresponding scripts on GitHub are always up to date and running. </p>
</div>
<p style="white-space: break-spaces"><div style="display:flex; flex-wrap:wrap; ">
    <iframe style="width:200px; height:200px; flex:200px; padding:10px;" src="https://www.youtube.com/embed/Il0UuJCXTWI" title="Swag Live Coding - The Flappy Bird Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="width:200px; height:200px; flex:200px; padding:10px;" src="https://www.youtube.com/embed/Bqr1pakewaU" title="Swag Live Coding - The Pacman Game (silent)" frameborder="0" allowfullscreen></iframe>
    <iframe style="width:200px; height:200px; flex:200px; padding:10px;" src="https://www.youtube.com/embed/f2rIXoH6H38" title="Swag Live Coding: The 2048 Game (silent)" frameborder="0" allowfullscreen></iframe>
</div></p>
<div class="blockquote blockquote-note">
<div class="blockquote-title-block"><i class="fa fa-info-circle"></i>  <span class="blockquote-title">Note</span></div><p> The pages of this web site and all the related documentations have been generated with Swag, as the compiler can also produce HTML based on markdown files and source code. </p>
</div>
<div class="swag-watermark">
Generated on 23-07-2024 with <a href="https://swag-lang.org/index.php">swag</a> 0.37.0</div>
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
