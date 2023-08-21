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

    .left {
        display:    block;
        overflow-y: scroll;
        width:      650px;
        height:     100vh;
    }
    .leftpage {
        margin:    10px;
    }
    .right {
        overflow-y: scroll;
        height:     100vh;
    }
    @media only screen and (max-width: 600px) {
        td {
            display: block;
            width:   100%;
        }
    }
    @media screen and (max-width: 600px) {
        .left {
            display: none;
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
<b>Work in progress</b>. Generated documentation (swag doc 0.22.0)</blockquote>
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
<td id="FreeType_FT_LOAD_ADVANCE_ONLY" class="tdname">
FT_LOAD_ADVANCE_ONLY</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_BITMAP_METRICS_ONLY" class="tdname">
FT_LOAD_BITMAP_METRICS_ONLY</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_COLOR" class="tdname">
FT_LOAD_COLOR</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_COMPUTE_METRICS" class="tdname">
FT_LOAD_COMPUTE_METRICS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_CROP_BITMAP" class="tdname">
FT_LOAD_CROP_BITMAP</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_DEFAULT" class="tdname">
FT_LOAD_DEFAULT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_FORCE_AUTOHINT" class="tdname">
FT_LOAD_FORCE_AUTOHINT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH" class="tdname">
FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_IGNORE_TRANSFORM" class="tdname">
FT_LOAD_IGNORE_TRANSFORM</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_LINEAR_DESIGN" class="tdname">
FT_LOAD_LINEAR_DESIGN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_MONOCHROME" class="tdname">
FT_LOAD_MONOCHROME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_AUTOHINT" class="tdname">
FT_LOAD_NO_AUTOHINT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_BITMAP" class="tdname">
FT_LOAD_NO_BITMAP</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_HINTING" class="tdname">
FT_LOAD_NO_HINTING</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_RECURSE" class="tdname">
FT_LOAD_NO_RECURSE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_SCALE" class="tdname">
FT_LOAD_NO_SCALE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_PEDANTIC" class="tdname">
FT_LOAD_PEDANTIC</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_RENDER" class="tdname">
FT_LOAD_RENDER</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_SBITS_ONLY" class="tdname">
FT_LOAD_SBITS_ONLY</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LCD" class="tdname">
FT_LOAD_TARGET_LCD</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LCD_V" class="tdname">
FT_LOAD_TARGET_LCD_V</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LIGHT" class="tdname">
FT_LOAD_TARGET_LIGHT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_MONO" class="tdname">
FT_LOAD_TARGET_MONO</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_NORMAL" class="tdname">
FT_LOAD_TARGET_NORMAL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_VERTICAL_LAYOUT" class="tdname">
FT_LOAD_VERTICAL_LAYOUT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_MAX_MODULES" class="tdname">
FT_MAX_MODULES</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_STYLE_FLAG_BOLD" class="tdname">
FT_STYLE_FLAG_BOLD</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="FreeType_FT_STYLE_FLAG_ITALIC" class="tdname">
FT_STYLE_FLAG_ITALIC</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
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
<td class="tdname">
xMin</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
yMin</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
xMax</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
yMax</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
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
<td class="tdname">
rows</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
width</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
pitch</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
buffer</td>
<td class="tdtype">
^u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
num_grays</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
pixel_mode</td>
<td class="tdtype">
u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
palette_mode</td>
<td class="tdtype">
u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
palette</td>
<td class="tdtype">
^void</td>
<td class="enumeration">
</td>
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
<td class="tdname">
height</td>
<td class="tdtype">
FreeType.FT_Short</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
width</td>
<td class="tdtype">
FreeType.FT_Short</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
size</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
x_ppem</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y_ppem</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
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
<td class="tdname">
face</td>
<td class="tdtype">
FreeType.FT_Face</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
encoding</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>FT_Encoding. </p>
</td>
</tr>
<tr>
<td class="tdname">
platform_id</td>
<td class="tdtype">
FreeType.FT_UShort</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
encoding_id</td>
<td class="tdtype">
FreeType.FT_UShort</td>
<td class="enumeration">
</td>
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
<td class="tdname">
num_faces</td>
<td class="tdtype">
FreeType.FT_Long</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
face_index</td>
<td class="tdtype">
FreeType.FT_Long</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
face_flags</td>
<td class="tdtype">
FreeType.FT_Long</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
style_flags</td>
<td class="tdtype">
FreeType.FT_Long</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
num_glyphs</td>
<td class="tdtype">
FreeType.FT_Long</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
family_name</td>
<td class="tdtype">
^FreeType.FT_String</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
style_name</td>
<td class="tdtype">
^FreeType.FT_String</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
num_fixed_sizes</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
available_sizes</td>
<td class="tdtype">
^FreeType.FT_Bitmap_Size</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
num_charmaps</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
charmaps</td>
<td class="tdtype">
^FreeType.FT_CharMap</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
generic</td>
<td class="tdtype">
<a href="#FreeType_FT_Generic">FreeType.FT_Generic</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bbox</td>
<td class="tdtype">
<a href="#FreeType_FT_BBox">FreeType.FT_BBox</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
units_per_EM</td>
<td class="tdtype">
FreeType.FT_UShort</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ascender</td>
<td class="tdtype">
FreeType.FT_Short</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
descender</td>
<td class="tdtype">
FreeType.FT_Short</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
height</td>
<td class="tdtype">
FreeType.FT_Short</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
max_advance_width</td>
<td class="tdtype">
FreeType.FT_Short</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
max_advance_height</td>
<td class="tdtype">
FreeType.FT_Short</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
underline_position</td>
<td class="tdtype">
FreeType.FT_Short</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
underline_thickness</td>
<td class="tdtype">
FreeType.FT_Short</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
glyph</td>
<td class="tdtype">
FreeType.FT_GlyphSlot</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
size</td>
<td class="tdtype">
FreeType.FT_Size</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
charmap</td>
<td class="tdtype">
FreeType.FT_CharMap</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
driver</td>
<td class="tdtype">
FreeType.FT_Driver</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
memory</td>
<td class="tdtype">
FreeType.FT_Memory</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
stream</td>
<td class="tdtype">
FreeType.FT_Stream</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sizes_list</td>
<td class="tdtype">
<a href="#FreeType_FT_ListRec">FreeType.FT_ListRec</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
autohint</td>
<td class="tdtype">
<a href="#FreeType_FT_Generic">FreeType.FT_Generic</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
extensions</td>
<td class="tdtype">
^void</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
finternal</td>
<td class="tdtype">
FreeType.FT_Face_Internal</td>
<td class="enumeration">
</td>
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
<td class="tdname">
data</td>
<td class="tdtype">
*void</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
finalizer</td>
<td class="tdtype">
*void</td>
<td class="enumeration">
<p>FT_Generic_Finalizer. </p>
</td>
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
<td class="tdname">
library</td>
<td class="tdtype">
FreeType.FT_Library</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
face</td>
<td class="tdtype">
FreeType.FT_Face</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
next</td>
<td class="tdtype">
FreeType.FT_GlyphSlot</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
glyph_index</td>
<td class="tdtype">
FreeType.FT_UInt</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
generic</td>
<td class="tdtype">
<a href="#FreeType_FT_Generic">FreeType.FT_Generic</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
metrics</td>
<td class="tdtype">
<a href="#FreeType_FT_Glyph_Metrics">FreeType.FT_Glyph_Metrics</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
linearHoriAdvance</td>
<td class="tdtype">
FreeType.FT_Fixed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
linearVertAdvance</td>
<td class="tdtype">
FreeType.FT_Fixed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
advance</td>
<td class="tdtype">
<a href="#FreeType_FT_Vector">FreeType.FT_Vector</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
format</td>
<td class="tdtype">
<a href="#FreeType_FT_Glyph_Format">FreeType.FT_Glyph_Format</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bitmap</td>
<td class="tdtype">
<a href="#FreeType_FT_Bitmap">FreeType.FT_Bitmap</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bitmap_left</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bitmap_top</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
outline</td>
<td class="tdtype">
<a href="#FreeType_FT_Outline">FreeType.FT_Outline</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
num_subglyphs</td>
<td class="tdtype">
FreeType.FT_UInt</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
subglyphs</td>
<td class="tdtype">
FreeType.FT_SubGlyph</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
control_data</td>
<td class="tdtype">
*void</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
control_len</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lsb_delta</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
rsb_delta</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
other</td>
<td class="tdtype">
*void</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
finternal</td>
<td class="tdtype">
FreeType.FT_Slot_Internal</td>
<td class="enumeration">
</td>
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
<td class="tdname">
FT_GLYPH_FORMAT_NONE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_GLYPH_FORMAT_COMPOSITE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_GLYPH_FORMAT_BITMAP</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_GLYPH_FORMAT_OUTLINE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_GLYPH_FORMAT_POLTTER</td>
<td class="enumeration">
</td>
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
<td class="tdname">
width</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
height</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
horiBearingX</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
horiBearingY</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
horiAdvance</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
vertBearingX</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
vertBearingY</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
vertAdvance</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
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
<td class="tdname">
FT_LCD_FILTER_NONE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_LCD_FILTER_DEFAULT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_LCD_FILTER_LIGHT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_LCD_FILTER_LEGACY1</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_LCD_FILTER_LEGACY</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_LCD_FILTER_MAX</td>
<td class="enumeration">
</td>
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
<td class="tdname">
memory</td>
<td class="tdtype">
FreeType.FT_Memory</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
version_major</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
version_minor</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
version_patch</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
num_modules</td>
<td class="tdtype">
FreeType.FT_UInt</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
modules</td>
<td class="tdtype">
[32] FreeType.FT_Module</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
renderers</td>
<td class="tdtype">
<a href="#FreeType_FT_ListRec">FreeType.FT_ListRec</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cur_renderer</td>
<td class="tdtype">
FreeType.FT_Renderer</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
auto_hinter</td>
<td class="tdtype">
FreeType.FT_Module</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
debug_hooks</td>
<td class="tdtype">
[4] FreeType.FT_DebugHook_Func</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lcd_geometry</td>
<td class="tdtype">
[3] FreeType.FT_Vector</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
refcount</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
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
<td class="tdname">
head</td>
<td class="tdtype">
FreeType.FT_ListNode</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tail</td>
<td class="tdtype">
FreeType.FT_ListNode</td>
<td class="enumeration">
</td>
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
<td class="tdname">
xx</td>
<td class="tdtype">
FreeType.FT_Fixed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
xy</td>
<td class="tdtype">
FreeType.FT_Fixed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
yx</td>
<td class="tdtype">
FreeType.FT_Fixed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
yy</td>
<td class="tdtype">
FreeType.FT_Fixed</td>
<td class="enumeration">
</td>
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
<td class="tdname">
n_contours</td>
<td class="tdtype">
s16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
n_points</td>
<td class="tdtype">
s16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
points</td>
<td class="tdtype">
^FreeType.FT_Vector</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tags</td>
<td class="tdtype">
^u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
contours</td>
<td class="tdtype">
^s16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
flags</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
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
<td class="tdname">
move_to</td>
<td class="tdtype">
*void</td>
<td class="enumeration">
<p>FT_Outline_MoveToFunc. </p>
</td>
</tr>
<tr>
<td class="tdname">
line_to</td>
<td class="tdtype">
*void</td>
<td class="enumeration">
<p>FT_Outline_LineToFunc. </p>
</td>
</tr>
<tr>
<td class="tdname">
conic_to</td>
<td class="tdtype">
*void</td>
<td class="enumeration">
<p>FT_Outline_ConicToFunc. </p>
</td>
</tr>
<tr>
<td class="tdname">
cubic_to</td>
<td class="tdtype">
*void</td>
<td class="enumeration">
<p>FT_Outline_CubicToFunc. </p>
</td>
</tr>
<tr>
<td class="tdname">
shift</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
delta</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
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
<td class="tdname">
FT_PIXEL_MODE_NONE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_PIXEL_MODE_MONO</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_PIXEL_MODE_GRAY</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_PIXEL_MODE_GRAY2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_PIXEL_MODE_GRAY4</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_PIXEL_MODE_LCD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_PIXEL_MODE_LCD_V</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_PIXEL_MODE_BGRA</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_PIXEL_MODE_MAX</td>
<td class="enumeration">
</td>
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
<td class="tdname">
FT_RENDER_MODE_NORMAL</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_RENDER_MODE_LIGHT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_RENDER_MODE_MONO</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_RENDER_MODE_LCD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_RENDER_MODE_LCD_V</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FT_RENDER_MODE_MAX</td>
<td class="enumeration">
</td>
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
<td class="tdname">
face</td>
<td class="tdtype">
FreeType.FT_Face</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
generic</td>
<td class="tdtype">
<a href="#FreeType_FT_Generic">FreeType.FT_Generic</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
metrics</td>
<td class="tdtype">
<a href="#FreeType_FT_Size_Metrics">FreeType.FT_Size_Metrics</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sinternal</td>
<td class="tdtype">
FreeType.FT_Size_Internal</td>
<td class="enumeration">
</td>
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
<td class="tdname">
x_ppem</td>
<td class="tdtype">
FreeType.FT_UShort</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y_ppem</td>
<td class="tdtype">
FreeType.FT_UShort</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
x_scale</td>
<td class="tdtype">
FreeType.FT_Fixed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y_scale</td>
<td class="tdtype">
FreeType.FT_Fixed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ascender</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
descender</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
height</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
max_advance</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
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
<td class="tdname">
index</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
flags</td>
<td class="tdtype">
FreeType.FT_UShort</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
arg1</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
arg2</td>
<td class="tdtype">
FreeType.FT_Int</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
transform</td>
<td class="tdtype">
<a href="#FreeType_FT_Matrix">FreeType.FT_Matrix</a></td>
<td class="enumeration">
</td>
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
<td class="tdname">
x</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y</td>
<td class="tdtype">
FreeType.FT_Pos</td>
<td class="enumeration">
</td>
</tr>
</table>
</div>
</div>
</div>
</body>
</html>
