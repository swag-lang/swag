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
        width:      650px;
        height:     100vh;
    }
    .right {
        display:    block;
        overflow-y: scroll;
        width:      100%;
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
<h1>Module gdi32</h1>
<h2>Structs</h2>
<h3></h3>
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
<h2>Constants</h2>
<h3></h3>
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
<h2>Functions</h2>
<h3></h3>
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
<div class="right">
<div class="page">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.22.0)</blockquote>
<h1>Overview</h1>
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
<td id="Gdi32_ANSI_CHARSET" class="tdname">
ANSI_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_ANSI_FIXED_FONT" class="tdname">
ANSI_FIXED_FONT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_ANSI_VAR_FONT" class="tdname">
ANSI_VAR_FONT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_ARABIC_CHARSET" class="tdname">
ARABIC_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BALTIC_CHARSET" class="tdname">
BALTIC_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BI_BITFIELDS" class="tdname">
BI_BITFIELDS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BI_JPEG" class="tdname">
BI_JPEG</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BI_PNG" class="tdname">
BI_PNG</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BI_RGB" class="tdname">
BI_RGB</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BI_RLE4" class="tdname">
BI_RLE4</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BI_RLE8" class="tdname">
BI_RLE8</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BKMODE_LAST" class="tdname">
BKMODE_LAST</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BLACKNESS" class="tdname">
BLACKNESS</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = BLACK. </p>
</td>
</tr>
<tr>
<td id="Gdi32_BLACK_BRUSH" class="tdname">
BLACK_BRUSH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_BLACK_PEN" class="tdname">
BLACK_PEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_CAPTUREBLT" class="tdname">
CAPTUREBLT</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>Include layered windows. </p>
</td>
</tr>
<tr>
<td id="Gdi32_CHINESEBIG5_CHARSET" class="tdname">
CHINESEBIG5_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_DEFAULT_CHARSET" class="tdname">
DEFAULT_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_DEFAULT_PALETTE" class="tdname">
DEFAULT_PALETTE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_DEVICE_DEFAULT_FONT" class="tdname">
DEVICE_DEFAULT_FONT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_DEVICE_FONTTYPE" class="tdname">
DEVICE_FONTTYPE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_DIB_PAL_COLORS" class="tdname">
DIB_PAL_COLORS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_DIB_RGB_COLORS" class="tdname">
DIB_RGB_COLORS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_DKGRAY_BRUSH" class="tdname">
DKGRAY_BRUSH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_DSTINVERT" class="tdname">
DSTINVERT</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = (NOT dest). </p>
</td>
</tr>
<tr>
<td id="Gdi32_EASTEUROPE_CHARSET" class="tdname">
EASTEUROPE_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_BLACK" class="tdname">
FW_BLACK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_BOLD" class="tdname">
FW_BOLD</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_DEMIBOLD" class="tdname">
FW_DEMIBOLD</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_DONTCARE" class="tdname">
FW_DONTCARE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_EXTRABOLD" class="tdname">
FW_EXTRABOLD</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_EXTRALIGHT" class="tdname">
FW_EXTRALIGHT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_HEAVY" class="tdname">
FW_HEAVY</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_LIGHT" class="tdname">
FW_LIGHT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_MEDIUM" class="tdname">
FW_MEDIUM</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_NORMAL" class="tdname">
FW_NORMAL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_REGULAR" class="tdname">
FW_REGULAR</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_SEMIBOLD" class="tdname">
FW_SEMIBOLD</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_THIN" class="tdname">
FW_THIN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_ULTRABOLD" class="tdname">
FW_ULTRABOLD</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_FW_ULTRALIGHT" class="tdname">
FW_ULTRALIGHT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_GB2312_CHARSET" class="tdname">
GB2312_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_GDI_ERROR" class="tdname">
GDI_ERROR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_GRAY_BRUSH" class="tdname">
GRAY_BRUSH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_GREEK_CHARSET" class="tdname">
GREEK_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_HANGEUL_CHARSET" class="tdname">
HANGEUL_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_HANGUL_CHARSET" class="tdname">
HANGUL_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_HEBREW_CHARSET" class="tdname">
HEBREW_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_HOLLOW_BRUSH" class="tdname">
HOLLOW_BRUSH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_JOHAB_CHARSET" class="tdname">
JOHAB_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_LF_FACESIZE" class="tdname">
LF_FACESIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_LF_FULLFACESIZE" class="tdname">
LF_FULLFACESIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_LTGRAY_BRUSH" class="tdname">
LTGRAY_BRUSH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_MAC_CHARSET" class="tdname">
MAC_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_MERGECOPY" class="tdname">
MERGECOPY</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = (source AND pattern). </p>
</td>
</tr>
<tr>
<td id="Gdi32_MERGEPAINT" class="tdname">
MERGEPAINT</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = (NOT source) OR dest. </p>
</td>
</tr>
<tr>
<td id="Gdi32_NOMIRRORBITMAP" class="tdname">
NOMIRRORBITMAP</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>Do not Mirror the bitmap in this call. </p>
</td>
</tr>
<tr>
<td id="Gdi32_NOTSRCCOPY" class="tdname">
NOTSRCCOPY</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = (NOT source). </p>
</td>
</tr>
<tr>
<td id="Gdi32_NOTSRCERASE" class="tdname">
NOTSRCERASE</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = (NOT src) AND (NOT dest). </p>
</td>
</tr>
<tr>
<td id="Gdi32_NULL_BRUSH" class="tdname">
NULL_BRUSH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_NULL_PEN" class="tdname">
NULL_PEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_OEM_CHARSET" class="tdname">
OEM_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_OEM_FIXED_FONT" class="tdname">
OEM_FIXED_FONT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_OPAQUE" class="tdname">
OPAQUE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PATCOPY" class="tdname">
PATCOPY</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = pattern. </p>
</td>
</tr>
<tr>
<td id="Gdi32_PATINVERT" class="tdname">
PATINVERT</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = pattern XOR dest. </p>
</td>
</tr>
<tr>
<td id="Gdi32_PATPAINT" class="tdname">
PATPAINT</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = DPSnoo. </p>
</td>
</tr>
<tr>
<td id="Gdi32_PFD_DEPTH_DONTCARE" class="tdname">
PFD_DEPTH_DONTCARE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_DIRECT3D_ACCELERATED" class="tdname">
PFD_DIRECT3D_ACCELERATED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_DOUBLEBUFFER" class="tdname">
PFD_DOUBLEBUFFER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_DOUBLEBUFFER_DONTCARE" class="tdname">
PFD_DOUBLEBUFFER_DONTCARE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_DRAW_TO_BITMAP" class="tdname">
PFD_DRAW_TO_BITMAP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_DRAW_TO_WINDOW" class="tdname">
PFD_DRAW_TO_WINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_GENERIC_ACCELERATED" class="tdname">
PFD_GENERIC_ACCELERATED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_GENERIC_FORMAT" class="tdname">
PFD_GENERIC_FORMAT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_MAIN_PLANE" class="tdname">
PFD_MAIN_PLANE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_NEED_PALETTE" class="tdname">
PFD_NEED_PALETTE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_NEED_SYSTEM_PALETTE" class="tdname">
PFD_NEED_SYSTEM_PALETTE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_OVERLAY_PLANE" class="tdname">
PFD_OVERLAY_PLANE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_STEREO" class="tdname">
PFD_STEREO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_STEREO_DONTCARE" class="tdname">
PFD_STEREO_DONTCARE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_COMPOSITION" class="tdname">
PFD_SUPPORT_COMPOSITION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_DIRECTDRAW" class="tdname">
PFD_SUPPORT_DIRECTDRAW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_GDI" class="tdname">
PFD_SUPPORT_GDI</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_SUPPORT_OPENGL" class="tdname">
PFD_SUPPORT_OPENGL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_SWAP_COPY" class="tdname">
PFD_SWAP_COPY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_SWAP_EXCHANGE" class="tdname">
PFD_SWAP_EXCHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_SWAP_LAYER_BUFFERS" class="tdname">
PFD_SWAP_LAYER_BUFFERS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_TYPE_COLORINDEX" class="tdname">
PFD_TYPE_COLORINDEX</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_TYPE_RGBA" class="tdname">
PFD_TYPE_RGBA</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PFD_UNDERLAY_PLANE" class="tdname">
PFD_UNDERLAY_PLANE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PS_ALTERNATE" class="tdname">
PS_ALTERNATE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PS_DASH" class="tdname">
PS_DASH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
<p>-------. </p>
</td>
</tr>
<tr>
<td id="Gdi32_PS_DASHDOT" class="tdname">
PS_DASHDOT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
<p>_._._._. </p>
</td>
</tr>
<tr>
<td id="Gdi32_PS_DASHDOTDOT" class="tdname">
PS_DASHDOTDOT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
<p>_.._.._. </p>
</td>
</tr>
<tr>
<td id="Gdi32_PS_DOT" class="tdname">
PS_DOT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
<p>....... </p>
</td>
</tr>
<tr>
<td id="Gdi32_PS_INSIDEFRAME" class="tdname">
PS_INSIDEFRAME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PS_NULL" class="tdname">
PS_NULL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PS_SOLID" class="tdname">
PS_SOLID</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PS_STYLE_MASK" class="tdname">
PS_STYLE_MASK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_PS_USERSTYLE" class="tdname">
PS_USERSTYLE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_RASTER_FONTTYPE" class="tdname">
RASTER_FONTTYPE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_RUSSIAN_CHARSET" class="tdname">
RUSSIAN_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_SHIFTJIS_CHARSET" class="tdname">
SHIFTJIS_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_SRCAND" class="tdname">
SRCAND</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = source AND dest. </p>
</td>
</tr>
<tr>
<td id="Gdi32_SRCCOPY" class="tdname">
SRCCOPY</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = source. </p>
</td>
</tr>
<tr>
<td id="Gdi32_SRCERASE" class="tdname">
SRCERASE</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = source AND (NOT dest ). </p>
</td>
</tr>
<tr>
<td id="Gdi32_SRCINVERT" class="tdname">
SRCINVERT</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = source XOR dest. </p>
</td>
</tr>
<tr>
<td id="Gdi32_SRCPAINT" class="tdname">
SRCPAINT</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = source OR dest. </p>
</td>
</tr>
<tr>
<td id="Gdi32_SYMBOL_CHARSET" class="tdname">
SYMBOL_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_SYSTEM_FIXED_FONT" class="tdname">
SYSTEM_FIXED_FONT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_SYSTEM_FONT" class="tdname">
SYSTEM_FONT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_THAI_CHARSET" class="tdname">
THAI_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_TRANSPARENT" class="tdname">
TRANSPARENT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_TRUETYPE_FONTTYPE" class="tdname">
TRUETYPE_FONTTYPE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_TURKISH_CHARSET" class="tdname">
TURKISH_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_VIETNAMESE_CHARSET" class="tdname">
VIETNAMESE_CHARSET</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_WHITENESS" class="tdname">
WHITENESS</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>dest = WHITE. </p>
</td>
</tr>
<tr>
<td id="Gdi32_WHITE_BRUSH" class="tdname">
WHITE_BRUSH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Gdi32_WHITE_PEN" class="tdname">
WHITE_PEN</td>
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
<span class="content" id="Gdi32_BITMAP"><span class="titletype">struct</span> <span class="titlelight">Gdi32.</span><span class="titlestrong">BITMAP</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/gdi32\src\gdi32.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="tdname">
bmType</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bmWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bmHeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bmWidthBytes</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bmPlanes</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bmBitsPixel</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bmBits</td>
<td class="tdtype">
Win32.LPVOID</td>
<td class="enumeration">
</td>
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
<td class="tdname">
bmiHeader</td>
<td class="tdtype">
<a href="#Gdi32_BITMAPINFOHEADER">Gdi32.BITMAPINFOHEADER</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bmiColors</td>
<td class="tdtype">
[1] Gdi32.RGBQUAD</td>
<td class="enumeration">
</td>
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
<td class="tdname">
biSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biHeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biPlanes</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biBitCount</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biCompression</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biSizeImage</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biXPelsPerMeter</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biYPelsPerMeter</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biClrUsed</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biClrImportant</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">BitBlt</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cx: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cy: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, hdcSrc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x1: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y1: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, rop: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ChoosePixelFormat</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, ppfd: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateCompatibleBitmap</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, cx: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cy: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateCompatibleDC</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateDIBSection</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, pbmi: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_BITMAPINFO">BITMAPINFO</a></span><span class="SyntaxCode">, usage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, ppvBits: **</span><span class="SyntaxType">void</span><span class="SyntaxCode">, hSection: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, offset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFontIndirectA</span><span class="SyntaxCode">(lplf: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_LOGFONTA">LOGFONTA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HFONT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFontIndirectW</span><span class="SyntaxCode">(lplf: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_LOGFONTW">LOGFONTW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HFONT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreatePen</span><span class="SyntaxCode">(iStyle: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HPEN</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateSolidBrush</span><span class="SyntaxCode">(color: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBRUSH</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteDC</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteObject</span><span class="SyntaxCode">(ho: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGDIOBJ</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DescribePixelFormat</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, iPixelFormat: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nBytes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, ppfd: *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
elfLogFont</td>
<td class="tdtype">
<a href="#Gdi32_LOGFONTA">Gdi32.LOGFONTA</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
elfFullName</td>
<td class="tdtype">
[64] Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
elfStyle</td>
<td class="tdtype">
[32] Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
elfScript</td>
<td class="tdtype">
[32] Win32.BYTE</td>
<td class="enumeration">
</td>
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
<td class="tdname">
elfLogFont</td>
<td class="tdtype">
<a href="#Gdi32_LOGFONTW">Gdi32.LOGFONTW</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
elfFullName</td>
<td class="tdtype">
[64] Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
elfStyle</td>
<td class="tdtype">
[32] Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
elfScript</td>
<td class="tdtype">
[32] Win32.WCHAR</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumFontFamiliesA</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lpLogfont: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpProc: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FONTENUMPROCA</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumFontFamiliesExA</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lpLogfont: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_LOGFONTA">LOGFONTA</a></span><span class="SyntaxCode">, lpProc: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FONTENUMPROCA</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumFontFamiliesExW</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lpLogfont: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_LOGFONTW">LOGFONTW</a></span><span class="SyntaxCode">, lpProc: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FONTENUMPROCW</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumFontFamiliesW</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lpLogfont: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpProc: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FONTENUMPROCW</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetBitmapBits</span><span class="SyntaxCode">(hbit: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode">, cb: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">, lpvBits: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetDIBits</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, hbm: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode">, start: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, cLines: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, lpvBits: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, lpbmi: *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_BITMAPINFO">BITMAPINFO</a></span><span class="SyntaxCode">, usage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFontData</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, dwTable: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwOffset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, pvBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, cjBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetObjectA</span><span class="SyntaxCode">(h: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pv: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetObjectA</span><span class="SyntaxCode">(h: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pv: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetObjectW</span><span class="SyntaxCode">(h: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pv: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetObjectW</span><span class="SyntaxCode">(h: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pv: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetPixel</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetStockObject</span><span class="SyntaxCode">(i: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGDIOBJ</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
lfHeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfEscapement</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfOrientation</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfWeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfItalic</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfUnderline</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfStrikeOut</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfCharSet</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfOutPrecision</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfClipPrecision</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfQuality</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfPitchAndFamily</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfFaceName</td>
<td class="tdtype">
[32] Win32.CHAR</td>
<td class="enumeration">
</td>
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
<td class="tdname">
lfHeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfEscapement</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfOrientation</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfWeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfItalic</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfUnderline</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfStrikeOut</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfCharSet</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfOutPrecision</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfClipPrecision</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfQuality</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfPitchAndFamily</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lfFaceName</td>
<td class="tdtype">
[32] Win32.WCHAR</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LineTo</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MoveTo</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
tmHeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmAscent</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDescent</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmInternalLeading</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmExternalLeading</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmAveCharWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmMaxCharWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmWeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmOverhang</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDigitizedAspectX</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDigitizedAspectY</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmFirstChar</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmLastChar</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDefaultChar</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmBreakChar</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmItalic</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmUnderlined</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmStruckOut</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmPitchAndFamily</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmCharSet</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ntmFlags</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ntmSizeEM</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ntmCellHeight</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ntmAvgWidth</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
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
<td class="tdname">
tmHeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmAscent</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDescent</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmInternalLeading</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmExternalLeading</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmAveCharWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmMaxCharWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmWeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmOverhang</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDigitizedAspectX</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDigitizedAspectY</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmFirstChar</td>
<td class="tdtype">
Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmLastChar</td>
<td class="tdtype">
Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDefaultChar</td>
<td class="tdtype">
Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmBreakChar</td>
<td class="tdtype">
Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmItalic</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmUnderlined</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmStruckOut</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmPitchAndFamily</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmCharSet</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ntmFlags</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ntmSizeEM</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ntmCellHeight</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ntmAvgWidth</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
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
<td class="tdname">
nSize</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
nVersion</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwFlags</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
iPixelType</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cColorBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cRedBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cRedShift</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cGreenBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cGreenShift</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cBlueBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cBlueShift</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAlphaBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAlphaShift</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAccumBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAccumRedBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAccumGreenBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAccumBlueBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAccumAlphaBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cDepthBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cStencilBits</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAuxBuffers</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
iLayerType</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bReserved</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwLayerMask</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwVisibleMask</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwDamageMask</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RGB</span><span class="SyntaxCode">(r: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
rgbBlue</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
rgbGreen</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
rgbRed</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
rgbReserved</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, left: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, right: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, bottom: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SelectObject</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, h: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGDIOBJ</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGDIOBJ</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetBitmapBits</span><span class="SyntaxCode">(hbit: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode">, cb: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpvBits: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetBkColor</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetBkMode</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, mode: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetPixelFormat</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, fmt: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, ppfd: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Gdi32_PIXELFORMATDESCRIPTOR">PIXELFORMATDESCRIPTOR</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetTextColor</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SwapBuffers</span><span class="SyntaxCode">(arg1: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
tmHeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmAscent</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDescent</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmInternalLeading</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmExternalLeading</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmAveCharWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmMaxCharWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmWeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmOverhang</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDigitizedAspectX</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDigitizedAspectY</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmFirstChar</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmLastChar</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDefaultChar</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmBreakChar</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmItalic</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmUnderlined</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmStruckOut</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmPitchAndFamily</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmCharSet</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
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
<td class="tdname">
tmHeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmAscent</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDescent</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmInternalLeading</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmExternalLeading</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmAveCharWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmMaxCharWidth</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmWeight</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmOverhang</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDigitizedAspectX</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDigitizedAspectY</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmFirstChar</td>
<td class="tdtype">
Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmLastChar</td>
<td class="tdtype">
Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmDefaultChar</td>
<td class="tdtype">
Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmBreakChar</td>
<td class="tdtype">
Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmItalic</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmUnderlined</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmStruckOut</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmPitchAndFamily</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
tmCharSet</td>
<td class="tdtype">
Win32.BYTE</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TextOutA</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TextOutW</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, c: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
</div>
</div>
</div>
</body>
</html>
