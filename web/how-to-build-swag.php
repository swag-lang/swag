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
        .container img      { margin:          0 auto; }
        
        .left a     { text-decoration: none; }
        .left ul    { list-style-type: none; margin-left: -20px; }
        .left h3    { background-color: Black; color: White; padding: 6px; }
        .right h1   { margin-top: 50px; margin-bottom: 50px; }
        .right h2   { margin-top: 35px; }
        
        .blockquote {
            border-radius:      5px;
            border:             1px solid;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-default     { border-color: Orange; border-left: 6px solid Orange; background-color: LightYellow; }
        .blockquote-note        { border-color: #ADCEDD; background-color: #CDEEFD; }
        .blockquote-tip         { border-color: #BCCFBC; background-color: #DCEFDC; }
        .blockquote-warning     { border-color: #DFBDB3; background-color: #FFDDD3; }
        .blockquote-attention   { border-color: #DDBAB8; background-color: #FDDAD8; }
        .blockquote-example     { border: 2px solid LightGrey; }
        .blockquote-title-block { margin-bottom: 10px; }
        .blockquote-title       { font-weight: bold; }
        .blockquote-default     p:first-child { margin-top: 0px; }
        .blockquote-default     p:last-child  { margin-bottom: 0px; }
        .blockquote             p:last-child  { margin-bottom: 0px; }
        
        .description-list-title   { font-weight: bold; font-style: italic; }
        .description-list-block   { margin-left: 30px; }
        
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
<div class="blockquote blockquote-warning">
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</div>
<h1 id="How_to_build_Swag">How to build Swag </h1>
<h2 id="LLVM">LLVM </h2>
<p>Swag has two backends, a <span class="code-inline">x86_64</span> custom backend written for fast compile, but with far from optimal generated code, and <span class="code-inline">llvm</span> for optimized builds. </p>
<p>The <a href="https://releases.llvm.org/download.html">LLVM</a> source tree is included in the Swag source tree for convenience. Version is <span class="code-inline">15.0.7</span>. </p>
<p>In order to build LLVM, you will have to install <a href="https://cmake.org/download/">cmake 3.23.2</a> (or later) and <a href="https://www.python.org/downloads/">python 3</a>. </p>
<h2 id="Build">Build </h2>
<p>You will need <span class="code-inline">Visual Studio 2022 17.1</span> or later. </p>
<ul>
<li>As there's no automatic detection, edit <span class="code-inline">vs_build_cfg.bat</span> to match your version of Visual Studio and of the Windows SDK.</li>
<li>Launch <span class="code-inline">swag/build/vs_build_llvm_release.bat</span>. Note that building LLVM takes a crazy amount of time and memory, and can require multiple tries.</li>
<li>Launch <span class="code-inline">swag/build/vs_build_swag_release.bat</span>.</li>
<li>You can also launch <span class="code-inline">swag/build/vs_build_extern.bat</span>. This will build and update some external libraries in the standard workspace, and copy some libraries from the windows SDK.</li>
</ul>
<p>If LLVM has been compiled once, you can also use the <span class="code-inline">Swag.sln</span> workspace in the <span class="code-inline">build</span> subfolder. </p>
<h2 id="Windows_SDK">Windows SDK </h2>
<p>The path to the SDK version is defined in <span class="code-inline">vs_build_cfg.bat</span>. </p>
<p>The Swag <b>runtime</b> contains a copy of some libraries from the SDK (<span class="code-inline">kernel32.lib</span>, <span class="code-inline">ucrt.lib</span>, <span class="code-inline">dbghelp.lib</span> and <span class="code-inline">user32.lib</span>). You will find them in <span class="code-inline">bin/runtime/windows-x86-64</span>. </p>
<p>They are shipped with the compiler to avoid the necessity to install the SDK before building with Swag. That way the compiler can be used "as is". </p>
<p>The standard modules (in <span class="code-inline">bin/std/modules</span>) can also have dependencies to some other libraries from the SDK. You will find those dependencies in the <span class="code-inline">publish</span> folder of the corresponding modules. </p>
</div>
</div>
</div>
</body>
</html>
