<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module freetype</title>
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
            margin:             20px;
            margin-left:        50px;
            margin-right:       50px;
            padding:            10px;
        }
        .container a {
            text-decoration: none;
            color:           DoggerBlue;
        }
        .precode a {
            text-decoration: revert;
            color:           inherit;
        }
        .codetype a {
            text-decoration: revert;
            color:           inherit;
        }
        .container a:hover {
            text-decoration: underline;
        }
        table.item {
            border-collapse:    separate;
            background-color:   Black;
            color:              White;
            width:              100%;
            margin-top:         70px;
            margin-right:       0px;
            font-size:          110%;
        }
        .item td:first-child {
            width:              33%;
            white-space:        nowrap;
        }
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
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
        .codetype {
            background-color:   #eeeeee;
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
        .precode {
            background-color:   #eeeeee;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
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
<?php include('common/end-head.php'); ?></head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="left">
<div class="leftpage">
<h2>Table of Contents</h2>
<h3>Structs</h3>
<h4></h4>
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
<h3>Enums</h3>
<h4></h4>
<ul>
<li><a href="#FreeType_FT_Glyph_Format">FT_Glyph_Format</a></li>
<li><a href="#FreeType_FT_LcdFilter">FT_LcdFilter</a></li>
<li><a href="#FreeType_FT_Pixel_Mode">FT_Pixel_Mode</a></li>
<li><a href="#FreeType_FT_Render_Mode">FT_Render_Mode</a></li>
</ul>
<h3>Constants</h3>
<h4></h4>
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
<h3>Type Aliases</h3>
<h4></h4>
<ul>
<li><a href="#FreeType_FT_Byte">FT_Byte</a></li>
<li><a href="#FreeType_FT_CharMap">FT_CharMap</a></li>
<li><a href="#FreeType_FT_DebugHook_Func">FT_DebugHook_Func</a></li>
<li><a href="#FreeType_FT_Driver">FT_Driver</a></li>
<li><a href="#FreeType_FT_Error">FT_Error</a></li>
<li><a href="#FreeType_FT_Face">FT_Face</a></li>
<li><a href="#FreeType_FT_Face_Internal">FT_Face_Internal</a></li>
<li><a href="#FreeType_FT_Fixed">FT_Fixed</a></li>
<li><a href="#FreeType_FT_GlyphSlot">FT_GlyphSlot</a></li>
<li><a href="#FreeType_FT_Int">FT_Int</a></li>
<li><a href="#FreeType_FT_Int32">FT_Int32</a></li>
<li><a href="#FreeType_FT_Library">FT_Library</a></li>
<li><a href="#FreeType_FT_ListNode">FT_ListNode</a></li>
<li><a href="#FreeType_FT_Long">FT_Long</a></li>
<li><a href="#FreeType_FT_Memory">FT_Memory</a></li>
<li><a href="#FreeType_FT_Module">FT_Module</a></li>
<li><a href="#FreeType_FT_Pos">FT_Pos</a></li>
<li><a href="#FreeType_FT_Renderer">FT_Renderer</a></li>
<li><a href="#FreeType_FT_Short">FT_Short</a></li>
<li><a href="#FreeType_FT_Size">FT_Size</a></li>
<li><a href="#FreeType_FT_Size_Internal">FT_Size_Internal</a></li>
<li><a href="#FreeType_FT_Slot_Internal">FT_Slot_Internal</a></li>
<li><a href="#FreeType_FT_Stream">FT_Stream</a></li>
<li><a href="#FreeType_FT_String">FT_String</a></li>
<li><a href="#FreeType_FT_SubGlyph">FT_SubGlyph</a></li>
<li><a href="#FreeType_FT_UInt">FT_UInt</a></li>
<li><a href="#FreeType_FT_ULong">FT_ULong</a></li>
<li><a href="#FreeType_FT_UShort">FT_UShort</a></li>
</ul>
<h3>Functions</h3>
<h4></h4>
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
<h1>Module freetype</h1>
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
<td id="FreeType_FT_LOAD_ADVANCE_ONLY" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_ADVANCE_ONLY</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_BITMAP_METRICS_ONLY" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_BITMAP_METRICS_ONLY</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_COLOR" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_COLOR</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_COMPUTE_METRICS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_COMPUTE_METRICS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_CROP_BITMAP" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_CROP_BITMAP</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_DEFAULT" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_DEFAULT</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_FORCE_AUTOHINT" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_FORCE_AUTOHINT</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_IGNORE_TRANSFORM" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_IGNORE_TRANSFORM</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_LINEAR_DESIGN" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_LINEAR_DESIGN</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_MONOCHROME" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_MONOCHROME</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_AUTOHINT" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_NO_AUTOHINT</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_BITMAP" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_NO_BITMAP</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_HINTING" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_NO_HINTING</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_RECURSE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_NO_RECURSE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_SCALE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_NO_SCALE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_PEDANTIC" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_PEDANTIC</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_RENDER" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_RENDER</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_SBITS_ONLY" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_SBITS_ONLY</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LCD" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_TARGET_LCD</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LCD_V" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_TARGET_LCD_V</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LIGHT" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_TARGET_LIGHT</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_MONO" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_TARGET_MONO</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_NORMAL" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_TARGET_NORMAL</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_VERTICAL_LAYOUT" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LOAD_VERTICAL_LAYOUT</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_MAX_MODULES" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_MAX_MODULES</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_STYLE_FLAG_BOLD" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_STYLE_FLAG_BOLD</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_STYLE_FLAG_ITALIC" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_STYLE_FLAG_ITALIC</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="FreeType_FT_Byte"><span class="titletype">type alias</span> <span class="titlelight">FreeType.</span><span class="titlestrong">Type Aliases</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/freetype\src\freetype.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="FreeType_FT_Byte" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Byte</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_CharMap" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_CharMap</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_CharMapRec">FT_CharMapRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_DebugHook_Func" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_DebugHook_Func</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Driver" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Driver</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Error" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Error</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Face" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Face</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_FaceRec">FT_FaceRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Face_Internal" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Face_Internal</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Fixed" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Fixed</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_GlyphSlot" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_GlyphSlot</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlotRec">FT_GlyphSlotRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Int" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Int</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Int32" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Int32</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Library" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Library</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_LibraryRec">FT_LibraryRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_ListNode" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_ListNode</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Long" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Long</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Memory" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Memory</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Module" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Module</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Pos" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Pos</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Renderer" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Renderer</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Short" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Short</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Size" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Size</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_SizeRec">FT_SizeRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Size_Internal" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Size_Internal</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Slot_Internal" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Slot_Internal</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Stream" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_Stream</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_String" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_String</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_SubGlyph" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_SubGlyph</span></span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_SubGlyphRec">FT_SubGlyphRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_UInt" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_UInt</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_ULong" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_ULong</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_UShort" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_UShort</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">xMin</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">yMin</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">xMax</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">yMax</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">rows</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">width</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pitch</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">buffer</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">num_grays</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pixel_mode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">palette_mode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">palette</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxType">void</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">width</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">size</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">x_ppem</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">y_ppem</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">face</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_FaceRec">FT_FaceRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">encoding</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>FT_Encoding. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">platform_id</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">encoding_id</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Done_Face</span>(face: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span>)</span></code>
</div>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Done_FreeType</span>(alibrary: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode">num_faces</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">face_index</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">face_flags</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">style_flags</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">num_glyphs</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">family_name</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_String">FT_String</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">style_name</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_String">FT_String</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">num_fixed_sizes</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">available_sizes</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Bitmap_Size">FT_Bitmap_Size</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">num_charmaps</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">charmaps</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_CharMap">FT_CharMap</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">generic</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Generic">FT_Generic</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bbox</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_BBox">FT_BBox</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">units_per_EM</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">ascender</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">descender</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">max_advance_width</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">max_advance_height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">underline_position</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">underline_thickness</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">glyph</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlotRec">FT_GlyphSlotRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">size</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_SizeRec">FT_SizeRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">charmap</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_CharMapRec">FT_CharMapRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">driver</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">memory</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">stream</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">sizes_list</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_ListRec">FT_ListRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">autohint</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Generic">FT_Generic</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">extensions</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">finternal</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">data</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">finalizer</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Get_Char_Index</span>(face: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span>, charcode: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_ULong">FT_ULong</a></span>)-&gt;<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_UInt">FT_UInt</a></span></span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode">library</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_LibraryRec">FT_LibraryRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">face</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_FaceRec">FT_FaceRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">next</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlotRec">FT_GlyphSlotRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">glyph_index</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">generic</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Generic">FT_Generic</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">metrics</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Glyph_Metrics">FT_Glyph_Metrics</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">linearHoriAdvance</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">linearVertAdvance</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">advance</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Vector">FT_Vector</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">format</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Glyph_Format">FT_Glyph_Format</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bitmap</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Bitmap">FT_Bitmap</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bitmap_left</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bitmap_top</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">outline</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">num_subglyphs</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">subglyphs</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_SubGlyphRec">FT_SubGlyphRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">control_data</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">control_len</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">lsb_delta</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">rsb_delta</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">other</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">finternal</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_GlyphSlot_Oblique</span>(slot: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlot">FT_GlyphSlot</a></span>)</span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_GLYPH_FORMAT_NONE</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_GLYPH_FORMAT_COMPOSITE</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_GLYPH_FORMAT_BITMAP</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_GLYPH_FORMAT_OUTLINE</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_GLYPH_FORMAT_POLTTER</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">width</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">horiBearingX</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">horiBearingY</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">horiAdvance</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">vertBearingX</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">vertBearingY</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">vertAdvance</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_IMAGE_TAG</span>(x1: <span class="SyntaxType">u32</span>, x2: <span class="SyntaxType">u32</span>, x3: <span class="SyntaxType">u32</span>, x4: <span class="SyntaxType">u32</span>)-&gt;<span class="SyntaxType">u32</span></span></code>
</div>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Init_FreeType</span>(alibrary: *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_LOAD_TARGET</span>(x: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Int32">FT_Int32</a></span>)-&gt;<span class="SyntaxType">s32</span></span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LCD_FILTER_NONE</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LCD_FILTER_DEFAULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LCD_FILTER_LIGHT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LCD_FILTER_LEGACY1</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LCD_FILTER_LEGACY</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_LCD_FILTER_MAX</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">memory</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">version_major</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">version_minor</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">version_patch</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">num_modules</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">modules</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">32</span>] <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Module">FT_Module</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">renderers</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_ListRec">FT_ListRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">cur_renderer</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">auto_hinter</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">debug_hooks</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">4</span>] <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_DebugHook_Func">FT_DebugHook_Func</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">lcd_geometry</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">3</span>] <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Vector">FT_Vector</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">refcount</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Library_SetLcdFilter</span>(library: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span>, filter: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_LcdFilter">FT_LcdFilter</a></span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Library_SetLcdFilterWeights</span>(library: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span>, weights: <span class="SyntaxKeyword">const</span> *<span class="SyntaxType">u8</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode">head</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">tail</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Load_Char</span>(face: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span>, char_code: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_ULong">FT_ULong</a></span>, load_flags: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Int32">FT_Int32</a></span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Load_Glyph</span>(face: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span>, glyph_index: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_UInt">FT_UInt</a></span>, load_flags: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Int32">FT_Int32</a></span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode">xx</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">xy</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">yx</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">yy</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_New_Memory_Face</span>(library: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span>, file_base: <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Byte">FT_Byte</a></span>, file_size: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Long">FT_Long</a></span>, face_index: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Long">FT_Long</a></span>, aface: *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode">n_contours</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">n_points</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">points</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Vector">FT_Vector</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">tags</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">contours</span></td>
<td class="codetype"><span class="SyntaxCode">^<span class="SyntaxType">s16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">flags</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Outline_Decompose</span>(outline: *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span>, func_interface: <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Outline_Funcs">FT_Outline_Funcs</a></span>, user: *<span class="SyntaxType">void</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Outline_Embolden</span>(outline: *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span>, strength: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Pos">FT_Pos</a></span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode">move_to</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td>FT_Outline_MoveToFunc. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">line_to</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td>FT_Outline_LineToFunc. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">conic_to</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td>FT_Outline_ConicToFunc. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">cubic_to</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td>FT_Outline_CubicToFunc. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">shift</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">delta</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Outline_Transform</span>(outline: <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span>, matrix: <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Matrix">FT_Matrix</a></span>)</span></code>
</div>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Outline_Translate</span>(outline: <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span>, xOffset: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Pos">FT_Pos</a></span>, yOffset: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Pos">FT_Pos</a></span>)</span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_PIXEL_MODE_NONE</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_PIXEL_MODE_MONO</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_PIXEL_MODE_GRAY</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_PIXEL_MODE_GRAY2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_PIXEL_MODE_GRAY4</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_PIXEL_MODE_LCD</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_PIXEL_MODE_LCD_V</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_PIXEL_MODE_BGRA</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_PIXEL_MODE_MAX</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Render_Glyph</span>(slot: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlot">FT_GlyphSlot</a></span>, render_mode: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Render_Mode">FT_Render_Mode</a></span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_RENDER_MODE_NORMAL</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_RENDER_MODE_LIGHT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_RENDER_MODE_MONO</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_RENDER_MODE_LCD</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_RENDER_MODE_LCD_V</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FT_RENDER_MODE_MAX</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">FT_Set_Pixel_Sizes</span>(face: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span>, pixel_width: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_UInt">FT_UInt</a></span>, pixel_height: <span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_UInt">FT_UInt</a></span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
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
<td class="codetype"><span class="SyntaxCode">face</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_FaceRec">FT_FaceRec</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">generic</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Generic">FT_Generic</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">metrics</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Size_Metrics">FT_Size_Metrics</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">sinternal</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">x_ppem</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">y_ppem</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">x_scale</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">y_scale</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">ascender</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">descender</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">max_advance</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">index</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">flags</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">arg1</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">arg2</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">transform</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant"><a href="#FreeType_FT_Matrix">FT_Matrix</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode">x</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">y</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
</table>
</div>
</div>
</div>
</body>
</html>
