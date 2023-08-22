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
            text-decoration: none;
            color:           DoggerBlue;
        }
        .precode a {
            text-decoration: revert;
            color:           inherit;
        }
        .container a:hover {
            text-decoration: underline;
        }
        .container a.src {
            font-size:          90%;
            color:              LightGrey;
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
<h1>Your first install </h1>
<p><a href="https://github.com/swag-lang/swag/releases">Download</a> the latest release from github, and unzip it in a folder. Of course a <i>SSD</i> is better. </p>
<h3>Under windows 10/11 </h3>
<p>You should register the location of the swag compiler (<code class="incode">swag.exe</code>) in the PATH environment variable to be able to call it from everywhere. </p>
<p>You can open a Powershell window, and run the following code : </p>
<div class="precode"><code></span><span class="SyntaxCode"># </span><span class="SyntaxConstant">You</span><span class="SyntaxCode"> must replace </span><span class="SyntaxString">`f:\swag-lang\swag\bin`</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">with</span><span class="SyntaxCode"> your location of </span><span class="SyntaxString">`swag.exe`</span><span class="SyntaxCode">

[</span><span class="SyntaxConstant">Environment</span><span class="SyntaxCode">]::</span><span class="SyntaxConstant">SetEnvironmentVariable</span><span class="SyntaxCode">(
   </span><span class="SyntaxString">"Path"</span><span class="SyntaxCode">,
   [</span><span class="SyntaxConstant">Environment</span><span class="SyntaxCode">]::</span><span class="SyntaxConstant">GetEnvironmentVariable</span><span class="SyntaxCode">(</span><span class="SyntaxString">"Path"</span><span class="SyntaxCode">, </span><span class="SyntaxString">"User"</span><span class="SyntaxCode">) + </span><span class="SyntaxString">";f:\swag-lang\swag\bin"</span><span class="SyntaxCode">,
   </span><span class="SyntaxString">"User"</span><span class="SyntaxCode">
)</code>
</div>
<h1>Your first project </h1>
<p>The compile unit of swag is a <b>workspace</b> which contains a variable number of <b>modules</b>. A module will compile to a dynamic library or an executable. </p>
<p>To create a fresh new workspace named <i>first</i> : </p>
<div class="precode"><code></span><span class="SyntaxCode">$ swag new -w:first
=&gt; workspace '</span><span class="SyntaxConstant">F</span><span class="SyntaxCode">:/</span><span class="SyntaxFunction">first</span><span class="SyntaxCode">' has been created
=&gt; module '</span><span class="SyntaxFunction">first</span><span class="SyntaxCode">' has been created
=&gt; type 'swag run -w:</span><span class="SyntaxConstant">F</span><span class="SyntaxCode">:\</span><span class="SyntaxFunction">first</span><span class="SyntaxCode">' to build </span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> run that module</code>
</div>
<p>This will also create a simple executable module <i>first</i> to print "Hello world !". </p>
<p><i>F:/first/modules/first/src/main.swg</i> </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxFunction">#main</span><span class="SyntaxCode">
{
    </span><span class="SyntaxIntrinsic">@print</span><span class="SyntaxCode">(</span><span class="SyntaxString">"Hello world!\n"</span><span class="SyntaxCode">)
}</code>
</div>
<p>A workspace contains a predefined number of sub folders: </p>
<ul>
<li><code class="incode">modules/</code> contains all the modules (sub folders) of that workspace.</li>
<li><code class="incode">output/</code> (generated) contains the result of the build (this is where the executable will be located).</li>
<li><code class="incode">tests/</code> (optional) contains a list of test modules.</li>
<li><code class="incode">dependencies/</code> (generated) contains a list of external modules needed to compile the workspace.</li>
</ul>
<p>A module is also organized in a predefined way: </p>
<ul>
<li><code class="incode">moduleName/</code> the folder name of the module is used to create output files.</li>
<li><code class="incode">src/</code> contains the source code.</li>
<li><code class="incode">public/</code> (generated) will contain all the exports needed by other modules to use that one (in case of a dynamic library).</li>
<li><code class="incode">publish/</code> contains additional files to use that module (like an external C dll).</li>
</ul>
<p>A module always contains a special file named <code class="incode">module.swg</code>. This file is used to configure the module, and is <b>mandatory</b>. </p>
<h3>To compile your workspace </h3>
<div class="precode"><code></span><span class="SyntaxCode">$ swag build -w:first
            </span><span class="SyntaxConstant">Workspace</span><span class="SyntaxCode"> first [fast-debug-windows-x86_64]
             </span><span class="SyntaxConstant">Building</span><span class="SyntaxCode"> first
                 </span><span class="SyntaxConstant">Done</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0.067</span><span class="SyntaxCode">s</code>
</div>
<p>You can omit the workspace name (<code class="incode">-w:first</code> or <code class="incode">--workspace:first</code>) if you call the compiler directly from the workspace folder. This command will compile all modules in <code class="incode">modules/</code>. </p>
<p>You can also build and run your workspace. </p>
<div class="precode"><code></span><span class="SyntaxCode">$ swag run -w:first
            </span><span class="SyntaxConstant">Workspace</span><span class="SyntaxCode"> first [fast-debug-windows-x86_64]
             </span><span class="SyntaxConstant">Building</span><span class="SyntaxCode"> first
      </span><span class="SyntaxConstant">Running</span><span class="SyntaxCode"> backend first
</span><span class="SyntaxConstant">Hello</span><span class="SyntaxCode"> world!
                 </span><span class="SyntaxConstant">Done</span><span class="SyntaxCode"> </span><span class="SyntaxNumber">0.093</span><span class="SyntaxCode">s</code>
</div>
<h3>Note on Windows Defender realtime protection </h3>
<p>It's activated by default under Windows 10, and runs each time you launch an executable or a process. This can increase the compile time of your project, so consider excluding your Swag folder from it ! </p>
<p><a href="https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26#:~:text=Go%20to%20Start%20%3E%20Settings%20%3E%20Update,%2C%20file%20types%2C%20or%20process">Reference</a> </p>
<h1>Content of the Swag folder </h1>
<p>The Swag folder contains the compiler <code class="incode">swag.exe</code>, but also a bunch of sub folders. </p>
<ul>
<li><code class="incode">reference/</code> is a workspace which contains an overview of the language, in the form of small tests.</li>
<li><code class="incode">testsuite/</code> is a workspace which contains all the tests to debug the compiler.</li>
<li><code class="incode">runtime/</code> contains the compiler runtime, which is included in all user modules.</li>
<li><code class="incode">std/</code> is the <a href="std.php">standard workspace</a> which contains all the standard modules that come with the compiler. A big work in progress.</li>
</ul>
<h1>The Swag language </h1>
<p>You should take a look at the <code class="incode">reference/</code> sub folder in the Swag directory, or to the corresponding <a href="language.php">documentation</a>. It contains the list of all that can be done with the language, in the form of small tests (in fact it's not really exhaustive, but should be...). </p>
<p>It's a good starting point to familiarize yourself with the language. </p>
<p>And as this is a normal Swag workspace, you could also build and test it with <code class="incode">swag test -w:swag/reference</code>. </p>
<p>You will also find some small examples (mostly written for tests) in <code class="incode">swag/bin/examples/modules</code>. To build and run one of them from the console, go to the workspace folder (<code class="incode">/examples</code>) and type for example <code class="incode">swag run -m:wnd</code>. </p>
</div>
</div>
</div>
</body>
</html>
