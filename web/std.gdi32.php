<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module gdi32</title>
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
        .container a:hover {
            text-decoration: underline;
        }
        .container a.src {
            font-size:          90%;
            color:              LightGrey;
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
<li><a href="#Gdi32_BITMAP">BITMAP</a></li>
<li><a href="#Gdi32_BITMAPINFO">BITMAPINFO</a></li>
<li><a href="#Gdi32_BITMAPINFOHEADER">BITMAPINFOHEADER</a></li>
<li><a href="#Gdi32_ENUMLOGFONTEXA">ENUMLOGFONTEXA</a></li>
<li><a href="#Gdi32_ENUMLOGFONTEXW">ENUMLOGFONTEXW</a></li>
<li><a href="#Gdi32_LOGFONTA">LOGFONTA</a></li>
<li><a href="#Gdi32_LOGFONTW">LOGFONTW</a></li>
<li><a href="#Gdi32_NEWTEXTMETRICA">NEWTEXTMETRICA</a></li>
<li><a href="#Gdi32_NEWTEXTMETRICW">NEWTEXTMETRICW</a></li>
<li><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></li>
<li><a href="#Gdi32_RGBQUAD">RGBQUAD</a></li>
<li><a href="#Gdi32_TEXTMETRICA">TEXTMETRICA</a></li>
<li><a href="#Gdi32_TEXTMETRICW">TEXTMETRICW</a></li>
</ul>
<h3>Constants</h3>
<h4></h4>
<ul>
<li><a href="#Gdi32_ANSI_CHARSET">ANSI_CHARSET</a></li>
<li><a href="#Gdi32_ANSI_FIXED_FONT">ANSI_FIXED_FONT</a></li>
<li><a href="#Gdi32_ANSI_VAR_FONT">ANSI_VAR_FONT</a></li>
<li><a href="#Gdi32_ARABIC_CHARSET">ARABIC_CHARSET</a></li>
<li><a href="#Gdi32_BALTIC_CHARSET">BALTIC_CHARSET</a></li>
<li><a href="#Gdi32_BI_BITFIELDS">BI_BITFIELDS</a></li>
<li><a href="#Gdi32_BI_JPEG">BI_JPEG</a></li>
<li><a href="#Gdi32_BI_PNG">BI_PNG</a></li>
<li><a href="#Gdi32_BI_RGB">BI_RGB</a></li>
<li><a href="#Gdi32_BI_RLE4">BI_RLE4</a></li>
<li><a href="#Gdi32_BI_RLE8">BI_RLE8</a></li>
<li><a href="#Gdi32_BKMODE_LAST">BKMODE_LAST</a></li>
<li><a href="#Gdi32_BLACKNESS">BLACKNESS</a></li>
<li><a href="#Gdi32_BLACK_BRUSH">BLACK_BRUSH</a></li>
<li><a href="#Gdi32_BLACK_PEN">BLACK_PEN</a></li>
<li><a href="#Gdi32_CAPTUREBLT">CAPTUREBLT</a></li>
<li><a href="#Gdi32_CHINESEBIG5_CHARSET">CHINESEBIG5_CHARSET</a></li>
<li><a href="#Gdi32_DEFAULT_CHARSET">DEFAULT_CHARSET</a></li>
<li><a href="#Gdi32_DEFAULT_PALETTE">DEFAULT_PALETTE</a></li>
<li><a href="#Gdi32_DEVICE_DEFAULT_FONT">DEVICE_DEFAULT_FONT</a></li>
<li><a href="#Gdi32_DEVICE_FONTTYPE">DEVICE_FONTTYPE</a></li>
<li><a href="#Gdi32_DIB_PAL_COLORS">DIB_PAL_COLORS</a></li>
<li><a href="#Gdi32_DIB_RGB_COLORS">DIB_RGB_COLORS</a></li>
<li><a href="#Gdi32_DKGRAY_BRUSH">DKGRAY_BRUSH</a></li>
<li><a href="#Gdi32_DSTINVERT">DSTINVERT</a></li>
<li><a href="#Gdi32_EASTEUROPE_CHARSET">EASTEUROPE_CHARSET</a></li>
<li><a href="#Gdi32_FW_BLACK">FW_BLACK</a></li>
<li><a href="#Gdi32_FW_BOLD">FW_BOLD</a></li>
<li><a href="#Gdi32_FW_DEMIBOLD">FW_DEMIBOLD</a></li>
<li><a href="#Gdi32_FW_DONTCARE">FW_DONTCARE</a></li>
<li><a href="#Gdi32_FW_EXTRABOLD">FW_EXTRABOLD</a></li>
<li><a href="#Gdi32_FW_EXTRALIGHT">FW_EXTRALIGHT</a></li>
<li><a href="#Gdi32_FW_HEAVY">FW_HEAVY</a></li>
<li><a href="#Gdi32_FW_LIGHT">FW_LIGHT</a></li>
<li><a href="#Gdi32_FW_MEDIUM">FW_MEDIUM</a></li>
<li><a href="#Gdi32_FW_NORMAL">FW_NORMAL</a></li>
<li><a href="#Gdi32_FW_REGULAR">FW_REGULAR</a></li>
<li><a href="#Gdi32_FW_SEMIBOLD">FW_SEMIBOLD</a></li>
<li><a href="#Gdi32_FW_THIN">FW_THIN</a></li>
<li><a href="#Gdi32_FW_ULTRABOLD">FW_ULTRABOLD</a></li>
<li><a href="#Gdi32_FW_ULTRALIGHT">FW_ULTRALIGHT</a></li>
<li><a href="#Gdi32_GB2312_CHARSET">GB2312_CHARSET</a></li>
<li><a href="#Gdi32_GDI_ERROR">GDI_ERROR</a></li>
<li><a href="#Gdi32_GRAY_BRUSH">GRAY_BRUSH</a></li>
<li><a href="#Gdi32_GREEK_CHARSET">GREEK_CHARSET</a></li>
<li><a href="#Gdi32_HANGEUL_CHARSET">HANGEUL_CHARSET</a></li>
<li><a href="#Gdi32_HANGUL_CHARSET">HANGUL_CHARSET</a></li>
<li><a href="#Gdi32_HEBREW_CHARSET">HEBREW_CHARSET</a></li>
<li><a href="#Gdi32_HOLLOW_BRUSH">HOLLOW_BRUSH</a></li>
<li><a href="#Gdi32_JOHAB_CHARSET">JOHAB_CHARSET</a></li>
<li><a href="#Gdi32_LF_FACESIZE">LF_FACESIZE</a></li>
<li><a href="#Gdi32_LF_FULLFACESIZE">LF_FULLFACESIZE</a></li>
<li><a href="#Gdi32_LTGRAY_BRUSH">LTGRAY_BRUSH</a></li>
<li><a href="#Gdi32_MAC_CHARSET">MAC_CHARSET</a></li>
<li><a href="#Gdi32_MERGECOPY">MERGECOPY</a></li>
<li><a href="#Gdi32_MERGEPAINT">MERGEPAINT</a></li>
<li><a href="#Gdi32_NOMIRRORBITMAP">NOMIRRORBITMAP</a></li>
<li><a href="#Gdi32_NOTSRCCOPY">NOTSRCCOPY</a></li>
<li><a href="#Gdi32_NOTSRCERASE">NOTSRCERASE</a></li>
<li><a href="#Gdi32_NULL_BRUSH">NULL_BRUSH</a></li>
<li><a href="#Gdi32_NULL_PEN">NULL_PEN</a></li>
<li><a href="#Gdi32_OEM_CHARSET">OEM_CHARSET</a></li>
<li><a href="#Gdi32_OEM_FIXED_FONT">OEM_FIXED_FONT</a></li>
<li><a href="#Gdi32_OPAQUE">OPAQUE</a></li>
<li><a href="#Gdi32_PATCOPY">PATCOPY</a></li>
<li><a href="#Gdi32_PATINVERT">PATINVERT</a></li>
<li><a href="#Gdi32_PATPAINT">PATPAINT</a></li>
<li><a href="#Gdi32_PFD_DEPTH_DONTCARE">PFD_DEPTH_DONTCARE</a></li>
<li><a href="#Gdi32_PFD_DIRECT3D_ACCELERATED">PFD_DIRECT3D_ACCELERATED</a></li>
<li><a href="#Gdi32_PFD_DOUBLEBUFFER">PFD_DOUBLEBUFFER</a></li>
<li><a href="#Gdi32_PFD_DOUBLEBUFFER_DONTCARE">PFD_DOUBLEBUFFER_DONTCARE</a></li>
<li><a href="#Gdi32_PFD_DRAW_TO_BITMAP">PFD_DRAW_TO_BITMAP</a></li>
<li><a href="#Gdi32_PFD_DRAW_TO_WINDOW">PFD_DRAW_TO_WINDOW</a></li>
<li><a href="#Gdi32_PFD_GENERIC_ACCELERATED">PFD_GENERIC_ACCELERATED</a></li>
<li><a href="#Gdi32_PFD_GENERIC_FORMAT">PFD_GENERIC_FORMAT</a></li>
<li><a href="#Gdi32_PFD_MAIN_PLANE">PFD_MAIN_PLANE</a></li>
<li><a href="#Gdi32_PFD_NEED_PALETTE">PFD_NEED_PALETTE</a></li>
<li><a href="#Gdi32_PFD_NEED_SYSTEM_PALETTE">PFD_NEED_SYSTEM_PALETTE</a></li>
<li><a href="#Gdi32_PFD_OVERLAY_PLANE">PFD_OVERLAY_PLANE</a></li>
<li><a href="#Gdi32_PFD_STEREO">PFD_STEREO</a></li>
<li><a href="#Gdi32_PFD_STEREO_DONTCARE">PFD_STEREO_DONTCARE</a></li>
<li><a href="#Gdi32_PFD_SUPPORT_COMPOSITION">PFD_SUPPORT_COMPOSITION</a></li>
<li><a href="#Gdi32_PFD_SUPPORT_DIRECTDRAW">PFD_SUPPORT_DIRECTDRAW</a></li>
<li><a href="#Gdi32_PFD_SUPPORT_GDI">PFD_SUPPORT_GDI</a></li>
<li><a href="#Gdi32_PFD_SUPPORT_OPENGL">PFD_SUPPORT_OPENGL</a></li>
<li><a href="#Gdi32_PFD_SWAP_COPY">PFD_SWAP_COPY</a></li>
<li><a href="#Gdi32_PFD_SWAP_EXCHANGE">PFD_SWAP_EXCHANGE</a></li>
<li><a href="#Gdi32_PFD_SWAP_LAYER_BUFFERS">PFD_SWAP_LAYER_BUFFERS</a></li>
<li><a href="#Gdi32_PFD_TYPE_COLORINDEX">PFD_TYPE_COLORINDEX</a></li>
<li><a href="#Gdi32_PFD_TYPE_RGBA">PFD_TYPE_RGBA</a></li>
<li><a href="#Gdi32_PFD_UNDERLAY_PLANE">PFD_UNDERLAY_PLANE</a></li>
<li><a href="#Gdi32_PS_ALTERNATE">PS_ALTERNATE</a></li>
<li><a href="#Gdi32_PS_DASH">PS_DASH</a></li>
<li><a href="#Gdi32_PS_DASHDOT">PS_DASHDOT</a></li>
<li><a href="#Gdi32_PS_DASHDOTDOT">PS_DASHDOTDOT</a></li>
<li><a href="#Gdi32_PS_DOT">PS_DOT</a></li>
<li><a href="#Gdi32_PS_INSIDEFRAME">PS_INSIDEFRAME</a></li>
<li><a href="#Gdi32_PS_NULL">PS_NULL</a></li>
<li><a href="#Gdi32_PS_SOLID">PS_SOLID</a></li>
<li><a href="#Gdi32_PS_STYLE_MASK">PS_STYLE_MASK</a></li>
<li><a href="#Gdi32_PS_USERSTYLE">PS_USERSTYLE</a></li>
<li><a href="#Gdi32_RASTER_FONTTYPE">RASTER_FONTTYPE</a></li>
<li><a href="#Gdi32_RUSSIAN_CHARSET">RUSSIAN_CHARSET</a></li>
<li><a href="#Gdi32_SHIFTJIS_CHARSET">SHIFTJIS_CHARSET</a></li>
<li><a href="#Gdi32_SRCAND">SRCAND</a></li>
<li><a href="#Gdi32_SRCCOPY">SRCCOPY</a></li>
<li><a href="#Gdi32_SRCERASE">SRCERASE</a></li>
<li><a href="#Gdi32_SRCINVERT">SRCINVERT</a></li>
<li><a href="#Gdi32_SRCPAINT">SRCPAINT</a></li>
<li><a href="#Gdi32_SYMBOL_CHARSET">SYMBOL_CHARSET</a></li>
<li><a href="#Gdi32_SYSTEM_FIXED_FONT">SYSTEM_FIXED_FONT</a></li>
<li><a href="#Gdi32_SYSTEM_FONT">SYSTEM_FONT</a></li>
<li><a href="#Gdi32_THAI_CHARSET">THAI_CHARSET</a></li>
<li><a href="#Gdi32_TRANSPARENT">TRANSPARENT</a></li>
<li><a href="#Gdi32_TRUETYPE_FONTTYPE">TRUETYPE_FONTTYPE</a></li>
<li><a href="#Gdi32_TURKISH_CHARSET">TURKISH_CHARSET</a></li>
<li><a href="#Gdi32_VIETNAMESE_CHARSET">VIETNAMESE_CHARSET</a></li>
<li><a href="#Gdi32_WHITENESS">WHITENESS</a></li>
<li><a href="#Gdi32_WHITE_BRUSH">WHITE_BRUSH</a></li>
<li><a href="#Gdi32_WHITE_PEN">WHITE_PEN</a></li>
</ul>
<h3>Type Aliases</h3>
<h4></h4>
<ul>
<li><a href="#Gdi32_FONTENUMPROCA">FONTENUMPROCA</a></li>
<li><a href="#Gdi32_FONTENUMPROCW">FONTENUMPROCW</a></li>
<li><a href="#Gdi32_HFONT">HFONT</a></li>
<li><a href="#Gdi32_HGDIOBJ">HGDIOBJ</a></li>
<li><a href="#Gdi32_HPEN">HPEN</a></li>
</ul>
<h3>Functions</h3>
<h4></h4>
<ul>
<li><a href="#Gdi32_BitBlt">Gdi32.BitBlt</a></li>
<li><a href="#Gdi32_ChoosePixelFormat">Gdi32.ChoosePixelFormat</a></li>
<li><a href="#Gdi32_CreateCompatibleBitmap">Gdi32.CreateCompatibleBitmap</a></li>
<li><a href="#Gdi32_CreateCompatibleDC">Gdi32.CreateCompatibleDC</a></li>
<li><a href="#Gdi32_CreateDIBSection">Gdi32.CreateDIBSection</a></li>
<li><a href="#Gdi32_CreateFontIndirectA">Gdi32.CreateFontIndirectA</a></li>
<li><a href="#Gdi32_CreateFontIndirectW">Gdi32.CreateFontIndirectW</a></li>
<li><a href="#Gdi32_CreatePen">Gdi32.CreatePen</a></li>
<li><a href="#Gdi32_CreateSolidBrush">Gdi32.CreateSolidBrush</a></li>
<li><a href="#Gdi32_DeleteDC">Gdi32.DeleteDC</a></li>
<li><a href="#Gdi32_DeleteObject">Gdi32.DeleteObject</a></li>
<li><a href="#Gdi32_DescribePixelFormat">Gdi32.DescribePixelFormat</a></li>
<li><a href="#Gdi32_EnumFontFamiliesA">Gdi32.EnumFontFamiliesA</a></li>
<li><a href="#Gdi32_EnumFontFamiliesExA">Gdi32.EnumFontFamiliesExA</a></li>
<li><a href="#Gdi32_EnumFontFamiliesExW">Gdi32.EnumFontFamiliesExW</a></li>
<li><a href="#Gdi32_EnumFontFamiliesW">Gdi32.EnumFontFamiliesW</a></li>
<li><a href="#Gdi32_GetBitmapBits">Gdi32.GetBitmapBits</a></li>
<li><a href="#Gdi32_GetDIBits">Gdi32.GetDIBits</a></li>
<li><a href="#Gdi32_GetFontData">Gdi32.GetFontData</a></li>
<li><a href="#Gdi32_GetObjectA">Gdi32.GetObjectA</a></li>
<li><a href="#Gdi32_GetObjectW">Gdi32.GetObjectW</a></li>
<li><a href="#Gdi32_GetPixel">Gdi32.GetPixel</a></li>
<li><a href="#Gdi32_GetStockObject">Gdi32.GetStockObject</a></li>
<li><a href="#Gdi32_LineTo">Gdi32.LineTo</a></li>
<li><a href="#Gdi32_MoveTo">Gdi32.MoveTo</a></li>
<li><a href="#Gdi32_RGB">Gdi32.RGB</a></li>
<li><a href="#Gdi32_Rectangle">Gdi32.Rectangle</a></li>
<li><a href="#Gdi32_SelectObject">Gdi32.SelectObject</a></li>
<li><a href="#Gdi32_SetBitmapBits">Gdi32.SetBitmapBits</a></li>
<li><a href="#Gdi32_SetBkColor">Gdi32.SetBkColor</a></li>
<li><a href="#Gdi32_SetBkMode">Gdi32.SetBkMode</a></li>
<li><a href="#Gdi32_SetPixelFormat">Gdi32.SetPixelFormat</a></li>
<li><a href="#Gdi32_SetTextColor">Gdi32.SetTextColor</a></li>
<li><a href="#Gdi32_SwapBuffers">Gdi32.SwapBuffers</a></li>
<li><a href="#Gdi32_TextOutA">Gdi32.TextOutA</a></li>
<li><a href="#Gdi32_TextOutW">Gdi32.TextOutW</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Module gdi32</h1>
<p>Wrapper for Microsoft Gdi32 library. </p>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_ANSI_CHARSET"><span class="titletype">const</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">Constants</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L178" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Gdi32_ANSI_CHARSET">ANSI_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_ANSI_FIXED_FONT">ANSI_FIXED_FONT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_ANSI_VAR_FONT">ANSI_VAR_FONT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_ARABIC_CHARSET">ARABIC_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BALTIC_CHARSET">BALTIC_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_BITFIELDS">BI_BITFIELDS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_JPEG">BI_JPEG</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_PNG">BI_PNG</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_RGB">BI_RGB</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_RLE4">BI_RLE4</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_RLE8">BI_RLE8</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BKMODE_LAST">BKMODE_LAST</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BLACKNESS">BLACKNESS</td>
<td>u32</td>
<td>dest = BLACK. </td>
</tr>
<tr>
<td id="Gdi32_BLACK_BRUSH">BLACK_BRUSH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BLACK_PEN">BLACK_PEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_CAPTUREBLT">CAPTUREBLT</td>
<td>u32</td>
<td>Include layered windows. </td>
</tr>
<tr>
<td id="Gdi32_CHINESEBIG5_CHARSET">CHINESEBIG5_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DEFAULT_CHARSET">DEFAULT_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DEFAULT_PALETTE">DEFAULT_PALETTE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DEVICE_DEFAULT_FONT">DEVICE_DEFAULT_FONT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DEVICE_FONTTYPE">DEVICE_FONTTYPE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DIB_PAL_COLORS">DIB_PAL_COLORS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DIB_RGB_COLORS">DIB_RGB_COLORS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DKGRAY_BRUSH">DKGRAY_BRUSH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DSTINVERT">DSTINVERT</td>
<td>u32</td>
<td>dest = (NOT dest). </td>
</tr>
<tr>
<td id="Gdi32_EASTEUROPE_CHARSET">EASTEUROPE_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_BLACK">FW_BLACK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_BOLD">FW_BOLD</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_DEMIBOLD">FW_DEMIBOLD</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_DONTCARE">FW_DONTCARE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_EXTRABOLD">FW_EXTRABOLD</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_EXTRALIGHT">FW_EXTRALIGHT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_HEAVY">FW_HEAVY</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_LIGHT">FW_LIGHT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_MEDIUM">FW_MEDIUM</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_NORMAL">FW_NORMAL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_REGULAR">FW_REGULAR</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_SEMIBOLD">FW_SEMIBOLD</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_THIN">FW_THIN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_ULTRABOLD">FW_ULTRABOLD</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_ULTRALIGHT">FW_ULTRALIGHT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_GB2312_CHARSET">GB2312_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_GDI_ERROR">GDI_ERROR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_GRAY_BRUSH">GRAY_BRUSH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_GREEK_CHARSET">GREEK_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HANGEUL_CHARSET">HANGEUL_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HANGUL_CHARSET">HANGUL_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HEBREW_CHARSET">HEBREW_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HOLLOW_BRUSH">HOLLOW_BRUSH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_JOHAB_CHARSET">JOHAB_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_LF_FACESIZE">LF_FACESIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_LF_FULLFACESIZE">LF_FULLFACESIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_LTGRAY_BRUSH">LTGRAY_BRUSH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_MAC_CHARSET">MAC_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_MERGECOPY">MERGECOPY</td>
<td>u32</td>
<td>dest = (source AND pattern). </td>
</tr>
<tr>
<td id="Gdi32_MERGEPAINT">MERGEPAINT</td>
<td>u32</td>
<td>dest = (NOT source) OR dest. </td>
</tr>
<tr>
<td id="Gdi32_NOMIRRORBITMAP">NOMIRRORBITMAP</td>
<td>u32</td>
<td>Do not Mirror the bitmap in this call. </td>
</tr>
<tr>
<td id="Gdi32_NOTSRCCOPY">NOTSRCCOPY</td>
<td>u32</td>
<td>dest = (NOT source). </td>
</tr>
<tr>
<td id="Gdi32_NOTSRCERASE">NOTSRCERASE</td>
<td>u32</td>
<td>dest = (NOT src) AND (NOT dest). </td>
</tr>
<tr>
<td id="Gdi32_NULL_BRUSH">NULL_BRUSH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_NULL_PEN">NULL_PEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_OEM_CHARSET">OEM_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_OEM_FIXED_FONT">OEM_FIXED_FONT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_OPAQUE">OPAQUE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PATCOPY">PATCOPY</td>
<td>u32</td>
<td>dest = pattern. </td>
</tr>
<tr>
<td id="Gdi32_PATINVERT">PATINVERT</td>
<td>u32</td>
<td>dest = pattern XOR dest. </td>
</tr>
<tr>
<td id="Gdi32_PATPAINT">PATPAINT</td>
<td>u32</td>
<td>dest = DPSnoo. </td>
</tr>
<tr>
<td id="Gdi32_PFD_DEPTH_DONTCARE">PFD_DEPTH_DONTCARE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DIRECT3D_ACCELERATED">PFD_DIRECT3D_ACCELERATED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DOUBLEBUFFER">PFD_DOUBLEBUFFER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DOUBLEBUFFER_DONTCARE">PFD_DOUBLEBUFFER_DONTCARE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DRAW_TO_BITMAP">PFD_DRAW_TO_BITMAP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DRAW_TO_WINDOW">PFD_DRAW_TO_WINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_GENERIC_ACCELERATED">PFD_GENERIC_ACCELERATED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_GENERIC_FORMAT">PFD_GENERIC_FORMAT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_MAIN_PLANE">PFD_MAIN_PLANE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_NEED_PALETTE">PFD_NEED_PALETTE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_NEED_SYSTEM_PALETTE">PFD_NEED_SYSTEM_PALETTE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_OVERLAY_PLANE">PFD_OVERLAY_PLANE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_STEREO">PFD_STEREO</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_STEREO_DONTCARE">PFD_STEREO_DONTCARE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_COMPOSITION">PFD_SUPPORT_COMPOSITION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_DIRECTDRAW">PFD_SUPPORT_DIRECTDRAW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_GDI">PFD_SUPPORT_GDI</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_OPENGL">PFD_SUPPORT_OPENGL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SWAP_COPY">PFD_SWAP_COPY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SWAP_EXCHANGE">PFD_SWAP_EXCHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SWAP_LAYER_BUFFERS">PFD_SWAP_LAYER_BUFFERS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_TYPE_COLORINDEX">PFD_TYPE_COLORINDEX</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_TYPE_RGBA">PFD_TYPE_RGBA</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_UNDERLAY_PLANE">PFD_UNDERLAY_PLANE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_ALTERNATE">PS_ALTERNATE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_DASH">PS_DASH</td>
<td>s32</td>
<td>-------. </td>
</tr>
<tr>
<td id="Gdi32_PS_DASHDOT">PS_DASHDOT</td>
<td>s32</td>
<td>_._._._. </td>
</tr>
<tr>
<td id="Gdi32_PS_DASHDOTDOT">PS_DASHDOTDOT</td>
<td>s32</td>
<td>_.._.._. </td>
</tr>
<tr>
<td id="Gdi32_PS_DOT">PS_DOT</td>
<td>s32</td>
<td>....... </td>
</tr>
<tr>
<td id="Gdi32_PS_INSIDEFRAME">PS_INSIDEFRAME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_NULL">PS_NULL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_SOLID">PS_SOLID</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_STYLE_MASK">PS_STYLE_MASK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_USERSTYLE">PS_USERSTYLE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_RASTER_FONTTYPE">RASTER_FONTTYPE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_RUSSIAN_CHARSET">RUSSIAN_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_SHIFTJIS_CHARSET">SHIFTJIS_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_SRCAND">SRCAND</td>
<td>u32</td>
<td>dest = source AND dest. </td>
</tr>
<tr>
<td id="Gdi32_SRCCOPY">SRCCOPY</td>
<td>u32</td>
<td>dest = source. </td>
</tr>
<tr>
<td id="Gdi32_SRCERASE">SRCERASE</td>
<td>u32</td>
<td>dest = source AND (NOT dest ). </td>
</tr>
<tr>
<td id="Gdi32_SRCINVERT">SRCINVERT</td>
<td>u32</td>
<td>dest = source XOR dest. </td>
</tr>
<tr>
<td id="Gdi32_SRCPAINT">SRCPAINT</td>
<td>u32</td>
<td>dest = source OR dest. </td>
</tr>
<tr>
<td id="Gdi32_SYMBOL_CHARSET">SYMBOL_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_SYSTEM_FIXED_FONT">SYSTEM_FIXED_FONT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_SYSTEM_FONT">SYSTEM_FONT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_THAI_CHARSET">THAI_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_TRANSPARENT">TRANSPARENT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_TRUETYPE_FONTTYPE">TRUETYPE_FONTTYPE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_TURKISH_CHARSET">TURKISH_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_VIETNAMESE_CHARSET">VIETNAMESE_CHARSET</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_WHITENESS">WHITENESS</td>
<td>u32</td>
<td>dest = WHITE. </td>
</tr>
<tr>
<td id="Gdi32_WHITE_BRUSH">WHITE_BRUSH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_WHITE_PEN">WHITE_PEN</td>
<td>s32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_FONTENUMPROCA"><span class="titletype">type alias</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">Type Aliases</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L371" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Gdi32_FONTENUMPROCA">FONTENUMPROCA</td>
<td>func(const *Gdi32.LOGFONTA, const *Gdi32.TEXTMETRICA, Win32.DWORD, Win32.LPARAM)->Win32.BOOL</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FONTENUMPROCW">FONTENUMPROCW</td>
<td>func(const *Gdi32.LOGFONTW, const *Gdi32.TEXTMETRICW, Win32.DWORD, Win32.LPARAM)->Win32.BOOL</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HFONT">HFONT</td>
<td>const *void</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HGDIOBJ">HGDIOBJ</td>
<td>const *void</td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HPEN">HPEN</td>
<td>const *void</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_BITMAP"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">BITMAP</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>bmType</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>bmWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>bmHeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>bmWidthBytes</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>bmPlanes</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>bmBitsPixel</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>bmBits</td>
<td>^void</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_BITMAPINFO"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">BITMAPINFO</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L133" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>bmiHeader</td>
<td><a href="#Gdi32_BITMAPINFOHEADER">Gdi32.BITMAPINFOHEADER</a></td>
<td></td>
</tr>
<tr>
<td>bmiColors</td>
<td>[1] Gdi32.RGBQUAD</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_BITMAPINFOHEADER"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">BITMAPINFOHEADER</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L118" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>biSize</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>biWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>biHeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>biPlanes</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>biBitCount</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>biCompression</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>biSizeImage</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>biXPelsPerMeter</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>biYPelsPerMeter</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>biClrUsed</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>biClrImportant</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_BitBlt"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">BitBlt</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L496" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">BitBlt</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cx: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cy: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, hdcSrc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x1: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y1: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, rop: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_ChoosePixelFormat"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">ChoosePixelFormat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L437" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ChoosePixelFormat</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, ppfd: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_CreateCompatibleBitmap"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">CreateCompatibleBitmap</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L474" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateCompatibleBitmap</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, cx: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cy: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_CreateCompatibleDC"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">CreateCompatibleDC</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L466" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateCompatibleDC</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_CreateDIBSection"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">CreateDIBSection</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L580" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateDIBSection</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, pbmi: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_BITMAPINFO">BITMAPINFO</a></span><span class="SyntaxCode">, usage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, ppvBits: **</span><span class="SyntaxType">void</span><span class="SyntaxCode">, hSection: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, offset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_CreateFontIndirectA"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">CreateFontIndirectA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L539" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFontIndirectA</span><span class="SyntaxCode">(lplf: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_LOGFONTA">LOGFONTA</a></span><span class="SyntaxCode">)-><a href=</span><span class="SyntaxString">"#Gdi32_HFONT"</span><span class="SyntaxCode">></span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_HFONT">HFONT</a></span><span class="SyntaxCode"></a> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_CreateFontIndirectW"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">CreateFontIndirectW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L547" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFontIndirectW</span><span class="SyntaxCode">(lplf: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_LOGFONTW">LOGFONTW</a></span><span class="SyntaxCode">)-><a href=</span><span class="SyntaxString">"#Gdi32_HFONT"</span><span class="SyntaxCode">></span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_HFONT">HFONT</a></span><span class="SyntaxCode"></a> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_CreatePen"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">CreatePen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L563" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreatePen</span><span class="SyntaxCode">(iStyle: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">)-><a href=</span><span class="SyntaxString">"#Gdi32_HPEN"</span><span class="SyntaxCode">></span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_HPEN">HPEN</a></span><span class="SyntaxCode"></a> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_CreateSolidBrush"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">CreateSolidBrush</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L555" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateSolidBrush</span><span class="SyntaxCode">(color: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBRUSH</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_DeleteDC"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">DeleteDC</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L482" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteDC</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_DeleteObject"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">DeleteObject</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L489" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteObject</span><span class="SyntaxCode">(ho: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_HGDIOBJ">HGDIOBJ</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_DescribePixelFormat"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">DescribePixelFormat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L458" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DescribePixelFormat</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, iPixelFormat: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nBytes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, ppfd: *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_ENUMLOGFONTEXA"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">ENUMLOGFONTEXA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L251" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>elfLogFont</td>
<td><a href="#Gdi32_LOGFONTA">Gdi32.LOGFONTA</a></td>
<td></td>
</tr>
<tr>
<td>elfFullName</td>
<td>[64] Win32.BYTE</td>
<td></td>
</tr>
<tr>
<td>elfStyle</td>
<td>[32] Win32.BYTE</td>
<td></td>
</tr>
<tr>
<td>elfScript</td>
<td>[32] Win32.BYTE</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_ENUMLOGFONTEXW"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">ENUMLOGFONTEXW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L259" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>elfLogFont</td>
<td><a href="#Gdi32_LOGFONTW">Gdi32.LOGFONTW</a></td>
<td></td>
</tr>
<tr>
<td>elfFullName</td>
<td>[64] Win32.WCHAR</td>
<td></td>
</tr>
<tr>
<td>elfStyle</td>
<td>[32] Win32.WCHAR</td>
<td></td>
</tr>
<tr>
<td>elfScript</td>
<td>[32] Win32.WCHAR</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_EnumFontFamiliesA"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">EnumFontFamiliesA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L392" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumFontFamiliesA</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lpLogfont: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpProc: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_FONTENUMPROCA">FONTENUMPROCA</a></span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_EnumFontFamiliesExA"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">EnumFontFamiliesExA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L394" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumFontFamiliesExA</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lpLogfont: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_LOGFONTA">LOGFONTA</a></span><span class="SyntaxCode">, lpProc: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_FONTENUMPROCA">FONTENUMPROCA</a></span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_EnumFontFamiliesExW"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">EnumFontFamiliesExW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L395" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumFontFamiliesExW</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lpLogfont: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_LOGFONTW">LOGFONTW</a></span><span class="SyntaxCode">, lpProc: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_FONTENUMPROCW">FONTENUMPROCW</a></span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_EnumFontFamiliesW"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">EnumFontFamiliesW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L393" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumFontFamiliesW</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lpLogfont: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpProc: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_FONTENUMPROCW">FONTENUMPROCW</a></span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_GetBitmapBits"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">GetBitmapBits</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L517" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetBitmapBits</span><span class="SyntaxCode">(hbit: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode">, cb: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">, lpvBits: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_GetDIBits"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">GetDIBits</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L572" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetDIBits</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, hbm: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode">, start: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, cLines: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, lpvBits: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, lpbmi: *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_BITMAPINFO">BITMAPINFO</a></span><span class="SyntaxCode">, usage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_GetFontData"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">GetFontData</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L531" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFontData</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, dwTable: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwOffset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, pvBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, cjBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_GetObjectA"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">GetObjectA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L503" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetObjectA</span><span class="SyntaxCode">(h: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pv: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetObjectA</span><span class="SyntaxCode">(h: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pv: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_GetObjectW"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">GetObjectW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L510" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetObjectW</span><span class="SyntaxCode">(h: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pv: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetObjectW</span><span class="SyntaxCode">(h: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pv: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_GetPixel"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">GetPixel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L382" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetPixel</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_GetStockObject"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">GetStockObject</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L383" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetStockObject</span><span class="SyntaxCode">(i: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-><a href=</span><span class="SyntaxString">"#Gdi32_HGDIOBJ"</span><span class="SyntaxCode">></span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_HGDIOBJ">HGDIOBJ</a></span><span class="SyntaxCode"></a></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_LOGFONTA"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">LOGFONTA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L214" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>lfHeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfEscapement</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfOrientation</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfWeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfItalic</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfUnderline</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfStrikeOut</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfCharSet</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfOutPrecision</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfClipPrecision</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfQuality</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfPitchAndFamily</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfFaceName</td>
<td>[32] Win32.CHAR</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_LOGFONTW"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">LOGFONTW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L232" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>lfHeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfEscapement</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfOrientation</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfWeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>lfItalic</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfUnderline</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfStrikeOut</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfCharSet</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfOutPrecision</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfClipPrecision</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfQuality</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfPitchAndFamily</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>lfFaceName</td>
<td>[32] Win32.WCHAR</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_LineTo"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">LineTo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L404" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LineTo</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_MoveTo"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">MoveTo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L403" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MoveTo</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_NEWTEXTMETRICA"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">NEWTEXTMETRICA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L315" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>tmHeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmAscent</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDescent</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmInternalLeading</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmExternalLeading</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmAveCharWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmMaxCharWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmWeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmOverhang</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDigitizedAspectX</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDigitizedAspectY</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmFirstChar</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmLastChar</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmDefaultChar</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmBreakChar</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmItalic</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmUnderlined</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmStruckOut</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmPitchAndFamily</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmCharSet</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>ntmFlags</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ntmSizeEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ntmCellHeight</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ntmAvgWidth</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_NEWTEXTMETRICW"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">NEWTEXTMETRICW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L343" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>tmHeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmAscent</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDescent</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmInternalLeading</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmExternalLeading</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmAveCharWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmMaxCharWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmWeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmOverhang</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDigitizedAspectX</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDigitizedAspectY</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmFirstChar</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>tmLastChar</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>tmDefaultChar</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>tmBreakChar</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>tmItalic</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmUnderlined</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmStruckOut</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmPitchAndFamily</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmCharSet</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>ntmFlags</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ntmSizeEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ntmCellHeight</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ntmAvgWidth</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_PIXELFORMATDESCRIPTOR"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">PIXELFORMATDESCRIPTOR</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>nSize</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>nVersion</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>dwFlags</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>iPixelType</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cColorBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cRedBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cRedShift</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cGreenBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cGreenShift</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cBlueBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cBlueShift</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cAlphaBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cAlphaShift</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cAccumBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cAccumRedBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cAccumGreenBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cAccumBlueBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cAccumAlphaBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cDepthBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cStencilBits</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>cAuxBuffers</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>iLayerType</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>bReserved</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>dwLayerMask</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>dwVisibleMask</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>dwDamageMask</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_RGB"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">RGB</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L375" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">(r: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_RGBQUAD"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">RGBQUAD</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>rgbBlue</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>rgbGreen</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>rgbRed</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>rgbReserved</td>
<td>u8</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_Rectangle"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">Rectangle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L405" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, left: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, right: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, bottom: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_SelectObject"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">SelectObject</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L390" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SelectObject</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, h: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_HGDIOBJ">HGDIOBJ</a></span><span class="SyntaxCode">)-><a href=</span><span class="SyntaxString">"#Gdi32_HGDIOBJ"</span><span class="SyntaxCode">></span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_HGDIOBJ">HGDIOBJ</a></span><span class="SyntaxCode"></a></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_SetBitmapBits"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">SetBitmapBits</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L524" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetBitmapBits</span><span class="SyntaxCode">(hbit: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode">, cb: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpvBits: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_SetBkColor"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">SetBkColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L398" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetBkColor</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_SetBkMode"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">SetBkMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L397" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetBkMode</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, mode: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_SetPixelFormat"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">SetPixelFormat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L445" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetPixelFormat</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, fmt: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, ppfd: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_SetTextColor"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">SetTextColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L399" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetTextColor</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_SwapBuffers"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">SwapBuffers</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L451" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SwapBuffers</span><span class="SyntaxCode">(arg1: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_TEXTMETRICA"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">TEXTMETRICA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L267" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>tmHeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmAscent</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDescent</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmInternalLeading</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmExternalLeading</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmAveCharWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmMaxCharWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmWeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmOverhang</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDigitizedAspectX</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDigitizedAspectY</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmFirstChar</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmLastChar</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmDefaultChar</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmBreakChar</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmItalic</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmUnderlined</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmStruckOut</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmPitchAndFamily</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmCharSet</td>
<td>u8</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_TEXTMETRICW"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">TEXTMETRICW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L291" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>tmHeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmAscent</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDescent</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmInternalLeading</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmExternalLeading</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmAveCharWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmMaxCharWidth</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmWeight</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmOverhang</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDigitizedAspectX</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmDigitizedAspectY</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>tmFirstChar</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>tmLastChar</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>tmDefaultChar</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>tmBreakChar</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>tmItalic</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmUnderlined</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmStruckOut</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmPitchAndFamily</td>
<td>u8</td>
<td></td>
</tr>
<tr>
<td>tmCharSet</td>
<td>u8</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_TextOutA"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">TextOutA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L401" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TextOutA</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Gdi32_TextOutW"><span class="titletype">func</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">TextOutW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L402" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TextOutW</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</div>
</div>
</div>
</div>
</body>
</html>
