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
<td id="FreeType_FT_LOAD_ADVANCE_ONLY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_ADVANCE_ONLY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_BITMAP_METRICS_ONLY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_BITMAP_METRICS_ONLY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_COLOR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_COLOR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_COMPUTE_METRICS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_COMPUTE_METRICS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_CROP_BITMAP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_CROP_BITMAP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_DEFAULT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_DEFAULT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_FORCE_AUTOHINT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_FORCE_AUTOHINT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_IGNORE_TRANSFORM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_IGNORE_TRANSFORM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_LINEAR_DESIGN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_LINEAR_DESIGN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_MONOCHROME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_MONOCHROME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_AUTOHINT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_NO_AUTOHINT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_BITMAP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_NO_BITMAP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_HINTING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_NO_HINTING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_RECURSE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_NO_RECURSE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_NO_SCALE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_NO_SCALE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_PEDANTIC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_PEDANTIC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_RENDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_RENDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_SBITS_ONLY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_SBITS_ONLY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LCD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_TARGET_LCD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LCD_V" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_TARGET_LCD_V</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_LIGHT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_TARGET_LIGHT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_MONO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_TARGET_MONO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_TARGET_NORMAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_TARGET_NORMAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_LOAD_VERTICAL_LAYOUT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LOAD_VERTICAL_LAYOUT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_MAX_MODULES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_MAX_MODULES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_STYLE_FLAG_BOLD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_STYLE_FLAG_BOLD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_STYLE_FLAG_ITALIC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_STYLE_FLAG_ITALIC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
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
<td id="FreeType_FT_Byte" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Byte</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_CharMap" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_CharMap</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_CharMapRec">FT_CharMapRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_DebugHook_Func" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_DebugHook_Func</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Driver" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Driver</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Error" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Error</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Face" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Face</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_FaceRec">FT_FaceRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Face_Internal" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Face_Internal</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Fixed" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Fixed</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_GlyphSlot" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_GlyphSlot</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlotRec">FT_GlyphSlotRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Int" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Int</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Int32" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Int32</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Library" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Library</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_LibraryRec">FT_LibraryRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_ListNode" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_ListNode</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Long" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Long</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Memory" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Memory</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Module" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Module</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Pos" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Pos</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Renderer" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Renderer</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Short" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Short</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Size" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Size</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_SizeRec">FT_SizeRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Size_Internal" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Size_Internal</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Slot_Internal" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Slot_Internal</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_Stream" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_Stream</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_String" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_String</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_SubGlyph" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_SubGlyph</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_SubGlyphRec">FT_SubGlyphRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_UInt" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_UInt</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_ULong" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_ULong</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="FreeType_FT_UShort" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_UShort</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">xMin</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">yMin</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">xMax</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">yMax</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">rows</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">width</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">pitch</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">buffer</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">num_grays</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">pixel_mode</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">palette_mode</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">palette</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">height</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">width</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">size</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">x_ppem</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">y_ppem</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">face</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_FaceRec">FT_FaceRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">encoding</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>FT_Encoding. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">platform_id</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">encoding_id</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Done_Face</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Done_FreeType</span><span class="SyntaxCode">(alibrary: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td class="codetype"></span><span class="SyntaxCode">num_faces</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">face_index</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">face_flags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">style_flags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">num_glyphs</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">family_name</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_String">FT_String</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">style_name</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_String">FT_String</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">num_fixed_sizes</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">available_sizes</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Bitmap_Size">FT_Bitmap_Size</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">num_charmaps</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">charmaps</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_CharMap">FT_CharMap</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">generic</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Generic">FT_Generic</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">bbox</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_BBox">FT_BBox</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">units_per_EM</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ascender</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">descender</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">height</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">max_advance_width</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">max_advance_height</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">underline_position</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">underline_thickness</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">glyph</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlotRec">FT_GlyphSlotRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">size</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_SizeRec">FT_SizeRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">charmap</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_CharMapRec">FT_CharMapRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">driver</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">memory</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">stream</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">sizes_list</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_ListRec">FT_ListRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">autohint</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Generic">FT_Generic</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">extensions</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">finternal</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">data</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">finalizer</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Get_Char_Index</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span><span class="SyntaxCode">, charcode: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_ULong">FT_ULong</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_UInt">FT_UInt</a></span><span class="SyntaxCode"></code>
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
<td class="codetype"></span><span class="SyntaxCode">library</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_LibraryRec">FT_LibraryRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">face</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_FaceRec">FT_FaceRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">next</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlotRec">FT_GlyphSlotRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">glyph_index</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">generic</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Generic">FT_Generic</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">metrics</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Glyph_Metrics">FT_Glyph_Metrics</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">linearHoriAdvance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">linearVertAdvance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">advance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Vector">FT_Vector</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">format</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Glyph_Format">FT_Glyph_Format</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">bitmap</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Bitmap">FT_Bitmap</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">bitmap_left</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">bitmap_top</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">outline</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">num_subglyphs</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">subglyphs</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_SubGlyphRec">FT_SubGlyphRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">control_data</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">control_len</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lsb_delta</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">rsb_delta</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">other</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">finternal</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_GlyphSlot_Oblique</span><span class="SyntaxCode">(slot: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlot">FT_GlyphSlot</a></span><span class="SyntaxCode">)</code>
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
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_GLYPH_FORMAT_NONE</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_GLYPH_FORMAT_COMPOSITE</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_GLYPH_FORMAT_BITMAP</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_GLYPH_FORMAT_OUTLINE</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_GLYPH_FORMAT_POLTTER</span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">width</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">height</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">horiBearingX</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">horiBearingY</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">horiAdvance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">vertBearingX</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">vertBearingY</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">vertAdvance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_IMAGE_TAG</span><span class="SyntaxCode">(x1: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, x2: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, x3: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, x4: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Init_FreeType</span><span class="SyntaxCode">(alibrary: *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_LOAD_TARGET</span><span class="SyntaxCode">(x: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Int32">FT_Int32</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
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
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LCD_FILTER_NONE</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LCD_FILTER_DEFAULT</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LCD_FILTER_LIGHT</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LCD_FILTER_LEGACY1</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LCD_FILTER_LEGACY</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_LCD_FILTER_MAX</span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">memory</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">version_major</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">version_minor</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">version_patch</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">num_modules</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">modules</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">32</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Module">FT_Module</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">renderers</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_ListRec">FT_ListRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cur_renderer</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">auto_hinter</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">debug_hooks</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">4</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_DebugHook_Func">FT_DebugHook_Func</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lcd_geometry</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">3</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Vector">FT_Vector</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">refcount</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Library_SetLcdFilter</span><span class="SyntaxCode">(library: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span><span class="SyntaxCode">, filter: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_LcdFilter">FT_LcdFilter</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Library_SetLcdFilterWeights</span><span class="SyntaxCode">(library: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span><span class="SyntaxCode">, weights: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td class="codetype"></span><span class="SyntaxCode">head</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">tail</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Load_Char</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span><span class="SyntaxCode">, char_code: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_ULong">FT_ULong</a></span><span class="SyntaxCode">, load_flags: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Int32">FT_Int32</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Load_Glyph</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span><span class="SyntaxCode">, glyph_index: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_UInt">FT_UInt</a></span><span class="SyntaxCode">, load_flags: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Int32">FT_Int32</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td class="codetype"></span><span class="SyntaxCode">xx</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">xy</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">yx</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">yy</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_New_Memory_Face</span><span class="SyntaxCode">(library: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Library">FT_Library</a></span><span class="SyntaxCode">, file_base: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Byte">FT_Byte</a></span><span class="SyntaxCode">, file_size: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Long">FT_Long</a></span><span class="SyntaxCode">, face_index: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Long">FT_Long</a></span><span class="SyntaxCode">, aface: *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td class="codetype"></span><span class="SyntaxCode">n_contours</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">n_points</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">points</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Vector">FT_Vector</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">tags</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">contours</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">flags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Outline_Decompose</span><span class="SyntaxCode">(outline: *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span><span class="SyntaxCode">, func_interface: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline_Funcs">FT_Outline_Funcs</a></span><span class="SyntaxCode">, user: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Outline_Embolden</span><span class="SyntaxCode">(outline: *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span><span class="SyntaxCode">, strength: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Pos">FT_Pos</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td class="codetype"></span><span class="SyntaxCode">move_to</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td>FT_Outline_MoveToFunc. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">line_to</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td>FT_Outline_LineToFunc. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">conic_to</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td>FT_Outline_ConicToFunc. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cubic_to</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td>FT_Outline_CubicToFunc. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">shift</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">delta</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Outline_Transform</span><span class="SyntaxCode">(outline: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span><span class="SyntaxCode">, matrix: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Matrix">FT_Matrix</a></span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Outline_Translate</span><span class="SyntaxCode">(outline: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Outline">FT_Outline</a></span><span class="SyntaxCode">, xOffset: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Pos">FT_Pos</a></span><span class="SyntaxCode">, yOffset: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Pos">FT_Pos</a></span><span class="SyntaxCode">)</code>
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
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_PIXEL_MODE_NONE</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_PIXEL_MODE_MONO</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_PIXEL_MODE_GRAY</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_PIXEL_MODE_GRAY2</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_PIXEL_MODE_GRAY4</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_PIXEL_MODE_LCD</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_PIXEL_MODE_LCD_V</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_PIXEL_MODE_BGRA</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_PIXEL_MODE_MAX</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Render_Glyph</span><span class="SyntaxCode">(slot: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_GlyphSlot">FT_GlyphSlot</a></span><span class="SyntaxCode">, render_mode: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Render_Mode">FT_Render_Mode</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_RENDER_MODE_NORMAL</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_RENDER_MODE_LIGHT</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_RENDER_MODE_MONO</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_RENDER_MODE_LCD</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_RENDER_MODE_LCD_V</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FT_RENDER_MODE_MAX</span><span class="SyntaxCode"></td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FT_Set_Pixel_Sizes</span><span class="SyntaxCode">(face: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Face">FT_Face</a></span><span class="SyntaxCode">, pixel_width: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_UInt">FT_UInt</a></span><span class="SyntaxCode">, pixel_height: </span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_UInt">FT_UInt</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td class="codetype"></span><span class="SyntaxCode">face</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_FaceRec">FT_FaceRec</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">generic</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Generic">FT_Generic</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">metrics</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Size_Metrics">FT_Size_Metrics</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">sinternal</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">x_ppem</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">y_ppem</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">x_scale</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">y_scale</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ascender</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">descender</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">height</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">max_advance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">index</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">flags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">arg1</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">arg2</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">transform</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#FreeType_FT_Matrix">FT_Matrix</a></span><span class="SyntaxCode"></td>
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
<td class="codetype"></span><span class="SyntaxCode">x</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">y</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
</div>
</div>
</div>
</body>
</html>
