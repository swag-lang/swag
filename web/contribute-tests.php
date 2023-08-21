<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<?php include('common/start-head.php'); ?><style>

    .container {
        display:        flex;
        flex-wrap:      nowrap;
        flex-direction: row;
        height:         100%;
        line-height:    1.3em;
        font-family:    Segoe UI;
    }
    .left {
        display:    block;
        overflow-y: scroll;
        width:      650px;
        height:     100%;
    }
    .right {
        display:     block;
        overflow-y:  scroll;
        line-height: 1.3em;
        width:       100%;
        height:      100%;
    }
    .page {
        width:  1000px;
        margin: 0px auto;
    }
    .container blockquote {
        background-color:   LightYellow;
        border-left:        6px solid Orange;
        padding:            10px;
        width:              90%;
    }
    .container a {
        text-decoration: none;
        color:           DoggerBlue;
    }
    .container a:hover {
        text-decoration: underline;
    }
    .container a.src {
        font-size:          90%;
        color:              LightGrey;
    }
    .container table.enumeration {
        border:             1px solid LightGrey;
        border-collapse:    collapse;
        width:              100%;
        font-size:          90%;
    }
    .container td.enumeration {
        padding:            6px;
        border:             1px solid LightGrey;
        border-collapse:    collapse;
        width:              30%;
    }
    .container td.tdname {
        padding:            6px;
        border:             1px solid LightGrey;
        border-collapse:    collapse;
        width:              20%;
        background-color:   #f8f8f8;
    }
    .container td.tdtype {
        padding:            6px;
        border:             1px solid LightGrey;
        border-collapse:    collapse;
        width:              auto;
    }
    .container td:last-child {
        width:              auto;
    }
    .left ul {
        list-style-type:    none;
        margin-left:        -20px;
    }
    .titletype {
        font-weight:        normal;
        font-size:          80%;
    }
    .titlelight {
        font-weight:        normal;
    }
    .titlestrong {
        font-weight:        bold;
        font-size:          100%;
    }
    .left h2 {
        background-color:   Black;
        color:              White;
        padding:            6px;
    }
    .right h1 {
        margin-top:         50px;
        margin-bottom:      50px;
    }
    .right h2 {
        margin-top:         35px;
    }
    table.item {
        background-color:   Black;
        color:              White;
        width:              100%;
        margin-top:         70px;
        margin-right:       0px;
        padding:            4px;
        font-size:          110%;
    }
    .srcref {
        text-align:         right;
    }
    .incode {
        background-color:   #eeeeee;
        padding:            2px;
        border: 1px dotted  #cccccc;
    }
    .tdname .incode {
        background-color:   revert;
        padding:            2px;
        border:             revert;
    }
    .addinfos {
        font-size:          90%;
        white-space:        break-spaces;
        overflow-wrap:      break-word;
    }
    .container pre {
        background-color:   #eeeeee;
        border:             1px solid LightGrey;
        padding:            10px;
        width:              94%;
        margin-left:        20px;
    }
    .SyntaxCode      { color: #7f7f7f; }
    .SyntaxComment   { color: #71a35b; }
    .SyntaxCompiler  { color: #7f7f7f; }
    .SyntaxFunction  { color: #ff6a00; }
    .SyntaxConstant  { color: #3173cd; }
    .SyntaxIntrinsic { color: #b4b44a; }
    .SyntaxType      { color: #3bc3a7; }
    .SyntaxKeyword   { color: #3186cd; }
    .SyntaxLogic     { color: #b040be; }
    .SyntaxNumber    { color: #74a35b; }
    .SyntaxString    { color: #bb6643; }
    .SyntaxAttribute { color: #7f7f7f; }
</style>
</head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="right">
<div class="page">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.22.0)</blockquote>
<h1>Contribute to the test suite </h1>
<p>You can help by writing small tests to debug the compiler (and the compiler alone). </p>
<h3>Create a workspace and a test module </h3>
<p>First create a workspace with the <code class="incode">--test</code> option. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode">$ swag new -w:myWorkspace --test</code>
</pre>
<p>This will also create a test module <code class="incode">myWorkspace</code> located in <code class="incode">myWorkspace\tests</code>. </p>
<p>You can then compile and run the tests with the <code class="incode">test</code> command. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode">$ swag test -w:myWorkspace</code>
</pre>
<p>To force the build, add <code class="incode">--rebuild</code>. </p>
<h3>Write tests </h3>
<ul>
<li>Tests should be small, located in different <b>small files</b>.</li>
<li>Just write files like <code class="incode">test1.swg</code>, <code class="incode">test2.swg</code> and so on, in the <code class="incode">\src</code> sub folder of your module.</li>
<li>Here you are only testing the compiler, and not the standard libraries. So do not add dependencies to external modules.</li>
<li>You can use <code class="incode">@print</code> temporary to see a result, but in the end tests should be silent. <code class="incode">@assert</code> is the way to go.</li>
<li><code class="incode">swag\bin\testsuite</code> is the official test suite for Swag. If your test module is cool, then it could ultimately find a place there.</li>
<li>Note that if a test works as expected, <b>keep it</b>, and <b>write another one</b> ! This is useful for testing regression.</li>
</ul>
</div>
</div>
</div>
<?php include('common/end-body.php'); ?></body>
</html>
