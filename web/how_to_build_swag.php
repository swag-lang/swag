<?php include('html_start.php'); include('header.php'); ?>

<style>
    html, body {
        height: 100%;
    }

    h1, h2, h3, h4, h5, h6 {
        margin: revert;
        font-size: revert;
        font-weight: revert;
    }

    pre {
        background-color: #eeeeee;
        border: 1px solid LightGrey;
        margin: 20px;
        padding: 20px;
    }

    code {
        font-size: 0.8em;
    }
</style>

<h1 id="how-to-build-swag">How to build Swag</h1>
<h2 id="llvm">LLVM</h2>
<p>Swag has two backends, a <code>x86_64</code> custom backend written
for fast compile, but with far from optimal generated code, and
<code>llvm</code> for optimized builds.</p>
<p>The LLVM source tree is included in the Swag source tree for
convenience. Version is <code>15.0.7</code></p>
<p>https://releases.llvm.org/download.html</p>
<p>In order to build LLVM, you will have to install
<code>cmake 3.23.2</code> or later (https://cmake.org/download/) and
<code>python 3</code> (https://www.python.org/downloads/)</p>
<h2 id="build">Build</h2>
<p>You will need <code>Visual Studio 2022 17.1</code> or later.</p>
<ul>
<li><p>As there’s no automatic detection, edit
<code>vs_build_cfg.bat</code> to match your version of Visual Studio and
of the Windows SDK.</p></li>
<li><p>Launch <code>swag/build/vs_build_llvm_release.bat</code>. Note
that building LLVM takes a crazy amount of time and memory, and can
require multiple tries.</p></li>
<li><p>Launch
<code>swag/build/vs_build_swag_release.bat</code>.</p></li>
<li><p>You can also launch <code>swag/build/vs_build_extern.bat</code>.
This will build and update some external libraries in the standard
workspace, and copy some libraries from the windows SDK.</p></li>
</ul>
<p>If LLVM has been compiled once, you can also use the
<code>Swag.sln</code> workspace in the <code>build</code> subfolder.</p>
<h2 id="windows-sdk">Windows SDK</h2>
<p>The path to the SDK version is defined in
<code>vs_build_cfg.bat</code>.</p>
<p>The Swag <strong>runtime</strong> contains a copy of some libraries
from the SDK (<code>kernel32.lib</code>, <code>ucrt.lib</code>,
<code>dbghelp.lib</code> and <code>user32.lib</code>). You will find
them in <code>bin/runtime/windows-x86-64</code>.</p>
<p>They are shipped with the compiler to avoid the necessity to install
the SDK before building with Swag. That way the compiler can be used “as
is”.</p>
<p>The standard modules (in <code>bin/std/modules</code>) can also have
dependencies to some other libraries from the SDK. You will find those
dependencies in the <code>publish</code> folder of the corresponding
module.</p>
<?php include('html_end.php'); ?>