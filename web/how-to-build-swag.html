<html>
<body>
<head>
<meta charset="UTF-8">
</head>
<style>
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
            width:      650;
            height:     100%;
        }
        .right {
            display:     block;
            overflow-y:  scroll;
            width:       100%;
            line-height: 1.3em;
            height:      100%;
        }
        .page {
            width:  1000;
            margin: 0 auto;
        }
        blockquote {
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            padding:            10px;
            width:              90%;
        }
        a {
            text-decoration: none;
            color:           DoggerBlue;
        }
        a:hover {
            text-decoration: underline;
        }
        a.src {
            font-size:          90%;
            color:              LightGrey;
        }
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        td.enumeration {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              30%;
        }
        td.tdname {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              20%;
            background-color:   #f8f8f8;
        }
        td.tdtype {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              auto;
        }
        td:last-child {
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
        .code {
            background-color:   #eeeeee;
            border:             1px solid LightGrey;
            padding:            10px;
            width:              94%;
            margin-left:        20px;
        }
    .SyntaxCode { color: #7f7f7f; }
.SyntaxComment { color: #71a35b; }
.SyntaxCompiler { color: #7f7f7f; }
.SyntaxFunction { color: #ff6a00; }
.SyntaxConstant { color: #3173cd; }
.SyntaxIntrinsic { color: #b4b44a; }
.SyntaxType { color: #3bc3a7; }
.SyntaxKeyword { color: #3186cd; }
.SyntaxLogic { color: #b040be; }
.SyntaxNumber { color: #74a35b; }
.SyntaxString { color: #bb6643; }
.SyntaxAttribute { color: #7f7f7f; }
</style>
<div class="container">
<div class="right">
<div class="page">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.22.0)</blockquote>
<p>
<h1 id="How to build Swag">How to build Swag</h1></p>
<p>
</p>
<p>
<h2 id="LLVM">LLVM</h2></p>
<p>
</p>
<p>
Swag has two backends, a <code class="incode">x86_64</code> custom backend written for fast compile, but with far from optimal generated code, and <code class="incode">llvm</code> for optimized builds.</p>
<p>
The <a href="https://releases.llvm.org/download.html">LLVM</a> source tree is included in the Swag source tree for convenience. Version is <code class="incode">15.0.7</code>.</p>
<p>
In order to build LLVM, you will have to install <a href="https://cmake.org/download/">cmake 3.23.2</a> (or later) and <a href="https://www.python.org/downloads/">python 3</a>.</p>
<p>
<h2 id="Build">Build</h2></p>
<p>
</p>
<p>
You will need <code class="incode">Visual Studio 2022 17.1</code> or later.</p>
<p>
</p>
<ul>
<li>As theres no automatic detection, edit <code class="incode">vs_build_cfg.bat</code> to match your version of Visual Studio and of the Windows SDK.</li>
<li>Launch <code class="incode">swag/build/vs_build_llvm_release.bat</code>. Note that building LLVM takes a crazy amount of time and memory, and can require multiple tries.</li>
<li>Launch <code class="incode">swag/build/vs_build_swag_release.bat</code>.</li>
<li>You can also launch <code class="incode">swag/build/vs_build_extern.bat</code>. This will build and update some external libraries in the standard workspace, and copy some libraries from the windows SDK.</li>
</ul>
<p>
</p>
<p>
If LLVM has been compiled once, you can also use the <code class="incode">Swag.sln</code> workspace in the <code class="incode">build</code> subfolder.</p>
<p>
<h2 id="Windows SDK">Windows SDK</h2></p>
<p>
The path to the SDK version is defined in <code class="incode">vs_build_cfg.bat</code>.</p>
<p>
The Swag <b>runtime</b> contains a copy of some libraries from the SDK (<code class="incode">kernel32.lib</code>, <code class="incode">ucrt.lib</code>, <code class="incode">dbghelp.lib</code> and <code class="incode">user32.lib</code>). You will find them in <code class="incode">bin/runtime/windows-x86-64</code>.</p>
<p>
They are shipped with the compiler to avoid the necessity to install the SDK before building with Swag. That way the compiler can be used "as is".</p>
<p>
The standard modules (in <code class="incode">bin/std/modules</code>) can also have dependencies to some other libraries from the SDK. You will find those dependencies in the <code class="incode">publish</code> folder of the corresponding modules.</p>
</div>
</div>
</div>
</body>
</html>
