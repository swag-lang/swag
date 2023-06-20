<?php include('html_start.php'); ?>
<?php include('header.php'); ?>
<style><?php include('css/style.php'); ?></style>
<div class="lg:m-auto lg:w-[76rem] pt-2 lg:pt-10 overflow-hidden p-5">
<div class="container">
<p><h1 id="How to build Swag">How to build Swag</h1></p>
<p></br>
<h2 id="LLVM">LLVM</h2></p>
<p></br>
Swag has two backends, a <code>x86_64</code> custom backend written for fast compile, but with far from optimal generated code, and <code>llvm</code> for optimized builds.</br>
The <a href="https://releases.llvm.org/download.html">LLVM</a> source tree is included in the Swag source tree for convenience. Version is <code>15.0.7</code>.</br>
In order to build LLVM, you will have to install <a href="https://cmake.org/download/">cmake 3.23.2</a> (or later) and <a href="https://www.python.org/downloads/">python 3</a>.</br>
<h2 id="Build">Build</h2></p>
<p></br>
You will need <code>Visual Studio 2022 17.1</code> or later.</br>
</p>
<ul><li>As there's no automatic detection, edit <code>vs_build_cfg.bat</code> to match your version of Visual Studio and of the Windows SDK.</li>
<li>Launch <code>swag/build/vs_build_llvm_release.bat</code>. Note that building LLVM takes a crazy amount of time and memory, and can require multiple tries.</li>
<li>Launch <code>swag/build/vs_build_swag_release.bat</code>.</li>
<li>You can also launch <code>swag/build/vs_build_extern.bat</code>. This will build and update some external libraries in the standard workspace, and copy some libraries from the windows SDK.</li>
</ul>
<p></br>
If LLVM has been compiled once, you can also use the <code>Swag.sln</code> workspace in the <code>build</code> subfolder.</br>
<h2 id="Windows SDK">Windows SDK</h2></p>
<p>The path to the SDK version is defined in <code>vs_build_cfg.bat</code>.</br>
The Swag <b>runtime</b> contains a copy of some libraries from the SDK (<code>kernel32.lib</code>, <code>ucrt.lib</code>, <code>dbghelp.lib</code> and <code>user32.lib</code>). You will find them in <code>bin/runtime/windows-x86-64</code>.</br>
They are shipped with the compiler to avoid the necessity to install the SDK before building with Swag. That way the compiler can be used "as is".</br>
The standard modules (in <code>bin/std/modules</code>) can also have dependencies to some other libraries from the SDK. You will find those dependencies in the <code>publish</code> folder of the corresponding modules.</p>
</div>
</div>
<?php include('html_end.php'); ?>
