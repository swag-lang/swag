<p>
<h1 id="Contribute to the test suite">Contribute to the test suite </h1></p>
<p>
</p>
<p>
You can help by writing small tests to debug the compiler (and the compiler alone). </p>
<p>
<h3 id="Create a workspace and a test module">Create a workspace and a test module </h3></p>
<p>
</p>
<p>
First create a workspace with the <code>--test</code> option. </p>
<p>
</p>
<pre><code><span style="color:#0">$ swag new -w:myWorkspace --test</span></code></pre><p>
</p>
<p>
This will also create a test module <code>myWorkspace</code> located in <code>myWorkspace\tests</code>. </p>
<p>
You can then compile and run the tests with the <code>test</code> command. </p>
<p>
</p>
<pre><code><span style="color:#0">$ swag test -w:myWorkspace</span></code></pre><p>
</p>
<p>
To force the build, add <code>--rebuild</code>. </p>
<p>
<h3 id="Write tests">Write tests </h3></p>
<p>
</p>
<p>
</p>
<ul>
<li>Tests should be small, located in different <b>small files</b>. </li>
<li>Just write files like <code>test1.swg</code>, <code>test2.swg</code> and so on, in the <code>\src</code> sub folder of your module. </li>
<li>Here you are only testing the compiler, and not the standard libraries. So do not add dependencies to external modules. </li>
<li>You can use <code>@print</code> temporary to see a result, but in the end tests should be silent. <code>@assert</code> is the way to go. </li>
<li><code>swag\bin\testsuite</code> is the official test suite for Swag. If your test module is cool, then it could ultimately find a place there. </li>
<li>Note that if a test works as expected, <b>keep it</b>, and <b>write another one</b> ! This is useful for testing regression. </li>
</ul>
</p>
