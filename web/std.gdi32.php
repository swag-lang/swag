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
        }
    .SCde { color: #7f7f7f; }
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
<td id="Gdi32_ANSI_CHARSET" class="codetype"><span class="SCst">ANSI_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_ANSI_FIXED_FONT" class="codetype"><span class="SCst">ANSI_FIXED_FONT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_ANSI_VAR_FONT" class="codetype"><span class="SCst">ANSI_VAR_FONT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_ARABIC_CHARSET" class="codetype"><span class="SCst">ARABIC_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BALTIC_CHARSET" class="codetype"><span class="SCst">BALTIC_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_BITFIELDS" class="codetype"><span class="SCst">BI_BITFIELDS</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_JPEG" class="codetype"><span class="SCst">BI_JPEG</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_PNG" class="codetype"><span class="SCst">BI_PNG</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_RGB" class="codetype"><span class="SCst">BI_RGB</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_RLE4" class="codetype"><span class="SCst">BI_RLE4</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BI_RLE8" class="codetype"><span class="SCst">BI_RLE8</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BKMODE_LAST" class="codetype"><span class="SCst">BKMODE_LAST</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BLACKNESS" class="codetype"><span class="SCst">BLACKNESS</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = BLACK. </td>
</tr>
<tr>
<td id="Gdi32_BLACK_BRUSH" class="codetype"><span class="SCst">BLACK_BRUSH</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_BLACK_PEN" class="codetype"><span class="SCst">BLACK_PEN</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_CAPTUREBLT" class="codetype"><span class="SCst">CAPTUREBLT</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Include layered windows. </td>
</tr>
<tr>
<td id="Gdi32_CHINESEBIG5_CHARSET" class="codetype"><span class="SCst">CHINESEBIG5_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DEFAULT_CHARSET" class="codetype"><span class="SCst">DEFAULT_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DEFAULT_PALETTE" class="codetype"><span class="SCst">DEFAULT_PALETTE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DEVICE_DEFAULT_FONT" class="codetype"><span class="SCst">DEVICE_DEFAULT_FONT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DEVICE_FONTTYPE" class="codetype"><span class="SCst">DEVICE_FONTTYPE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DIB_PAL_COLORS" class="codetype"><span class="SCst">DIB_PAL_COLORS</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DIB_RGB_COLORS" class="codetype"><span class="SCst">DIB_RGB_COLORS</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DKGRAY_BRUSH" class="codetype"><span class="SCst">DKGRAY_BRUSH</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_DSTINVERT" class="codetype"><span class="SCst">DSTINVERT</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = (NOT dest). </td>
</tr>
<tr>
<td id="Gdi32_EASTEUROPE_CHARSET" class="codetype"><span class="SCst">EASTEUROPE_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_BLACK" class="codetype"><span class="SCst">FW_BLACK</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_BOLD" class="codetype"><span class="SCst">FW_BOLD</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_DEMIBOLD" class="codetype"><span class="SCst">FW_DEMIBOLD</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_DONTCARE" class="codetype"><span class="SCst">FW_DONTCARE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_EXTRABOLD" class="codetype"><span class="SCst">FW_EXTRABOLD</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_EXTRALIGHT" class="codetype"><span class="SCst">FW_EXTRALIGHT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_HEAVY" class="codetype"><span class="SCst">FW_HEAVY</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_LIGHT" class="codetype"><span class="SCst">FW_LIGHT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_MEDIUM" class="codetype"><span class="SCst">FW_MEDIUM</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_NORMAL" class="codetype"><span class="SCst">FW_NORMAL</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_REGULAR" class="codetype"><span class="SCst">FW_REGULAR</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_SEMIBOLD" class="codetype"><span class="SCst">FW_SEMIBOLD</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_THIN" class="codetype"><span class="SCst">FW_THIN</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_ULTRABOLD" class="codetype"><span class="SCst">FW_ULTRABOLD</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FW_ULTRALIGHT" class="codetype"><span class="SCst">FW_ULTRALIGHT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_GB2312_CHARSET" class="codetype"><span class="SCst">GB2312_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_GDI_ERROR" class="codetype"><span class="SCst">GDI_ERROR</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_GRAY_BRUSH" class="codetype"><span class="SCst">GRAY_BRUSH</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_GREEK_CHARSET" class="codetype"><span class="SCst">GREEK_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HANGEUL_CHARSET" class="codetype"><span class="SCst">HANGEUL_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HANGUL_CHARSET" class="codetype"><span class="SCst">HANGUL_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HEBREW_CHARSET" class="codetype"><span class="SCst">HEBREW_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HOLLOW_BRUSH" class="codetype"><span class="SCst">HOLLOW_BRUSH</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_JOHAB_CHARSET" class="codetype"><span class="SCst">JOHAB_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_LF_FACESIZE" class="codetype"><span class="SCst">LF_FACESIZE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_LF_FULLFACESIZE" class="codetype"><span class="SCst">LF_FULLFACESIZE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_LTGRAY_BRUSH" class="codetype"><span class="SCst">LTGRAY_BRUSH</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_MAC_CHARSET" class="codetype"><span class="SCst">MAC_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_MERGECOPY" class="codetype"><span class="SCst">MERGECOPY</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = (source AND pattern). </td>
</tr>
<tr>
<td id="Gdi32_MERGEPAINT" class="codetype"><span class="SCst">MERGEPAINT</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = (NOT source) OR dest. </td>
</tr>
<tr>
<td id="Gdi32_NOMIRRORBITMAP" class="codetype"><span class="SCst">NOMIRRORBITMAP</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Do not Mirror the bitmap in this call. </td>
</tr>
<tr>
<td id="Gdi32_NOTSRCCOPY" class="codetype"><span class="SCst">NOTSRCCOPY</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = (NOT source). </td>
</tr>
<tr>
<td id="Gdi32_NOTSRCERASE" class="codetype"><span class="SCst">NOTSRCERASE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = (NOT src) AND (NOT dest). </td>
</tr>
<tr>
<td id="Gdi32_NULL_BRUSH" class="codetype"><span class="SCst">NULL_BRUSH</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_NULL_PEN" class="codetype"><span class="SCst">NULL_PEN</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_OEM_CHARSET" class="codetype"><span class="SCst">OEM_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_OEM_FIXED_FONT" class="codetype"><span class="SCst">OEM_FIXED_FONT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_OPAQUE" class="codetype"><span class="SCst">OPAQUE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PATCOPY" class="codetype"><span class="SCst">PATCOPY</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = pattern. </td>
</tr>
<tr>
<td id="Gdi32_PATINVERT" class="codetype"><span class="SCst">PATINVERT</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = pattern XOR dest. </td>
</tr>
<tr>
<td id="Gdi32_PATPAINT" class="codetype"><span class="SCst">PATPAINT</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = DPSnoo. </td>
</tr>
<tr>
<td id="Gdi32_PFD_DEPTH_DONTCARE" class="codetype"><span class="SCst">PFD_DEPTH_DONTCARE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DIRECT3D_ACCELERATED" class="codetype"><span class="SCst">PFD_DIRECT3D_ACCELERATED</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DOUBLEBUFFER" class="codetype"><span class="SCst">PFD_DOUBLEBUFFER</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DOUBLEBUFFER_DONTCARE" class="codetype"><span class="SCst">PFD_DOUBLEBUFFER_DONTCARE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DRAW_TO_BITMAP" class="codetype"><span class="SCst">PFD_DRAW_TO_BITMAP</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_DRAW_TO_WINDOW" class="codetype"><span class="SCst">PFD_DRAW_TO_WINDOW</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_GENERIC_ACCELERATED" class="codetype"><span class="SCst">PFD_GENERIC_ACCELERATED</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_GENERIC_FORMAT" class="codetype"><span class="SCst">PFD_GENERIC_FORMAT</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_MAIN_PLANE" class="codetype"><span class="SCst">PFD_MAIN_PLANE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_NEED_PALETTE" class="codetype"><span class="SCst">PFD_NEED_PALETTE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_NEED_SYSTEM_PALETTE" class="codetype"><span class="SCst">PFD_NEED_SYSTEM_PALETTE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_OVERLAY_PLANE" class="codetype"><span class="SCst">PFD_OVERLAY_PLANE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_STEREO" class="codetype"><span class="SCst">PFD_STEREO</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_STEREO_DONTCARE" class="codetype"><span class="SCst">PFD_STEREO_DONTCARE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_COMPOSITION" class="codetype"><span class="SCst">PFD_SUPPORT_COMPOSITION</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_DIRECTDRAW" class="codetype"><span class="SCst">PFD_SUPPORT_DIRECTDRAW</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_GDI" class="codetype"><span class="SCst">PFD_SUPPORT_GDI</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_OPENGL" class="codetype"><span class="SCst">PFD_SUPPORT_OPENGL</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SWAP_COPY" class="codetype"><span class="SCst">PFD_SWAP_COPY</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SWAP_EXCHANGE" class="codetype"><span class="SCst">PFD_SWAP_EXCHANGE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_SWAP_LAYER_BUFFERS" class="codetype"><span class="SCst">PFD_SWAP_LAYER_BUFFERS</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_TYPE_COLORINDEX" class="codetype"><span class="SCst">PFD_TYPE_COLORINDEX</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_TYPE_RGBA" class="codetype"><span class="SCst">PFD_TYPE_RGBA</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PFD_UNDERLAY_PLANE" class="codetype"><span class="SCst">PFD_UNDERLAY_PLANE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_ALTERNATE" class="codetype"><span class="SCst">PS_ALTERNATE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_DASH" class="codetype"><span class="SCst">PS_DASH</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_DASHDOT" class="codetype"><span class="SCst">PS_DASHDOT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>_._._._. </td>
</tr>
<tr>
<td id="Gdi32_PS_DASHDOTDOT" class="codetype"><span class="SCst">PS_DASHDOTDOT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>_.._.._. </td>
</tr>
<tr>
<td id="Gdi32_PS_DOT" class="codetype"><span class="SCst">PS_DOT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>....... </td>
</tr>
<tr>
<td id="Gdi32_PS_INSIDEFRAME" class="codetype"><span class="SCst">PS_INSIDEFRAME</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_NULL" class="codetype"><span class="SCst">PS_NULL</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_SOLID" class="codetype"><span class="SCst">PS_SOLID</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_STYLE_MASK" class="codetype"><span class="SCst">PS_STYLE_MASK</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_PS_USERSTYLE" class="codetype"><span class="SCst">PS_USERSTYLE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_RASTER_FONTTYPE" class="codetype"><span class="SCst">RASTER_FONTTYPE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_RUSSIAN_CHARSET" class="codetype"><span class="SCst">RUSSIAN_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_SHIFTJIS_CHARSET" class="codetype"><span class="SCst">SHIFTJIS_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_SRCAND" class="codetype"><span class="SCst">SRCAND</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = source AND dest. </td>
</tr>
<tr>
<td id="Gdi32_SRCCOPY" class="codetype"><span class="SCst">SRCCOPY</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = source. </td>
</tr>
<tr>
<td id="Gdi32_SRCERASE" class="codetype"><span class="SCst">SRCERASE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = source AND (NOT dest ). </td>
</tr>
<tr>
<td id="Gdi32_SRCINVERT" class="codetype"><span class="SCst">SRCINVERT</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = source XOR dest. </td>
</tr>
<tr>
<td id="Gdi32_SRCPAINT" class="codetype"><span class="SCst">SRCPAINT</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = source OR dest. </td>
</tr>
<tr>
<td id="Gdi32_SYMBOL_CHARSET" class="codetype"><span class="SCst">SYMBOL_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_SYSTEM_FIXED_FONT" class="codetype"><span class="SCst">SYSTEM_FIXED_FONT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_SYSTEM_FONT" class="codetype"><span class="SCst">SYSTEM_FONT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_THAI_CHARSET" class="codetype"><span class="SCst">THAI_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_TRANSPARENT" class="codetype"><span class="SCst">TRANSPARENT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_TRUETYPE_FONTTYPE" class="codetype"><span class="SCst">TRUETYPE_FONTTYPE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_TURKISH_CHARSET" class="codetype"><span class="SCst">TURKISH_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_VIETNAMESE_CHARSET" class="codetype"><span class="SCst">VIETNAMESE_CHARSET</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_WHITENESS" class="codetype"><span class="SCst">WHITENESS</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>dest = WHITE. </td>
</tr>
<tr>
<td id="Gdi32_WHITE_BRUSH" class="codetype"><span class="SCst">WHITE_BRUSH</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_WHITE_PEN" class="codetype"><span class="SCst">WHITE_PEN</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
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
<td id="Gdi32_FONTENUMPROCA" class="codetype"><span class="SCst">FONTENUMPROCA</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(<span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_LOGFONTA">LOGFONTA</a></span>, <span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_TEXTMETRICA">TEXTMETRICA</a></span>, <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>, <span class="SCst">Win32</span>.<span class="SCst">LPARAM</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">BOOL</span></span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_FONTENUMPROCW" class="codetype"><span class="SCst">FONTENUMPROCW</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(<span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_LOGFONTW">LOGFONTW</a></span>, <span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_TEXTMETRICW">TEXTMETRICW</a></span>, <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>, <span class="SCst">Win32</span>.<span class="SCst">LPARAM</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">BOOL</span></span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HFONT" class="codetype"><span class="SCst">HFONT</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HGDIOBJ" class="codetype"><span class="SCst">HGDIOBJ</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Gdi32_HPEN" class="codetype"><span class="SCst">HPEN</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
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
<td class="codetype"><span class="SCde">bmType</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bmWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bmHeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bmWidthBytes</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bmPlanes</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bmBitsPixel</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bmBits</span></td>
<td class="codetype"><span class="SCde">^<span class="STpe">void</span></span></td>
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
<td class="codetype"><span class="SCde">bmiHeader</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_BITMAPINFOHEADER">BITMAPINFOHEADER</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bmiColors</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">1</span>] <span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_RGBQUAD">RGBQUAD</a></span></span></td>
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
<td class="codetype"><span class="SCde">biSize</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biHeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biPlanes</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biBitCount</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biCompression</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biSizeImage</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biXPelsPerMeter</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biYPelsPerMeter</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biClrUsed</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">biClrImportant</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">BitBlt</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, cx: <span class="STpe">s32</span>, cy: <span class="STpe">s32</span>, hdcSrc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, x1: <span class="STpe">s32</span>, y1: <span class="STpe">s32</span>, rop: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ChoosePixelFormat</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, ppfd: <span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></span>)-&gt;<span class="STpe">s32</span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateCompatibleBitmap</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, cx: <span class="STpe">s32</span>, cy: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HBITMAP</span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateCompatibleDC</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HDC</span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateDIBSection</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, pbmi: <span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_BITMAPINFO">BITMAPINFO</a></span>, usage: <span class="SCst">Win32</span>.<span class="SCst">UINT</span>, ppvBits: **<span class="STpe">void</span>, hSection: <span class="SCst">Win32</span>.<span class="SCst">HANDLE</span>, offset: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HBITMAP</span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateFontIndirectA</span>(lplf: <span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_LOGFONTA">LOGFONTA</a></span>)-&gt;<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_HFONT">HFONT</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateFontIndirectW</span>(lplf: <span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_LOGFONTW">LOGFONTW</a></span>)-&gt;<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_HFONT">HFONT</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreatePen</span>(iStyle: <span class="STpe">s32</span>, cWidth: <span class="STpe">s32</span>, color: <span class="SCst">Win32</span>.<span class="SCst">COLORREF</span>)-&gt;<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_HPEN">HPEN</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateSolidBrush</span>(color: <span class="SCst">Win32</span>.<span class="SCst">COLORREF</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HBRUSH</span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DeleteDC</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DeleteObject</span>(ho: <span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_HGDIOBJ">HGDIOBJ</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DescribePixelFormat</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, iPixelFormat: <span class="STpe">s32</span>, nBytes: <span class="SCst">Win32</span>.<span class="SCst">UINT</span>, ppfd: *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></span>)-&gt;<span class="STpe">s32</span> <span class="SKwd">throw</span></span></code>
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
<td class="codetype"><span class="SCde">elfLogFont</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_LOGFONTA">LOGFONTA</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">elfFullName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">64</span>] <span class="SCst">Win32</span>.<span class="SCst">BYTE</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">elfStyle</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">32</span>] <span class="SCst">Win32</span>.<span class="SCst">BYTE</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">elfScript</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">32</span>] <span class="SCst">Win32</span>.<span class="SCst">BYTE</span></span></td>
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
<td class="codetype"><span class="SCde">elfLogFont</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_LOGFONTW">LOGFONTW</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">elfFullName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">64</span>] <span class="SCst">Win32</span>.<span class="SCst">WCHAR</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">elfStyle</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">32</span>] <span class="SCst">Win32</span>.<span class="SCst">WCHAR</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">elfScript</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">32</span>] <span class="SCst">Win32</span>.<span class="SCst">WCHAR</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">EnumFontFamiliesA</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, lpLogfont: <span class="SCst">Win32</span>.<span class="SCst">LPCSTR</span>, lpProc: <span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_FONTENUMPROCA">FONTENUMPROCA</a></span>, lParam: <span class="SCst">Win32</span>.<span class="SCst">LPARAM</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">EnumFontFamiliesExA</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, lpLogfont: <span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_LOGFONTA">LOGFONTA</a></span>, lpProc: <span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_FONTENUMPROCA">FONTENUMPROCA</a></span>, lParam: <span class="SCst">Win32</span>.<span class="SCst">LPARAM</span>, dwFlags: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">EnumFontFamiliesExW</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, lpLogfont: <span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_LOGFONTW">LOGFONTW</a></span>, lpProc: <span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_FONTENUMPROCW">FONTENUMPROCW</a></span>, lParam: <span class="SCst">Win32</span>.<span class="SCst">LPARAM</span>, dwFlags: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">EnumFontFamiliesW</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, lpLogfont: <span class="SCst">Win32</span>.<span class="SCst">LPCWSTR</span>, lpProc: <span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_FONTENUMPROCW">FONTENUMPROCW</a></span>, lParam: <span class="SCst">Win32</span>.<span class="SCst">LPARAM</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetBitmapBits</span>(hbit: <span class="SCst">Win32</span>.<span class="SCst">HBITMAP</span>, cb: <span class="SCst">Win32</span>.<span class="SCst">LONG</span>, lpvBits: <span class="SCst">Win32</span>.<span class="SCst">LPVOID</span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetDIBits</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, hbm: <span class="SCst">Win32</span>.<span class="SCst">HBITMAP</span>, start: <span class="SCst">Win32</span>.<span class="SCst">UINT</span>, cLines: <span class="SCst">Win32</span>.<span class="SCst">UINT</span>, lpvBits: <span class="SCst">Win32</span>.<span class="SCst">LPVOID</span>, lpbmi: *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_BITMAPINFO">BITMAPINFO</a></span>, usage: <span class="SCst">Win32</span>.<span class="SCst">UINT</span>)-&gt;<span class="STpe">s32</span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetFontData</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, dwTable: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>, dwOffset: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>, pvBuffer: <span class="SCst">Win32</span>.<span class="SCst">LPVOID</span>, cjBuffer: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">DWORD</span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetObjectA</span>(h: <span class="SCst">Win32</span>.<span class="SCst">HANDLE</span>, c: <span class="STpe">s32</span>, pv: <span class="SCst">Win32</span>.<span class="SCst">LPVOID</span>) <span class="SKwd">throw</span>
<span class="SKwd">func</span> <span class="SCst">GetObjectA</span>(h: <span class="SCst">Win32</span>.<span class="SCst">HANDLE</span>, c: <span class="STpe">s32</span>, pv: <span class="SCst">Win32</span>.<span class="SCst">LPVOID</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetObjectW</span>(h: <span class="SCst">Win32</span>.<span class="SCst">HANDLE</span>, c: <span class="STpe">s32</span>, pv: <span class="SCst">Win32</span>.<span class="SCst">LPVOID</span>) <span class="SKwd">throw</span>
<span class="SKwd">func</span> <span class="SCst">GetObjectW</span>(h: <span class="SCst">Win32</span>.<span class="SCst">HANDLE</span>, c: <span class="STpe">s32</span>, pv: <span class="SCst">Win32</span>.<span class="SCst">LPVOID</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetPixel</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">COLORREF</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetStockObject</span>(i: <span class="STpe">s32</span>)-&gt;<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_HGDIOBJ">HGDIOBJ</a></span></span></code>
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
<td class="codetype"><span class="SCde">lfHeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfEscapement</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfOrientation</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfWeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfItalic</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfUnderline</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfStrikeOut</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfCharSet</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfOutPrecision</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfClipPrecision</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfQuality</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfPitchAndFamily</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfFaceName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">32</span>] <span class="SCst">Win32</span>.<span class="SCst">CHAR</span></span></td>
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
<td class="codetype"><span class="SCde">lfHeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfEscapement</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfOrientation</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfWeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfItalic</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfUnderline</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfStrikeOut</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfCharSet</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfOutPrecision</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfClipPrecision</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfQuality</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfPitchAndFamily</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lfFaceName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">32</span>] <span class="SCst">Win32</span>.<span class="SCst">WCHAR</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">LineTo</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">BOOL</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MoveTo</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">BOOL</span></span></code>
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
<td class="codetype"><span class="SCde">tmHeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmAscent</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDescent</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmInternalLeading</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmExternalLeading</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmAveCharWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmMaxCharWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmWeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmOverhang</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDigitizedAspectX</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDigitizedAspectY</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmFirstChar</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmLastChar</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDefaultChar</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmBreakChar</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmItalic</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmUnderlined</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmStruckOut</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmPitchAndFamily</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmCharSet</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ntmFlags</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ntmSizeEM</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ntmCellHeight</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ntmAvgWidth</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
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
<td class="codetype"><span class="SCde">tmHeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmAscent</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDescent</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmInternalLeading</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmExternalLeading</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmAveCharWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmMaxCharWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmWeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmOverhang</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDigitizedAspectX</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDigitizedAspectY</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmFirstChar</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmLastChar</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDefaultChar</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmBreakChar</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmItalic</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmUnderlined</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmStruckOut</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmPitchAndFamily</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmCharSet</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ntmFlags</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ntmSizeEM</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ntmCellHeight</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ntmAvgWidth</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
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
<td class="codetype"><span class="SCde">nSize</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nVersion</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwFlags</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">iPixelType</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cColorBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cRedBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cRedShift</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cGreenBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cGreenShift</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cBlueBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cBlueShift</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAlphaBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAlphaShift</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAccumBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAccumRedBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAccumGreenBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAccumBlueBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAccumAlphaBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cDepthBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cStencilBits</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAuxBuffers</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">iLayerType</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bReserved</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwLayerMask</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwVisibleMask</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwDamageMask</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">RGB</span>(r: <span class="STpe">s32</span>, g: <span class="STpe">s32</span>, b: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">COLORREF</span></span></code>
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
<td class="codetype"><span class="SCde">rgbBlue</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rgbGreen</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rgbRed</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rgbReserved</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">Rectangle</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, left: <span class="STpe">s32</span>, top: <span class="STpe">s32</span>, right: <span class="STpe">s32</span>, bottom: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">BOOL</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SelectObject</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, h: <span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_HGDIOBJ">HGDIOBJ</a></span>)-&gt;<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_HGDIOBJ">HGDIOBJ</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetBitmapBits</span>(hbit: <span class="SCst">Win32</span>.<span class="SCst">HBITMAP</span>, cb: <span class="SCst">Win32</span>.<span class="SCst">DWORD</span>, lpvBits: <span class="SCst">Win32</span>.<span class="SCst">LPVOID</span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetBkColor</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, color: <span class="SCst">Win32</span>.<span class="SCst">COLORREF</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">COLORREF</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetBkMode</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, mode: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetPixelFormat</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, fmt: <span class="STpe">s32</span>, ppfd: <span class="SKwd">const</span> *<span class="SCst">Gdi32</span>.<span class="SCst"><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetTextColor</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, color: <span class="SCst">Win32</span>.<span class="SCst">COLORREF</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">COLORREF</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SwapBuffers</span>(arg1: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>) <span class="SKwd">throw</span></span></code>
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
<td class="codetype"><span class="SCde">tmHeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmAscent</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDescent</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmInternalLeading</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmExternalLeading</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmAveCharWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmMaxCharWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmWeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmOverhang</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDigitizedAspectX</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDigitizedAspectY</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmFirstChar</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmLastChar</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDefaultChar</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmBreakChar</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmItalic</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmUnderlined</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmStruckOut</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmPitchAndFamily</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmCharSet</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
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
<td class="codetype"><span class="SCde">tmHeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmAscent</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDescent</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmInternalLeading</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmExternalLeading</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmAveCharWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmMaxCharWidth</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmWeight</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmOverhang</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDigitizedAspectX</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDigitizedAspectY</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmFirstChar</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmLastChar</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmDefaultChar</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmBreakChar</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmItalic</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmUnderlined</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmStruckOut</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmPitchAndFamily</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">tmCharSet</span></td>
<td class="codetype"><span class="STpe">u8</span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">TextOutA</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, lpString: <span class="SCst">Win32</span>.<span class="SCst">LPCSTR</span>, c: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">BOOL</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">TextOutW</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, lpString: <span class="SCst">Win32</span>.<span class="SCst">LPCWSTR</span>, c: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">BOOL</span></span></code>
</div>
</div>
</div>
</div>
</body>
</html>
