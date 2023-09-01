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
        
        .code-inline            { font-size: 110%; font-family: monospace; display: inline-block; background-color: #eeeeee; padding: 0px 8px; border-radius: 5px; border: 1px dotted #cccccc; }
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
</head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="right">
<div class="right-page">
<div class="blockquote-warning">
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</div>
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
        <p>Swag is a system programming language made for fun because, let's be honest, C++ is now an horrible and ugly beast !</p>
        This is my third compiler (the other ones were developed for AAA game engines), but that one is by far the most advanced.
    </div>
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Native or interpreted</h2>
        <p>The Swag compiler can generate fast <b>native</b> code, or act as an interpreter for a <b>scripting</b> language.</p>
        Imagine C++, but where everything could be <i>constexpr</i>.
    </div>
    <div style="flex:200px; padding-left:30px; padding-right:30px;">
        <h2>Modern</h2>
        <p>Swag has <b>type reflection</b> at both runtime and compile time, <b>meta programmation</b>, <b>generics</b>, a powerful <b>macro system</b>...</p>
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
</div>

<div align="center">
    <p>To take a look at an explained implementation of the <b>Flappy Bird</b> game in one single little script file.</p>
    <div class="round-button">
        <a href="flappy.php" class="no-decoration">Flappy Bird</a>
    </div>
</div></p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">loadAssets</span>(wnd: *<span class="SCst">Wnd</span>) <span class="SKwd">throw</span>
{
    <span class="SKwd">let</span> render = &wnd.<span class="SFct">getApp</span>().renderer

    <span class="SKwd">var</span> dataPath: <span class="SCst">String</span> = <span class="SCst">Path</span>.<span class="SFct">getDirectoryName</span>(<span class="SCmp">#location</span>.fileName)
    dataPath = <span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"datas"</span>)
    dataPath = <span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"flappy"</span>)

    g_BirdTexture[<span class="SNum">0</span>] = render.<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"yellowbird-upflap.png"</span>))
    g_BirdTexture[<span class="SNum">1</span>] = render.<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"yellowbird-midflap.png"</span>))
    g_BirdTexture[<span class="SNum">2</span>] = render.<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"yellowbird-downflap.png"</span>))
    g_OverTexture    = render.<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"gameover.png"</span>))
    g_BaseTexture    = render.<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"base.png"</span>))
    g_BackTexture    = render.<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"background-day.png"</span>))
    g_MsgTexture     = render.<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"message.png"</span>))

    <span class="SKwd">var</span> img = <span class="SCst">Image</span>.<span class="SFct">load</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"pipe-green.png"</span>))
    g_PipeTextureD = render.<span class="SFct">addImage</span>(img)
    img.<span class="SFct">flip</span>()
    g_PipeTextureU = render.<span class="SFct">addImage</span>(img)

    g_Font = <span class="SCst">Font</span>.<span class="SFct">create</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"FlappyBirdy.ttf"</span>), <span class="SNum">50</span>)
}</span></code>
</div>
<div class="blockquote-warning">
<div class="blockquote-title-block"><i class="fa fa-exclamation-triangle"></i> <span class="blockquote-title">Warning</span></div><p> There's <b>no official</b> release yet. </p>
<p> Everything (including this web site) is still under construction... </p>
</div>
</div>
</div>
</div>
</body>
</html>
