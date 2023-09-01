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
<h1 id="Script_file">Script file </h1>
<p>Swag can be used to build and run a simple script file, thanks to the fact that the compiler can run anything at compile time. No executable will be generated, the compiler will do all the job. To create a new script file with the special extension <span class="code-inline">swgs</span>: </p>
<div class="code-block"><code><span class="SCde">$ swag new -f:myScript
=&gt; script file 'myScript.swgs' has been created
=&gt; type 'swag script -f:myScript.swgs' to run that script</span></code>
</div>
<p>This will generate a simple file with a <span class="code-inline">#dependency</span> block and one <span class="code-inline">#run</span> compiler function. </p>
<div class="code-block"><code><span class="SCde"><span class="SCmt">// Swag script file</span>
<span class="SFct">#dependencies</span>
{
    <span class="SCmt">// Here you can add your external dependencies</span>
    <span class="SCmt">// #import "core" location="swag@std"</span>
}

<span class="SFct">#run</span>
{
    <span class="SItr">@print</span>(<span class="SStr">"Hello world !\n"</span>)
}</span></code>
</div>
<p>You can then run your script with the <span class="code-inline">script</span> command. </p>
<div class="code-block"><code><span class="SCde">$ swag script -f:myScript
Hello world !</span></code>
</div>
<p>You can also just specify the script file <b>with the extension</b> as a command. </p>
<div class="code-block"><code><span class="SCde">$ swag myScript.swgs
Hello world !</span></code>
</div>
<p>You will find a bunch of small scripts in <span class="code-inline">swag/bin/examples/scripts</span>. To run one of them from the console, go to the folder and type for example <span class="code-inline">swag flappy.swgs</span>. </p>
<p style="white-space: break-spaces"><div align="center">
    <div class="round-button">
        <a href="flappy.php" class="no-decoration">Flappy Bird</a>
    </div>
</div></p>
<h2 id="Dependencies">Dependencies </h2>
<p>You can add external dependencies, and they will be compiled and used as native code. </p>
<div class="code-block"><code><span class="SCde"><span class="SFct">#dependencies</span>
{
    <span class="SCmt">// Import the standard module `core` from the swag standard workspace (which comes with the compiler)</span>
    <span class="SCmp">#import</span> <span class="SStr">"core"</span> location=<span class="SStr">"swag@std"</span>
}</span></code>
</div>
<p>A special hidden workspace (in the Swag cache folder) will be created to contain all the corresponding native code. </p>
<ul>
<li>To locate the Swag cache folder, add <span class="code-inline">--verbose-path</span> to the command line.</li>
<li>To force the build of dependencies, add <span class="code-inline">--rebuildall</span> to the command line.</li>
</ul>
<h2 id="More_than_one_script_file">More than one script file </h2>
<p>If your script is divided in more than one single file, you can add <span class="code-inline">#load &lt;filename&gt;</span> in the <span class="code-inline">#dependencies</span> block. </p>
<div class="code-block"><code><span class="SCde"><span class="SFct">#dependencies</span>
{
    <span class="SCmp">#load</span> <span class="SStr">"myOtherFile.swgs"</span>
    <span class="SCmp">#load</span> <span class="SStr">"folder/myOtherOtherFile.swgs"</span>
}</span></code>
</div>
<h2 id="Debug">Debug </h2>
<p>The compiler comes with a <b>bytecode debugger</b> that can be used to trace and debug compile time execution. Add <span class="code-inline">@breakpoint()</span> in your code when you want the debugger to trigger. </p>
<p>The debugger command set is inspired by <a href="https://docs.python.org/3/library/pdb.html">Pdb</a>, the python debugger. </p>
</div>
</div>
</div>
</body>
</html>
