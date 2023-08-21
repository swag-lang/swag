<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><style>

        .container {
            display:        flex;
            flex-wrap:      nowrap;
            flex-direction: row;
            margin:         0px auto;
            padding:        0px;
        }
        .right {
            display:    block;
            width:      100%;
        }
        .rightpage {
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
        body {
            margin:         0px;
            line-height:    1.3em;
            font-family:    Segoe UI;
        }
        .container blockquote {
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            margin-right:       50px;
            padding:            10px;
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
        .container .enumeration td {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              auto;
        }
        .container .item td:first-child {
            width: 33%;
        }
        .container .enumeration td:first-child {
            background-color:   #f8f8f8;
            white-space:        nowrap;
            max-width:          33%;
        }
        .container td:last-child {
            width:              100%;
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
            margin-left:        20px;
            margin-right:       20px;
        }    .SyntaxCode      { color: #7f7f7f; }
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
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<blockquote>
<p>There's <b>no official</b> release yet. </p><p>Everything (including this web site) is still under construction... </p>
</blockquote>
<p style="white-space: break-spaces"><div style="background-color:Black" align="center">
    <p style="padding-top:30px;"> <img src="imgs/swag_logo.png" width=50%> </p>
    <p style="color:white;font-size:40px;"> <b>SWAG</b> Programming Language </p>
    <div style="margin-top:50px; padding-bottom:40px; color:White; font-size:20px;">
        <p><a href="getting-started.php" style="color:Orange;">Getting Started</a></p>
        <p><a href="https://github.com/swag-lang/swag/releases" style="color:Orange;">Download the Compiler</a> </p>
    </div>
</div>

<table style="table-layout:fixed;">
<tr>
    <td width=33% style="vertical-align:top; padding-left:30px; padding-right:30px;">
        <h2>A sophisticated toy</h2>
        <p>Swag is a system programming language made for fun because, let's be honest, C++ is now an horrible and ugly beast !</p>
        This is my third compiler (the other ones were developed for AAA game engines), but that one is by far the most advanced.
    </td>
    <td width=33% style="vertical-align:top; padding-left:30px; padding-right:30px;">
        <h2>Native or interpreted</h2>
        <p>The Swag compiler can generate fast <b>native</b> code, or act as an interpreter for a <b>scripting</b> language.</p>
        Imagine C++, but where everything could be <i>constexpr</i>.
    </td>
    <td width=33% style="vertical-align:top; padding-left:30px; padding-right:30px;">
        <h2>Modern</h2>
        <p>Swag has type reflection at both runtime and compile time, meta programmation, generics, a powerful macro system...</p>
        <b>Enjoy.</b>
    </td>
</tr>
</table>
<p align="center" style="padding:50px;"> <img src="imgs/syntax.png"> </p></p>
<blockquote>
<p>To take a look at the language, this is <a href="language.php">here</a>. </p><p>To take a look at the standard list of modules, this is <a href="std.php">here</a>. </p><p>To use Swag as a scripting language, this is <a href="swag-as-script.php">here</a>. </p>
</blockquote>
</div>
</div>
</div>
</body>
</html>
