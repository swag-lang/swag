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
            border-radius:      5px;
            border:             1px solid Orange;
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
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
        .left a, .enumeration a {
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
            border-radius:      5px;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
        }
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
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</blockquote>
<h1 id="How_to_build_Swag">How to build Swag </h1>
<h2 id="LLVM">LLVM </h2>
<p>Swag has two backends, a <code class="incode">x86_64</code> custom backend written for fast compile, but with far from optimal generated code, and <code class="incode">llvm</code> for optimized builds. </p>
<p>The <a href="https://releases.llvm.org/download.html">LLVM</a> source tree is included in the Swag source tree for convenience. Version is <code class="incode">15.0.7</code>. </p>
<p>In order to build LLVM, you will have to install <a href="https://cmake.org/download/">cmake 3.23.2</a> (or later) and <a href="https://www.python.org/downloads/">python 3</a>. </p>
<h2 id="Build">Build </h2>
<p>You will need <code class="incode">Visual Studio 2022 17.1</code> or later. </p>
<ul>
<li>As there's no automatic detection, edit <code class="incode">vs_build_cfg.bat</code> to match your version of Visual Studio and of the Windows SDK.</li>
<li>Launch <code class="incode">swag/build/vs_build_llvm_release.bat</code>. Note that building LLVM takes a crazy amount of time and memory, and can require multiple tries.</li>
<li>Launch <code class="incode">swag/build/vs_build_swag_release.bat</code>.</li>
<li>You can also launch <code class="incode">swag/build/vs_build_extern.bat</code>. This will build and update some external libraries in the standard workspace, and copy some libraries from the windows SDK.</li>
</ul>
<p>If LLVM has been compiled once, you can also use the <code class="incode">Swag.sln</code> workspace in the <code class="incode">build</code> subfolder. </p>
<h2 id="Windows_SDK">Windows SDK </h2>
<p>The path to the SDK version is defined in <code class="incode">vs_build_cfg.bat</code>. </p>
<p>The Swag <b>runtime</b> contains a copy of some libraries from the SDK (<code class="incode">kernel32.lib</code>, <code class="incode">ucrt.lib</code>, <code class="incode">dbghelp.lib</code> and <code class="incode">user32.lib</code>). You will find them in <code class="incode">bin/runtime/windows-x86-64</code>. </p>
<p>They are shipped with the compiler to avoid the necessity to install the SDK before building with Swag. That way the compiler can be used "as is". </p>
<p>The standard modules (in <code class="incode">bin/std/modules</code>) can also have dependencies to some other libraries from the SDK. You will find those dependencies in the <code class="incode">publish</code> folder of the corresponding modules. </p>
</div>
</div>
</div>
</body>
</html>
