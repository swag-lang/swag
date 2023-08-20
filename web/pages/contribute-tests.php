<html>
<body>
<head>
<meta charset="UTF-8">
</head>
<style>
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
            width:      650;
            height:     100%;
        }
        .right {
            display:     block;
            overflow-y:  scroll;
            width:       100%;
            line-height: 1.3em;
            height:      100%;
        }
        .page {
            width:  1000;
            margin: 0 auto;
        }
        blockquote {
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            padding:            10px;
            width:              90%;
        }
        a {
            text-decoration: none;
            color:           DoggerBlue;
        }
        a:hover {
            text-decoration: underline;
        }
        a.src {
            font-size:          90%;
            color:              LightGrey;
        }
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        td.enumeration {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              30%;
        }
        td.tdname {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              20%;
            background-color:   #f8f8f8;
        }
        td.tdtype {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              auto;
        }
        td:last-child {
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
        .code {
            background-color:   #eeeeee;
            border:             1px solid LightGrey;
            padding:            10px;
            width:              94%;
            margin-left:        20px;
        }
    .SyntaxCode { color: #7f7f7f; }
.SyntaxComment { color: #71a35b; }
.SyntaxCompiler { color: #7f7f7f; }
.SyntaxFunction { color: #ff6a00; }
.SyntaxConstant { color: #3173cd; }
.SyntaxIntrinsic { color: #b4b44a; }
.SyntaxType { color: #3bc3a7; }
.SyntaxKeyword { color: #3186cd; }
.SyntaxLogic { color: #b040be; }
.SyntaxNumber { color: #74a35b; }
.SyntaxString { color: #bb6643; }
.SyntaxAttribute { color: #7f7f7f; }
</style>
<div class="container">
<div class="right">
<div class="page">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.22.0)</blockquote>
<h2>Contribute to the test suite </h2>
<p>You can help by writing small tests to debug the compiler (and the compiler alone). </p>
<p>### Create a workspace and a test module </p>
<p>First create a workspace with the <code class="incode">--test</code> option. </p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode">$ swag new -w:myWorkspace --test</code>
</p>
<p>This will also create a test module <code class="incode">myWorkspace</code> located in <code class="incode">myWorkspace\tests</code>. </p>
<p>You can then compile and run the tests with the <code class="incode">test</code> command. </p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode">$ swag test -w:myWorkspace</code>
</p>
<p>To force the build, add <code class="incode">--rebuild</code>. </p>
<p>### Write tests </p>
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
</body>
</html>
