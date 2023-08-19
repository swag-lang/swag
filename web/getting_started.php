<?php include('html_start.php'); ?>
<?php include('header.php'); ?>
<style><?php include('css/style.php'); ?></style>
<div class="lg:m-auto lg:w-[76rem] pt-2 lg:pt-10 overflow-hidden p-5">
<div class="container">
<p>
<h1 id="Your first install">Your first install </h1></p>
<p>
<a href="https://github.com/swag-lang/swag/releases">Download</a> the latest release from github, and unzip it in a folder. Of course a <i>SSD</i> is better. </p>
<p>
<h3 id="Under windows 10/11">Under windows 10/11 </h3></p>
<p>
You should register the location of the swag compiler (<code>swag.exe</code>) in the PATH environment variable to be able to call it from everywhere. </p>
<p>
You can open a Powershell window, and run the following code : </p>
<p>
</p>
<pre><code><span style="color:#0"># </span><span style="color:#3BC3A7">You</span><span style="color:#0"> must replace `f:\swag-lang\swag\bin` </span><span style="color:#3186CD">with</span><span style="color:#0"> your location of `swag.exe`

[</span><span style="color:#3BC3A7">Environment</span><span style="color:#0">]::</span><span style="color:#3BC3A7">SetEnvironmentVariable</span><span style="color:#0">(
   </span><span style="color:#BB6643">"Path"</span><span style="color:#0">,
   [</span><span style="color:#3BC3A7">Environment</span><span style="color:#0">]::</span><span style="color:#3BC3A7">GetEnvironmentVariable</span><span style="color:#0">(</span><span style="color:#BB6643">"Path"</span><span style="color:#0">, </span><span style="color:#BB6643">"User"</span><span style="color:#0">) + </span><span style="color:#BB6643">";f:\swag-lang\swag\bin"</span><span style="color:#0">,
   </span><span style="color:#BB6643">"User"</span><span style="color:#0">
)</span></code></pre><p>
</p>
<p>
<h1 id="Your first project">Your first project </h1></p>
<p>
</p>
<p>
The compile unit of swag is a <b>workspace</b> which contains a variable number of <b>modules</b>. A module will compile to a dynamic library or an executable. </p>
<p>
To create a fresh new workspace named <i>first</i> : </p>
<p>
</p>
<pre><code><span style="color:#0">$ swag new -w:first
=> workspace '</span><span style="color:#3BC3A7">F</span><span style="color:#0">:/first' has been created
=> module 'first' has been created
=> type 'swag run -w:</span><span style="color:#3BC3A7">F</span><span style="color:#0">:\first' to build </span><span style="color:#B040BE">and</span><span style="color:#0"> run that module</span></code></pre><p>
</p>
<p>
This will also create a simple executable module <i>first</i> to print "Hello world !". </p>
<p>
<i>F:/first/modules/first/src/main.swg</i> </p>
<p>
</p>
<pre><code><span style="color:#FF6A00">#main</span><span style="color:#0">
{
    </span><span style="color:#B4B44A">@print</span><span style="color:#0">(</span><span style="color:#BB6643">"Hello world!\n"</span><span style="color:#0">)
}</span></code></pre><p>
</p>
<p>
A workspace contains a predefined number of sub folders: </p>
<p>
</p>
<ul>
<li><code>modules/</code> contains all the modules (sub folders) of that workspace. </li>
<li><code>output/</code> (generated) contains the result of the build (this is where the executable will be located). </li>
<li><code>tests/</code> (optional) contains a list of test modules. </li>
<li><code>examples/</code> (optional) contains a list of 'examples' modules. </li>
<li><code>dependencies/</code> (generated) contains a list of external modules needed to compile the workspace. </li>
</ul>
<p>
</p>
<p>
A module is also organized in a predefined way: </p>
<p>
</p>
<ul>
<li><code>moduleName/</code> the folder name of the module is used to create output files. </li>
<li><code>src/</code> contains the source code. </li>
<li><code>public/</code> (generated) will contain all the exports needed by other modules to use that one (in case of a dynamic library). </li>
<li><code>publish/</code> contains additional files to use that module (like an external C dll). </li>
</ul>
<p>
</p>
<p>
A module always contains a special file named <code>module.swg</code>. This file is used to configure the module, and is <b>mandatory</b>. </p>
<p>
<h3 id="To compile your workspace">To compile your workspace </h3></p>
<p>
</p>
<pre><code><span style="color:#0">$ swag build -w:first
            </span><span style="color:#3BC3A7">Workspace</span><span style="color:#0"> first [fast-debug-windows-x86_64]
             </span><span style="color:#3BC3A7">Building</span><span style="color:#0"> first
                 </span><span style="color:#3BC3A7">Done</span><span style="color:#0"> </span><span style="color:#74A35B">0.067</span><span style="color:#0">s</span></code></pre><p>
</p>
<p>
You can omit the workspace name (<code>-w:first</code> or <code>--workspace:first</code>) if you call the compiler directly from the workspace folder. This command will compile all modules in <code>modules/</code> and <code>examples/</code>. </p>
<p>
You can also build and run your workspace. </p>
<p>
</p>
<pre><code><span style="color:#0">$ swag run -w:first
            </span><span style="color:#3BC3A7">Workspace</span><span style="color:#0"> first [fast-debug-windows-x86_64]
             </span><span style="color:#3BC3A7">Building</span><span style="color:#0"> first
      </span><span style="color:#3BC3A7">Running</span><span style="color:#0"> backend first
</span><span style="color:#3BC3A7">Hello</span><span style="color:#0"> world!
                 </span><span style="color:#3BC3A7">Done</span><span style="color:#0"> </span><span style="color:#74A35B">0.093</span><span style="color:#0">s</span></code></pre><p>
</p>
<p>
<h3 id="Note on Windows Defender realtime protection">Note on Windows Defender realtime protection </h3></p>
<p>
It's activated by default under Windows 10, and runs each time you launch an executable or a process. This can increase the compile time of your project, so consider excluding your Swag folder from it ! </p>
<p>
<a href="https://support.microsoft.com/en-us/windows/add-an-exclusion-to-windows-security-811816c0-4dfd-af4a-47e4-c301afe13b26#:~:text=Go%20to%20Start%20%3E%20Settings%20%3E%20Update,%2C%20file%20types%2C%20or%20process">Reference</a> </p>
<p>
<h1 id="Content of the Swag folder">Content of the Swag folder </h1></p>
<p>
The Swag folder contains the compiler <code>swag.exe</code>, but also a bunch of sub folders. </p>
<p>
</p>
<ul>
<li><code>examples/</code> is a workspace with various code examples (mostly for testing). </li>
<li><code>reference/</code> is a workspace which contains an overview of the language, in the form of small tests. </li>
<li><code>testsuite/</code> is a workspace which contains all the tests to debug the compiler. </li>
<li><code>runtime/</code> contains the compiler runtime, which is included in all user modules. </li>
<li><code>std/</code> is the <a href="std.php">standard workspace</a> which contains all the standard modules that come with the compiler. A big work in progress. </li>
</ul>
<p>
</p>
<p>
<h1 id="The Swag language">The Swag language </h1></p>
<p>
You should take a look at the <code>reference/</code> sub folder in the Swag directory, or to the corresponding <a href="doc/language.html">documentation</a>. It contains the list of all that can be done with the language, in the form of small tests (in fact it's not really exhaustive, but should be...). </p>
<p>
It's a good starting point to familiarize yourself with the language. </p>
<p>
And as this is a normal Swag workspace, you could also build and test it with <code>swag test -w:swag/reference</code>. </p>
<p>
You will also find some small examples (mostly written for tests) in <code>swag/bin/examples/modules</code>. To build and run one of them from the console, go to the workspace folder (<code>/examples</code>) and type for example <code>swag run -m:wnd</code>. </p>
</div>
</div>
<?php include('html_end.php'); ?>
