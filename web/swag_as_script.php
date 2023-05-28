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

<h1 id="script-file">Script file</h1>
<p>Instead of a workspace, Swag can also be used to build and run a
simple script file, thanks to the fact that the compiler can run
anything at compile time. No executable will be generated, the compiler
will do all the job. To create a new script file with the special
extension ‘swgs’:</p>
<pre><code>$ swag new -f:myScript
=&gt; script file &#39;myScript.swgs&#39; has been created
=&gt; type &#39;swag script -f:myScript.swgs&#39; to run that script</code></pre>
<div class="sourceCode" id="cb2"><pre
class="sourceCode csharp"><code class="sourceCode cs"><span id="cb2-1"><a href="#cb2-1" aria-hidden="true" tabindex="-1"></a><span class="co">// Swag script file</span></span>
<span id="cb2-2"><a href="#cb2-2" aria-hidden="true" tabindex="-1"></a>#dependencies</span>
<span id="cb2-3"><a href="#cb2-3" aria-hidden="true" tabindex="-1"></a><span class="op">{</span></span>
<span id="cb2-4"><a href="#cb2-4" aria-hidden="true" tabindex="-1"></a>    <span class="co">// Here you can add your external dependencies</span></span>
<span id="cb2-5"><a href="#cb2-5" aria-hidden="true" tabindex="-1"></a>    <span class="co">// #import &quot;core&quot; location=&quot;swag@std&quot;</span></span>
<span id="cb2-6"><a href="#cb2-6" aria-hidden="true" tabindex="-1"></a><span class="op">}</span></span>
<span id="cb2-7"><a href="#cb2-7" aria-hidden="true" tabindex="-1"></a></span>
<span id="cb2-8"><a href="#cb2-8" aria-hidden="true" tabindex="-1"></a>#run</span>
<span id="cb2-9"><a href="#cb2-9" aria-hidden="true" tabindex="-1"></a><span class="op">{</span></span>
<span id="cb2-10"><a href="#cb2-10" aria-hidden="true" tabindex="-1"></a>    @<span class="fu">print</span><span class="op">(</span><span class="st">&quot;Hello world !</span><span class="sc">\n</span><span class="st">&quot;</span><span class="op">)</span></span>
<span id="cb2-11"><a href="#cb2-11" aria-hidden="true" tabindex="-1"></a><span class="op">}</span></span></code></pre></div>
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
<div class="sourceCode" id="cb5"><pre
class="sourceCode csharp"><code class="sourceCode cs"><span id="cb5-1"><a href="#cb5-1" aria-hidden="true" tabindex="-1"></a>#dependencies</span>
<span id="cb5-2"><a href="#cb5-2" aria-hidden="true" tabindex="-1"></a><span class="op">{</span></span>
<span id="cb5-3"><a href="#cb5-3" aria-hidden="true" tabindex="-1"></a>    <span class="co">// Import the standard module `core` from the swag standard workspace (which comes with the compiler)</span></span>
<span id="cb5-4"><a href="#cb5-4" aria-hidden="true" tabindex="-1"></a>    #import <span class="st">&quot;core&quot;</span> location<span class="op">=</span><span class="st">&quot;swag@std&quot;</span></span>
<span id="cb5-5"><a href="#cb5-5" aria-hidden="true" tabindex="-1"></a><span class="op">}</span></span></code></pre></div>
<p>A special hidden workspace (in the Swag cache folder) will be created
to contain all the corresponding native code. * To locate the Swag cache
folder, add <code>--verbose-path</code> to the command line. * To force
the build of dependencies, add <code>--rebuildall</code> to the command
line.</p>
<h2 id="more-than-one-script-file">More than one script file</h2>
<p>If your script is divided in more than one single file, you can add
<code>#load &lt;filename&gt;</code> in the <code>#dependencies</code>
block.</p>
<div class="sourceCode" id="cb6"><pre
class="sourceCode csharp"><code class="sourceCode cs"><span id="cb6-1"><a href="#cb6-1" aria-hidden="true" tabindex="-1"></a>#dependencies</span>
<span id="cb6-2"><a href="#cb6-2" aria-hidden="true" tabindex="-1"></a><span class="op">{</span></span>
<span id="cb6-3"><a href="#cb6-3" aria-hidden="true" tabindex="-1"></a>    #load <span class="st">&quot;myOtherFile.swgs&quot;</span></span>
<span id="cb6-4"><a href="#cb6-4" aria-hidden="true" tabindex="-1"></a>    #load <span class="st">&quot;folder/myOtherOtherFile.swgs&quot;</span></span>
<span id="cb6-5"><a href="#cb6-5" aria-hidden="true" tabindex="-1"></a><span class="op">}</span></span></code></pre></div>
<h2 id="debug">Debug</h2>
<p>The compiler comes with a bytecode debugger that can be used to trace
and debug compile time execution. Add <code>@breakpoint()</code> in your
code when you want the debugger to trigger.</p>
<p>The debugger command set is inspired by <a
href="https://docs.python.org/3/library/pdb.html">Pdb</a>, the python
debugger.</p>
<?php include('html_end.php'); ?>