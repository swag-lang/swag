<?php include('html_start.php'); include('header.php'); ?>

<style>
<?php include('css/style.php'); ?>
</style>

<div class="lg:m-auto lg:w-[76rem] pt-2 lg:pt-10 overflow-hidden p-5">
<div class="container">
<h1 id="script-file">Script file</h1>
<p>Instead of a workspace, Swag can also be used to build and run a
simple script file, thanks to the fact that the compiler can run
anything at compile time. No executable will be generated, the compiler
will do all the job. To create a new script file with the special
extension ‘swgs’:</p>
<pre><code>$ swag new -f:myScript
=&gt; script file &#39;myScript.swgs&#39; has been created
=&gt; type &#39;swag script -f:myScript.swgs&#39; to run that script</code></pre>
<pre><code>// Swag script file
#dependencies
{
    // Here you can add your external dependencies
    // #import &quot;core&quot; location=&quot;swag@std&quot;
}

#run
{
    @print(&quot;Hello world !\n&quot;)
}</code></pre>
<p>You can then run your script with the ‘script’ command.</p>
<pre><code>$ swag script -f:myScript
Hello world !</code></pre>
<p>You can also just specify the script file <strong>with the
extension</strong> as a command.</p>
<pre><code>$ swag myScript.swgs
Hello world !</code></pre>
<p>You will find a bunch of small scripts in
<code>swag/bin/examples/scripts</code>. To run one of them from the
console, go to the folder and type for example
<code>swag pendulum.swgs</code></p>
<h2 id="dependencies">Dependencies</h2>
<p>You can add external dependencies, and they will be compiled and used
as native code.</p>
<pre><code>#dependencies
{
    // Import the standard module `core` from the swag standard workspace (which comes with the compiler)
    #import &quot;core&quot; location=&quot;swag@std&quot;
}</code></pre>
<p>A special hidden workspace (in the Swag cache folder) will be created
to contain all the corresponding native code. * To locate the Swag cache
folder, add <code>--verbose-path</code> to the command line. * To force
the build of dependencies, add <code>--rebuildall</code> to the command
line.</p>
<h2 id="more-than-one-script-file">More than one script file</h2>
<p>If your script is divided in more than one single file, you can add
<code>#load &lt;filename&gt;</code> in the <code>#dependencies</code>
block.</p>
<pre><code>#dependencies
{
    #load &quot;myOtherFile.swgs&quot;
    #load &quot;folder/myOtherOtherFile.swgs&quot;
}</code></pre>
<h2 id="debug">Debug</h2>
<p>The compiler comes with a bytecode debugger that can be used to trace
and debug compile time execution. Add <code>@breakpoint()</code> in your
code when you want the debugger to trigger.</p>
<p>The debugger command set is inspired by <a
href="https://docs.python.org/3/library/pdb.html">Pdb</a>, the python
debugger.</p>
</div>
</div>
<?php include('html_end.php'); ?>