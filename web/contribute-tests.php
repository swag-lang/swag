<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Swag</title>
<link rel="icon" type="image/x-icon" href="favicon.ico">
<link rel="stylesheet" type="text/css" href="css/style.css">
<style>

        .container {
            display:        flex;
            flex-wrap:      nowrap;
            flex-direction: row;
            margin:         0px auto;
            padding:        0px;
        }
        .left {
            display:    block;
            overflow-y: scroll;
            width:      500px;
        }
        .left-page {
            margin:     10px;
        }
        .right {
            display:    block;
            width:      100%;
        }
        .right-page {
            max-width:  1024px;
            margin:     10px auto;
        }
        @media(min-width: 640px) {
            .container {
                max-width: 640px;
            }
        }
        @media(min-width: 768px) {
            .container {
                max-width: 768px;
            }
        }
        @media(min-width: 1024px) {
            .container {
                max-width: 1024px;
            }
        }
        @media(min-width: 1280px) {
            .container {
                max-width: 1280px;
            }
        }
        @media(min-width: 1536px) {
            .container {
                max-width: 1536px;
            }
        }
        @media screen and (max-width: 600px) {
            .left {
                display: none;
            }
            .right-page {
                margin:  10px;
            }
        }

        body {
            margin:         0px;
            line-height:    1.3em;
            font-family:    Segoe UI;
        }
        .container blockquote {
            border-radius:      5px;
            border:             1px solid Orange;
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .container a {
            color:              DoggerBlue;
        }
        .container a:hover {
            text-decoration:    underline;
        }
        .left a {
            text-decoration:    none;
        }
        .left ul {
            list-style-type:    none;
            margin-left:        -20px;
        }
        .left h3 {
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
        table.api-item {
            border-collapse:    separate;
            background-color:   Black;
            color:              White;
            width:              100%;
            margin-top:         70px;
            margin-right:       0px;
            font-size:          110%;
        }
        .api-item td:first-child {
            width:              33%;
            white-space:        nowrap;
        }
        .api-item-title-src-ref {
            text-align:         right;
        }
        .api-item-title-src-ref a {
            color:              inherit;
        }
        .api-item-title-kind {
            font-weight:        normal;
            font-size:          80%;
        }
        .api-item-title-light {
            font-weight:        normal;
        }
        .api-item-title-strong {
            font-weight:        bold;
            font-size:          100%;
        }
        .api-additional-infos {
            font-size:          90%;
            white-space:        break-spaces;
            overflow-wrap:      break-word;
        }
        table.table-enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .table-enumeration td {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            min-width:          100px;
        }
        .table-enumeration td:first-child {
            background-color:   #f8f8f8;
            white-space:        nowrap;
        }
        .table-enumeration a {
            text-decoration:    none;
        }
        .container td:last-child {
            width:              100%;
        }
        .tdname .inline-code {
            background-color:   revert;
            padding:            2px;
            border:             revert;
        }
        .code-type {
            background-color:   #eeeeee;
        }
        .inline-code {
            background-color:   #eeeeee;
            padding:            2px;
            border: 1px dotted  #cccccc;
        }
        .code-type a {
            color:              inherit;
        }
        .code-block {
            background-color:   #eeeeee;
            border-radius:      5px;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
        }
        .code-block a {
            color:  inherit; 
        }
            .SCde { color: #222222; }
    .SCmt { color: #71a35b; }
    .SCmp { color: #7f7f7f; }
    .SFct { color: #ff6a00; }
    .SCst { color: #3bc3a7; }
    .SItr { color: #b4b44a; }
    .STpe { color: #ed9a11; }
    .SKwd { color: #3186cd; }
    .SLgc { color: #b040be; }
    .SNum { color: #74a35b; }
    .SStr { color: #bb6643; }
    .SAtr { color: #7f7f7f; }
    .SInv { color: #ff0000; }
</style>
</head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="right">
<div class="right-page">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</blockquote>
<h1 id="Contribute_to_the_test_suite">Contribute to the test suite </h1>
<p>You can help by writing small tests to debug the compiler (and the compiler alone). </p>
<h3 id="Create_a_workspace_and_a_test_module">Create a workspace and a test module </h3>
<p>First create a workspace with the <code class="inline-code">--test</code> option. </p>
<div class="code-block"><code><span class="SCde">$ swag new -w:myWorkspace --test</span></code>
</div>
<p>This will also create a test module <code class="inline-code">myWorkspace</code> located in <code class="inline-code">myWorkspacetests</code>. </p>
<p>You can then compile and run the tests with the <code class="inline-code">test</code> command. </p>
<div class="code-block"><code><span class="SCde">$ swag test -w:myWorkspace</span></code>
</div>
<p>To force the build, add <code class="inline-code">--rebuild</code>. </p>
<h3 id="Write_tests">Write tests </h3>
<ul>
<li>Tests should be small, located in different <b>small files</b>.</li>
<li>Just write files like <code class="inline-code">test1.swg</code>, <code class="inline-code">test2.swg</code> and so on, in the <code class="inline-code">src</code> sub folder of your module.</li>
<li>Here you are only testing the compiler, and not the standard libraries. So do not add dependencies to external modules.</li>
<li>You can use <code class="inline-code">@print</code> temporary to see a result, but in the end tests should be silent. <code class="inline-code">@assert</code> is the way to go.</li>
<li><code class="inline-code">swagbintestsuite</code> is the official test suite for Swag. If your test module is cool, then it could ultimately find a place there.</li>
<li>Note that if a test works as expected, <b>keep it</b>, and <b>write another one</b> ! This is useful for testing regression.</li>
</ul>
</div>
</div>
</div>
</body>
</html>
