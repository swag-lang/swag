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
    .page {
        max-width:  1024px;
        margin:     8px auto;
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
        margin-left:        -30px;
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
<table class="enumeration">
<tr><td class="tdname"> <a href="std.core.php">std.core</a>         </td><td class="tdtype"> Main core module, the base of everything else</td></tr>
<tr><td class="tdname"> <a href="std.pixel.php">std.pixel</a>       </td><td class="tdtype"> A 2D painting module</td></tr>
<tr><td class="tdname"> <a href="std.gui.php">std.gui</a>           </td><td class="tdtype"> A user interface module (windows, widgets...)</td></tr>
<tr><td class="tdname"> <a href="std.ogl.php">std.ogl</a>           </td><td class="tdtype"> Opengl wrapper</td></tr>
<tr><td class="tdname"> <a href="std.libc.php">std.libc</a>         </td><td class="tdtype"> Libc wrapper</td></tr>
<tr><td class="tdname"> <a href="std.freetype.php">std.freetype</a> </td><td class="tdtype"> Freetype wrapper</td></tr>
<tr><td class="tdname"> <a href="std.win32.php">std.win32</a>       </td><td class="tdtype"> Win32 wrapper</td></tr>
<tr><td class="tdname"> <a href="std.gdi32.php">std.gdi32</a>       </td><td class="tdtype"> Gdi32 wrapper</td></tr>
<tr><td class="tdname"> <a href="std.xinput.php">std.xinput</a>     </td><td class="tdtype"> XInput wrapper</td></tr>
<tr><td class="tdname"> <a href="std.audio.php">std.audio</a>       </td><td class="tdtype"> Audio module to play sounds</td></tr>
</table>
</div>
</div>
</div>
</body>
</html>
