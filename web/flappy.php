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
<p><img src="imgs/flappy.png" alt=""> </p>
<p>Here is a very simple script that implements the <a href="https://en.wikipedia.org/wiki/Flappy_Bird">Flappy Bird</a> game. To have some fun and play with it, go to the <span class="code-inline">bin/examples/scripts</span> folder, and type : </p>
<div class="code-block"><code><span class="SCde">$ swag flappy.swgs</span></code>
</div>
<p>The goal here is to comment a real life example of swag usage. But it's better to read the basic language <a href="language.php">documentation</a> first, as we will not going too deep into the details. </p>
<div class="blockquote blockquote-note">
<div class="blockquote-title-block"><i class="fa fa-info-circle"></i>  <span class="blockquote-title">Note</span></div><p> This page has been generated with Swag directly from the <a href="https://github.com/swag-lang/swag/blob/master/bin/examples/scripts/flappy.swgs">source code</a>. </p>
</div>
<p>So, let's begin. </p>
<h1 id="Dependencies">Dependencies </h1>
<p>Normally, you'd put the <span class="code-inline">#dependency</span> block in the <span class="code-inline">module.swg</span> file of a module. But if it's a script and there's no <span class="code-inline">module.swg</span>, you just put it at the top of the script file. </p>
<p>This special <span class="code-inline">#dependencies</span> compiler block is used to specify : </p>
<div class="description-list-title"><p>External dependencies</p></div>
<div class="description-list-block">
<p>Dependencies are other modules you depend on. For example for Flappy, we will use the <span class="code-inline">gui</span> module. </p>
</div>
<div class="description-list-title"><p>Additional files</p></div>
<div class="description-list-block">
<p>In case of scripts, you can add more files to compile with the <span class="code-inline">#load</span> directive. </p>
</div>
<div class="description-list-title"><p>Module configuration</p></div>
<div class="description-list-block">
<p>If present, a special <span class="code-inline">#run</span> block will be executed by the compiler at the very beginning of the compilation stage. It gives the opportunity to change some build configurations. </p>
</div>
<p>So in our case, we need to import the module <span class="code-inline">gui</span>. This module is used to create windows and widgets, and will bring other modules like <span class="code-inline">core</span> and <span class="code-inline">pixel</span> (2D painting). </p>
<div class="code-block"><code><span class="SCde"><span class="SFct">#dependencies</span>
{
    <span class="SCmt">// The location "swag@std" tells swag that 'gui' is a standard module that is located</span>
    <span class="SCmt">// with the compiler.</span>
    <span class="SCmp">#import</span> <span class="SStr">"gui"</span> location=<span class="SStr">"swag@std"</span>

    <span class="SCmt">// Import the audio module, from the same default location</span>
    <span class="SCmp">#import</span> <span class="SStr">"audio"</span> location=<span class="SStr">"swag@std"</span>

    <span class="SCmt">// This is the optional '#run' block executed by the compiler before compiling the script itself.</span>
    <span class="SCmt">// We define it just to make the point because Flappy does not really need one.</span>
    <span class="SFct">#run</span>
    {
        <span class="SCmt">// Get the compiler interface to communicate with the compiler.</span>
        <span class="SKwd">let</span> itf = <span class="SItr">@compiler</span>()

        <span class="SCmt">// Get the build configuration</span>
        <span class="SKwd">let</span> cfg = itf.<span class="SFct">getBuildCfg</span>()

        <span class="SCmt">// Change something...</span>
        <span class="SCmt">// Here, for example, we force all safety guards to be present in 'debug', and we remove all</span>
        <span class="SCmt">// of them in 'release'.</span>
        <span class="SCmp">#if</span> <span class="SCmp">#cfg</span> == <span class="SStr">"debug"</span>
            cfg.safetyGuards = <span class="SCst">Swag</span>.<span class="SCst">SafetyAll</span>
        <span class="SCmp">#else</span>
            cfg.safetyGuards = <span class="SCst">Swag</span>.<span class="SCst">SafetyNone</span>
    }
}</span></code>
</div>
<p>Every module has its individual namespace. To avoid the necessity of mentioning it each time we wish to reference something, we include a global <span class="code-inline">using</span> statement immediately after the <span class="code-inline">#dependency</span> block. </p>
<p>The <span class="code-inline">gui</span> module depends on <span class="code-inline">pixel</span> which depends on <span class="code-inline">core</span>. So we bring all the three namespaces into the file scope. Note that we keep <span class="code-inline">Audio</span> as it is. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">using</span> <span class="SCst">Core</span>, <span class="SCst">Pixel</span>, <span class="SCst">Gui</span></span></code>
</div>
<h1 id="Entry_point">Entry point </h1>
<p>The compiler's <span class="code-inline">#run</span> function serves as the initial execution point for the script. This category of block is executed by the compiler while it's compiling. While it's possible to include multiple <span class="code-inline">#run</span> blocks, a single one is sufficient for the Flappy application. </p>
<div class="blockquote blockquote-note">
<div class="blockquote-title-block"><i class="fa fa-info-circle"></i>  <span class="blockquote-title">Note</span></div><p> You might observe that the arrangement of global declarations doesn't make a difference, as we're using the <span class="code-inline">onEvent</span> function before even defining it. Swag does not bother about the global declaration order. </p>
</div>
<div class="code-block"><code><span class="SCde"><span class="SFct">#run</span>
{
    <span class="SCmt">// Creates audio engine</span>
    <span class="SKwd">assume</span> <span class="SCst">Audio</span>.<span class="SFct">createEngine</span>()

    <span class="SCmt">// Thanks to 'defer', the audio engine will be destroyed when</span>
    <span class="SCmt">// leaving this scope, i.e. when exiting the '#run' function.</span>
    <span class="SLgc">defer</span> <span class="SCst">Audio</span>.<span class="SFct">destroyEngine</span>()

    <span class="SCmt">// From the command line, if the script is run with '--arg:swag.test', then we force the application</span>
    <span class="SCmt">// to exit after 100 frames. This is usefull for batch testing.</span>
    <span class="SKwd">func</span> <span class="SFct">test</span>(app: *<span class="SCst">Application</span>) = <span class="SLgc">if</span> <span class="SCst">Env</span>.<span class="SFct">hasArg</span>(<span class="SStr">"swag.test"</span>) app.maxRunFrame = <span class="SNum">100</span>

    <span class="SCmt">// Creates and run one surface (i.e. window) at the given position and with the given size and title.</span>
    <span class="SCmt">// 'hook' defines a lambda that will receive and treat all gui events</span>
    <span class="SCmt">// 'init' defines a lambda that will be called for surface initialization</span>
    <span class="SCst">Application</span>.<span class="SFct">runSurface</span>(<span class="SNum">4100</span>, <span class="SNum">100</span>, <span class="SNum">300</span>, <span class="SNum">512</span>, title: <span class="SStr">"Flappy Bird"</span>, hook: &onEvent, init: &test)
}</span></code>
</div>
<h1 id="Global_definitions">Global definitions </h1>
<h2 id="Constants">Constants </h2>
<p>We declare global constants with <span class="code-inline">const</span>. Note that we do not specify types for thoses constants. They will be deduced thanks to the affection. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">const</span> <span class="SCst">Gravity</span>      = <span class="SNum">2.5</span>        <span class="SCmt">// 2.5 is a 32 bits float, so the type of Gravity is 'f32'</span>
<span class="SKwd">const</span> <span class="SCst">GroundHeight</span> = <span class="SNum">40.0</span>
<span class="SKwd">const</span> <span class="SCst">SpeedHorz</span>    = <span class="SNum">100.0</span>
<span class="SKwd">const</span> <span class="SCst">BirdImpulseY</span> = <span class="SNum">350</span>        <span class="SCmt">// 350 is an integer, so the type of BirdImpulseY is 's32'</span></span></code>
</div>
<h2 id="Variables">Variables </h2>
<div class="code-block"><code><span class="SCde"><span class="SKwd">var</span> g_Bird:      <span class="SCst">Bird</span>
<span class="SKwd">var</span> g_Pipes:     <span class="SCst">Array</span>'<span class="SCst">Pipe</span></span></code>
</div>
<p><span class="code-inline">g_Pipes</span> is a <b>dynamic</b> and <b>generic array</b> where all the elements are of type <span class="code-inline">Pipe</span>. In other languages, you would write <span class="code-inline">Array&lt;Pipe&gt;</span>. <span class="code-inline">Array</span> comes from the <span class="code-inline">Core</span> module. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">var</span> g_Rect:      <span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></code>
</div>
<p><span class="code-inline">Math</span> is a namespace part of the <span class="code-inline">Core</span> module. We could have specified a global <span class="code-inline">using Core.Math</span> at the top of the script file, but here, we prefer the explicit reference. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">var</span> g_Dt:        <span class="STpe">f32</span>
<span class="SKwd">var</span> g_Time:      <span class="STpe">f32</span>
<span class="SKwd">var</span> g_BasePos:   <span class="STpe">f32</span>
<span class="SKwd">var</span> g_Score:     <span class="STpe">s32</span>
<span class="SKwd">var</span> g_GameOver:  <span class="STpe">bool</span>
<span class="SKwd">var</span> g_Start:     <span class="STpe">bool</span>
<span class="SKwd">var</span> g_FirstStart = <span class="SKwd">true</span></span></code>
</div>
<p>Again, no need to specify the type of <span class="code-inline">g_FirstStart</span>. It is deduced from the affectation to be <span class="code-inline">bool</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SCmt">// Texture assets</span>
<span class="SKwd">var</span> g_DigitTexture: [<span class="SNum">10</span>] <span class="SCst">Texture</span>    <span class="SCmt">// A static array of 10 textures</span>
<span class="SKwd">var</span> g_BirdTexture:  [<span class="SNum">3</span>] <span class="SCst">Texture</span>     <span class="SCmt">// A static array of 3 textures</span>
<span class="SKwd">var</span> g_BackTexture:  <span class="SCst">Texture</span>
<span class="SKwd">var</span> g_OverTexture:  <span class="SCst">Texture</span>
<span class="SKwd">var</span> g_BaseTexture:  <span class="SCst">Texture</span>
<span class="SKwd">var</span> g_PipeTextureU: <span class="SCst">Texture</span>
<span class="SKwd">var</span> g_PipeTextureD: <span class="SCst">Texture</span>
<span class="SKwd">var</span> g_MsgTexture:   <span class="SCst">Texture</span>

<span class="SCmt">// Sound assets</span>
<span class="SKwd">var</span> g_SoundDie:     <span class="SCst">Audio</span>.<span class="SCst">SoundFile</span>
<span class="SKwd">var</span> g_SoundScore:   <span class="SCst">Audio</span>.<span class="SCst">SoundFile</span>
<span class="SKwd">var</span> g_SoundHit:     <span class="SCst">Audio</span>.<span class="SCst">SoundFile</span>
<span class="SKwd">var</span> g_SoundFlap:    <span class="SCst">Audio</span>.<span class="SCst">SoundFile</span></span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">var</span> g_Font: *<span class="SCst">Font</span></span></code>
</div>
<p><span class="code-inline">g_Font</span> is a <b>value</b> pointer to the score font. </p>
<p>In Swag, their are two types of pointers. </p>
<ul>
<li><span class="code-inline">value</span> pointers, which points to one single value. Declared with <span class="code-inline">*</span></li>
<li><span class="code-inline">range</span> pointers, which points to multiple values. Declared with <span class="code-inline">^</span>.</li>
</ul>
<p>Pointer arithmetic is not enabled on value pointers, but it is on range pointers. </p>
<h2 id="Types">Types </h2>
<div class="code-block"><code><span class="SCde"><span class="SCmt">// Defines the Bird</span>
<span class="SKwd">struct</span> <span class="SCst">Bird</span>
{
    <span class="SCmt">// Position of the bird. Full qualified name.</span>
    pos:        <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span>

    <span class="SCmt">// Speed of the bird. No need to specify 'Core' thanks to the global 'using'.</span>
    speed:      <span class="SCst">Math</span>.<span class="SCst">Vector2</span>

    <span class="SCmt">// Sprite frame</span>
    frame:      <span class="STpe">f32</span>
}

<span class="SCmt">// Defines one Pipe</span>
<span class="SKwd">struct</span> <span class="SCst">Pipe</span>
{
    rectUp:     <span class="SCst">Math</span>.<span class="SCst">Rectangle</span>  <span class="SCmt">// Position of the up part of the Pipe</span>
    rectDown:   <span class="SCst">Math</span>.<span class="SCst">Rectangle</span>  <span class="SCmt">// Position of the down part of the Pipe</span>
    distToNext: <span class="STpe">f32</span>             <span class="SCmt">// Distance to the next Pipe</span>
    scored:     <span class="STpe">bool</span>            <span class="SCmt">// 'true' if the Bird has passed that Pipe</span>
}</span></code>
</div>
<h1 id="The_actual_code">The actual code </h1>
<p>This is the callback that will deal with all gui events. This feels like <i>Windows</i> API, but their are other ways of dealing with gui, in a more 'object like' way. You can look at the <span class="code-inline">captme</span> tool for example, which does not use a callback but <b>interfaces</b> instead. </p>
<p>For a simple script, this is more simple to process events that way. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">onEvent</span>(wnd: *<span class="SCst">Wnd</span>, evt: *<span class="SCst">Event</span>)-&gt;<span class="STpe">bool</span>
{
    <span class="SLgc">switch</span> evt.kind
    {
    <span class="SCmt">// Called once for the surface. Can be used to initialize some stuff.</span>
    <span class="SLgc">case</span> <span class="SCst">Create</span>:
        g_Rect = wnd.<span class="SFct">getClientRect</span>()

        <span class="SCmt">// 'loadAssets' can raise some errors (we'll see later). So here we 'assume' that</span>
        <span class="SCmt">// everyting will be fine. If an error is raised, a 'panic' will occur.</span>
        <span class="SKwd">assume</span> <span class="SFct">loadAssets</span>(wnd)

        <span class="SFct">start</span>()

    <span class="SCmt">// Each time the surface (i.e. window) is resized</span>
    <span class="SLgc">case</span> <span class="SCst">Resize</span>:
        g_Rect = wnd.<span class="SFct">getClientRect</span>()

    <span class="SCmt">// Each time the surface needs to be repainted</span>
    <span class="SLgc">case</span> <span class="SCst">Paint</span>:
        <span class="SKwd">let</span> paintEvt = <span class="SKwd">cast</span>(*<span class="SCst">PaintEvent</span>) evt
        <span class="SKwd">let</span> painter = paintEvt.bc.painter

        <span class="SCmt">// This is the elapsed time between two 'frames', in seconds.</span>
        g_Dt = wnd.<span class="SFct">getApp</span>().<span class="SFct">getDt</span>()

        <span class="SFct">input</span>(wnd)          <span class="SCmt">// IO (keyword test)</span>
        <span class="SFct">paint</span>(painter)      <span class="SCmt">// Paint game</span>
        <span class="SFct">move</span>()              <span class="SCmt">// Update game</span>

        <span class="SCmt">// We force the window to be dirty, which means that the 'Paint' event will</span>
        <span class="SCmt">// be sent again next frame.</span>
        <span class="SCmt">// That way we have a cheap 'frame' update.</span>
        wnd.<span class="SFct">invalidate</span>()
    }

    <span class="SLgc">return</span> <span class="SKwd">false</span>
}</span></code>
</div>
<p>Paint function. Here we are mostly using functions from the <span class="code-inline">Pixel</span> module. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">paint</span>(painter: *<span class="SCst">Painter</span>)
{
    <span class="SCmt">// Draw the background texture at position (0, 0). The size of the texture is preserved.</span>
    painter.<span class="SFct">drawTexture</span>(<span class="SNum">0</span>, <span class="SNum">0</span>, g_BackTexture)

    <span class="SCmt">// Bird</span>
    <span class="SLgc">if</span> !g_FirstStart
    {
        <span class="SCmt">// We save the state of the painter, in order to restore it at the end of the scope.</span>
        <span class="SCmt">// The state contains various painting parameters, like the current transformation.</span>
        painter.<span class="SFct">pushState</span>()
        <span class="SLgc">defer</span> painter.<span class="SFct">popState</span>()

        <span class="SKwd">var</span> trf = painter.<span class="SFct">getTransform</span>()

        painter.<span class="SFct">resetTransform</span>()
        <span class="SKwd">let</span> speed = <span class="SCst">Math</span>.<span class="SFct">clamp</span>(g_Bird.speed.y, -<span class="SNum">200</span>, <span class="SNum">200</span>)
        <span class="SKwd">let</span> angle = <span class="SCst">Math</span>.<span class="SFct">map</span>(speed, -<span class="SNum">200</span>, <span class="SNum">200</span>, -<span class="SCst">Math</span>.<span class="SCst">ConstF32</span>.<span class="SCst">PiBy6</span>, <span class="SCst">Math</span>.<span class="SCst">ConstF32</span>.<span class="SCst">PiBy6</span>)
        painter.<span class="SFct">rotateTransform</span>(angle)

        painter.<span class="SFct">translateTransform</span>(g_Bird.pos.x + trf.tx, g_Bird.pos.y + trf.ty)

        <span class="SCmt">// This is the sprite frame of the bird</span>
        <span class="SKwd">let</span> frame = (<span class="SKwd">cast</span>(<span class="STpe">s32</span>) g_Bird.frame) % <span class="SNum">3</span>

        <span class="SCmt">// No need to initialize the 'pt' variable here, as we will set both 'x' and 'y'</span>
        <span class="SCmt">// just after. So we initialize it with 'undefined'.</span>
        <span class="SKwd">var</span> pt: <span class="SCst">Math</span>.<span class="SCst">Point</span> = <span class="SKwd">undefined</span>
        pt.x = -g_BirdTexture[frame].width * <span class="SNum">0.5</span>
        pt.y = -g_BirdTexture[frame].height * <span class="SNum">0.5</span>

        <span class="SCmt">// Add a little position wave</span>
        <span class="SLgc">if</span> !g_Start
        {
            pt.y += <span class="SNum">5</span> * <span class="SCst">Math</span>.<span class="SFct">sin</span>(g_Time)
            g_Time += <span class="SNum">5</span> * g_Dt
        }

        painter.<span class="SFct">drawTexture</span>(pt.x, pt.y, g_BirdTexture[frame])
    }

    <span class="SCmt">// Draw pipes</span>
    <span class="SLgc">visit</span> pipe: g_Pipes
    {
        painter.<span class="SFct">drawTexture</span>(pipe.rectUp, g_PipeTextureU)
        painter.<span class="SFct">drawTexture</span>(pipe.rectDown, g_PipeTextureD)
    }

    <span class="SCmt">// Base</span>
    painter.<span class="SFct">drawTexture</span>(-g_BasePos, g_Rect.<span class="SFct">bottom</span>() - <span class="SCst">GroundHeight</span>, <span class="SKwd">cast</span>(<span class="STpe">f32</span>) g_BaseTexture.width, <span class="SCst">GroundHeight</span>, g_BaseTexture)
    painter.<span class="SFct">drawTexture</span>(-g_BasePos + g_BaseTexture.width, g_Rect.<span class="SFct">bottom</span>() - <span class="SCst">GroundHeight</span>, <span class="SKwd">cast</span>(<span class="STpe">f32</span>) g_BaseTexture.width, <span class="SCst">GroundHeight</span>, g_BaseTexture)
    <span class="SLgc">if</span> !g_GameOver
        g_BasePos += <span class="SCst">SpeedHorz</span>*g_Dt
    <span class="SLgc">if</span> g_BasePos &gt;= g_BaseTexture.width
        g_BasePos = <span class="SNum">0</span>

    <span class="SCmt">// Gameover text, centered</span>
    <span class="SLgc">if</span> g_GameOver
    {
        <span class="SCmt">// This is another way of initializing a struct variable or constant.</span>
        <span class="SKwd">var</span> pt = <span class="SCst">Math</span>.<span class="SCst">Point</span>{
            g_Rect.<span class="SFct">horzCenter</span>() - g_OverTexture.width/<span class="SNum">2</span>,
            g_Rect.<span class="SFct">vertCenter</span>() - g_OverTexture.height/<span class="SNum">2</span>
        }

        painter.<span class="SFct">drawTexture</span>(pt.x, pt.y, g_OverTexture)
    }

    <span class="SCmt">// Score</span>
    <span class="SLgc">if</span> !g_FirstStart
    {
        painter.<span class="SFct">drawStringCenter</span>(g_Rect.<span class="SFct">horzCenter</span>(), <span class="SNum">50</span>, <span class="SCst">Format</span>.<span class="SFct">toString</span>(<span class="SStr">"%"</span>, g_Score), g_Font, <span class="SCst">Argb</span>.<span class="SCst">White</span>)
    }

    <span class="SCmt">// Message</span>
    <span class="SLgc">if</span> g_FirstStart
    {
        <span class="SKwd">var</span> pt: <span class="SCst">Math</span>.<span class="SCst">Point</span> = <span class="SKwd">undefined</span>
        pt.x = g_Rect.<span class="SFct">horzCenter</span>() - g_MsgTexture.width/<span class="SNum">2</span>
        pt.y = g_Rect.<span class="SFct">vertCenter</span>() - g_MsgTexture.height/<span class="SNum">2</span>
        painter.<span class="SFct">drawTexture</span>(pt.x, pt.y, g_MsgTexture)
    }
}</span></code>
</div>
<p>Test IO. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">input</span>(wnd: *<span class="SCst">Wnd</span>)
{
    <span class="SKwd">let</span> kb = wnd.<span class="SFct">getApp</span>().<span class="SFct">getKeyboard</span>()

    <span class="SLgc">if</span> g_GameOver <span class="SLgc">or</span> g_FirstStart
    {
        <span class="SLgc">if</span> kb.<span class="SFct">isKeyJustPressed</span>(.<span class="SCst">Space</span>)
        {
            <span class="SFct">start</span>()
            g_FirstStart = <span class="SKwd">false</span>
        }

        <span class="SLgc">return</span>
    }

    <span class="SLgc">if</span> kb.<span class="SFct">isKeyJustPressed</span>(.<span class="SCst">Up</span>)
    {
        g_Start = <span class="SKwd">true</span>
        g_Bird.speed.y = -<span class="SCst">BirdImpulseY</span>
        <span class="SKwd">assume</span> <span class="SCst">Audio</span>.<span class="SCst">Voice</span>.<span class="SFct">play</span>(&g_SoundFlap)
    }
}</span></code>
</div>
<p>Collision detection (kind of) between the bird and a given rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">birdInRect</span>(rect: <span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)-&gt;<span class="STpe">bool</span>
{
    <span class="SKwd">var</span> rectBird: <span class="SCst">Math</span>.<span class="SCst">Rectangle</span>
    rectBird.x = g_Bird.pos.x - g_BirdTexture[<span class="SNum">0</span>].width/<span class="SNum">2</span>
    rectBird.y = g_Bird.pos.y - g_BirdTexture[<span class="SNum">0</span>].height/<span class="SNum">2</span>
    rectBird.width = g_BirdTexture[<span class="SNum">0</span>].width
    rectBird.height = g_BirdTexture[<span class="SNum">0</span>].height
    <span class="SLgc">return</span> rect.<span class="SFct">intersectWith</span>(rectBird)
}</span></code>
</div>
<p>The <span class="code-inline">update</span> part of the game. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">move</span>()
{
    <span class="SLgc">if</span> g_GameOver
        <span class="SLgc">return</span>

    g_Bird.pos += g_Bird.speed * g_Dt
    g_Bird.pos.y = <span class="SCst">Math</span>.<span class="SFct">max</span>(g_Bird.pos.y, <span class="SNum">0</span>)
    <span class="SLgc">if</span> g_Start
        g_Bird.speed += {<span class="SNum">0</span>, <span class="SCst">Gravity</span>}

    g_Bird.frame += <span class="SNum">10</span> * g_Dt

    <span class="SCmt">// Be sure to have at least one pipe</span>
    <span class="SLgc">if</span> g_Pipes.count == <span class="SNum">0</span>
        <span class="SFct">createPipe</span>()

    <span class="SCmt">// Move each pipe, and test collisions against the bird</span>
    <span class="SLgc">if</span> g_Start
    {
        <span class="SLgc">visit</span> &pipe: g_Pipes
        {
            pipe.rectUp.x -= <span class="SCst">SpeedHorz</span> * g_Dt
            pipe.rectDown.x -= <span class="SCst">SpeedHorz</span> * g_Dt

            <span class="SCmt">// Collision with the pipes</span>
            <span class="SLgc">if</span> <span class="SFct">birdInRect</span>(pipe.rectUp) <span class="SLgc">or</span> <span class="SFct">birdInRect</span>(pipe.rectDown)
            {
                <span class="SKwd">assume</span> <span class="SCst">Audio</span>.<span class="SCst">Voice</span>.<span class="SFct">play</span>(&g_SoundHit)
                g_GameOver = <span class="SKwd">true</span>
            }

            <span class="SCmt">// If bird moves to the right of a pipe, then this is a win !</span>
            <span class="SCmt">// Score up.</span>
            <span class="SLgc">if</span> g_Bird.pos.x &gt; pipe.rectUp.<span class="SFct">right</span>() <span class="SLgc">and</span> !pipe.scored
            {
                <span class="SKwd">assume</span> <span class="SCst">Audio</span>.<span class="SCst">Voice</span>.<span class="SFct">play</span>(&g_SoundScore)
                pipe.scored = <span class="SKwd">true</span>
                g_Score += <span class="SNum">1</span>
            }
        }
    }

    <span class="SCmt">// If the first pipe is out of screen, remove it</span>
    <span class="SLgc">if</span> g_Pipes[<span class="SNum">0</span>].rectUp.<span class="SFct">right</span>() &lt; <span class="SNum">0</span>
        g_Pipes.<span class="SFct">removeAt</span>(<span class="SNum">0</span>)

    <span class="SCmt">// If the last pipe is enough inside, create a new one</span>
    <span class="SLgc">if</span> g_Rect.width - g_Pipes.<span class="SFct">back</span>().rectUp.<span class="SFct">right</span>() &gt; g_Pipes.<span class="SFct">back</span>().distToNext
        <span class="SFct">createPipe</span>()

    <span class="SCmt">// Collision with the ground</span>
    <span class="SLgc">if</span> g_Bird.pos.y + g_BirdTexture[<span class="SNum">0</span>].height &gt; g_Rect.<span class="SFct">bottom</span>() - <span class="SCst">GroundHeight</span>
    {
        g_GameOver = <span class="SKwd">true</span>
        <span class="SKwd">assume</span> <span class="SCst">Audio</span>.<span class="SCst">Voice</span>.<span class="SFct">play</span>(&g_SoundHit)
    }

    <span class="SCmt">// Play dying sound</span>
    <span class="SLgc">if</span> g_GameOver
        <span class="SKwd">assume</span> <span class="SCst">Audio</span>.<span class="SCst">Voice</span>.<span class="SFct">play</span>(&g_SoundDie)
}</span></code>
</div>
<p>Creates a random up and down part of a new Pipe. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createPipe</span>()
{
    <span class="SKwd">let</span> pos = g_Rect.width
    <span class="SKwd">let</span> sizePassage = <span class="SCst">Random</span>.<span class="SFct">shared</span>().<span class="SFct">nextF32</span>(<span class="SNum">100</span>, <span class="SNum">175</span>)
    <span class="SKwd">let</span> heightUp = <span class="SCst">Random</span>.<span class="SFct">shared</span>().<span class="SFct">nextF32</span>(<span class="SNum">50</span>, g_Rect.height - sizePassage - <span class="SNum">50</span>)
    <span class="SKwd">let</span> heightDown = g_Rect.height - heightUp - <span class="SCst">GroundHeight</span> - sizePassage

    <span class="SKwd">var</span> pipe: <span class="SCst">Pipe</span>
    pipe.rectUp.x = pos
    pipe.rectUp.y = heightUp - g_PipeTextureU.height
    pipe.rectUp.width = g_PipeTextureU.width
    pipe.rectUp.height = g_PipeTextureU.height

    pipe.rectDown.x = pos
    pipe.rectDown.y = g_Rect.<span class="SFct">bottom</span>() - heightDown - <span class="SCst">GroundHeight</span>
    pipe.rectDown.width = g_PipeTextureU.width
    pipe.rectDown.height = g_PipeTextureU.height

    pipe.distToNext = <span class="SCst">Random</span>.<span class="SFct">shared</span>().<span class="SFct">nextF32</span>(<span class="SNum">100</span>, <span class="SNum">200</span>)

    g_Pipes.<span class="SFct">add</span>(pipe)
}</span></code>
</div>
<p>Reinitialize the game. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">start</span>()
{
    <span class="SCmt">// '@init' can be called to reinitialize a variable to its default value</span>
    <span class="SItr">@init</span>(&g_Bird)

    g_Bird.pos.x = g_Rect.<span class="SFct">horzCenter</span>()
    g_Bird.pos.y = g_Rect.<span class="SFct">vertCenter</span>()
    g_Score = <span class="SNum">0</span>
    g_Pipes.<span class="SFct">clear</span>()

    <span class="SCmt">// Assign two variables with the same value</span>
    g_GameOver, g_Start = <span class="SKwd">false</span>
}</span></code>
</div>
<p>Load all assets. </p>
<p>You can notice that the function has <span class="code-inline">throw</span> after its declaration. This indicated that it can return some errors, and that the caller will have to deal with it. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">loadAssets</span>(wnd: *<span class="SCst">Wnd</span>) <span class="SKwd">throw</span>
{
    <span class="SKwd">let</span> render = &wnd.<span class="SFct">getApp</span>().renderer

    <span class="SKwd">var</span> dataPath: <span class="SCst">String</span> = <span class="SCst">Path</span>.<span class="SFct">getDirectoryName</span>(<span class="SCmp">#location</span>.fileName)
    dataPath = <span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"datas"</span>)
    dataPath = <span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"flappy"</span>)

    <span class="SCmt">// Load all sounds</span>
    <span class="SCmt">//</span>
    <span class="SCmt">// 'with' tells the compiler that what follows is in the 'context' of the namespace 'Audio.SoundFile'.</span>
    <span class="SCmt">// So instead of having to write 'Audio.SoundFile.load(...)', we can now only write '.load(...)'.</span>
    <span class="SKwd">with</span> <span class="SCst">Audio</span>.<span class="SCst">SoundFile</span>
    {
        g_SoundDie   = .<span class="SFct">load</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"die.wav"</span>))
        g_SoundScore = .<span class="SFct">load</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"point.wav"</span>))
        g_SoundHit   = .<span class="SFct">load</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"hit.wav"</span>))
        g_SoundFlap  = .<span class="SFct">load</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"flap.wav"</span>))
    }

    <span class="SCmt">// Load all textures</span>
    <span class="SCmt">//</span>
    <span class="SCmt">// 'with' can also be used with a variable.</span>
    <span class="SKwd">with</span> render
    {
        g_BirdTexture[<span class="SNum">0</span>] = .<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"yellowbird-upflap.png"</span>))
        g_BirdTexture[<span class="SNum">1</span>] = .<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"yellowbird-midflap.png"</span>))
        g_BirdTexture[<span class="SNum">2</span>] = .<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"yellowbird-downflap.png"</span>))
        g_OverTexture    = .<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"gameover.png"</span>))
        g_BaseTexture    = .<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"base.png"</span>))
        g_BackTexture    = .<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"background-day.png"</span>))
        g_MsgTexture     = .<span class="SFct">addImage</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"message.png"</span>))
    }

    <span class="SKwd">var</span> img = <span class="SCst">Image</span>.<span class="SFct">load</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"pipe-green.png"</span>))
    g_PipeTextureD = render.<span class="SFct">addImage</span>(img)
    img.<span class="SFct">flip</span>()
    g_PipeTextureU = render.<span class="SFct">addImage</span>(img)

    g_Font = <span class="SCst">Font</span>.<span class="SFct">create</span>(<span class="SCst">Path</span>.<span class="SFct">combine</span>(dataPath, <span class="SStr">"FlappyBirdy.ttf"</span>), <span class="SNum">50</span>)
}</span></code>
</div>
<div class="swag-watermark">
Generated on 08-09-2023 with <a href="https://swag-lang.org/index.php">swag</a> 0.25.0</div>
</div>
</div>
</div>
</body>
</html>
