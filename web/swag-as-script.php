<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Swag</title>
<link rel="icon" type="image/x-icon" href="favicon.ico">
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
            color:              DoggerBlue;
        }
        .precode a {
            color:              inherit;
        }
        .codetype a {
            color:              inherit;
        }
        .left a {
            text-decoration:    none;
        }
        .container a:hover {
            text-decoration:    underline;
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
    .SInv { color: #ff0000; }
</style>
</head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</blockquote>
<p style="white-space: break-spaces"><div align="center">
    <div class="round-button">
        <a href="flappy.php" class="no-decoration">Flappy Bird</a>
    </div>
</div></p>
<h1 id="Script_file">Script file </h1>
<p>Swag can be used to build and run a simple script file, thanks to the fact that the compiler can run anything at compile time. No executable will be generated, the compiler will do all the job. To create a new script file with the special extension <code class="incode">swgs</code>: </p>
<div class="precode"><code><span class="SCde">$ swag new -f:myScript
=&gt; script file 'myScript.swgs' has been created
=&gt; type 'swag script -f:myScript.swgs' to run that script</span></code>
</div>
<p>This will generate a simple file with a <code class="incode">#dependency</code> block and one <code class="incode">#run</code> compiler function. </p>
<div class="precode"><code><span class="SCde"><span class="SCmt">// Swag script file</span>
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
<p>You can then run your script with the <code class="incode">script</code> command. </p>
<div class="precode"><code><span class="SCde">$ swag script -f:myScript
Hello world !</span></code>
</div>
<p>You can also just specify the script file <b>with the extension</b> as a command. </p>
<div class="precode"><code><span class="SCde">$ swag myScript.swgs
Hello world !</span></code>
</div>
<p>You will find a bunch of small scripts in <code class="incode">swag/bin/examples/scripts</code>. To run one of them from the console, go to the folder and type for example <code class="incode">swag pendulum.swgs</code>. </p>
<h2 id="Dependencies">Dependencies </h2>
<p>You can add external dependencies, and they will be compiled and used as native code. </p>
<div class="precode"><code><span class="SCde"><span class="SFct">#dependencies</span>
{
    <span class="SCmt">// Import the standard module `core` from the swag standard workspace (which comes with the compiler)</span>
    <span class="SCmp">#import</span> <span class="SStr">"core"</span> location=<span class="SStr">"swag@std"</span>
}</span></code>
</div>
<p>A special hidden workspace (in the Swag cache folder) will be created to contain all the corresponding native code. </p>
<ul>
<li>To locate the Swag cache folder, add <code class="incode">--verbose-path</code> to the command line.</li>
<li>To force the build of dependencies, add <code class="incode">--rebuildall</code> to the command line.</li>
</ul>
<h2 id="More_than_one_script_file">More than one script file </h2>
<p>If your script is divided in more than one single file, you can add <code class="incode">#load &lt;filename&gt;</code> in the <code class="incode">#dependencies</code> block. </p>
<div class="precode"><code><span class="SCde"><span class="SFct">#dependencies</span>
{
    <span class="SCmp">#load</span> <span class="SStr">"myOtherFile.swgs"</span>
    <span class="SCmp">#load</span> <span class="SStr">"folder/myOtherOtherFile.swgs"</span>
}</span></code>
</div>
<h2 id="Debug">Debug </h2>
<p>The compiler comes with a <b>bytecode debugger</b> that can be used to trace and debug compile time execution. Add <code class="incode">@breakpoint()</code> in your code when you want the debugger to trigger. </p>
<p>The debugger command set is inspired by <a href="https://docs.python.org/3/library/pdb.html">Pdb</a>, the python debugger. </p>
</div>
</div>
</div>
</body>
</html>
