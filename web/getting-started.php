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
        
        table.table-enumeration           { border: 1px solid LightGrey; border-collapse: collapse; width: 100%; font-size: 90%; }
        .table-enumeration td             { padding: 6px; border: 1px solid LightGrey; border-collapse: collapse; min-width: 100px; }
        .table-enumeration td:first-child { background-color: #f8f8f8; white-space: nowrap; }
        .table-enumeration a              { text-decoration: none; }
        
        .container td:last-child { width: 100%; }
        .tdname .inline-code     { background-color: revert; padding: 2px; border: revert; }
        .inline-code             { font-size: 110%; font-family: monospace; display: inline-block; background-color: #eeeeee; padding: 2px; border-radius: 5px; border: 1px dotted #cccccc; }
        .code-type               { background-color: #eeeeee; }
        .code-type a             { color: inherit; }
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
<h1 id="Your_first_install">Your first install </h1>
<p><a href="https://github.com/swag-lang/swag/releases">Download</a> the latest release from github, and unzip it in a folder. Of course a <i>SSD</i> is better. </p>
<h3 id="Under_windows_10/11">Under windows 10/11 </h3>
<p>You should register the location of the swag compiler (<span class="inline-code">swag.exe</span>) in the PATH environment variable to be able to call it from everywhere. </p>
<p>You can open a Powershell window, and run the following code : </p>
<div class="code-block"><code><span class="SCde"># You must replace `f:\swag-lang\swag\bin` with your location of `swag.exe`
[Environment]::SetEnvironmentVariable(
    "Path",
    [Environment]::GetEnvironmentVariable("Path", "User") + ";f:\swag-lang\swag\bin",
    "User"
)</span></code>
</div>
<h1 id="Your_first_project">Your first project </h1>
<p>The compile unit of swag is a <b>workspace</b> which contains a variable number of <b>modules</b>. A module will compile to a dynamic library or an executable. </p>
<p>To create a fresh new workspace named <i>first</i> : </p>
<div class="code-block"><code><span class="SCde">$ swag new -w:first
=&gt; workspace 'F:/first' has been created
=&gt; module 'first' has been created
=&gt; type 'swag run -w:F:\first' to build and run that module</span></code>
</div>
<p>This will also create a simple executable module <i>first</i> to print "Hello world !". </p>
<p><i>F:/first/modules/first/src/main.swg</i> </p>
<div class="code-block"><code><span class="SCde"><span class="SFct">#main</span>
{
    <span class="SItr">@print</span>(<span class="SStr">"Hello world!\n"</span>)
}</span></code>
</div>
<p>A workspace contains a predefined number of sub folders: </p>
<ul>
<li><span class="inline-code">modules/</span> contains all the modules (sub folders) of that workspace.</li>
<li><span class="inline-code">output/</span> (generated) contains the result of the build (this is where the executable will be located).</li>
<li><span class="inline-code">tests/</span> (optional) contains a list of test modules.</li>
<li><span class="inline-code">dependencies/</span> (generated) contains a list of external modules needed to compile the workspace.</li>
</ul>
<p>A module is also organized in a predefined way: </p>
<ul>
<li><span class="inline-code">moduleName/</span> the folder name of the module is used to create output files.</li>
<li><span class="inline-code">src/</span> contains the source code.</li>
<li><span class="inline-code">public/</span> (generated) will contain all the exports needed by other modules to use that one (in case of a dynamic library).</li>
<li><span class="inline-code">publish/</span> contains additional files to use that module (like an external C dll).</li>
</ul>
<p>A module always contains a special file named <span class="inline-code">module.swg</span>. This file is used to configure the module, and is <b>mandatory</b>. </p>
<h3 id="To_compile_your_workspace">To compile your workspace </h3>
<div class="code-block"><code><span class="SCde">$ swag build -w:first
            Workspace first [fast-debug-windows-x86_64]
            Building first
                Done 0.067s</span></code>
</div>
<p>You can omit the workspace name (<span class="inline-code">-w:first</span> or <span class="inline-code">--workspace:first</span>) if you call the compiler directly from the workspace folder. This command will compile all modules in <span class="inline-code">modules/</span>. </p>
<p>You can also build and run your workspace. </p>
<div class="code-block"><code><span class="SCde">$ swag run -w:first
            Workspace first [fast-debug-windows-x86_64]
            Building first
    Running backend first
Hello world!
            Done 0.093s</span></code>
</div>
<h3 id="Note_on_Windows_Defender_realtime_protection">Note on Windows Defender realtime protection </h3>
<p>It's activated by default under Windows 10, and runs each time you launch an executable or a process. This can increase the compile time of your project, so consider excluding your Swag folder from it ! </p>
<p><a href="https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26#:~:text=Go%20to%20Start%20%3E%20Settings%20%3E%20Update,%2C%20file%20types%2C%20or%20process">Reference</a> </p>
<h1 id="Content_of_the_Swag_folder">Content of the Swag folder </h1>
<p>The Swag folder contains the compiler <span class="inline-code">swag.exe</span>, but also a bunch of sub folders. </p>
<ul>
<li><span class="inline-code">reference/</span> is a workspace which contains an overview of the language, in the form of small tests.</li>
<li><span class="inline-code">testsuite/</span> is a workspace which contains all the tests to debug the compiler.</li>
<li><span class="inline-code">runtime/</span> contains the compiler runtime, which is included in all user modules.</li>
<li><span class="inline-code">std/</span> is the <a href="std.php">standard workspace</a> which contains all the standard modules that come with the compiler. A big work in progress.</li>
</ul>
<h1 id="The_Swag_language">The Swag language </h1>
<p>You should take a look at the <span class="inline-code">reference/</span> sub folder in the Swag directory, or to the corresponding generated <a href="language.php">documentation</a>. It contains the list of all that can be done with the language, in the form of small tests (in fact it's not really exhaustive, but should be...). </p>
<p>It's a good starting point to familiarize yourself with the language. </p>
<p>And as this is a normal Swag workspace, you could also build and test it with <span class="inline-code">swag test -w:swag/reference</span>. </p>
<p>You will also find some small examples (mostly written for tests) in <span class="inline-code">swag/bin/examples/modules</span>. To build and run one of them from the console, go to the workspace folder (<span class="inline-code">/examples</span>) and type for example <span class="inline-code">swag run -m:wnd</span>. </p>
</div>
</div>
</div>
</body>
</html>
