<p>
<h1 id="How to build Swag">How to build Swag </h1></p>
<p>
</p>
<p>
<h2 id="LLVM">LLVM </h2></p>
<p>
</p>
<p>
Swag has two backends, a <code>x86_64</code> custom backend written for fast compile, but with far from optimal generated code, and <code>llvm</code> for optimized builds. </p>
<p>
The <a href="https://releases.llvm.org/download.html">LLVM</a> source tree is included in the Swag source tree for convenience. Version is <code>15.0.7</code>. </p>
<p>
In order to build LLVM, you will have to install <a href="https://cmake.org/download/">cmake 3.23.2</a> (or later) and <a href="https://www.python.org/downloads/">python 3</a>. </p>
<p>
<h2 id="Build">Build </h2></p>
<p>
</p>
<p>
You will need <code>Visual Studio 2022 17.1</code> or later. </p>
<p>
</p>
<ul>
<li>As there's no automatic detection, edit <code>vs_build_cfg.bat</code> to match your version of Visual Studio and of the Windows SDK. </li>
<li>Launch <code>swag/build/vs_build_llvm_release.bat</code>. Note that building LLVM takes a crazy amount of time and memory, and can require multiple tries. </li>
<li>Launch <code>swag/build/vs_build_swag_release.bat</code>. </li>
<li>You can also launch <code>swag/build/vs_build_extern.bat</code>. This will build and update some external libraries in the standard workspace, and copy some libraries from the windows SDK. </li>
</ul>
<p>
</p>
<p>
If LLVM has been compiled once, you can also use the <code>Swag.sln</code> workspace in the <code>build</code> subfolder. </p>
<p>
<h2 id="Windows SDK">Windows SDK </h2></p>
<p>
The path to the SDK version is defined in <code>vs_build_cfg.bat</code>. </p>
<p>
The Swag <b>runtime</b> contains a copy of some libraries from the SDK (<code>kernel32.lib</code>, <code>ucrt.lib</code>, <code>dbghelp.lib</code> and <code>user32.lib</code>). You will find them in <code>bin/runtime/windows-x86-64</code>. </p>
<p>
They are shipped with the compiler to avoid the necessity to install the SDK before building with Swag. That way the compiler can be used "as is". </p>
<p>
The standard modules (in <code>bin/std/modules</code>) can also have dependencies to some other libraries from the SDK. You will find those dependencies in the <code>publish</code> folder of the corresponding modules. </p>
