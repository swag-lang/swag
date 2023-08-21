<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><style>

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
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .item td:first-child {
            width:              33%;
            white-space:        nowrap;
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
        .precode {
            background-color:   #eeeeee;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
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
<?php include('common/start-body.php'); ?><div class="container">
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Script file </h1>
<p>Instead of a workspace, Swag can also be used to build and run a simple script file, thanks to the fact that the compiler can run anything at compile time. No executable will be generated, the compiler will do all the job. To create a new script file with the special extension <code class="incode">swgs</code>: </p>
<div class="precode"><code></span><span class="SyntaxCode">$ swag new -f:myScript
=&gt; script file 'myScript.</span><span class="SyntaxFunction">swgs</span><span class="SyntaxCode">' has been created
=&gt; type 'swag script -f:myScript.</span><span class="SyntaxFunction">swgs</span><span class="SyntaxCode">' to run that script</code>
</div>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxComment">// Swag script file</span><span class="SyntaxCode">
#dependencies
{
    </span><span class="SyntaxComment">// Here you can add your external dependencies</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// #import "core" location="swag@std"</span><span class="SyntaxCode">
}

</span><span class="SyntaxFunction">#run</span><span class="SyntaxCode">
{
    </span><span class="SyntaxIntrinsic">@print</span><span class="SyntaxCode">(</span><span class="SyntaxString">"Hello world !\n"</span><span class="SyntaxCode">)
}</code>
</div>
<p>You can then run your script with the <code class="incode">script</code> command. </p>
<div class="precode"><code></span><span class="SyntaxCode">$ swag script -f:myScript
</span><span class="SyntaxConstant">Hello</span><span class="SyntaxCode"> world !</code>
</div>
<p>You can also just specify the script file <b>with the extension</b> as a command. </p>
<div class="precode"><code></span><span class="SyntaxCode">$ swag myScript.swgs
</span><span class="SyntaxConstant">Hello</span><span class="SyntaxCode"> world !</code>
</div>
<p>You will find a bunch of small scripts in <code class="incode">swag/bin/examples/scripts</code>. To run one of them from the console, go to the folder and type for example <code class="incode">swag pendulum.swgs</code> </p>
<h2>Dependencies </h2>
<p>You can add external dependencies, and they will be compiled and used as native code. </p>
<div class="precode"><code></span><span class="SyntaxCode">#dependencies
{
    </span><span class="SyntaxComment">// Import the standard module `core` from the swag standard workspace (which comes with the compiler)</span><span class="SyntaxCode">
    #import </span><span class="SyntaxString">"core"</span><span class="SyntaxCode"> location=</span><span class="SyntaxString">"swag@std"</span><span class="SyntaxCode">
}</code>
</div>
<p>A special hidden workspace (in the Swag cache folder) will be created to contain all the corresponding native code. </p>
<ul>
<li>To locate the Swag cache folder, add <code class="incode">--verbose-path</code> to the command line.</li>
<li>To force the build of dependencies, add <code class="incode">--rebuildall</code> to the command line.</li>
</ul>
<h2>More than one script file </h2>
<p>If your script is divided in more than one single file, you can add <code class="incode">#load &lt;filename&gt;</code> in the <code class="incode">#dependencies</code> block. </p>
<div class="precode"><code></span><span class="SyntaxCode">#dependencies
{
    #load </span><span class="SyntaxString">"myOtherFile.swgs"</span><span class="SyntaxCode">
    #load </span><span class="SyntaxString">"folder/myOtherOtherFile.swgs"</span><span class="SyntaxCode">
}</code>
</div>
<h2>Debug </h2>
<p>The compiler comes with a <b>bytecode debugger</b> that can be used to trace and debug compile time execution. Add <code class="incode">@breakpoint()</code> in your code when you want the debugger to trigger. </p>
<p>The debugger command set is inspired by <a href="https://docs.python.org/3/library/pdb.html">Pdb</a>, the python debugger. </p>
</div>
</div>
</div>
</body>
</html>
