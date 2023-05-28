<?php include('html_start.php'); include('header.php'); ?>

<style>
    h1, h2, h3, h4, h5, h6 {
        margin: revert;
        font-size: revert;
        font-weight: revert;
    }

    p {
        margin-top:     10px;
        margin-bottom:  10px;
    }

    p code {
        background-color: #eeeeee;
        border:             1px dotted #cccccc;
        padding:            2px;
        margin:             0px;
        font-size:          0.8em;
    }

    pre {
        background-color: #eeeeee;
        border:             1px solid LightGrey;
        margin:             20px;
        padding:            20px;
        font-size:          0.8em;
        white-space:        break-spaces;
        overflow-wrap:      break-word;
    }

    p a {
        text-decoration: none;
        color:           Blue;
    }

    p a:hover {
        text-decoration: underline;
    }

    blockquote {
        background-color:   LightYellow;
        border-left:        6px solid Orange;
        padding:            10px;
        margin-right:       10px;
    }
</style>

<div class="lg:m-auto lg:w-[76rem] pt-2 lg:pt-10 overflow-hidden p-5">
<ul>
<li><a href="std/std.core.html">std.core</a></li>
<li><a href="std/std.pixel.html">std.pixel</a></li>
<li><a href="std/std.gui.html">std.gui</a></li>
<li><a href="std/std.ogl.html">std.ogl</a></li>
<li><a href="std/std.libc.html">std.libc</a></li>
<li><a href="std/std.freetype.html">std.freetype</a></li>
<li><a href="std/std.win32.html">std.win32</a></li>
<li><a href="std/std.gdi32.html">std.gdi32</a></li>
<li><a href="std/std.xinput.html">std.xinput</a></li>
</ul>
</div>

<?php include('html_end.php'); ?>