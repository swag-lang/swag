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

    p code {
        background-color: #eeeeee;
        border:             1px dotted #cccccc;
        padding:            2px;
        margin:             0px;
        font-size:          0.8em;
    }

    pre {
        background-color: #eeeeee;
        border:             1px solid LightGrey;
        margin:             20px;
        padding:            20px;
        font-size:          0.8em;
    }

</style>

<h1 id="your-first-install">Your first install</h1>
<p>Download the latest release from github, and unzip it in a folder. Of
course SSD is better.</p>
<h4 id="under-windows-1011">Under windows 10/11</h4>
<p>You should register the location of the swag compiler
(<code>swag.exe</code>) in the PATH environment variable to be able to
call it from everywhere.</p>
<p>You can open a Powershell window, and run the following code :</p>
<pre><code># You must replace `f:\swag-lang\swag\bin` with your location of `swag.exe`

[Environment]::SetEnvironmentVariable(
   &quot;Path&quot;,
   [Environment]::GetEnvironmentVariable(&quot;Path&quot;, &quot;User&quot;) + &quot;;f:\swag-lang\swag\bin&quot;,
   &quot;User&quot;
)</code></pre>
<h1 id="your-first-project">Your first project</h1>
<p>The compile unit of swag is a <strong>workspace</strong> which
contains a variable number of <strong>modules</strong>. A module will
compile to a dynamic library or an executable.</p>
<p>To create a fresh new workspace named <em>first</em> :</p>
<pre><code>$ swag new -w:first
=&gt; workspace &#39;F:/first&#39; has been created
=&gt; module &#39;first&#39; has been created
=&gt; type &#39;swag run -w:F:\first&#39; to build and run that module</code></pre>
<p>This will also create a simple executable module <em>first</em> to
print “Hello world !”.</p>
<p><em>F:/first/modules/first/src/main.swg</em></p>
<pre><code>#main
{
    @print(&quot;Hello world!\n&quot;)
}</code></pre>
<p>A workspace contains a predefined number of sub folders : *
<code>modules/</code> contains all the modules (sub folders) of that
workspace. * <code>output/</code> (generated) contains the result of the
build (this is where the executable will be located). *
<code>tests/</code> (optional) contains a list of test modules. *
<code>examples/</code> (optional) contains a list of ‘examples’ modules.
* <code>dependencies/</code> (generated) contains a list of external
modules needed to compile the workspace.</p>
<p>A module is also organized in a predefined way : *
<code>moduleName/</code> the folder name of the module is used to create
output files. * <code>src/</code> contains the source code. *
<code>public/</code> (generated) will contain all the exports needed by
other modules to use that one (in case of a dynamic library). *
<code>publish/</code> contains additional files to use that module (like
an external C dll).</p>
<p>A module always contains a special file named
<code>module.swg</code>. This file is used to configure the module, and
is <strong>mandatory</strong>.</p>
<h4 id="to-compile-your-workspace">To compile your workspace</h4>
<pre><code>$ swag build -w:first
            Workspace first [fast-debug-windows-x86_64]
             Building first
                 Done 0.067s</code></pre>
<p>You can omit the workspace name (<code>-w:first</code> or
<code>--workspace:first</code>) if you call the compiler directly from
the workspace folder. This command will compile all modules in
<code>modules/</code> and <code>examples/</code>.</p>
<p>You can also build and run your workspace.</p>
<pre><code>$ swag run -w:first
            Workspace first [fast-debug-windows-x86_64]
             Building first
      Running backend first
Hello world!
                 Done 0.093s</code></pre>
<h4 id="note-on-windows-defender-realtime-protection">Note on Windows
Defender realtime protection</h4>
<p>It’s activated by default under Windows 10, and runs each time you
launch an executable or a process. This can increase the compile time of
your project, so consider excluding your Swag folder from it !</p>
<p>https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26#:~:text=Go%20to%20Start%20%3E%20Settings%20%3E%20Update,%2C%20file%20types%2C%20or%20process.</p>
<h1 id="content-of-the-swag-folder">Content of the Swag folder</h1>
<p>The Swag folder contains the compiler <code>swag.exe</code>, but also
a bunch of sub folders. * <code>examples/</code> is a workspace with
various code examples (mostly for testing). * <code>reference/</code> is
a workspace which contains an overview of the language, in the form of
small tests. * <code>testsuite/</code> is a workspace which contains all
the tests to debug the compiler. * <code>runtime/</code> contains the
compiler runtime, which is included in all user modules. *
<code>std/</code> is the <em>standard</em> workspace which contains the
<em>standard</em> modules that come with the compiler. A big work in
progress.</p>
<h1 id="the-swag-language">The Swag language</h1>
<p>You should take a look at the <code>reference/</code> sub folder in
the Swag directory. It contains the list of all that can be done with
the language, in the form of small tests (in fact it’s not really
exhaustive, but should be…).</p>
<p>It’s a good starting point to familiarize yourself with the
language.</p>
<p>And as this is a normal Swag workspace, you could also build and test
it with <code>swag test -w:swag/reference</code>.</p>
<p>You will also find some small examples (mostly written for tests) in
<code>swag/bin/examples/modules</code>. To build and run one of them
from the console, go to the workspace folder (<code>/examples</code>)
and type for example <code>swag run -m:wnd</code></p>
<?php include('html_end.php'); ?>