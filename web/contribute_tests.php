<?php include('html_start.php'); include('header.php'); ?>

<style>
<?php include('css/style.php'); ?>
</style>

<div class="lg:m-auto lg:w-[76rem] pt-2 lg:pt-10 overflow-hidden p-5">
<div class="container">
<h1 id="contribute-to-the-test-suite">Contribute to the test suite</h1>
<p>You can help by writing small tests to debug the compiler (and the
compiler alone).</p>
<h3 id="create-a-workspace-and-a-test-module">Create a workspace and a
test module</h3>
<p>First create a workspace with the <code>--test</code> option.</p>
<pre><code>$ swag new -w:myWorkspace --test</code></pre>
<p>This will also create a test module <code>myWorkspace</code> located
in <code>myWorkspace\tests</code>.</p>
<p>You can then compile and run the tests with the <code>test</code>
command.</p>
<pre><code>$ swag test -w:myWorkspace</code></pre>
<p>To force the build, add <code>--rebuild</code>.</p>
<h3 id="write-tests">Write tests</h3>
<ul>
<li>Tests should be small, located in different <strong>small
files</strong>. Just write files like <code>test1.swg</code>,
<code>test2.swg</code> and so on, in the <code>\src</code> sub folder of
your module.</li>
<li>Here you are only testing the compiler, and not the standard
libraries. So do not add dependencies to external modules.</li>
<li>You can use <code>@print</code> temporary to see a result, but in
the end tests should be silent. <code>@assert</code> is the way to
go.</li>
<li><code>swag\bin\testsuite</code> is the official test suite for Swag.
If your test module is cool, then it could ultimately find a place
there.</li>
<li>Note that if a test works as expected, <strong>keep it</strong>, and
<strong>write another one</strong> ! This is useful for testing
regression.</li>
</ul>
</div>
</div>
<?php include('html_end.php'); ?>