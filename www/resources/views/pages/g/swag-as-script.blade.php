<p>
<h1 id="Script file">Script file </h1></p>
<p>
Instead of a workspace, Swag can also be used to build and run a simple script file, thanks to the fact that the compiler can run anything at compile time. No executable will be generated, the compiler will do all the job. To create a new script file with the special extension 'swgs': </p>
<p>
</p>
<pre><code><span style="color:#0">$ swag new -f:myScript
=> script file 'myScript.swgs' has been created
=> type 'swag script -f:myScript.swgs' to run that script</span></code></pre><p>
</p>
<p>
</p>
<pre><code><span style="color:#6A9955">// Swag script file</span><span style="color:#0">
</span><span style="color:#FF6A00">#dependencies</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// Here you can add your external dependencies</span><span style="color:#0">
    </span><span style="color:#6A9955">// #import "core" location="swag@std"</span><span style="color:#0">
}

</span><span style="color:#FF6A00">#run</span><span style="color:#0">
{
    </span><span style="color:#B4B44A">@print</span><span style="color:#0">(</span><span style="color:#BB6643">"Hello world !\n"</span><span style="color:#0">)
}</span></code></pre><p>
</p>
<p>
You can then run your script with the 'script' command. </p>
<p>
</p>
<pre><code><span style="color:#0">$ swag script -f:myScript
</span><span style="color:#3BC3A7">Hello</span><span style="color:#0"> world !</span></code></pre><p>
</p>
<p>
You can also just specify the script file <b>with the extension</b> as a command. </p>
<p>
</p>
<pre><code><span style="color:#0">$ swag myScript.swgs
</span><span style="color:#3BC3A7">Hello</span><span style="color:#0"> world !</span></code></pre><p>
</p>
<p>
You will find a bunch of small scripts in <code>swag/bin/examples/scripts</code>. To run one of them from the console, go to the folder and type for example <code>swag pendulum.swgs</code> </p>
<p>
<h2 id="Dependencies">Dependencies </h2></p>
<p>
</p>
<p>
You can add external dependencies, and they will be compiled and used as native code. </p>
<p>
</p>
<pre><code><span style="color:#FF6A00">#dependencies</span><span style="color:#0">
{
    </span><span style="color:#6A9955">// Import the standard module `core` from the swag standard workspace (which comes with the compiler)</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#import</span><span style="color:#0"> </span><span style="color:#BB6643">"core"</span><span style="color:#0"> location=</span><span style="color:#BB6643">"swag@std"</span><span style="color:#0">
}</span></code></pre><p>
</p>
<p>
A special hidden workspace (in the Swag cache folder) will be created to contain all the corresponding native code. </p>
<p>
</p>
<ul>
<li>To locate the Swag cache folder, add <code>--verbose-path</code> to the command line. </li>
<li>To force the build of dependencies, add <code>--rebuildall</code> to the command line. </li>
</ul>
<p>
</p>
<p>
<h2 id="More than one script file">More than one script file </h2></p>
<p>
</p>
<p>
If your script is divided in more than one single file, you can add <code>#load <filename></code> in the <code>#dependencies</code> block. </p>
<p>
</p>
<pre><code><span style="color:#FF6A00">#dependencies</span><span style="color:#0">
{
    </span><span style="color:#7F7F7F">#load</span><span style="color:#0"> </span><span style="color:#BB6643">"myOtherFile.swgs"</span><span style="color:#0">
    </span><span style="color:#7F7F7F">#load</span><span style="color:#0"> </span><span style="color:#BB6643">"folder/myOtherOtherFile.swgs"</span><span style="color:#0">
}</span></code></pre><p>
</p>
<p>
<h2 id="Debug">Debug </h2></p>
<p>
</p>
<p>
The compiler comes with a <b>bytecode debugger</b> that can be used to trace and debug compile time execution. Add <code>@breakpoint()</code> in your code when you want the debugger to trigger. </p>
<p>
The debugger command set is inspired by <a href="https://docs.python.org/3/library/pdb.html">Pdb</a>, the python debugger. </p>
