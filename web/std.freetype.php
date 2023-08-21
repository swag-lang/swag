<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
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
            width:      600px;
        }
        .leftpage {
            margin:     10px;
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
        @media screen and (max-width: 600px) {
            .left {
                display: none;
            }
            .rightpage {
                margin:  10px;
            }
        }
            .container {
                height:     100vh;
            }
            .right {
                overflow-y: scroll;
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
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .item td:first-child {
            width:              33%;
            white-space:        nowrap;
        }
        .enumeration td {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            min-width:          100px;
        }
        .enumeration td:first-child {
            background-color:   #f8f8f8;
            white-space:        nowrap;
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
<div class="container">
<div class="left">
<div class="leftpage">
<h1>Module freetype</h1>
<h2>Structs</h2>
<h3></h3>
<ul>
<li><a href="#FreeType_FT_BBox">FT_BBox</a></li>
<li><a href="#FreeType_FT_Bitmap">FT_Bitmap</a></li>
<li><a href="#FreeType_FT_Bitmap_Size">FT_Bitmap_Size</a></li>
<li><a href="#FreeType_FT_CharMapRec">FT_CharMapRec</a></li>
<li><a href="#FreeType_FT_FaceRec">FT_FaceRec</a></li>
<li><a href="#FreeType_FT_Generic">FT_Generic</a></li>
<li><a href="#FreeType_FT_GlyphSlotRec">FT_GlyphSlotRec</a></li>
<li><a href="#FreeType_FT_Glyph_Metrics">FT_Glyph_Metrics</a></li>
<li><a href="#FreeType_FT_LibraryRec">FT_LibraryRec</a></li>
<li><a href="#FreeType_FT_ListRec">FT_ListRec</a></li>
<li><a href="#FreeType_FT_Matrix">FT_Matrix</a></li>
<li><a href="#FreeType_FT_Outline">FT_Outline</a></li>
<li><a href="#FreeType_FT_Outline_Funcs">FT_Outline_Funcs</a></li>
<li><a href="#FreeType_FT_SizeRec">FT_SizeRec</a></li>
<li><a href="#FreeType_FT_Size_Metrics">FT_Size_Metrics</a></li>
<li><a href="#FreeType_FT_SubGlyphRec">FT_SubGlyphRec</a></li>
<li><a href="#FreeType_FT_Vector">FT_Vector</a></li>
</ul>
<h2>Enums</h2>
<h3></h3>
<ul>
<li><a href="#FreeType_FT_Glyph_Format">FT_Glyph_Format</a></li>
<li><a href="#FreeType_FT_LcdFilter">FT_LcdFilter</a></li>
<li><a href="#FreeType_FT_Pixel_Mode">FT_Pixel_Mode</a></li>
<li><a href="#FreeType_FT_Render_Mode">FT_Render_Mode</a></li>
</ul>
<h2>Constants</h2>
<h3></h3>
<ul>
<li><a href="#FreeType_FT_LOAD_ADVANCE_ONLY">FT_LOAD_ADVANCE_ONLY</a></li>
<li><a href="#FreeType_FT_LOAD_BITMAP_METRICS_ONLY">FT_LOAD_BITMAP_METRICS_ONLY</a></li>
<li><a href="#FreeType_FT_LOAD_COLOR">FT_LOAD_COLOR</a></li>
<li><a href="#FreeType_FT_LOAD_COMPUTE_METRICS">FT_LOAD_COMPUTE_METRICS</a></li>
<li><a href="#FreeType_FT_LOAD_CROP_BITMAP">FT_LOAD_CROP_BITMAP</a></li>
<li><a href="#FreeType_FT_LOAD_DEFAULT">FT_LOAD_DEFAULT</a></li>
<li><a href="#FreeType_FT_LOAD_FORCE_AUTOHINT">FT_LOAD_FORCE_AUTOHINT</a></li>
<li><a href="#FreeType_FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH">FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH</a></li>
<li><a href="#FreeType_FT_LOAD_IGNORE_TRANSFORM">FT_LOAD_IGNORE_TRANSFORM</a></li>
<li><a href="#FreeType_FT_LOAD_LINEAR_DESIGN">FT_LOAD_LINEAR_DESIGN</a></li>
<li><a href="#FreeType_FT_LOAD_MONOCHROME">FT_LOAD_MONOCHROME</a></li>
<li><a href="#FreeType_FT_LOAD_NO_AUTOHINT">FT_LOAD_NO_AUTOHINT</a></li>
<li><a href="#FreeType_FT_LOAD_NO_BITMAP">FT_LOAD_NO_BITMAP</a></li>
<li><a href="#FreeType_FT_LOAD_NO_HINTING">FT_LOAD_NO_HINTING</a></li>
<li><a href="#FreeType_FT_LOAD_NO_RECURSE">FT_LOAD_NO_RECURSE</a></li>
<li><a href="#FreeType_FT_LOAD_NO_SCALE">FT_LOAD_NO_SCALE</a></li>
<li><a href="#FreeType_FT_LOAD_PEDANTIC">FT_LOAD_PEDANTIC</a></li>
<li><a href="#FreeType_FT_LOAD_RENDER">FT_LOAD_RENDER</a></li>
<li><a href="#FreeType_FT_LOAD_SBITS_ONLY">FT_LOAD_SBITS_ONLY</a></li>
<li><a href="#FreeType_FT_LOAD_TARGET_LCD">FT_LOAD_TARGET_LCD</a></li>
<li><a href="#FreeType_FT_LOAD_TARGET_LCD_V">FT_LOAD_TARGET_LCD_V</a></li>
<li><a href="#FreeType_FT_LOAD_TARGET_LIGHT">FT_LOAD_TARGET_LIGHT</a></li>
<li><a href="#FreeType_FT_LOAD_TARGET_MONO">FT_LOAD_TARGET_MONO</a></li>
<li><a href="#FreeType_FT_LOAD_TARGET_NORMAL">FT_LOAD_TARGET_NORMAL</a></li>
<li><a href="#FreeType_FT_LOAD_VERTICAL_LAYOUT">FT_LOAD_VERTICAL_LAYOUT</a></li>
<li><a href="#FreeType_FT_MAX_MODULES">FT_MAX_MODULES</a></li>
<li><a href="#FreeType_FT_STYLE_FLAG_BOLD">FT_STYLE_FLAG_BOLD</a></li>
<li><a href="#FreeType_FT_STYLE_FLAG_ITALIC">FT_STYLE_FLAG_ITALIC</a></li>
</ul>
<h2>Functions</h2>
<h3></h3>
<ul>
<li><a href="#FreeType_FT_Done_Face">FreeType.FT_Done_Face</a></li>
<li><a href="#FreeType_FT_Done_FreeType">FreeType.FT_Done_FreeType</a></li>
<li><a href="#FreeType_FT_Get_Char_Index">FreeType.FT_Get_Char_Index</a></li>
<li><a href="#FreeType_FT_GlyphSlot_Oblique">FreeType.FT_GlyphSlot_Oblique</a></li>
<li><a href="#FreeType_FT_IMAGE_TAG">FreeType.FT_IMAGE_TAG</a></li>
<li><a href="#FreeType_FT_Init_FreeType">FreeType.FT_Init_FreeType</a></li>
<li><a href="#FreeType_FT_LOAD_TARGET">FreeType.FT_LOAD_TARGET</a></li>
<li><a href="#FreeType_FT_Library_SetLcdFilter">FreeType.FT_Library_SetLcdFilter</a></li>
<li><a href="#FreeType_FT_Library_SetLcdFilterWeights">FreeType.FT_Library_SetLcdFilterWeights</a></li>
<li><a href="#FreeType_FT_Load_Char">FreeType.FT_Load_Char</a></li>
<li><a href="#FreeType_FT_Load_Glyph">FreeType.FT_Load_Glyph</a></li>
<li><a href="#FreeType_FT_New_Memory_Face">FreeType.FT_New_Memory_Face</a></li>
<li><a href="#FreeType_FT_Outline_Decompose">FreeType.FT_Outline_Decompose</a></li>
<li><a href="#FreeType_FT_Outline_Embolden">FreeType.FT_Outline_Embolden</a></li>
<li><a href="#FreeType_FT_Outline_Transform">FreeType.FT_Outline_Transform</a></li>
<li><a href="#FreeType_FT_Outline_Translate">FreeType.FT_Outline_Translate</a></li>
<li><a href="#FreeType_FT_Render_Glyph">FreeType.FT_Render_Glyph</a></li>
<li><a href="#FreeType_FT_Set_Pixel_Sizes">FreeType.FT_Set_Pixel_Sizes</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Overview</h1>
<p>:LICENCE     The freetype module is a wrapper for the great Freetype 2 library.     See LICENCE.md for the corresponding licence (FTL). </p>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_LOAD_ADVANCE_ONLY"><span class="titletype">const</span> <span class="titlelight">FreeType.</span><span class="titlestrong">Constants</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="FreeType_FT_LOAD_ADVANCE_ONLY">FT_LOAD_ADVANCE_ONLY</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_BITMAP_METRICS_ONLY">FT_LOAD_BITMAP_METRICS_ONLY</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_COLOR">FT_LOAD_COLOR</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_COMPUTE_METRICS">FT_LOAD_COMPUTE_METRICS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_CROP_BITMAP">FT_LOAD_CROP_BITMAP</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_DEFAULT">FT_LOAD_DEFAULT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_FORCE_AUTOHINT">FT_LOAD_FORCE_AUTOHINT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH">FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_IGNORE_TRANSFORM">FT_LOAD_IGNORE_TRANSFORM</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_LINEAR_DESIGN">FT_LOAD_LINEAR_DESIGN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_MONOCHROME">FT_LOAD_MONOCHROME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_AUTOHINT">FT_LOAD_NO_AUTOHINT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_BITMAP">FT_LOAD_NO_BITMAP</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_HINTING">FT_LOAD_NO_HINTING</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_RECURSE">FT_LOAD_NO_RECURSE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_SCALE">FT_LOAD_NO_SCALE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_PEDANTIC">FT_LOAD_PEDANTIC</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_RENDER">FT_LOAD_RENDER</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_SBITS_ONLY">FT_LOAD_SBITS_ONLY</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LCD">FT_LOAD_TARGET_LCD</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LCD_V">FT_LOAD_TARGET_LCD_V</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LIGHT">FT_LOAD_TARGET_LIGHT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_MONO">FT_LOAD_TARGET_MONO</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_NORMAL">FT_LOAD_TARGET_NORMAL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_VERTICAL_LAYOUT">FT_LOAD_VERTICAL_LAYOUT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_MAX_MODULES">FT_MAX_MODULES</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_STYLE_FLAG_BOLD">FT_STYLE_FLAG_BOLD</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_STYLE_FLAG_ITALIC">FT_STYLE_FLAG_ITALIC</td>
<td>s32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_BBox"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_BBox</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L129" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>xMin</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>yMin</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>xMax</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>yMax</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Bitmap"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Bitmap</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L213" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>rows</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>width</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>pitch</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>buffer</td>
<td>^u8</td>
<td></td>
</tr>
<tr>
<td>num_grays</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>pixel_mode</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>palette_mode</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>palette</td>
<td>^void</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Bitmap_Size"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Bitmap_Size</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>height</td>
<td>FreeType.FT_Short</td>
<td></td>
</tr>
<tr>
<td>width</td>
<td>FreeType.FT_Short</td>
<td></td>
</tr>
<tr>
<td>size</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>x_ppem</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>y_ppem</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_CharMapRec"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_CharMapRec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L141" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>face</td>
<td>FreeType.FT_Face</td>
<td></td>
</tr>
<tr>
<td>encoding</td>
<td>u32</td>
<td>FT_Encoding. </td>
</tr>
<tr>
<td>platform_id</td>
<td>FreeType.FT_UShort</td>
<td></td>
</tr>
<tr>
<td>encoding_id</td>
<td>FreeType.FT_UShort</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Done_Face"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Done_Face</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L408" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Done_Face</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Face</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Done_FreeType"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Done_FreeType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L441" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Done_FreeType</span><span class="SyntaxCode">(alibrary: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Library</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_FaceRec"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_FaceRec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L94" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>num_faces</td>
<td>FreeType.FT_Long</td>
<td></td>
</tr>
<tr>
<td>face_index</td>
<td>FreeType.FT_Long</td>
<td></td>
</tr>
<tr>
<td>face_flags</td>
<td>FreeType.FT_Long</td>
<td></td>
</tr>
<tr>
<td>style_flags</td>
<td>FreeType.FT_Long</td>
<td></td>
</tr>
<tr>
<td>num_glyphs</td>
<td>FreeType.FT_Long</td>
<td></td>
</tr>
<tr>
<td>family_name</td>
<td>^FreeType.FT_String</td>
<td></td>
</tr>
<tr>
<td>style_name</td>
<td>^FreeType.FT_String</td>
<td></td>
</tr>
<tr>
<td>num_fixed_sizes</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>available_sizes</td>
<td>^FreeType.FT_Bitmap_Size</td>
<td></td>
</tr>
<tr>
<td>num_charmaps</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>charmaps</td>
<td>^FreeType.FT_CharMap</td>
<td></td>
</tr>
<tr>
<td>generic</td>
<td><a href="#FreeType_FT_Generic">FreeType.FT_Generic</a></td>
<td></td>
</tr>
<tr>
<td>bbox</td>
<td><a href="#FreeType_FT_BBox">FreeType.FT_BBox</a></td>
<td></td>
</tr>
<tr>
<td>units_per_EM</td>
<td>FreeType.FT_UShort</td>
<td></td>
</tr>
<tr>
<td>ascender</td>
<td>FreeType.FT_Short</td>
<td></td>
</tr>
<tr>
<td>descender</td>
<td>FreeType.FT_Short</td>
<td></td>
</tr>
<tr>
<td>height</td>
<td>FreeType.FT_Short</td>
<td></td>
</tr>
<tr>
<td>max_advance_width</td>
<td>FreeType.FT_Short</td>
<td></td>
</tr>
<tr>
<td>max_advance_height</td>
<td>FreeType.FT_Short</td>
<td></td>
</tr>
<tr>
<td>underline_position</td>
<td>FreeType.FT_Short</td>
<td></td>
</tr>
<tr>
<td>underline_thickness</td>
<td>FreeType.FT_Short</td>
<td></td>
</tr>
<tr>
<td>glyph</td>
<td>FreeType.FT_GlyphSlot</td>
<td></td>
</tr>
<tr>
<td>size</td>
<td>FreeType.FT_Size</td>
<td></td>
</tr>
<tr>
<td>charmap</td>
<td>FreeType.FT_CharMap</td>
<td></td>
</tr>
<tr>
<td>driver</td>
<td>FreeType.FT_Driver</td>
<td></td>
</tr>
<tr>
<td>memory</td>
<td>FreeType.FT_Memory</td>
<td></td>
</tr>
<tr>
<td>stream</td>
<td>FreeType.FT_Stream</td>
<td></td>
</tr>
<tr>
<td>sizes_list</td>
<td><a href="#FreeType_FT_ListRec">FreeType.FT_ListRec</a></td>
<td></td>
</tr>
<tr>
<td>autohint</td>
<td><a href="#FreeType_FT_Generic">FreeType.FT_Generic</a></td>
<td></td>
</tr>
<tr>
<td>extensions</td>
<td>^void</td>
<td></td>
</tr>
<tr>
<td>finternal</td>
<td>FreeType.FT_Face_Internal</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Generic"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Generic</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L135" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>data</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>finalizer</td>
<td>*void</td>
<td>FT_Generic_Finalizer. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Get_Char_Index"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Get_Char_Index</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L409" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Get_Char_Index</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Face</span><span class="SyntaxCode">, charcode: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_ULong</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_UInt</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_GlyphSlotRec"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_GlyphSlotRec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L176" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>library</td>
<td>FreeType.FT_Library</td>
<td></td>
</tr>
<tr>
<td>face</td>
<td>FreeType.FT_Face</td>
<td></td>
</tr>
<tr>
<td>next</td>
<td>FreeType.FT_GlyphSlot</td>
<td></td>
</tr>
<tr>
<td>glyph_index</td>
<td>FreeType.FT_UInt</td>
<td></td>
</tr>
<tr>
<td>generic</td>
<td><a href="#FreeType_FT_Generic">FreeType.FT_Generic</a></td>
<td></td>
</tr>
<tr>
<td>metrics</td>
<td><a href="#FreeType_FT_Glyph_Metrics">FreeType.FT_Glyph_Metrics</a></td>
<td></td>
</tr>
<tr>
<td>linearHoriAdvance</td>
<td>FreeType.FT_Fixed</td>
<td></td>
</tr>
<tr>
<td>linearVertAdvance</td>
<td>FreeType.FT_Fixed</td>
<td></td>
</tr>
<tr>
<td>advance</td>
<td><a href="#FreeType_FT_Vector">FreeType.FT_Vector</a></td>
<td></td>
</tr>
<tr>
<td>format</td>
<td><a href="#FreeType_FT_Glyph_Format">FreeType.FT_Glyph_Format</a></td>
<td></td>
</tr>
<tr>
<td>bitmap</td>
<td><a href="#FreeType_FT_Bitmap">FreeType.FT_Bitmap</a></td>
<td></td>
</tr>
<tr>
<td>bitmap_left</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>bitmap_top</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>outline</td>
<td><a href="#FreeType_FT_Outline">FreeType.FT_Outline</a></td>
<td></td>
</tr>
<tr>
<td>num_subglyphs</td>
<td>FreeType.FT_UInt</td>
<td></td>
</tr>
<tr>
<td>subglyphs</td>
<td>FreeType.FT_SubGlyph</td>
<td></td>
</tr>
<tr>
<td>control_data</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>control_len</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lsb_delta</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>rsb_delta</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>other</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>finternal</td>
<td>FreeType.FT_Slot_Internal</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_GlyphSlot_Oblique"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_GlyphSlot_Oblique</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L412" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_GlyphSlot_Oblique</span><span class="SyntaxCode">(slot: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_GlyphSlot</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Glyph_Format"><span class="titletype">enum</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Glyph_Format</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L204" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>FT_GLYPH_FORMAT_NONE</td>
<td></td>
</tr>
<tr>
<td>FT_GLYPH_FORMAT_COMPOSITE</td>
<td></td>
</tr>
<tr>
<td>FT_GLYPH_FORMAT_BITMAP</td>
<td></td>
</tr>
<tr>
<td>FT_GLYPH_FORMAT_OUTLINE</td>
<td></td>
</tr>
<tr>
<td>FT_GLYPH_FORMAT_POLTTER</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Glyph_Metrics"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Glyph_Metrics</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L235" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>width</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>height</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>horiBearingX</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>horiBearingY</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>horiAdvance</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>vertBearingX</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>vertBearingY</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>vertAdvance</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_IMAGE_TAG"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_IMAGE_TAG</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L203" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_IMAGE_TAG</span><span class="SyntaxCode">(x1: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, x2: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, x3: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, x4: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Init_FreeType"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Init_FreeType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L435" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Init_FreeType</span><span class="SyntaxCode">(alibrary: *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Library</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_LOAD_TARGET"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_LOAD_TARGET</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_LOAD_TARGET</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Int32</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_LcdFilter"><span class="titletype">enum</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_LcdFilter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L286" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>FT_LCD_FILTER_NONE</td>
<td></td>
</tr>
<tr>
<td>FT_LCD_FILTER_DEFAULT</td>
<td></td>
</tr>
<tr>
<td>FT_LCD_FILTER_LIGHT</td>
<td></td>
</tr>
<tr>
<td>FT_LCD_FILTER_LEGACY1</td>
<td></td>
</tr>
<tr>
<td>FT_LCD_FILTER_LEGACY</td>
<td></td>
</tr>
<tr>
<td>FT_LCD_FILTER_MAX</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_LibraryRec"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_LibraryRec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>memory</td>
<td>FreeType.FT_Memory</td>
<td></td>
</tr>
<tr>
<td>version_major</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>version_minor</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>version_patch</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>num_modules</td>
<td>FreeType.FT_UInt</td>
<td></td>
</tr>
<tr>
<td>modules</td>
<td>[32] FreeType.FT_Module</td>
<td></td>
</tr>
<tr>
<td>renderers</td>
<td><a href="#FreeType_FT_ListRec">FreeType.FT_ListRec</a></td>
<td></td>
</tr>
<tr>
<td>cur_renderer</td>
<td>FreeType.FT_Renderer</td>
<td></td>
</tr>
<tr>
<td>auto_hinter</td>
<td>FreeType.FT_Module</td>
<td></td>
</tr>
<tr>
<td>debug_hooks</td>
<td>[4] FreeType.FT_DebugHook_Func</td>
<td></td>
</tr>
<tr>
<td>lcd_geometry</td>
<td>[3] FreeType.FT_Vector</td>
<td></td>
</tr>
<tr>
<td>refcount</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Library_SetLcdFilter"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Library_SetLcdFilter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L489" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Library_SetLcdFilter</span><span class="SyntaxCode">(library: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Library</span><span class="SyntaxCode">, filter: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_LcdFilter">FT_LcdFilter</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Library_SetLcdFilterWeights"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Library_SetLcdFilterWeights</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L495" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Library_SetLcdFilterWeights</span><span class="SyntaxCode">(library: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Library</span><span class="SyntaxCode">, weights: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_ListRec"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_ListRec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>head</td>
<td>FreeType.FT_ListNode</td>
<td></td>
</tr>
<tr>
<td>tail</td>
<td>FreeType.FT_ListNode</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Load_Char"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Load_Char</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L465" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Load_Char</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Face</span><span class="SyntaxCode">, char_code: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_ULong</span><span class="SyntaxCode">, load_flags: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Int32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Load_Glyph"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Load_Glyph</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L459" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Load_Glyph</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Face</span><span class="SyntaxCode">, glyph_index: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_UInt</span><span class="SyntaxCode">, load_flags: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Int32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Matrix"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Matrix</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L170" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>xx</td>
<td>FreeType.FT_Fixed</td>
<td></td>
</tr>
<tr>
<td>xy</td>
<td>FreeType.FT_Fixed</td>
<td></td>
</tr>
<tr>
<td>yx</td>
<td>FreeType.FT_Fixed</td>
<td></td>
</tr>
<tr>
<td>yy</td>
<td>FreeType.FT_Fixed</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_New_Memory_Face"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_New_Memory_Face</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L447" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_New_Memory_Face</span><span class="SyntaxCode">(library: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Library</span><span class="SyntaxCode">, file_base: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Byte</span><span class="SyntaxCode">, file_size: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Long</span><span class="SyntaxCode">, face_index: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Long</span><span class="SyntaxCode">, aface: *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Face</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Outline"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Outline</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L225" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>n_contours</td>
<td>s16</td>
<td></td>
</tr>
<tr>
<td>n_points</td>
<td>s16</td>
<td></td>
</tr>
<tr>
<td>points</td>
<td>^FreeType.FT_Vector</td>
<td></td>
</tr>
<tr>
<td>tags</td>
<td>^u8</td>
<td></td>
</tr>
<tr>
<td>contours</td>
<td>^s16</td>
<td></td>
</tr>
<tr>
<td>flags</td>
<td>s32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Outline_Decompose"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Outline_Decompose</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L477" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Outline_Decompose</span><span class="SyntaxCode">(outline: *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span><span class="SyntaxCode">, func_interface: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline_Funcs">FT_Outline_Funcs</a></span><span class="SyntaxCode">, user: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Outline_Embolden"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Outline_Embolden</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L483" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Outline_Embolden</span><span class="SyntaxCode">(outline: *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span><span class="SyntaxCode">, strength: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Pos</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Outline_Funcs"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Outline_Funcs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L276" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>move_to</td>
<td>*void</td>
<td>FT_Outline_MoveToFunc. </td>
</tr>
<tr>
<td>line_to</td>
<td>*void</td>
<td>FT_Outline_LineToFunc. </td>
</tr>
<tr>
<td>conic_to</td>
<td>*void</td>
<td>FT_Outline_ConicToFunc. </td>
</tr>
<tr>
<td>cubic_to</td>
<td>*void</td>
<td>FT_Outline_CubicToFunc. </td>
</tr>
<tr>
<td>shift</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>delta</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Outline_Transform"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Outline_Transform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L410" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Outline_Transform</span><span class="SyntaxCode">(outline: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span><span class="SyntaxCode">, matrix: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Matrix">FT_Matrix</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Outline_Translate"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Outline_Translate</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L411" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Outline_Translate</span><span class="SyntaxCode">(outline: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span><span class="SyntaxCode">, xOffset: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Pos</span><span class="SyntaxCode">, yOffset: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Pos</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Pixel_Mode"><span class="titletype">enum</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Pixel_Mode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L296" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>FT_PIXEL_MODE_NONE</td>
<td></td>
</tr>
<tr>
<td>FT_PIXEL_MODE_MONO</td>
<td></td>
</tr>
<tr>
<td>FT_PIXEL_MODE_GRAY</td>
<td></td>
</tr>
<tr>
<td>FT_PIXEL_MODE_GRAY2</td>
<td></td>
</tr>
<tr>
<td>FT_PIXEL_MODE_GRAY4</td>
<td></td>
</tr>
<tr>
<td>FT_PIXEL_MODE_LCD</td>
<td></td>
</tr>
<tr>
<td>FT_PIXEL_MODE_LCD_V</td>
<td></td>
</tr>
<tr>
<td>FT_PIXEL_MODE_BGRA</td>
<td></td>
</tr>
<tr>
<td>FT_PIXEL_MODE_MAX</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Render_Glyph"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Render_Glyph</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L471" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Render_Glyph</span><span class="SyntaxCode">(slot: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_GlyphSlot</span><span class="SyntaxCode">, render_mode: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Render_Mode">FT_Render_Mode</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Render_Mode"><span class="titletype">enum</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Render_Mode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L68" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>FT_RENDER_MODE_NORMAL</td>
<td></td>
</tr>
<tr>
<td>FT_RENDER_MODE_LIGHT</td>
<td></td>
</tr>
<tr>
<td>FT_RENDER_MODE_MONO</td>
<td></td>
</tr>
<tr>
<td>FT_RENDER_MODE_LCD</td>
<td></td>
</tr>
<tr>
<td>FT_RENDER_MODE_LCD_V</td>
<td></td>
</tr>
<tr>
<td>FT_RENDER_MODE_MAX</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Set_Pixel_Sizes"><span class="titletype">func</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Set_Pixel_Sizes</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L453" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Set_Pixel_Sizes</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Face</span><span class="SyntaxCode">, pixel_width: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_UInt</span><span class="SyntaxCode">, pixel_height: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_UInt</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_SizeRec"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_SizeRec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L256" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>face</td>
<td>FreeType.FT_Face</td>
<td></td>
</tr>
<tr>
<td>generic</td>
<td><a href="#FreeType_FT_Generic">FreeType.FT_Generic</a></td>
<td></td>
</tr>
<tr>
<td>metrics</td>
<td><a href="#FreeType_FT_Size_Metrics">FreeType.FT_Size_Metrics</a></td>
<td></td>
</tr>
<tr>
<td>sinternal</td>
<td>FreeType.FT_Size_Internal</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Size_Metrics"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Size_Metrics</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L264" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>x_ppem</td>
<td>FreeType.FT_UShort</td>
<td></td>
</tr>
<tr>
<td>y_ppem</td>
<td>FreeType.FT_UShort</td>
<td></td>
</tr>
<tr>
<td>x_scale</td>
<td>FreeType.FT_Fixed</td>
<td></td>
</tr>
<tr>
<td>y_scale</td>
<td>FreeType.FT_Fixed</td>
<td></td>
</tr>
<tr>
<td>ascender</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>descender</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>height</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>max_advance</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_SubGlyphRec"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_SubGlyphRec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L247" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>index</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>flags</td>
<td>FreeType.FT_UShort</td>
<td></td>
</tr>
<tr>
<td>arg1</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>arg2</td>
<td>FreeType.FT_Int</td>
<td></td>
</tr>
<tr>
<td>transform</td>
<td><a href="#FreeType_FT_Matrix">FreeType.FT_Matrix</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Vector"><span class="titletype">struct</span> <span class="titlelight">FreeType.</span><span class="titlestrong">FT_Vector</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L164" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>x</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
<tr>
<td>y</td>
<td>FreeType.FT_Pos</td>
<td></td>
</tr>
</table>
</div>
</div>
</div>
</body>
</html>
