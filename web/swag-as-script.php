<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<?php include('common/start-head.php'); ?><style>

    .container {
        display:        flex;
        flex-wrap:      nowrap;
        flex-direction: row;
        height:         100%;
        line-height:    1.3em;
        font-family:    Segoe UI;
    }
    .left {
        display:    block;
        overflow-y: scroll;
        width:      650px;
        height:     100%;
    }
    .right {
        display:     block;
        overflow-y:  scroll;
        line-height: 1.3em;
        width:       100%;
        height:      100%;
    }
    .page {
        width:  1000px;
        margin: 0px auto;
    }
    .container blockquote {
        background-color:   LightYellow;
        border-left:        6px solid Orange;
        padding:            10px;
        width:              90%;
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
    .container pre {
        background-color:   #eeeeee;
        border:             1px solid LightGrey;
        padding:            10px;
        width:              94%;
        margin-left:        20px;
    }
    .SyntaxCode      { color: #7f7f7f; }
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
<div class="page">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.22.0)</blockquote>
<h1>Script file </h1>
<p>Instead of a workspace, Swag can also be used to build and run a simple script file, thanks to the fact that the compiler can run anything at compile time. No executable will be generated, the compiler will do all the job. To create a new script file with the special extension <code class="incode">swgs</code>: </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode">$ swag new -f:myScript
=&gt; script file 'myScript.</span><span class="SyntaxFunction">swgs</span><span class="SyntaxCode">' has been created
=&gt; type 'swag script -f:myScript.</span><span class="SyntaxFunction">swgs</span><span class="SyntaxCode">' to run that script</code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxComment">// Swag script file</span><span class="SyntaxCode">
#dependencies
{
    </span><span class="SyntaxComment">// Here you can add your external dependencies</span><span class="SyntaxCode">
    </span><span class="SyntaxComment">// #import "core" location="swag@std"</span><span class="SyntaxCode">
}

#run
{
    </span><span class="SyntaxIntrinsic">@print</span><span class="SyntaxCode">(</span><span class="SyntaxString">"Hello world !\n"</span><span class="SyntaxCode">)
}</code>
</pre>
<p>You can then run your script with the <code class="incode">script</code> command. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode">$ swag script -f:myScript
</span><span class="SyntaxConstant">Hello</span><span class="SyntaxCode"> world !</code>
</pre>
<p>You can also just specify the script file <b>with the extension</b> as a command. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode">$ swag myScript.swgs
</span><span class="SyntaxConstant">Hello</span><span class="SyntaxCode"> world !</code>
</pre>
<p>You will find a bunch of small scripts in <code class="incode">swag/bin/examples/scripts</code>. To run one of them from the console, go to the folder and type for example <code class="incode">swag pendulum.swgs</code> </p>
<h2>Dependencies </h2>
<p>You can add external dependencies, and they will be compiled and used as native code. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode">#dependencies
{
    </span><span class="SyntaxComment">// Import the standard module `core` from the swag standard workspace (which comes with the compiler)</span><span class="SyntaxCode">
    #import </span><span class="SyntaxString">"core"</span><span class="SyntaxCode"> location=</span><span class="SyntaxString">"swag@std"</span><span class="SyntaxCode">
}</code>
</pre>
<p>A special hidden workspace (in the Swag cache folder) will be created to contain all the corresponding native code. </p>
<ul>
<li>To locate the Swag cache folder, add <code class="incode">--verbose-path</code> to the command line.</li>
<li>To force the build of dependencies, add <code class="incode">--rebuildall</code> to the command line.</li>
</ul>
<h2>More than one script file </h2>
<p>If your script is divided in more than one single file, you can add <code class="incode">#load &lt;filename&gt;</code> in the <code class="incode">#dependencies</code> block. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode">#dependencies
{
    #load </span><span class="SyntaxString">"myOtherFile.swgs"</span><span class="SyntaxCode">
    #load </span><span class="SyntaxString">"folder/myOtherOtherFile.swgs"</span><span class="SyntaxCode">
}</code>
</pre>
<h2>Debug </h2>
<p>The compiler comes with a <b>bytecode debugger</b> that can be used to trace and debug compile time execution. Add <code class="incode">@breakpoint()</code> in your code when you want the debugger to trigger. </p>
<p>The debugger command set is inspired by <a href="https://docs.python.org/3/library/pdb.html">Pdb</a>, the python debugger. </p>
</div>
</div>
</div>
<?php include('common/end-body.php'); ?></body>
</html>
