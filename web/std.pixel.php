<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module pixel</title>
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
<h4>image</h4>
<ul>
<li><a href="#Pixel_Image">Image</a></li>
<li><a href="#Pixel_RenderTarget">RenderTarget</a></li>
<li><a href="#Pixel_Texture">Texture</a></li>
</ul>
<h4>image/decode</h4>
<ul>
<li><a href="#Pixel_Bmp_Decoder">Bmp.Decoder</a></li>
<li><a href="#Pixel_DecodeOptions">DecodeOptions</a></li>
<li><a href="#Pixel_Gif_Decoder">Gif.Decoder</a></li>
<li><a href="#Pixel_Bmp_Header">Header</a></li>
<li><a href="#Pixel_Jpg_Decoder">Jpg.Decoder</a></li>
<li><a href="#Pixel_Png_Decoder">Png.Decoder</a></li>
<li><a href="#Pixel_Tga_Decoder">Tga.Decoder</a></li>
</ul>
<h4>image/encode</h4>
<ul>
<li><a href="#Pixel_Bmp_Encoder">Bmp.Encoder</a></li>
<li><a href="#Pixel_Jpg_EncodeOptions">Jpg.EncodeOptions</a></li>
<li><a href="#Pixel_Jpg_Encoder">Jpg.Encoder</a></li>
<li><a href="#Pixel_Png_EncodeOptions">Png.EncodeOptions</a></li>
<li><a href="#Pixel_Png_Encoder">Png.Encoder</a></li>
<li><a href="#Pixel_Tga_Encoder">Tga.Encoder</a></li>
</ul>
<h4>painter</h4>
<ul>
<li><a href="#Pixel_ColorMask">ColorMask</a></li>
<li><a href="#Pixel_Command">Command</a></li>
<li><a href="#Pixel_PaintState">PaintState</a></li>
<li><a href="#Pixel_Painter">Painter</a></li>
<li><a href="#Pixel_RichStringFormat">RichStringFormat</a></li>
<li><a href="#Pixel_StringMetrics">StringMetrics</a></li>
<li><a href="#Pixel_StringPainter">StringPainter</a></li>
<li><a href="#Pixel_VertexLayout">VertexLayout</a></li>
</ul>
<h4>poly</h4>
<ul>
<li><a href="#Pixel_Poly2Tri_Edge">Edge</a></li>
<li><a href="#Pixel_Clipper_IntPoint">IntPoint</a></li>
<li><a href="#Pixel_Poly2Tri_Point">Point</a></li>
<li><a href="#Pixel_Polygon">Polygon</a></li>
<li><a href="#Pixel_Poly2Tri_Tesselate">Tesselate</a></li>
<li><a href="#Pixel_Clipper_Transform">Transform</a></li>
<li><a href="#Pixel_Poly2Tri_Triangle">Triangle</a></li>
</ul>
<h4>render/ogl</h4>
<ul>
<li><a href="#Pixel_NativeRenderOgl">NativeRenderOgl</a></li>
<li><a href="#Pixel_RenderOgl">RenderOgl</a></li>
<li><a href="#Pixel_RenderingContext">RenderingContext</a></li>
<li><a href="#Pixel_ShaderAA">ShaderAA</a></li>
<li><a href="#Pixel_ShaderBase">ShaderBase</a></li>
<li><a href="#Pixel_ShaderBlur">ShaderBlur</a></li>
<li><a href="#Pixel_ShaderSimple">ShaderSimple</a></li>
</ul>
<h4>text</h4>
<ul>
<li><a href="#Pixel_Font">Font</a></li>
<li><a href="#Pixel_FontFamily">FontFamily</a></li>
<li><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></li>
<li><a href="#Pixel_FontFamilyStyleEnumerate">FontFamilyStyleEnumerate</a></li>
<li><a href="#Pixel_GlyphAtlas">GlyphAtlas</a></li>
<li><a href="#Pixel_GlyphDesc">GlyphDesc</a></li>
<li><a href="#Pixel_RichChunk">RichChunk</a></li>
<li><a href="#Pixel_RichString">RichString</a></li>
<li><a href="#Pixel_TypeFace">TypeFace</a></li>
</ul>
<h4>types</h4>
<ul>
<li><a href="#Pixel_Brush">Brush</a></li>
<li><a href="#Pixel_Color">Color</a></li>
<li><a href="#Pixel_Element">Element</a></li>
<li><a href="#Pixel_LinePath">LinePath</a></li>
<li><a href="#Pixel_LinePathList">LinePathList</a></li>
<li><a href="#Pixel_Pen">Pen</a></li>
</ul>
<h3>Interfaces</h3>
<h4>image/decode</h4>
<ul>
<li><a href="#Pixel_IImageDecoder">IImageDecoder</a></li>
</ul>
<h4>image/encode</h4>
<ul>
<li><a href="#Pixel_IImageEncoder">IImageEncoder</a></li>
</ul>
<h3>Enums</h3>
<h4>image</h4>
<ul>
<li><a href="#Pixel_HatchStyle">HatchStyle</a></li>
<li><a href="#Pixel_InterpolationMode">InterpolationMode</a></li>
<li><a href="#Pixel_PixelFormat">PixelFormat</a></li>
<li><a href="#Pixel_UVMode">UVMode</a></li>
</ul>
<h4>image/filter</h4>
<ul>
<li><a href="#Pixel_Image_ChannelMode">ChannelMode</a></li>
<li><a href="#Pixel_Image_ChannelValueMode">ChannelValueMode</a></li>
<li><a href="#Pixel_Image_FillHslType">FillHslType</a></li>
<li><a href="#Pixel_Image_Kernel">Kernel</a></li>
</ul>
<h4>image/transform</h4>
<ul>
<li><a href="#Pixel_Image_MixMode">MixMode</a></li>
<li><a href="#Pixel_Image_ResizeMode">ResizeMode</a></li>
<li><a href="#Pixel_Image_TurnAngle">TurnAngle</a></li>
</ul>
<h4>painter</h4>
<ul>
<li><a href="#Pixel_BlendingMode">BlendingMode</a></li>
<li><a href="#Pixel_ClippingMode">ClippingMode</a></li>
<li><a href="#Pixel_CommandId">CommandId</a></li>
<li><a href="#Pixel_DefaultShaderId">DefaultShaderId</a></li>
<li><a href="#Pixel_DrawPathListMode">DrawPathListMode</a></li>
<li><a href="#Pixel_PaintParams">PaintParams</a></li>
<li><a href="#Pixel_PaintQuality">PaintQuality</a></li>
<li><a href="#Pixel_StringFormatFlags">StringFormatFlags</a></li>
<li><a href="#Pixel_StringHorzAlignment">StringHorzAlignment</a></li>
<li><a href="#Pixel_StringVertAlignment">StringVertAlignment</a></li>
<li><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></li>
</ul>
<h4>poly</h4>
<ul>
<li><a href="#Pixel_Clipper_ClipType">ClipType</a></li>
<li><a href="#Pixel_Clipper_InitOptions">InitOptions</a></li>
<li><a href="#Pixel_Clipper_JoinType">JoinType</a></li>
<li><a href="#Pixel_Clipper_PolyFillType">PolyFillType</a></li>
<li><a href="#Pixel_Clipper_PolyType">PolyType</a></li>
</ul>
<h4>text</h4>
<ul>
<li><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></li>
</ul>
<h4>types</h4>
<ul>
<li><a href="#Pixel_Argb">Argb</a></li>
<li><a href="#Pixel_BorderPos">BorderPos</a></li>
<li><a href="#Pixel_BrushType">BrushType</a></li>
<li><a href="#Pixel_DashCapStyle">DashCapStyle</a></li>
<li><a href="#Pixel_DashStyle">DashStyle</a></li>
<li><a href="#Pixel_ElementType">ElementType</a></li>
<li><a href="#Pixel_JoinStyle">JoinStyle</a></li>
<li><a href="#Pixel_LineCapStyle">LineCapStyle</a></li>
</ul>
<h3>Constants</h3>
<h4>image/decode</h4>
<ul>
<li><a href="#Pixel_Bmp_BI_BITFIELDS">BI_BITFIELDS</a></li>
<li><a href="#Pixel_Bmp_BI_RGB">BI_RGB</a></li>
<li><a href="#Pixel_Bmp_BI_RLE4">BI_RLE4</a></li>
<li><a href="#Pixel_Bmp_BI_RLE8">BI_RLE8</a></li>
</ul>
<h4>painter</h4>
<ul>
<li><a href="#Pixel_MaxAAEdge">MaxAAEdge</a></li>
</ul>
<h4>poly</h4>
<ul>
<li><a href="#Pixel_ClipperScaleCoords">ClipperScaleCoords</a></li>
</ul>
<h4>render/ogl</h4>
<ul>
<li><a href="#Pixel_InvalidRenderTargetHandle">InvalidRenderTargetHandle</a></li>
<li><a href="#Pixel_InvalidRenderTargetSurfaceHandle">InvalidRenderTargetSurfaceHandle</a></li>
<li><a href="#Pixel_InvalidShaderHandle">InvalidShaderHandle</a></li>
<li><a href="#Pixel_InvalidShaderParamHandle">InvalidShaderParamHandle</a></li>
<li><a href="#Pixel_InvalidTextureHandle">InvalidTextureHandle</a></li>
</ul>
<h3>Type Aliases</h3>
<h4>poly</h4>
<ul>
<li><a href="#Pixel_Clipper_CInt">CInt</a></li>
</ul>
<h4>render/ogl</h4>
<ul>
<li><a href="#Pixel_RenderTargetHandle">RenderTargetHandle</a></li>
<li><a href="#Pixel_RenderTargetSurfaceHandle">RenderTargetSurfaceHandle</a></li>
<li><a href="#Pixel_ShaderHandle">ShaderHandle</a></li>
<li><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></li>
<li><a href="#Pixel_TextureHandle">TextureHandle</a></li>
</ul>
<h3>Functions</h3>
<h4>image</h4>
<ul>
<li><a href="#Pixel_Image_allocPixels">Image.allocPixels</a></li>
<li><a href="#Pixel_Image_allocWorkingBuffer">Image.allocWorkingBuffer</a></li>
<li><a href="#Pixel_Image_applyWorkingBuffer">Image.applyWorkingBuffer</a></li>
<li><a href="#Pixel_Image_clear">Image.clear</a></li>
<li><a href="#Pixel_Image_copyPixel">Image.copyPixel</a></li>
<li><a href="#Pixel_Image_create">Image.create</a></li>
<li><a href="#Pixel_Image_freeWorkingBuffer">Image.freeWorkingBuffer</a></li>
<li><a href="#Pixel_Image_from">Image.from</a></li>
<li><a href="#Pixel_Image_getPixelColor">Image.getPixelColor</a></li>
<li><a href="#Pixel_Image_init">Image.init</a></li>
<li><a href="#Pixel_Image_isValid">Image.isValid</a></li>
<li><a href="#Pixel_Image_opVisit">Image.opVisit</a></li>
<li><a href="#Pixel_Image_release">Image.release</a></li>
<li><a href="#Pixel_Image_setPixelRGBA8">Image.setPixelRGBA8</a></li>
<li><a href="#Pixel_Image_toHICON">Image.toHICON</a></li>
<li><a href="#Pixel_Image_visitPixels">Image.visitPixels</a></li>
<li><a href="#Pixel_PixelFormat_bitDepth">PixelFormat.bitDepth</a></li>
<li><a href="#Pixel_PixelFormat_bpp">PixelFormat.bpp</a></li>
<li><a href="#Pixel_PixelFormat_channels">PixelFormat.channels</a></li>
<li><a href="#Pixel_PixelFormat_hasAlpha">PixelFormat.hasAlpha</a></li>
<li><a href="#Pixel_Texture_getByteSize">Texture.getByteSize</a></li>
<li><a href="#Pixel_Texture_isValid">Texture.isValid</a></li>
</ul>
<h4>image/decode</h4>
<ul>
<li><a href="#Pixel_Bmp_Decoder_IImageDecoder_canDecode">Bmp.Decoder.IImageDecoder.canDecode</a></li>
<li><a href="#Pixel_Bmp_Decoder_IImageDecoder_decode">Bmp.Decoder.IImageDecoder.decode</a></li>
<li><a href="#Pixel_Gif_Decoder_getFrame">Decoder.getFrame</a></li>
<li><a href="#Pixel_Gif_Decoder_getNumFrames">Decoder.getNumFrames</a></li>
<li><a href="#Pixel_Gif_Decoder_init">Decoder.init</a></li>
<li><a href="#Pixel_Gif_Decoder_nextFrame">Decoder.nextFrame</a></li>
<li><a href="#Pixel_Gif_Decoder_rewind">Decoder.rewind</a></li>
<li><a href="#Pixel_Gif_Decoder_IImageDecoder_canDecode">Gif.Decoder.IImageDecoder.canDecode</a></li>
<li><a href="#Pixel_Gif_Decoder_IImageDecoder_decode">Gif.Decoder.IImageDecoder.decode</a></li>
<li><a href="#Pixel_Image_addDecoder">Image.addDecoder</a></li>
<li><a href="#Pixel_Image_canLoad">Image.canLoad</a></li>
<li><a href="#Pixel_Image_decode">Image.decode</a></li>
<li><a href="#Pixel_Image_load">Image.load</a></li>
<li><a href="#Pixel_Jpg_Decoder_IImageDecoder_canDecode">Jpg.Decoder.IImageDecoder.canDecode</a></li>
<li><a href="#Pixel_Jpg_Decoder_IImageDecoder_decode">Jpg.Decoder.IImageDecoder.decode</a></li>
<li><a href="#Pixel_Png_Decoder_IImageDecoder_canDecode">Png.Decoder.IImageDecoder.canDecode</a></li>
<li><a href="#Pixel_Png_Decoder_IImageDecoder_decode">Png.Decoder.IImageDecoder.decode</a></li>
<li><a href="#Pixel_Tga_Decoder_IImageDecoder_canDecode">Tga.Decoder.IImageDecoder.canDecode</a></li>
<li><a href="#Pixel_Tga_Decoder_IImageDecoder_decode">Tga.Decoder.IImageDecoder.decode</a></li>
</ul>
<h4>image/encode</h4>
<ul>
<li><a href="#Pixel_Bmp_Encoder_IImageEncoder_canEncode">Bmp.Encoder.IImageEncoder.canEncode</a></li>
<li><a href="#Pixel_Bmp_Encoder_IImageEncoder_encode">Bmp.Encoder.IImageEncoder.encode</a></li>
<li><a href="#Pixel_Image_addEncoder">Image.addEncoder</a></li>
<li><a href="#Pixel_Image_canSave">Image.canSave</a></li>
<li><a href="#Pixel_Image_save">Image.save</a></li>
<li><a href="#Pixel_Jpg_Encoder_IImageEncoder_canEncode">Jpg.Encoder.IImageEncoder.canEncode</a></li>
<li><a href="#Pixel_Jpg_Encoder_IImageEncoder_encode">Jpg.Encoder.IImageEncoder.encode</a></li>
<li><a href="#Pixel_Png_Encoder_IImageEncoder_canEncode">Png.Encoder.IImageEncoder.canEncode</a></li>
<li><a href="#Pixel_Png_Encoder_IImageEncoder_encode">Png.Encoder.IImageEncoder.encode</a></li>
<li><a href="#Pixel_Tga_Encoder_IImageEncoder_canEncode">Tga.Encoder.IImageEncoder.canEncode</a></li>
<li><a href="#Pixel_Tga_Encoder_IImageEncoder_encode">Tga.Encoder.IImageEncoder.encode</a></li>
</ul>
<h4>image/filter</h4>
<ul>
<li><a href="#Pixel_Image_applyKernel">Image.applyKernel</a></li>
<li><a href="#Pixel_Image_colorize">Image.colorize</a></li>
<li><a href="#Pixel_Image_contrast">Image.contrast</a></li>
<li><a href="#Pixel_Image_fade">Image.fade</a></li>
<li><a href="#Pixel_Image_fill">Image.fill</a></li>
<li><a href="#Pixel_Image_fillGradient2">Image.fillGradient2</a></li>
<li><a href="#Pixel_Image_fillGradient4">Image.fillGradient4</a></li>
<li><a href="#Pixel_Image_fillHsl">Image.fillHsl</a></li>
<li><a href="#Pixel_Image_gamma">Image.gamma</a></li>
<li><a href="#Pixel_Image_grayScale">Image.grayScale</a></li>
<li><a href="#Pixel_Image_invert">Image.invert</a></li>
<li><a href="#Pixel_Image_lightness">Image.lightness</a></li>
<li><a href="#Pixel_Image_setAlpha">Image.setAlpha</a></li>
<li><a href="#Pixel_Image_setChannel">Image.setChannel</a></li>
<li><a href="#Pixel_Image_toApplyKernel">Image.toApplyKernel</a></li>
</ul>
<h4>image/transform</h4>
<ul>
<li><a href="#Pixel_Image_crop">Image.crop</a></li>
<li><a href="#Pixel_Image_flip">Image.flip</a></li>
<li><a href="#Pixel_Image_halfSize">Image.halfSize</a></li>
<li><a href="#Pixel_Image_mirror">Image.mirror</a></li>
<li><a href="#Pixel_Image_mix">Image.mix</a></li>
<li><a href="#Pixel_Image_resize">Image.resize</a></li>
<li><a href="#Pixel_Image_setPixelFormat">Image.setPixelFormat</a></li>
<li><a href="#Pixel_Image_toCrop">Image.toCrop</a></li>
<li><a href="#Pixel_Image_toFlip">Image.toFlip</a></li>
<li><a href="#Pixel_Image_toHalfSize">Image.toHalfSize</a></li>
<li><a href="#Pixel_Image_toMirror">Image.toMirror</a></li>
<li><a href="#Pixel_Image_toMix">Image.toMix</a></li>
<li><a href="#Pixel_Image_toResize">Image.toResize</a></li>
<li><a href="#Pixel_Image_toSetPixelFormat">Image.toSetPixelFormat</a></li>
<li><a href="#Pixel_Image_toTurn">Image.toTurn</a></li>
<li><a href="#Pixel_Image_turn">Image.turn</a></li>
</ul>
<h4>painter</h4>
<ul>
<li><a href="#Pixel_Painter_addParams">Painter.addParams</a></li>
<li><a href="#Pixel_Painter_begin">Painter.begin</a></li>
<li><a href="#Pixel_Painter_bindRenderTarget">Painter.bindRenderTarget</a></li>
<li><a href="#Pixel_Painter_capStyleExtent">Painter.capStyleExtent</a></li>
<li><a href="#Pixel_Painter_clear">Painter.clear</a></li>
<li><a href="#Pixel_Painter_computeLayout">Painter.computeLayout</a></li>
<li><a href="#Pixel_Painter_drawArc">Painter.drawArc</a></li>
<li><a href="#Pixel_Painter_drawCircle">Painter.drawCircle</a></li>
<li><a href="#Pixel_Painter_drawEllipse">Painter.drawEllipse</a></li>
<li><a href="#Pixel_Painter_drawLine">Painter.drawLine</a></li>
<li><a href="#Pixel_Painter_drawPath">Painter.drawPath</a></li>
<li><a href="#Pixel_Painter_drawRect">Painter.drawRect</a></li>
<li><a href="#Pixel_Painter_drawRichString">Painter.drawRichString</a></li>
<li><a href="#Pixel_Painter_drawRoundRect">Painter.drawRoundRect</a></li>
<li><a href="#Pixel_Painter_drawString">Painter.drawString</a></li>
<li><a href="#Pixel_Painter_drawStringCenter">Painter.drawStringCenter</a></li>
<li><a href="#Pixel_Painter_drawTexture">Painter.drawTexture</a></li>
<li><a href="#Pixel_Painter_end">Painter.end</a></li>
<li><a href="#Pixel_Painter_endClippingRegion">Painter.endClippingRegion</a></li>
<li><a href="#Pixel_Painter_fillCircle">Painter.fillCircle</a></li>
<li><a href="#Pixel_Painter_fillEllipse">Painter.fillEllipse</a></li>
<li><a href="#Pixel_Painter_fillPath">Painter.fillPath</a></li>
<li><a href="#Pixel_Painter_fillPolygon">Painter.fillPolygon</a></li>
<li><a href="#Pixel_Painter_fillRect">Painter.fillRect</a></li>
<li><a href="#Pixel_Painter_fillRoundRect">Painter.fillRoundRect</a></li>
<li><a href="#Pixel_Painter_getClippingRect">Painter.getClippingRect</a></li>
<li><a href="#Pixel_Painter_getGlyphs">Painter.getGlyphs</a></li>
<li><a href="#Pixel_Painter_getParams">Painter.getParams</a></li>
<li><a href="#Pixel_Painter_getQuality">Painter.getQuality</a></li>
<li><a href="#Pixel_Painter_getTransform">Painter.getTransform</a></li>
<li><a href="#Pixel_Painter_isEmptyClippingRect">Painter.isEmptyClippingRect</a></li>
<li><a href="#Pixel_Painter_measureRune">Painter.measureRune</a></li>
<li><a href="#Pixel_Painter_measureString">Painter.measureString</a></li>
<li><a href="#Pixel_Painter_popClippingRect">Painter.popClippingRect</a></li>
<li><a href="#Pixel_Painter_popState">Painter.popState</a></li>
<li><a href="#Pixel_Painter_popTransform">Painter.popTransform</a></li>
<li><a href="#Pixel_Painter_pushClippingRect">Painter.pushClippingRect</a></li>
<li><a href="#Pixel_Painter_pushClippingSurfaceRect">Painter.pushClippingSurfaceRect</a></li>
<li><a href="#Pixel_Painter_pushState">Painter.pushState</a></li>
<li><a href="#Pixel_Painter_pushTransform">Painter.pushTransform</a></li>
<li><a href="#Pixel_Painter_releaseRenderTarget">Painter.releaseRenderTarget</a></li>
<li><a href="#Pixel_Painter_removeParams">Painter.removeParams</a></li>
<li><a href="#Pixel_Painter_resetClippingRect">Painter.resetClippingRect</a></li>
<li><a href="#Pixel_Painter_resetClippingRegion">Painter.resetClippingRegion</a></li>
<li><a href="#Pixel_Painter_resetState">Painter.resetState</a></li>
<li><a href="#Pixel_Painter_resetTransform">Painter.resetTransform</a></li>
<li><a href="#Pixel_Painter_rotateTransform">Painter.rotateTransform</a></li>
<li><a href="#Pixel_Painter_rotateTransformInPlace">Painter.rotateTransformInPlace</a></li>
<li><a href="#Pixel_Painter_scaleTransform">Painter.scaleTransform</a></li>
<li><a href="#Pixel_Painter_setAntialiased">Painter.setAntialiased</a></li>
<li><a href="#Pixel_Painter_setBlendingMode">Painter.setBlendingMode</a></li>
<li><a href="#Pixel_Painter_setClippingRect">Painter.setClippingRect</a></li>
<li><a href="#Pixel_Painter_setClippingRegionMode">Painter.setClippingRegionMode</a></li>
<li><a href="#Pixel_Painter_setColorMask">Painter.setColorMask</a></li>
<li><a href="#Pixel_Painter_setColorMaskAlpha">Painter.setColorMaskAlpha</a></li>
<li><a href="#Pixel_Painter_setColorMaskColor">Painter.setColorMaskColor</a></li>
<li><a href="#Pixel_Painter_setColorMaskFull">Painter.setColorMaskFull</a></li>
<li><a href="#Pixel_Painter_setInterpolationMode">Painter.setInterpolationMode</a></li>
<li><a href="#Pixel_Painter_setParams">Painter.setParams</a></li>
<li><a href="#Pixel_Painter_setQuality">Painter.setQuality</a></li>
<li><a href="#Pixel_Painter_setShader">Painter.setShader</a></li>
<li><a href="#Pixel_Painter_setShaderParam">Painter.setShaderParam</a></li>
<li><a href="#Pixel_Painter_setState">Painter.setState</a></li>
<li><a href="#Pixel_Painter_setTransform">Painter.setTransform</a></li>
<li><a href="#Pixel_Painter_startClippingRegion">Painter.startClippingRegion</a></li>
<li><a href="#Pixel_Painter_translateTransform">Painter.translateTransform</a></li>
<li><a href="#Pixel_Painter_unbindRenderTarget">Painter.unbindRenderTarget</a></li>
<li><a href="#Pixel_StringPainter_color">StringPainter.color</a></li>
<li><a href="#Pixel_StringPainter_draw">StringPainter.draw</a></li>
<li><a href="#Pixel_StringPainter_flags">StringPainter.flags</a></li>
<li><a href="#Pixel_StringPainter_font">StringPainter.font</a></li>
<li><a href="#Pixel_StringPainter_horzAlign">StringPainter.horzAlign</a></li>
<li><a href="#Pixel_StringPainter_vertAlign">StringPainter.vertAlign</a></li>
</ul>
<h4>poly</h4>
<ul>
<li><a href="#Pixel_Clipper_IntPoint_opEquals">IntPoint.opEquals</a></li>
<li><a href="#Pixel_Polygon_add">Polygon.add</a></li>
<li><a href="#Pixel_Polygon_clean">Polygon.clean</a></li>
<li><a href="#Pixel_Polygon_clear">Polygon.clear</a></li>
<li><a href="#Pixel_Polygon_compute">Polygon.compute</a></li>
<li><a href="#Pixel_Polygon_getClean">Polygon.getClean</a></li>
<li><a href="#Pixel_Polygon_getOffset">Polygon.getOffset</a></li>
<li><a href="#Pixel_Polygon_getPoint">Polygon.getPoint</a></li>
<li><a href="#Pixel_Polygon_normalizeDist">Polygon.normalizeDist</a></li>
<li><a href="#Pixel_Polygon_offset">Polygon.offset</a></li>
<li><a href="#Pixel_Poly2Tri_Tesselate_addPoint">Tesselate.addPoint</a></li>
<li><a href="#Pixel_Poly2Tri_Tesselate_clear">Tesselate.clear</a></li>
<li><a href="#Pixel_Poly2Tri_Tesselate_endPolyLine">Tesselate.endPolyLine</a></li>
<li><a href="#Pixel_Poly2Tri_Tesselate_getTriangles">Tesselate.getTriangles</a></li>
<li><a href="#Pixel_Poly2Tri_Tesselate_startPolyLine">Tesselate.startPolyLine</a></li>
<li><a href="#Pixel_Poly2Tri_Tesselate_triangulate">Tesselate.triangulate</a></li>
</ul>
<h4>render/ogl</h4>
<ul>
<li><a href="#Pixel_NativeRenderOgl_createContext">NativeRenderOgl.createContext</a></li>
<li><a href="#Pixel_NativeRenderOgl_dropContext">NativeRenderOgl.dropContext</a></li>
<li><a href="#Pixel_NativeRenderOgl_setCurrentContext">NativeRenderOgl.setCurrentContext</a></li>
<li><a href="#Pixel_NativeRenderOgl_swapBuffers">NativeRenderOgl.swapBuffers</a></li>
<li><a href="#Pixel_RenderOgl_addImage">RenderOgl.addImage</a></li>
<li><a href="#Pixel_RenderOgl_begin">RenderOgl.begin</a></li>
<li><a href="#Pixel_RenderOgl_bindRenderTarget">RenderOgl.bindRenderTarget</a></li>
<li><a href="#Pixel_RenderOgl_clear">RenderOgl.clear</a></li>
<li><a href="#Pixel_RenderOgl_computeMatrices">RenderOgl.computeMatrices</a></li>
<li><a href="#Pixel_RenderOgl_createRenderTarget">RenderOgl.createRenderTarget</a></li>
<li><a href="#Pixel_RenderOgl_createShader">RenderOgl.createShader</a></li>
<li><a href="#Pixel_RenderOgl_deleteRenderTarget">RenderOgl.deleteRenderTarget</a></li>
<li><a href="#Pixel_RenderOgl_draw">RenderOgl.draw</a></li>
<li><a href="#Pixel_RenderOgl_end">RenderOgl.end</a></li>
<li><a href="#Pixel_RenderOgl_getRenderTarget">RenderOgl.getRenderTarget</a></li>
<li><a href="#Pixel_RenderOgl_getTargetSize">RenderOgl.getTargetSize</a></li>
<li><a href="#Pixel_RenderOgl_getTexturePixels">RenderOgl.getTexturePixels</a></li>
<li><a href="#Pixel_RenderOgl_init">RenderOgl.init</a></li>
<li><a href="#Pixel_RenderOgl_pixelFormatToNative">RenderOgl.pixelFormatToNative</a></li>
<li><a href="#Pixel_RenderOgl_releaseRenderTarget">RenderOgl.releaseRenderTarget</a></li>
<li><a href="#Pixel_RenderOgl_removeTexture">RenderOgl.removeTexture</a></li>
<li><a href="#Pixel_RenderOgl_resetScissorRect">RenderOgl.resetScissorRect</a></li>
<li><a href="#Pixel_RenderOgl_resetTransform">RenderOgl.resetTransform</a></li>
<li><a href="#Pixel_RenderOgl_rotateTransform">RenderOgl.rotateTransform</a></li>
<li><a href="#Pixel_RenderOgl_scaleTransform">RenderOgl.scaleTransform</a></li>
<li><a href="#Pixel_RenderOgl_setCurrentContext">RenderOgl.setCurrentContext</a></li>
<li><a href="#Pixel_RenderOgl_setScissorRect">RenderOgl.setScissorRect</a></li>
<li><a href="#Pixel_RenderOgl_setShader">RenderOgl.setShader</a></li>
<li><a href="#Pixel_RenderOgl_translateTransform">RenderOgl.translateTransform</a></li>
<li><a href="#Pixel_RenderOgl_unbindRenderTarget">RenderOgl.unbindRenderTarget</a></li>
<li><a href="#Pixel_RenderOgl_unbindRenderTargetToImage">RenderOgl.unbindRenderTargetToImage</a></li>
<li><a href="#Pixel_RenderOgl_updateTexture">RenderOgl.updateTexture</a></li>
</ul>
<h4>text</h4>
<ul>
<li><a href="#Pixel_Font_create">Font.create</a></li>
<li><a href="#Pixel_Font_getGlyphDesc">Font.getGlyphDesc</a></li>
<li><a href="#Pixel_Font_release">Font.release</a></li>
<li><a href="#Pixel_FontFamily_createTypeFace">FontFamily.createTypeFace</a></li>
<li><a href="#Pixel_FontFamily_enumerateFromOs">FontFamily.enumerateFromOs</a></li>
<li><a href="#Pixel_FontFamily_getFont">FontFamily.getFont</a></li>
<li><a href="#Pixel_FontFamily_getFromOs">FontFamily.getFromOs</a></li>
<li><a href="#Pixel_FontFamily_setFont">FontFamily.setFont</a></li>
<li><a href="#Pixel_RichString_compute">RichString.compute</a></li>
<li><a href="#Pixel_RichString_getNaked">RichString.getNaked</a></li>
<li><a href="#Pixel_RichString_invalidate">RichString.invalidate</a></li>
<li><a href="#Pixel_RichString_isEmpty">RichString.isEmpty</a></li>
<li><a href="#Pixel_RichString_opAffect">RichString.opAffect</a></li>
<li><a href="#Pixel_RichString_opCount">RichString.opCount</a></li>
<li><a href="#Pixel_RichString_opEquals">RichString.opEquals</a></li>
<li><a href="#Pixel_RichString_set">RichString.set</a></li>
<li><a href="#Pixel_TypeFace_create">TypeFace.create</a></li>
<li><a href="#Pixel_TypeFace_createFromHFONT">TypeFace.createFromHFONT</a></li>
<li><a href="#Pixel_TypeFace_getFamilyName">TypeFace.getFamilyName</a></li>
<li><a href="#Pixel_TypeFace_getRuneOutline">TypeFace.getRuneOutline</a></li>
<li><a href="#Pixel_TypeFace_getStringOutline">TypeFace.getStringOutline</a></li>
<li><a href="#Pixel_TypeFace_isBold">TypeFace.isBold</a></li>
<li><a href="#Pixel_TypeFace_isItalic">TypeFace.isItalic</a></li>
<li><a href="#Pixel_TypeFace_load">TypeFace.load</a></li>
<li><a href="#Pixel_TypeFace_release">TypeFace.release</a></li>
<li><a href="#Pixel_TypeFace_renderGlyph">TypeFace.renderGlyph</a></li>
</ul>
<h4>types</h4>
<ul>
<li><a href="#Pixel_Argb_fromName">Argb.fromName</a></li>
<li><a href="#Pixel_Brush_createHatch">Brush.createHatch</a></li>
<li><a href="#Pixel_Brush_createSolid">Brush.createSolid</a></li>
<li><a href="#Pixel_Brush_createTexture">Brush.createTexture</a></li>
<li><a href="#Pixel_Brush_createTiled">Brush.createTiled</a></li>
<li><a href="#Pixel_Color_fromAbgr">Color.fromAbgr</a></li>
<li><a href="#Pixel_Color_fromArgb">Color.fromArgb</a></li>
<li><a href="#Pixel_Color_fromArgbf">Color.fromArgbf</a></li>
<li><a href="#Pixel_Color_fromHsl">Color.fromHsl</a></li>
<li><a href="#Pixel_Color_fromRgb">Color.fromRgb</a></li>
<li><a href="#Pixel_Color_fromRgbf">Color.fromRgbf</a></li>
<li><a href="#Pixel_Color_fromVector4">Color.fromVector4</a></li>
<li><a href="#Pixel_Color_getBlend">Color.getBlend</a></li>
<li><a href="#Pixel_Color_getBlendKeepAlpha">Color.getBlendKeepAlpha</a></li>
<li><a href="#Pixel_Color_getDarker">Color.getDarker</a></li>
<li><a href="#Pixel_Color_getDistanceRgb">Color.getDistanceRgb</a></li>
<li><a href="#Pixel_Color_getLighter">Color.getLighter</a></li>
<li><a href="#Pixel_Color_isOpaque">Color.isOpaque</a></li>
<li><a href="#Pixel_Color_opAffect">Color.opAffect</a></li>
<li><a href="#Pixel_Color_opEquals">Color.opEquals</a></li>
<li><a href="#Pixel_Color_setArgb">Color.setArgb</a></li>
<li><a href="#Pixel_Color_setArgbf">Color.setArgbf</a></li>
<li><a href="#Pixel_Color_setHsl">Color.setHsl</a></li>
<li><a href="#Pixel_Color_setRgb">Color.setRgb</a></li>
<li><a href="#Pixel_Color_setRgbf">Color.setRgbf</a></li>
<li><a href="#Pixel_Color_toArgbf">Color.toArgbf</a></li>
<li><a href="#Pixel_Color_toHsl">Color.toHsl</a></li>
<li><a href="#Pixel_Color_toRgbf">Color.toRgbf</a></li>
<li><a href="#Pixel_Color_toVector4">Color.toVector4</a></li>
<li><a href="#Pixel_Color_IPokeValue_poke">IPokeValue.poke</a></li>
<li><a href="#Pixel_LinePath_arcTo">LinePath.arcTo</a></li>
<li><a href="#Pixel_LinePath_bezierTo">LinePath.bezierTo</a></li>
<li><a href="#Pixel_LinePath_clearCache">LinePath.clearCache</a></li>
<li><a href="#Pixel_LinePath_close">LinePath.close</a></li>
<li><a href="#Pixel_LinePath_curveTo">LinePath.curveTo</a></li>
<li><a href="#Pixel_LinePath_flatten">LinePath.flatten</a></li>
<li><a href="#Pixel_LinePath_lineTo">LinePath.lineTo</a></li>
<li><a href="#Pixel_LinePath_setArc">LinePath.setArc</a></li>
<li><a href="#Pixel_LinePath_setCircle">LinePath.setCircle</a></li>
<li><a href="#Pixel_LinePath_setEllipse">LinePath.setEllipse</a></li>
<li><a href="#Pixel_LinePath_setRect">LinePath.setRect</a></li>
<li><a href="#Pixel_LinePath_setRoundRect">LinePath.setRoundRect</a></li>
<li><a href="#Pixel_LinePath_start">LinePath.start</a></li>
<li><a href="#Pixel_LinePathList_clean">LinePathList.clean</a></li>
<li><a href="#Pixel_LinePathList_clear">LinePathList.clear</a></li>
<li><a href="#Pixel_LinePathList_count">LinePathList.count</a></li>
<li><a href="#Pixel_LinePathList_flatten">LinePathList.flatten</a></li>
<li><a href="#Pixel_LinePathList_newPath">LinePathList.newPath</a></li>
<li><a href="#Pixel_LinePathList_offset">LinePathList.offset</a></li>
<li><a href="#Pixel_Pen_createDash">Pen.createDash</a></li>
<li><a href="#Pixel_Pen_createHatch">Pen.createHatch</a></li>
<li><a href="#Pixel_Pen_createSolid">Pen.createSolid</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Module pixel</h1>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Bmp_BI_BITFIELDS"><span class="titletype">const</span> <span class="titlelight">Bmp.</span><span class="titlestrong">Constants</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Pixel_Bmp_BI_BITFIELDS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BI_BITFIELDS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_Bmp_BI_RGB" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BI_RGB</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_Bmp_BI_RLE4" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BI_RLE4</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_Bmp_BI_RLE8" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BI_RLE8</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_ClipperScaleCoords" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ClipperScaleCoords</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidRenderTargetHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">InvalidRenderTargetHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidRenderTargetSurfaceHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">InvalidRenderTargetSurfaceHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidShaderHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">InvalidShaderHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidShaderParamHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">InvalidShaderParamHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidTextureHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">InvalidTextureHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_MaxAAEdge" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MaxAAEdge</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Clipper_CInt"><span class="titletype">type alias</span> <span class="titlelight">Clipper.</span><span class="titlestrong">Type Aliases</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Pixel_Clipper_CInt" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">CInt</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_RenderTargetHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RenderTargetHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_RenderTargetSurfaceHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RenderTargetSurfaceHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_ShaderHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ShaderHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_ShaderParamHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ShaderParamHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_TextureHandle" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">TextureHandle</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Argb"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Argb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\argb.swg#L1" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AliceBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AntiqueWhite</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Aqua</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Aquamarine</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Azure</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Beige</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Bisque</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Black</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BlanchedAlmond</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Blue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BlueViolet</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Brown</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BurlyWood</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">CadetBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Chartreuse</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Chocolate</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Coral</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">CornflowerBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Cornsilk</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Crimson</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Cyan</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkCyan</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkGoldenrod</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkGray</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkKhaki</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkMagenta</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkOliveGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkOrange</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkOrchid</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkRed</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkSalmon</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkSeaGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkSlateBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkSlateGray</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkTurquoise</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DarkViolet</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DeepPink</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DeepSkyBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DimGray</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DodgerBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Firebrick</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FloralWhite</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ForestGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Fuchsia</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Gainsboro</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">GhostWhite</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Gold</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Goldenrod</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Gray</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Green</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">GreenYellow</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Honeydew</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HotPink</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">IndianRed</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Indigo</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Ivory</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Khaki</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Lavender</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LavenderBlush</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LawnGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LemonChiffon</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightCoral</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightCyan</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightGoldenrodYellow</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightGray</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightPink</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightSalmon</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightSeaGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightSkyBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightSlateGray</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightSteelBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightYellow</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Lime</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LimeGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Linen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Magenta</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Maroon</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MediumAquamarine</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MediumBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MediumOrchid</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MediumPurple</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MediumSeaGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MediumSlateBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MediumSpringGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MediumTurquoise</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MediumVioletRed</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MidnightBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MintCream</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MistyRose</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Moccasin</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">NavajoWhite</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Navy</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">OldLace</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Olive</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">OliveDrab</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Orange</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">OrangeRed</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Orchid</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PaleGoldenrod</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PaleGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PaleTurquoise</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PaleVioletRed</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PapayaWhip</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PeachPuff</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Peru</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pink</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Plum</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PowderBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Purple</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Red</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RosyBrown</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RoyalBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SaddleBrown</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Salmon</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SandyBrown</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SeaGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SeaShell</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Sienna</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Silver</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SkyBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SlateBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SlateGray</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Snow</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SpringGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SteelBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Tan</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Teal</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Thistle</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Tomato</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Turquoise</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Violet</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Wheat</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">White</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">WhiteSmoke</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Yellow</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">YellowGreen</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Zero</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Argb_fromName"><span class="titletype">func</span> <span class="titlelight">Argb.</span><span class="titlestrong">fromName</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\argb.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the Argb value that matches <code class="incode">name</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromName</span>(name: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_BlendingMode"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">BlendingMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Copy</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Alpha</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Add</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Sub</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SubDst</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Min</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Max</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Bmp_Decoder"><span class="titletype">struct</span> <span class="titlelight">Bmp.</span><span class="titlestrong">Decoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Bmp_Decoder_IImageDecoder_canDecode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">canDecode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canDecode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Bmp_Decoder_IImageDecoder_decode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">decode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L61" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">decode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, buffer: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>, options: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Bmp_Encoder"><span class="titletype">struct</span> <span class="titlelight">Bmp.</span><span class="titlestrong">Encoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\bmp.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Bmp_Encoder_IImageEncoder_canEncode"><span class="titletype">func</span> <span class="titlelight">IImageEncoder.</span><span class="titlestrong">canEncode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\bmp.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canEncode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Bmp_Encoder_IImageEncoder_encode"><span class="titletype">func</span> <span class="titlelight">IImageEncoder.</span><span class="titlestrong">encode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\bmp.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">encode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, result: *<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">ConcatBuffer</span>, image: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>, options: <span class="SyntaxType">any</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Bmp_Header"><span class="titletype">struct</span> <span class="titlelight">Bmp.</span><span class="titlestrong">Header</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">biSize</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biWidth</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biHeight</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biPlanes</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biBitCount</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biCompression</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biSizeImage</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biXPelsPerMeter</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biYPelsPerMeter</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biClrUsed</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">biClrImportant</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_BorderPos"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">BorderPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">None</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Inside</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Brush"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Brush</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">color</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">type</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_BrushType">BrushType</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">hatch</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_HatchStyle">HatchStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">texture</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">uvMode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_UVMode">UVMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">uvRect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Brush_createHatch"><span class="SyntaxCode"><span class="SyntaxFunction">createHatch</span>()</spa</a></td>
<td>Returns a hatch brush. </td>
</tr>
<tr>
<td><a href="#Pixel_Brush_createSolid"><span class="SyntaxCode"><span class="SyntaxFunction">createSolid</span>()</spa</a></td>
<td>Returns a solid color brush. </td>
</tr>
<tr>
<td><a href="#Pixel_Brush_createTexture"><span class="SyntaxCode"><span class="SyntaxFunction">createTexture</span>()</spa</a></td>
<td>Returns a full texture brush. </td>
</tr>
<tr>
<td><a href="#Pixel_Brush_createTiled"><span class="SyntaxCode"><span class="SyntaxFunction">createTiled</span>()</spa</a></td>
<td>Returns a tiled texture brush. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Brush_createHatch"><span class="titletype">func</span> <span class="titlelight">Brush.</span><span class="titlestrong">createHatch</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L55" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a hatch brush. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createHatch</span>(style: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_HatchStyle">HatchStyle</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Brush_createSolid"><span class="titletype">func</span> <span class="titlelight">Brush.</span><span class="titlestrong">createSolid</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a solid color brush. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createSolid</span>(color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Brush_createTexture"><span class="titletype">func</span> <span class="titlelight">Brush.</span><span class="titlestrong">createTexture</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a full texture brush. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createTexture</span>(texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Brush_createTiled"><span class="titletype">func</span> <span class="titlelight">Brush.</span><span class="titlestrong">createTiled</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a tiled texture brush. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createTiled</span>(texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_BrushType"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">BrushType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SolidColor</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Texture</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Hatch</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Clipper_ClipType"><span class="titletype">enum</span> <span class="titlelight">Clipper.</span><span class="titlestrong">ClipType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Intersection</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Union</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Difference</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Xor</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Clipper_InitOptions"><span class="titletype">enum</span> <span class="titlelight">Clipper.</span><span class="titlestrong">InitOptions</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ReverseSolution</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">StrictlySimple</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PreserveCollinear</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Clipper_IntPoint"><span class="titletype">struct</span> <span class="titlelight">Clipper.</span><span class="titlestrong">IntPoint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">x</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">y</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Clipper_IntPoint_opEquals"><span class="SyntaxCode"><span class="SyntaxFunction">opEquals</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Clipper_IntPoint_opEquals"><span class="titletype">func</span> <span class="titlelight">IntPoint.</span><span class="titlestrong">opEquals</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opEquals</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, other: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Clipper</span>.<span class="SyntaxConstant"><a href="#Pixel_Clipper_IntPoint">IntPoint</a></span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Clipper_JoinType"><span class="titletype">enum</span> <span class="titlelight">Clipper.</span><span class="titlestrong">JoinType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Square</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Round</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Miter</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Clipper_PolyFillType"><span class="titletype">enum</span> <span class="titlelight">Clipper.</span><span class="titlestrong">PolyFillType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EvenOdd</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">NonZero</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Positive</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Negative</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Clipper_PolyType"><span class="titletype">enum</span> <span class="titlelight">Clipper.</span><span class="titlestrong">PolyType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Subject</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Clip</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Clipper_Transform"><span class="titletype">struct</span> <span class="titlelight">Clipper.</span><span class="titlestrong">Transform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L142" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_ClippingMode"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">ClippingMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Set</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Clear</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Color</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">a</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">r</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">g</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">b</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">argb</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Color_fromAbgr"><span class="SyntaxCode"><span class="SyntaxFunction">fromAbgr</span>()</spa</a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_fromArgb"><span class="SyntaxCode"><span class="SyntaxFunction">fromArgb</span>(<span class="SyntaxConstant">Argb</span>)</span></a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_fromArgb"><span class="SyntaxCode"><span class="SyntaxFunction">fromArgb</span>(<span class="SyntaxType">u32</span>)</span></a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_fromArgb"><span class="SyntaxCode"><span class="SyntaxFunction">fromArgb</span>(<span class="SyntaxType">u8</span>, <span class="SyntaxConstant">Argb</span>)</span></a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_fromArgb"><span class="SyntaxCode"><span class="SyntaxFunction">fromArgb</span>(<span class="SyntaxType">u8</span>, <span class="SyntaxType">u8</span>, <span class="SyntaxType">u8</span>, <span class="SyntaxType">u8</span>)</span></a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_fromArgbf"><span class="SyntaxCode"><span class="SyntaxFunction">fromArgbf</span>()</spa</a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_fromHsl"><span class="SyntaxCode"><span class="SyntaxFunction">fromHsl</span>()</spa</a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_fromRgb"><span class="SyntaxCode"><span class="SyntaxFunction">fromRgb</span>()</spa</a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_fromRgbf"><span class="SyntaxCode"><span class="SyntaxFunction">fromRgbf</span>()</spa</a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_fromVector4"><span class="SyntaxCode"><span class="SyntaxFunction">fromVector4</span>()</spa</a></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_getBlend"><span class="SyntaxCode"><span class="SyntaxFunction">getBlend</span>()</spa</a></td>
<td>Blend two colors. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_getBlendKeepAlpha"><span class="SyntaxCode"><span class="SyntaxFunction">getBlendKeepAlpha</span>()</spa</a></td>
<td>Blend two colors, keeping the alpha channel of <code class="incode">col0</code> untouched. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_getDarker"><span class="SyntaxCode"><span class="SyntaxFunction">getDarker</span>()</spa</a></td>
<td>Returns a darker color. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_getDistanceRgb"><span class="SyntaxCode"><span class="SyntaxFunction">getDistanceRgb</span>()</spa</a></td>
<td>Returns the distance between the other color. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_getLighter"><span class="SyntaxCode"><span class="SyntaxFunction">getLighter</span>()</spa</a></td>
<td>Returns a lighter color. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_isOpaque"><span class="SyntaxCode"><span class="SyntaxFunction">isOpaque</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Color_setArgb"><span class="SyntaxCode"><span class="SyntaxFunction">setArgb</span>()</spa</a></td>
<td>Set all components. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_setArgbf"><span class="SyntaxCode"><span class="SyntaxFunction">setArgbf</span>()</spa</a></td>
<td>Set all components with floats in the range [0 1]. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_setHsl"><span class="SyntaxCode"><span class="SyntaxFunction">setHsl</span>()</spa</a></td>
<td>Initialize color with hue, saturation, lightness. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_setRgb"><span class="SyntaxCode"><span class="SyntaxFunction">setRgb</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td>Just set r, g and b components. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_setRgb"><span class="SyntaxCode"><span class="SyntaxFunction">setRgb</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">u8</span>, <span class="SyntaxType">u8</span>, <span class="SyntaxType">u8</span>)</span></a></td>
<td>Just set r, g and b components. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_setRgbf"><span class="SyntaxCode"><span class="SyntaxFunction">setRgbf</span>()</spa</a></td>
<td>Set r,g,b with floats in the range [0 1]. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_toArgbf"><span class="SyntaxCode"><span class="SyntaxFunction">toArgbf</span>()</spa</a></td>
<td>Retrieve all components as floating point values between 0 and 1. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_toHsl"><span class="SyntaxCode"><span class="SyntaxFunction">toHsl</span>()</spa</a></td>
<td>Returns hue, saturation, lightness. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_toRgbf"><span class="SyntaxCode"><span class="SyntaxFunction">toRgbf</span>()</spa</a></td>
<td>Retrieve all components as floating point values between 0 and 1. </td>
</tr>
<tr>
<td><a href="#Pixel_Color_toVector4"><span class="SyntaxCode"><span class="SyntaxFunction">toVector4</span>()</spa</a></td>
<td>Get the color as a [Math.Vector4]. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Color_opAffect"><span class="SyntaxCode"><span class="SyntaxFunction">opAffect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxConstant">Argb</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Color_opAffect"><span class="SyntaxCode"><span class="SyntaxFunction">opAffect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">string</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Color_opAffect"><span class="SyntaxCode"><span class="SyntaxFunction">opAffect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">u32</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Color_opEquals"><span class="SyntaxCode"><span class="SyntaxFunction">opEquals</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxConstant">Argb</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Color_opEquals"><span class="SyntaxCode"><span class="SyntaxFunction">opEquals</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Color_opEquals"><span class="SyntaxCode"><span class="SyntaxFunction">opEquals</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">u32</span>)</span></a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_IPokeValue_poke"><span class="titletype">func</span> <span class="titlelight">IPokeValue.</span><span class="titlestrong">poke</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L353" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">poke</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, buf: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">string</span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_fromAbgr"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">fromAbgr</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromAbgr</span>(abgr: <span class="SyntaxType">u32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_fromArgb"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">fromArgb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L156" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromArgb</span>(argb: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromArgb</span>(argb: <span class="SyntaxType">u32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromArgb</span>(alpha: <span class="SyntaxType">u8</span>, argb: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromArgb</span>(a: <span class="SyntaxType">u8</span>, r: <span class="SyntaxType">u8</span>, g: <span class="SyntaxType">u8</span>, b: <span class="SyntaxType">u8</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_fromArgbf"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">fromArgbf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L193" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromArgbf</span>(a: <span class="SyntaxType">f32</span>, r: <span class="SyntaxType">f32</span>, g: <span class="SyntaxType">f32</span>, b: <span class="SyntaxType">f32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_fromHsl"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">fromHsl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L211" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromHsl</span>(h: <span class="SyntaxType">f32</span>, s: <span class="SyntaxType">f32</span>, l: <span class="SyntaxType">f32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_fromRgb"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">fromRgb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L129" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromRgb</span>(r: <span class="SyntaxType">u8</span>, g: <span class="SyntaxType">u8</span>, b: <span class="SyntaxType">u8</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_fromRgbf"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">fromRgbf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L138" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromRgbf</span>(r: <span class="SyntaxType">f32</span>, g: <span class="SyntaxType">f32</span>, b: <span class="SyntaxType">f32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_fromVector4"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">fromVector4</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L202" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fromVector4</span>(vec: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector4</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_getBlend"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">getBlend</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L315" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Blend two colors. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getBlend</span>(col0: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, col1: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, factor: <span class="SyntaxType">f32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_getBlendKeepAlpha"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">getBlendKeepAlpha</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L303" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Blend two colors, keeping the alpha channel of <code class="incode">col0</code> untouched. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getBlendKeepAlpha</span>(col0: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, col1: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, factor: <span class="SyntaxType">f32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_getDarker"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">getDarker</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L327" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a darker color. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getDarker</span>(col: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, factor: <span class="SyntaxType">f32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_getDistanceRgb"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">getDistanceRgb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L340" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the distance between the other color. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getDistanceRgb</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, col: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)-&gt;<span class="SyntaxType">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_getLighter"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">getLighter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L334" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a lighter color. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getLighter</span>(col: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, factor: <span class="SyntaxType">f32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_isOpaque"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">isOpaque</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">isOpaque</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_opAffect"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">opAffect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opAffect</span>(<span class="SyntaxKeyword">self</span>, argb: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opAffect</span>(<span class="SyntaxKeyword">self</span>, name: <span class="SyntaxType">string</span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opAffect</span>(<span class="SyntaxKeyword">self</span>, argb: <span class="SyntaxType">u32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_opEquals"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">opEquals</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opEquals</span>(<span class="SyntaxKeyword">self</span>, argb: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>)-&gt;<span class="SyntaxType">bool</span>
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opEquals</span>(<span class="SyntaxKeyword">self</span>, other: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)-&gt;<span class="SyntaxType">bool</span>
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opEquals</span>(<span class="SyntaxKeyword">self</span>, argb: <span class="SyntaxType">u32</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_setArgb"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">setArgb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set all components. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setArgb</span>(<span class="SyntaxKeyword">self</span>, a: <span class="SyntaxType">u8</span>, r: <span class="SyntaxType">u8</span>, g: <span class="SyntaxType">u8</span>, b: <span class="SyntaxType">u8</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_setArgbf"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">setArgbf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set all components with floats in the range [0 1]. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setArgbf</span>(<span class="SyntaxKeyword">self</span>, a: <span class="SyntaxType">f32</span>, r: <span class="SyntaxType">f32</span>, g: <span class="SyntaxType">f32</span>, b: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_setHsl"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">setHsl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L219" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize color with hue, saturation, lightness. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setHsl</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, h: <span class="SyntaxType">f32</span>, s: <span class="SyntaxType">f32</span>, l: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_setRgb"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">setRgb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Just set r, g and b components. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setRgb</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rgb: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>Just set r, g and b components. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setRgb</span>(<span class="SyntaxKeyword">self</span>, r: <span class="SyntaxType">u8</span>, g: <span class="SyntaxType">u8</span>, b: <span class="SyntaxType">u8</span>)</span></code>
</div>
<p> Alpha will be opaque. </p>
<p> Alpha will be opaque. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_setRgbf"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">setRgbf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set r,g,b with floats in the range [0 1]. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setRgbf</span>(<span class="SyntaxKeyword">self</span>, r: <span class="SyntaxType">f32</span>, g: <span class="SyntaxType">f32</span>, b: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p> Alpha will be opaque. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_toArgbf"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">toArgbf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L94" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Retrieve all components as floating point values between 0 and 1. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toArgbf</span>(<span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;{a: <span class="SyntaxType">f32</span>, r: <span class="SyntaxType">f32</span>, g: <span class="SyntaxType">f32</span>, b: <span class="SyntaxType">f32</span>}</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_toHsl"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">toHsl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L261" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns hue, saturation, lightness. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toHsl</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;{h: <span class="SyntaxType">f32</span>, s: <span class="SyntaxType">f32</span>, l: <span class="SyntaxType">f32</span>}</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_toRgbf"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">toRgbf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L118" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Retrieve all components as floating point values between 0 and 1. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toRgbf</span>(<span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;{r: <span class="SyntaxType">f32</span>, g: <span class="SyntaxType">f32</span>, b: <span class="SyntaxType">f32</span>}</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Color_toVector4"><span class="titletype">func</span> <span class="titlelight">Color.</span><span class="titlestrong">toVector4</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L106" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the color as a [Math.Vector4]. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toVector4</span>(<span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector4</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_ColorMask"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">ColorMask</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">r</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">g</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">b</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">a</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Command"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Command</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">id</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_CommandId">CommandId</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">using</span> params</span></td>
<td class="codetype"><span class="SyntaxCode">{clear: {color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>}, drawTriangles: {start: <span class="SyntaxType">u32</span>, count: <span class="SyntaxType">u32</span>}, transform: {tr: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Transform2</span>}, clippingRect: {rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>}, clippingRegion: {mode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ClippingMode">ClippingMode</a></span>}, font: {fontRef: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>}, blendingMode: {mode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_BlendingMode">BlendingMode</a></span>}, textureFont: {font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>, atlasIndex: <span class="SyntaxType">s32</span>}, colorMask: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ColorMask">ColorMask</a></span>, renderTgt: {tgt: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span>, paintAlpha: <span class="SyntaxType">bool</span>}, shader: {shader: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span>}, shaderParam: {param: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>, type: <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">Swag</span>.<span class="SyntaxConstant">TypeInfo</span>, _f32: <span class="SyntaxType">f32</span>, _s32: <span class="SyntaxType">s32</span>}, texture0: {boundRect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector4</span>, textureRect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector4</span>, type: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_BrushType">BrushType</a></span>, hatch: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_HatchStyle">HatchStyle</a></span>, uvMode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_UVMode">UVMode</a></span>, interpolationMode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_InterpolationMode">InterpolationMode</a></span>, texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>}}</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_CommandId"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">CommandId</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Clear</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ColorMask</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Transform</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DrawTriangles</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BindTexture0</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ResetTexture0</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BindTextureFont</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ResetTextureFont</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SetClippingRect</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ResetClippingRect</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">StartClippingRegion</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EndClippingRegion</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ResetClippingRegion</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SetClippingRegionMode</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">UpdateFont</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">StartNoOverlap</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">StopNoOverlap</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SetBlendingMode</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BindRenderTarget</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">UnbindRenderTarget</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ReleaseRenderTarget</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SetShader</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SetShaderParam</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_DashCapStyle"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">DashCapStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">None</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Triangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Round</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Square</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_DashStyle"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">DashStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">None</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pattern</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Dot</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Dash</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DashDot</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_DecodeOptions"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">DecodeOptions</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">decodePixels</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_DefaultShaderId"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">DefaultShaderId</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Default</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Blur</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_DrawPathListMode"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">DrawPathListMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawpath.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Separate</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Merge</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Element"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Element</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">type</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ElementType">ElementType</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">position</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">p1</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">p2</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_ElementType"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">ElementType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Line</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Arc</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Bezier1</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Bezier2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Curve</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Font"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Font</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">ascent</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">descent</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">underlinePos</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">underlineSize</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">forceBold</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">forceItalic</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">fullname</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">typeFace</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">size</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">atlases</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_GlyphAtlas">GlyphAtlas</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">asciiRef</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">128</span>] <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">unicodeRef</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">HashTable</span>'(<span class="SyntaxType">rune</span>, *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">dirty</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Font_create"><span class="SyntaxCode"><span class="SyntaxFunction">create</span>(*<span class="SyntaxConstant">TypeFace</span>, <span class="SyntaxType">u32</span>, <span class="SyntaxType">bool</span>, <span class="SyntaxType">bool</span>)</span></a></td>
<td>Creates a new Font of the given typeface and size. </td>
</tr>
<tr>
<td><a href="#Pixel_Font_create"><span class="SyntaxCode"><span class="SyntaxFunction">create</span>(<span class="SyntaxType">string</span>, <span class="SyntaxType">u32</span>, <span class="SyntaxType">bool</span>, <span class="SyntaxType">bool</span>)</span></a></td>
<td>Creates a new Font with the given filename and size. </td>
</tr>
<tr>
<td><a href="#Pixel_Font_getGlyphDesc"><span class="SyntaxCode"><span class="SyntaxFunction">getGlyphDesc</span>()</spa</a></td>
<td>Returns the glyph descriptor of a given rune. </td>
</tr>
<tr>
<td><a href="#Pixel_Font_release"><span class="SyntaxCode"><span class="SyntaxFunction">release</span>()</spa</a></td>
<td>Release the given font. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Font_create"><span class="titletype">func</span> <span class="titlelight">Font.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new Font of the given typeface and size. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">create</span>(typeFace: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span>, size: <span class="SyntaxType">u32</span>, forceBold = <span class="SyntaxKeyword">false</span>, forceItalic = <span class="SyntaxKeyword">false</span>)-&gt;*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span></span></code>
</div>
<p>Creates a new Font with the given filename and size. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">create</span>(fullname: <span class="SyntaxType">string</span>, size: <span class="SyntaxType">u32</span>, forceBold = <span class="SyntaxKeyword">false</span>, forceItalic = <span class="SyntaxKeyword">false</span>)-&gt;*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Font_getGlyphDesc"><span class="titletype">func</span> <span class="titlelight">Font.</span><span class="titlestrong">getGlyphDesc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L126" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the glyph descriptor of a given rune. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getGlyphDesc</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, c: <span class="SyntaxType">rune</span>)-&gt;<span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Font_release"><span class="titletype">func</span> <span class="titlelight">Font.</span><span class="titlestrong">release</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L73" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the given font. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">release</span>(font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_FontFamily"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">FontFamily</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">regular</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bold</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">italic</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">boldItalic</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_FontFamily_createTypeFace"><span class="SyntaxCode"><span class="SyntaxFunction">createTypeFace</span>()</spa</a></td>
<td>Creates a typeface from a FontFamilyEnumerate and FontFamilyStyle. </td>
</tr>
<tr>
<td><a href="#Pixel_FontFamily_enumerateFromOs"><span class="SyntaxCode"><span class="SyntaxFunction">enumerateFromOs</span>()</spa</a></td>
<td>Enumerate the font families available in the os. </td>
</tr>
<tr>
<td><a href="#Pixel_FontFamily_getFont"><span class="SyntaxCode"><span class="SyntaxFunction">getFont</span>()</spa</a></td>
<td>Get the font corresponding to the style. </td>
</tr>
<tr>
<td><a href="#Pixel_FontFamily_getFromOs"><span class="SyntaxCode"><span class="SyntaxFunction">getFromOs</span>()</spa</a></td>
<td>Get a FontFamilyEnumerate from a font family name. </td>
</tr>
<tr>
<td><a href="#Pixel_FontFamily_setFont"><span class="SyntaxCode"><span class="SyntaxFunction">setFont</span>()</spa</a></td>
<td>Set the font corresponding to a given style. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_FontFamily_createTypeFace"><span class="titletype">func</span> <span class="titlelight">FontFamily.</span><span class="titlestrong">createTypeFace</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L109" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a typeface from a FontFamilyEnumerate and FontFamilyStyle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createTypeFace</span>(enumerate: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></span>, style: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span>)-&gt;*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_FontFamily_enumerateFromOs"><span class="titletype">func</span> <span class="titlelight">FontFamily.</span><span class="titlestrong">enumerateFromOs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enumerate the font families available in the os. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">enumerateFromOs</span>()-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_FontFamily_getFont"><span class="titletype">func</span> <span class="titlelight">FontFamily.</span><span class="titlestrong">getFont</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the font corresponding to the style. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getFont</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, style: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span>)-&gt;*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span></span></code>
</div>
<p> If not evailable, get another one. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_FontFamily_getFromOs"><span class="titletype">func</span> <span class="titlelight">FontFamily.</span><span class="titlestrong">getFromOs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get a FontFamilyEnumerate from a font family name. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getFromOs</span>(fontFamily: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_FontFamily_setFont"><span class="titletype">func</span> <span class="titlelight">FontFamily.</span><span class="titlestrong">setFont</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the font corresponding to a given style. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setFont</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fnt: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>, style: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_FontFamilyEnumerate"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">FontFamilyEnumerate</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">name</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">faces</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">4</span>] <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamilyStyleEnumerate">FontFamilyStyleEnumerate</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_FontFamilyStyle"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">FontFamilyStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Regular</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Bold</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Italic</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BoldItalic</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_FontFamilyStyleEnumerate"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">FontFamilyStyleEnumerate</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">name</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">face</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Gdi32</span>.<span class="SyntaxConstant">LOGFONTW</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Gif_Decoder"><span class="titletype">struct</span> <span class="titlelight">Gif.</span><span class="titlestrong">Decoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Gif_Decoder_getFrame"><span class="SyntaxCode"><span class="SyntaxFunction">getFrame</span>()</spa</a></td>
<td>Create an image for the current frame. </td>
</tr>
<tr>
<td><a href="#Pixel_Gif_Decoder_getNumFrames"><span class="SyntaxCode"><span class="SyntaxFunction">getNumFrames</span>()</spa</a></td>
<td>Returns the number of frames  As Gif does not store it, we need to go threw all images (!). </td>
</tr>
<tr>
<td><a href="#Pixel_Gif_Decoder_init"><span class="SyntaxCode"><span class="SyntaxFunction">init</span>()</spa</a></td>
<td>Initialize the decoder. </td>
</tr>
<tr>
<td><a href="#Pixel_Gif_Decoder_nextFrame"><span class="SyntaxCode"><span class="SyntaxFunction">nextFrame</span>()</spa</a></td>
<td>Get the next frame. Returns false if we have reached the end. </td>
</tr>
<tr>
<td><a href="#Pixel_Gif_Decoder_rewind"><span class="SyntaxCode"><span class="SyntaxFunction">rewind</span>()</spa</a></td>
<td>Restart at frame 0. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Gif_Decoder_IImageDecoder_canDecode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">canDecode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L555" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canDecode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Gif_Decoder_IImageDecoder_decode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">decode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L561" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">decode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, buffer: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>, options: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Gif_Decoder_getFrame"><span class="titletype">func</span> <span class="titlelight">Decoder.</span><span class="titlestrong">getFrame</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L501" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create an image for the current frame. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getFrame</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, image: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>, decodePixels = <span class="SyntaxKeyword">true</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Gif_Decoder_getNumFrames"><span class="titletype">func</span> <span class="titlelight">Decoder.</span><span class="titlestrong">getNumFrames</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L529" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the number of frames  As Gif does not store it, we need to go threw all images (!). </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getNumFrames</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">s32</span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Gif_Decoder_init"><span class="titletype">func</span> <span class="titlelight">Decoder.</span><span class="titlestrong">init</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L439" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the decoder. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">init</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, buffer: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>, decodePixels: <span class="SyntaxType">bool</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Gif_Decoder_nextFrame"><span class="titletype">func</span> <span class="titlelight">Decoder.</span><span class="titlestrong">nextFrame</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L511" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the next frame. Returns false if we have reached the end. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">nextFrame</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, img: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>)-&gt;<span class="SyntaxType">bool</span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Gif_Decoder_rewind"><span class="titletype">func</span> <span class="titlelight">Decoder.</span><span class="titlestrong">rewind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L544" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Restart at frame 0. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">rewind</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_GlyphAtlas"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">GlyphAtlas</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">texture</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">width</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bpp</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">datas</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxType">u8</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">dirty</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">curX</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">curY</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">nextY</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_GlyphDesc"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">GlyphDesc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">uv</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">atlasIndex</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">advanceX</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">advanceY</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">shiftX</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">shiftY</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">width</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_HatchStyle"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">HatchStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\hatch.swg#L2" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Dot</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Horizontal</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HorizontalLight</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HorizontalNarrow</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Vertical</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">VerticalLight</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">VerticalNarrow</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SolidDiamond</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">OutlinedDiamond</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HorizontalDark</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">VerticalDark</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Grid</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">GridLarge</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">GridDotted</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Cross</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">CrossLarge</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent5</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent10</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent20</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent25</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent30</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent40</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent50</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent60</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent70</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent75</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent80</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Percent90</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">CheckBoard</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">CheckBoardLarge</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Weave</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DiagonalForward</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DiagonalForwardLight</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DiagonalBackward</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DiagonalBackwardLight</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DiagonalCross</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_IImageDecoder"><span class="titletype">interface</span> <span class="titlelight">Pixel.</span><span class="titlestrong">IImageDecoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Interface to decode a buffer. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">canDecode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_IImageDecoder">IImageDecoder</a></span>, <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">decode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_IImageDecoder">IImageDecoder</a></span>, <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>, <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_IImageEncoder"><span class="titletype">interface</span> <span class="titlelight">Pixel.</span><span class="titlestrong">IImageEncoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\encode.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Interface to decode a buffer. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">canEncode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_IImageEncoder">IImageEncoder</a></span>, <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">encode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_IImageEncoder">IImageEncoder</a></span>, *<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">ConcatBuffer</span>, <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>, <span class="SyntaxType">any</span>) <span class="SyntaxKeyword">throw</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Image</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>An image buffer, in various pixel formats. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">pixels</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxType">u8</span>)</span></td>
<td>All the pixels. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">size</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td>Size in bytes. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">width</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>The width of the image. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>The height of the image. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">width8</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>The width, in bytes. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pf</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span></span></td>
<td>Format of one pixel. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bpp</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td>Bits per pixel. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bpp8</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td>Bytes per pixel. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">workingBuffer</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></td>
<td>Temporary buffer for filters and transformations. </td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Image_addDecoder"><span class="SyntaxCode"><span class="SyntaxFunction">addDecoder</span>()</spa</a></td>
<td>Register an image decoder. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_addEncoder"><span class="SyntaxCode"><span class="SyntaxFunction">addEncoder</span>()</spa</a></td>
<td>Register an image encoder. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_allocPixels"><span class="SyntaxCode"><span class="SyntaxFunction">allocPixels</span>()</spa</a></td>
<td>Allocate pixels. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_allocWorkingBuffer"><span class="SyntaxCode"><span class="SyntaxFunction">allocWorkingBuffer</span>(<span class="SyntaxKeyword">self</span>)</span></a></td>
<td>Allocate a computing buffer with current image size. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_allocWorkingBuffer"><span class="SyntaxCode"><span class="SyntaxFunction">allocWorkingBuffer</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxConstant">PixelFormat</span>)</span></a></td>
<td>Allocate a computing buffer with new sizes. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_applyKernel"><span class="SyntaxCode"><span class="SyntaxFunction">applyKernel</span>()</spa</a></td>
<td>Apply a kernel to the image. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_applyWorkingBuffer"><span class="SyntaxCode"><span class="SyntaxFunction">applyWorkingBuffer</span>()</spa</a></td>
<td>Replace the current image content with the working buffer. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_canLoad"><span class="SyntaxCode"><span class="SyntaxFunction">canLoad</span>()</spa</a></td>
<td>Returns <code class="incode">true</code> if the given filename has a corresponding decoder. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_canSave"><span class="SyntaxCode"><span class="SyntaxFunction">canSave</span>()</spa</a></td>
<td>Returns <code class="incode">true</code> if the given filename has a corresponding encoder. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_clear"><span class="SyntaxCode"><span class="SyntaxFunction">clear</span>()</spa</a></td>
<td>Clear the content of the image. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_colorize"><span class="SyntaxCode"><span class="SyntaxFunction">colorize</span>()</spa</a></td>
<td>Colorize the image by setting the hue and saturation. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_contrast"><span class="SyntaxCode"><span class="SyntaxFunction">contrast</span>()</spa</a></td>
<td>Change the contrast. <code class="incode">factor</code> is [-1, 1]. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_copyPixel"><span class="SyntaxCode"><span class="SyntaxFunction">copyPixel</span>()</spa</a></td>
<td>Copy one pixel, depending on bpp. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_create"><span class="SyntaxCode"><span class="SyntaxFunction">create</span>()</spa</a></td>
<td>Creates a new image. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_crop"><span class="SyntaxCode"><span class="SyntaxFunction">crop</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Image_crop"><span class="SyntaxCode"><span class="SyntaxFunction">crop</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxType">s32</span>)</span></a></td>
<td>Crop image. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_decode"><span class="SyntaxCode"><span class="SyntaxFunction">decode</span>()</spa</a></td>
<td>Decode the given image buffer. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_fade"><span class="SyntaxCode"><span class="SyntaxFunction">fade</span>()</spa</a></td>
<td>Fade to a given color. <code class="incode">factor</code> is [0, 1]. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_fill"><span class="SyntaxCode"><span class="SyntaxFunction">fill</span>()</spa</a></td>
<td>Fill image with <code class="incode">color</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_fillGradient2"><span class="SyntaxCode"><span class="SyntaxFunction">fillGradient2</span>()</spa</a></td>
<td>Fill with an horizontal gradient from <code class="incode">color0</code> to <code class="incode">color1</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_fillGradient4"><span class="SyntaxCode"><span class="SyntaxFunction">fillGradient4</span>()</spa</a></td>
<td>Fill with a gradient with a different color at each corner. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_fillHsl"><span class="SyntaxCode"><span class="SyntaxFunction">fillHsl</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Image_flip"><span class="SyntaxCode"><span class="SyntaxFunction">flip</span>()</spa</a></td>
<td>Flip image vertically. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_freeWorkingBuffer"><span class="SyntaxCode"><span class="SyntaxFunction">freeWorkingBuffer</span>()</spa</a></td>
<td>Free the working buffer if allocated. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_from"><span class="SyntaxCode"><span class="SyntaxFunction">from</span>(<span class="SyntaxConstant">HBITMAP</span>)</span></a></td>
<td>Creates an image from a windows HBITMAP. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_from"><span class="SyntaxCode"><span class="SyntaxFunction">from</span>(<span class="SyntaxConstant">HICON</span>)</span></a></td>
<td>Creates an image from a windows HICON. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_gamma"><span class="SyntaxCode"><span class="SyntaxFunction">gamma</span>()</spa</a></td>
<td>Change the gamma. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_getPixelColor"><span class="SyntaxCode"><span class="SyntaxFunction">getPixelColor</span>()</spa</a></td>
<td>Returns the color at the given coordinate. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_grayScale"><span class="SyntaxCode"><span class="SyntaxFunction">grayScale</span>()</spa</a></td>
<td>Transform image to grayscale, with a given factor. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_halfSize"><span class="SyntaxCode"><span class="SyntaxFunction">halfSize</span>()</spa</a></td>
<td>Divide image size by 2 with a bilinear 2x2 filter. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_init"><span class="SyntaxCode"><span class="SyntaxFunction">init</span>()</spa</a></td>
<td>Initialize image informations  <code class="incode">pixels</code> is set to null, and must be initialized after a call to that function  This gives the opportunity to set the pixels with an external buffer. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_invert"><span class="SyntaxCode"><span class="SyntaxFunction">invert</span>()</spa</a></td>
<td>Invert colors. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_isValid"><span class="SyntaxCode"><span class="SyntaxFunction">isValid</span>()</spa</a></td>
<td>Returns true if the image is valid. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_lightness"><span class="SyntaxCode"><span class="SyntaxFunction">lightness</span>()</spa</a></td>
<td>Change the lightness. <code class="incode">factor</code> is [-1, 1]. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_load"><span class="SyntaxCode"><span class="SyntaxFunction">load</span>()</spa</a></td>
<td>Load the given image file. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_mirror"><span class="SyntaxCode"><span class="SyntaxFunction">mirror</span>()</spa</a></td>
<td>Flip image horizontally. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_mix"><span class="SyntaxCode"><span class="SyntaxFunction">mix</span>()</spa</a></td>
<td>Mix with another image. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_release"><span class="SyntaxCode"><span class="SyntaxFunction">release</span>()</spa</a></td>
<td>Release the content of the image. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_resize"><span class="SyntaxCode"><span class="SyntaxFunction">resize</span>()</spa</a></td>
<td>Resize image  Put <code class="incode">newWidth</code> or <code class="incode">newHeight</code> to 0 to keep aspect ratio. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_save"><span class="SyntaxCode"><span class="SyntaxFunction">save</span>()</spa</a></td>
<td>Save the image to a file. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_setAlpha"><span class="SyntaxCode"><span class="SyntaxFunction">setAlpha</span>()</spa</a></td>
<td>Change the alpha channel. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_setChannel"><span class="SyntaxCode"><span class="SyntaxFunction">setChannel</span>()</spa</a></td>
<td>Change specified color channels. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_setPixelFormat"><span class="SyntaxCode"><span class="SyntaxFunction">setPixelFormat</span>()</spa</a></td>
<td>Change image pixel format. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_setPixelRGBA8"><span class="SyntaxCode"><span class="SyntaxFunction">setPixelRGBA8</span>()</spa</a></td>
<td>Set pixel values depending on pixel format. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toApplyKernel"><span class="SyntaxCode"><span class="SyntaxFunction">toApplyKernel</span>()</spa</a></td>
<td>Apply a kernel to the image. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toCrop"><span class="SyntaxCode"><span class="SyntaxFunction">toCrop</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Image_toCrop"><span class="SyntaxCode"><span class="SyntaxFunction">toCrop</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxType">s32</span>)</span></a></td>
<td>Crop image. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toFlip"><span class="SyntaxCode"><span class="SyntaxFunction">toFlip</span>()</spa</a></td>
<td>Flip image vertically. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toHICON"><span class="SyntaxCode"><span class="SyntaxFunction">toHICON</span>()</spa</a></td>
<td>Transform an image to a windows icon. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toHalfSize"><span class="SyntaxCode"><span class="SyntaxFunction">toHalfSize</span>()</spa</a></td>
<td>Divide image size by 2 with a bilinear 2x2 filter. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toMirror"><span class="SyntaxCode"><span class="SyntaxFunction">toMirror</span>()</spa</a></td>
<td>Flip image horizontally. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toMix"><span class="SyntaxCode"><span class="SyntaxFunction">toMix</span>()</spa</a></td>
<td>Mix with another image. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toResize"><span class="SyntaxCode"><span class="SyntaxFunction">toResize</span>()</spa</a></td>
<td>Resize image  Put <code class="incode">newWidth</code> or <code class="incode">newHeight</code> to 0 to keep aspect ratio. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toSetPixelFormat"><span class="SyntaxCode"><span class="SyntaxFunction">toSetPixelFormat</span>()</spa</a></td>
<td>Change image pixel format. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_toTurn"><span class="SyntaxCode"><span class="SyntaxFunction">toTurn</span>()</spa</a></td>
<td>Turn image by a given predefined angle. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_turn"><span class="SyntaxCode"><span class="SyntaxFunction">turn</span>()</spa</a></td>
<td>Turn image by a given predefined angle. </td>
</tr>
<tr>
<td><a href="#Pixel_Image_visitPixels"><span class="SyntaxCode"><span class="SyntaxFunction">visitPixels</span>()</spa</a></td>
<td>Macro to visit all pixels of the image in parallel chunks. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Image_opVisit"><span class="SyntaxCode"><span class="SyntaxFunction">opVisit</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_ChannelMode"><span class="titletype">enum</span> <span class="titlelight">Image.</span><span class="titlestrong">ChannelMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\setchannel.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Red</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Green</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Blue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Alpha</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RGB</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RGBA</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_ChannelValueMode"><span class="titletype">enum</span> <span class="titlelight">Image.</span><span class="titlestrong">ChannelValueMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\setchannel.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Color</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MinRGB</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MaxRGB</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MeanRGB</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Alpha</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_FillHslType"><span class="titletype">enum</span> <span class="titlelight">Image.</span><span class="titlestrong">FillHslType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fillhsl.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HueVert</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HueHorz</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SaturationVert</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SaturationHorz</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightnessVert</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LightnessHorz</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HueSaturation</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HueLightness</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SaturationLightness</span></span></td>
<td>(h,0,1) (h,1,0.5) (h,0,0) (h,1,0). </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">GreenBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RedBlue</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RedGreen</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_Kernel"><span class="titletype">enum</span> <span class="titlelight">Image.</span><span class="titlestrong">Kernel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\applykernel.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Kernel format is : kernelSize, kernelNormalizeValue, kernelValues... </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">GaussianBlur3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">GaussianBlur3x3B</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">GaussianBlur5x5A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BoxBlur3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BoxBlur5x5A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Sharpen3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Sharpen3x3B</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Soften3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EmbossLeft3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EmbossLeft3x3B</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EmbossLeft3x3C</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EmbossRight3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EmbossRight3x3B</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EmbossRight3x3C</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EdgeDetect3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EdgeDetect3x3B</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EdgeDetect3x3C</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EdgeDetect3x3D</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EdgeDetect3x3E</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">EdgeDetect3x3F</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SobelHorizontal3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SobelVertical3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PrevitHorizontal3x3A</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PrevitVertical3x3A</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_MixMode"><span class="titletype">enum</span> <span class="titlelight">Image.</span><span class="titlestrong">MixMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mix.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Copy</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AlphaBlend</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_ResizeMode"><span class="titletype">enum</span> <span class="titlelight">Image.</span><span class="titlestrong">ResizeMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\resize.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Raw</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Bilinear</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Bicubic</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Gaussian</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Quadratic</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Hermite</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Hamming</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Catrom</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_TurnAngle"><span class="titletype">enum</span> <span class="titlelight">Image.</span><span class="titlestrong">TurnAngle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\turn.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">T90</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">T180</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">T270</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_addDecoder"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">addDecoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register an image decoder. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(<span class="SyntaxConstant">T</span>) <span class="SyntaxFunction">addDecoder</span>()</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_addEncoder"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">addEncoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\encode.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register an image encoder. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(<span class="SyntaxConstant">T</span>) <span class="SyntaxFunction">addEncoder</span>()</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_allocPixels"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">allocPixels</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Allocate pixels. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">allocPixels</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_allocWorkingBuffer"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">allocWorkingBuffer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L90" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Allocate a computing buffer with current image size. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">allocWorkingBuffer</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>Allocate a computing buffer with new sizes. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">allocWorkingBuffer</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, newWidth: <span class="SyntaxType">s32</span>, newHeight: <span class="SyntaxType">s32</span>, newPf: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_applyKernel"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">applyKernel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\applykernel.swg#L112" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Apply a kernel to the image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">applyKernel</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, kernel: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>.<span class="SyntaxConstant"><a href="#Pixel_Image_Kernel">Kernel</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_applyWorkingBuffer"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">applyWorkingBuffer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L117" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Replace the current image content with the working buffer. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">applyWorkingBuffer</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_canLoad"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">canLoad</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L77" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns <code class="incode">true</code> if the given filename has a corresponding decoder. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canLoad</span>(fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_canSave"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">canSave</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\encode.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns <code class="incode">true</code> if the given filename has a corresponding encoder. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canSave</span>(fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_clear"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L173" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear the content of the image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">clear</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_colorize"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">colorize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\colorize.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Colorize the image by setting the hue and saturation. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">colorize</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, hue: <span class="SyntaxType">f32</span>, saturation: <span class="SyntaxType">f32</span>, strength = <span class="SyntaxNumber">1.0</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_contrast"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">contrast</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\contrast.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the contrast. <code class="incode">factor</code> is [-1, 1]. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">contrast</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, factor: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">0.5</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_copyPixel"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">copyPixel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Copy one pixel, depending on bpp. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(<span class="SyntaxConstant">BPP</span>: <span class="SyntaxType">u8</span>) <span class="SyntaxFunction">copyPixel</span>(pixDst: ^<span class="SyntaxType">u8</span>, pixSrc: ^<span class="SyntaxType">u8</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_create"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">create</span>(width: <span class="SyntaxType">s32</span>, height: <span class="SyntaxType">s32</span>, pf: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span>, buffer: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span> = <span class="SyntaxKeyword">null</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_crop"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">crop</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\crop.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Crop image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">crop</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, cropRect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">crop</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">s32</span>, y: <span class="SyntaxType">s32</span>, w: <span class="SyntaxType">s32</span>, h: <span class="SyntaxType">s32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_decode"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">decode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Decode the given image buffer. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">decode</span>(fileName: <span class="SyntaxType">string</span>, bytes: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>, options: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span> = {})-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_fade"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">fade</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fade.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fade to a given color. <code class="incode">factor</code> is [0, 1]. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fade</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, factor: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">0.5</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_fill"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">fill</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fill.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill image with <code class="incode">color</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fill</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_fillGradient2"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">fillGradient2</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fillgradient.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill with an horizontal gradient from <code class="incode">color0</code> to <code class="incode">color1</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillGradient2</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, color0: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, color1: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_fillGradient4"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">fillGradient4</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fillgradient.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill with a gradient with a different color at each corner. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillGradient4</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, topLeft: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, topRight: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, bottomLeft: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, bottomRight: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_fillHsl"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">fillHsl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fillhsl.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillHsl</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fillType: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>.<span class="SyntaxConstant"><a href="#Pixel_Image_FillHslType">FillHslType</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span> = <span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>.<span class="SyntaxConstant">White</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_flip"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">flip</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\flip.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flip image vertically. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">flip</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_freeWorkingBuffer"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">freeWorkingBuffer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L110" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Free the working buffer if allocated. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">freeWorkingBuffer</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_from"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">from</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.win32.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates an image from a windows HBITMAP. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">from</span>(hbitmap: <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HBITMAP</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>Creates an image from a windows HICON. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">from</span>(hicon: <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HICON</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_gamma"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">gamma</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\gamma.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the gamma. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">gamma</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, level: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">2.2</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_getPixelColor"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">getPixelColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L208" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the color at the given coordinate. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getPixelColor</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, pixX: <span class="SyntaxType">s32</span>, pixY: <span class="SyntaxType">s32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_grayScale"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">grayScale</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\grayscale.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform image to grayscale, with a given factor. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">grayScale</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, factor: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_halfSize"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">halfSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\halfsize.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Divide image size by 2 with a bilinear 2x2 filter. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">halfSize</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_init"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">init</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize image informations  <code class="incode">pixels</code> is set to null, and must be initialized after a call to that function  This gives the opportunity to set the pixels with an external buffer. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">init</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, width: <span class="SyntaxType">s32</span>, height: <span class="SyntaxType">s32</span>, pf: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_invert"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">invert</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\invert.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Invert colors. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">invert</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_isValid"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">isValid</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the image is valid. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">isValid</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_lightness"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">lightness</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\lightness.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the lightness. <code class="incode">factor</code> is [-1, 1]. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">lightness</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, factor: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">0.5</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_load"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">load</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Load the given image file. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">load</span>(fileName: <span class="SyntaxType">string</span>, options: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span> = {})-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_mirror"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">mirror</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mirror.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flip image horizontally. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">mirror</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_mix"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">mix</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mix.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Mix with another image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">mix</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, srcImage: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>, x: <span class="SyntaxType">s32</span>, y: <span class="SyntaxType">s32</span>, mode = <span class="SyntaxConstant"><a href="#Pixel_Image_MixMode">MixMode</a></span>.<span class="SyntaxConstant">Copy</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_opVisit"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">opVisit</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxAttribute">#[<a href="swag.runtime.php#Swag_Macro">Swag.Macro</a>]</span>
<span class="SyntaxKeyword">func</span>(ptr: <span class="SyntaxType">bool</span>) <span class="SyntaxFunction">opVisit</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, stmt: <span class="SyntaxType">code</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_release"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">release</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L166" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the content of the image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">release</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_resize"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">resize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\resize.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Resize image  Put <code class="incode">newWidth</code> or <code class="incode">newHeight</code> to 0 to keep aspect ratio. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">resize</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, newWidth: <span class="SyntaxType">s32</span>, newHeight: <span class="SyntaxType">s32</span>, mode = <span class="SyntaxConstant"><a href="#Pixel_Image_ResizeMode">ResizeMode</a></span>.<span class="SyntaxConstant">Raw</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_save"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">save</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\encode.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Save the image to a file. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">save</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>, options: <span class="SyntaxType">any</span> = <span class="SyntaxKeyword">null</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_setAlpha"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">setAlpha</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\setchannel.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the alpha channel. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setAlpha</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, value: <span class="SyntaxType">u8</span> = <span class="SyntaxNumber">0</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_setChannel"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">setChannel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\setchannel.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change specified color channels. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setChannel</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, mode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>.<span class="SyntaxConstant"><a href="#Pixel_Image_ChannelMode">ChannelMode</a></span>, value: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>.<span class="SyntaxConstant"><a href="#Pixel_Image_ChannelValueMode">ChannelValueMode</a></span>, color: <span class="SyntaxType">u8</span> = <span class="SyntaxNumber">0</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_setPixelFormat"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">setPixelFormat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\setpixelformat.swg#L154" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change image pixel format. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setPixelFormat</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, newPf: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_setPixelRGBA8"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">setPixelRGBA8</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L199" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set pixel values depending on pixel format. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(<span class="SyntaxConstant">PF</span>: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span>) <span class="SyntaxFunction">setPixelRGBA8</span>(pixDst: ^<span class="SyntaxType">u8</span>, r: <span class="SyntaxType">u8</span>, g: <span class="SyntaxType">u8</span>, b: <span class="SyntaxType">u8</span>, _a: <span class="SyntaxType">u8</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toApplyKernel"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toApplyKernel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\applykernel.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Apply a kernel to the image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toApplyKernel</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, kernel: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>.<span class="SyntaxConstant"><a href="#Pixel_Image_Kernel">Kernel</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toCrop"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toCrop</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\crop.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Crop image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toCrop</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>, cropRect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toCrop</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">s32</span>, y: <span class="SyntaxType">s32</span>, w: <span class="SyntaxType">s32</span>, h: <span class="SyntaxType">s32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toFlip"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toFlip</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\flip.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flip image vertically. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toFlip</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toHICON"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toHICON</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.win32.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform an image to a windows icon. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toHICON</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HICON</span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toHalfSize"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toHalfSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\halfsize.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Divide image size by 2 with a bilinear 2x2 filter. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toHalfSize</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toMirror"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toMirror</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mirror.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flip image horizontally. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toMirror</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toMix"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toMix</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mix.swg#L81" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Mix with another image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toMix</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>, srcImage: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>, x: <span class="SyntaxType">s32</span>, y: <span class="SyntaxType">s32</span>, mode = <span class="SyntaxConstant"><a href="#Pixel_Image_MixMode">MixMode</a></span>.<span class="SyntaxConstant">Copy</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toResize"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toResize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\resize.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Resize image  Put <code class="incode">newWidth</code> or <code class="incode">newHeight</code> to 0 to keep aspect ratio. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toResize</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>, newWidth: <span class="SyntaxType">s32</span>, newHeight: <span class="SyntaxType">s32</span>, mode = <span class="SyntaxConstant"><a href="#Pixel_Image_ResizeMode">ResizeMode</a></span>.<span class="SyntaxConstant">Raw</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toSetPixelFormat"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toSetPixelFormat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\setpixelformat.swg#L167" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change image pixel format. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toSetPixelFormat</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>, newPf: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_toTurn"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">toTurn</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\turn.swg#L80" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Turn image by a given predefined angle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">toTurn</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>, angle: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>.<span class="SyntaxConstant"><a href="#Pixel_Image_TurnAngle">TurnAngle</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_turn"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">turn</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\turn.swg#L68" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Turn image by a given predefined angle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">turn</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, angle: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>.<span class="SyntaxConstant"><a href="#Pixel_Image_TurnAngle">TurnAngle</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Image_visitPixels"><span class="titletype">func</span> <span class="titlelight">Image.</span><span class="titlestrong">visitPixels</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L142" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Macro to visit all pixels of the image in parallel chunks. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxAttribute">#[<a href="swag.runtime.php#Swag_Macro">Swag.Macro</a>]</span>
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">visitPixels</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>, userData: *<span class="SyntaxType">void</span> = <span class="SyntaxKeyword">null</span>, stride: <span class="SyntaxType">s32</span> = <span class="SyntaxNumber">1</span>, stmt: <span class="SyntaxType">code</span>)</span></code>
</div>
<p style="white-space: break-spaces"> Exported variables:
 'pix'        address of the pixel
 'image'      the processed image
 'index'      the pixel index
 'x'          the pixel x coordinates
 'y'          the pixel y coordinates
 'userData'</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_InterpolationMode"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">InterpolationMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Linear</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_JoinStyle"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">JoinStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">None</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Bevel</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Round</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Miter</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MiterBevel</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Jpg_Decoder"><span class="titletype">struct</span> <span class="titlelight">Jpg.</span><span class="titlestrong">Decoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\jpg.swg#L141" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Jpg_Decoder_IImageDecoder_canDecode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">canDecode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\jpg.swg#L2053" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canDecode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Jpg_Decoder_IImageDecoder_decode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">decode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\jpg.swg#L2059" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">decode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, buffer: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>, options: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Jpg_EncodeOptions"><span class="titletype">struct</span> <span class="titlelight">Jpg.</span><span class="titlestrong">EncodeOptions</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\jpg.swg#L174" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">quality</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u8</span></span></td>
<td>Encoding quality between 1 and 100. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Jpg_Encoder"><span class="titletype">struct</span> <span class="titlelight">Jpg.</span><span class="titlestrong">Encoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\jpg.swg#L180" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Jpg_Encoder_IImageEncoder_canEncode"><span class="titletype">func</span> <span class="titlelight">IImageEncoder.</span><span class="titlestrong">canEncode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\jpg.swg#L731" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canEncode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Jpg_Encoder_IImageEncoder_encode"><span class="titletype">func</span> <span class="titlelight">IImageEncoder.</span><span class="titlestrong">encode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\jpg.swg#L737" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">encode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, result: *<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">ConcatBuffer</span>, image: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>, options: <span class="SyntaxType">any</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LineCapStyle"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">LineCapStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">None</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Triangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Round</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Square</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ArrowAnchor</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">SquareAnchor</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RoundAnchor</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">LinePath</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">elements</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Element">Element</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">startPoint</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">endPoint</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">isClosed</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">isDirty</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">serial</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">flattenQuality</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintQuality">PaintQuality</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">isPolyOnly</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">isFlatten</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bvMin</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bvMax</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">endPosBuffer</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">polygon</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Polygon">Polygon</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">points</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Point">Point</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">triangles</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Triangle">Triangle</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">edgeList</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Edge">Edge</a></span>)</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_LinePath_arcTo"><span class="SyntaxCode"><span class="SyntaxFunction">arcTo</span>()</spa</a></td>
<td>Add an arc from the previous point. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_bezierTo"><span class="SyntaxCode"><span class="SyntaxFunction">bezierTo</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Add a curve from the previous point to <code class="incode">pt</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_bezierTo"><span class="SyntaxCode"><span class="SyntaxFunction">bezierTo</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Add a curve from the previous point to <code class="incode">pt</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_clearCache"><span class="SyntaxCode"><span class="SyntaxFunction">clearCache</span>()</spa</a></td>
<td>Clear internal cache. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_close"><span class="SyntaxCode"><span class="SyntaxFunction">close</span>()</spa</a></td>
<td>Close the figure  Will add a line or a curve to the first point if necessary. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_curveTo"><span class="SyntaxCode"><span class="SyntaxFunction">curveTo</span>()</spa</a></td>
<td>Add a curve from the previous point to <code class="incode">pt</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_flatten"><span class="SyntaxCode"><span class="SyntaxFunction">flatten</span>()</spa</a></td>
<td>Convert the path to a list of points. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_lineTo"><span class="SyntaxCode"><span class="SyntaxFunction">lineTo</span>()</spa</a></td>
<td>Add a line from the previous point to <code class="incode">pt</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_setArc"><span class="SyntaxCode"><span class="SyntaxFunction">setArc</span>()</spa</a></td>
<td>Initialize the path with an arc. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_setCircle"><span class="SyntaxCode"><span class="SyntaxFunction">setCircle</span>()</spa</a></td>
<td>Initialize the path with a circle. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_setEllipse"><span class="SyntaxCode"><span class="SyntaxFunction">setEllipse</span>()</spa</a></td>
<td>Initialize the path with an ellipse. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_setRect"><span class="SyntaxCode"><span class="SyntaxFunction">setRect</span>()</spa</a></td>
<td>Initialize the path with a rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_setRoundRect"><span class="SyntaxCode"><span class="SyntaxFunction">setRoundRect</span>()</spa</a></td>
<td>Initialize the path with a round rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePath_start"><span class="SyntaxCode"><span class="SyntaxFunction">start</span>()</spa</a></td>
<td>Reset the path. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_arcTo"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">arcTo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L145" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add an arc from the previous point. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">arcTo</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, initAngle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>, maxAngle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_bezierTo"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">bezierTo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L112" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a curve from the previous point to <code class="incode">pt</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">bezierTo</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>, ctx: <span class="SyntaxType">f32</span>, cty: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>Add a curve from the previous point to <code class="incode">pt</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">bezierTo</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>, ct1x: <span class="SyntaxType">f32</span>, ct1y: <span class="SyntaxType">f32</span>, ct2x: <span class="SyntaxType">f32</span>, ct2y: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_clearCache"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">clearCache</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear internal cache. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">clearCache</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_close"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">close</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L174" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Close the figure  Will add a line or a curve to the first point if necessary. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">close</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_curveTo"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">curveTo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a curve from the previous point to <code class="incode">pt</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">curveTo</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_flatten"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">flatten</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L324" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert the path to a list of points. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">flatten</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, quality = <span class="SyntaxConstant"><a href="#Pixel_PaintQuality">PaintQuality</a></span>.<span class="SyntaxConstant">Normal</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_lineTo"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">lineTo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a line from the previous point to <code class="incode">pt</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">lineTo</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_setArc"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">setArc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L279" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with an arc. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setArc</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radius: <span class="SyntaxType">f32</span>, startAngle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>, endAngle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_setCircle"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">setCircle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L264" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with a circle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setCircle</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radius: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_setEllipse"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">setEllipse</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L253" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with an ellipse. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setEllipse</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_setRect"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">setRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with a rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_setRoundRect"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">setRoundRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L216" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with a round rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setRoundRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePath_start"><span class="titletype">func</span> <span class="titlelight">LinePath.</span><span class="titlestrong">start</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the path. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">start</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, startX: <span class="SyntaxType">f32</span>, startY: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePathList"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">LinePathList</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">paths</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bvMin</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bvMax</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_LinePathList_clean"><span class="SyntaxCode"><span class="SyntaxFunction">clean</span>()</spa</a></td>
<td>Clean all paths  They must before be flattened. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePathList_clear"><span class="SyntaxCode"><span class="SyntaxFunction">clear</span>()</spa</a></td>
<td>Remove all internal line paths. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePathList_count"><span class="SyntaxCode"><span class="SyntaxFunction">count</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_LinePathList_flatten"><span class="SyntaxCode"><span class="SyntaxFunction">flatten</span>()</spa</a></td>
<td>Flatten all paths. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePathList_newPath"><span class="SyntaxCode"><span class="SyntaxFunction">newPath</span>()</spa</a></td>
<td>Returns a new path. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePathList_offset"><span class="SyntaxCode"><span class="SyntaxFunction">offset</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Call fast offset on all paths  They must before be flattened. </td>
</tr>
<tr>
<td><a href="#Pixel_LinePathList_offset"><span class="SyntaxCode"><span class="SyntaxFunction">offset</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxConstant">JoinStyle</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Call quality offset on all paths  They must before be flattened. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePathList_clean"><span class="titletype">func</span> <span class="titlelight">LinePathList.</span><span class="titlestrong">clean</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clean all paths  They must before be flattened. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">clean</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePathList_clear"><span class="titletype">func</span> <span class="titlelight">LinePathList.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove all internal line paths. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">clear</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePathList_count"><span class="titletype">func</span> <span class="titlelight">LinePathList.</span><span class="titlestrong">count</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">count</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">u64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePathList_flatten"><span class="titletype">func</span> <span class="titlelight">LinePathList.</span><span class="titlestrong">flatten</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flatten all paths. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">flatten</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, quality: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintQuality">PaintQuality</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePathList_newPath"><span class="titletype">func</span> <span class="titlelight">LinePathList.</span><span class="titlestrong">newPath</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a new path. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">newPath</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_LinePathList_offset"><span class="titletype">func</span> <span class="titlelight">LinePathList.</span><span class="titlestrong">offset</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Call fast offset on all paths  They must before be flattened. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">offset</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, value: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>Call quality offset on all paths  They must before be flattened. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">offset</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, value: <span class="SyntaxType">f32</span>, joinStyle: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_JoinStyle">JoinStyle</a></span>, toler: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">0.5</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_NativeRenderOgl"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">NativeRenderOgl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_NativeRenderOgl_createContext"><span class="SyntaxCode"><span class="SyntaxFunction">createContext</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxConstant">HDC</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxConstant">HGLRC</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_NativeRenderOgl_createContext"><span class="SyntaxCode"><span class="SyntaxFunction">createContext</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxConstant">HWND</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxType">s32</span>, <span class="SyntaxConstant">HGLRC</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_NativeRenderOgl_dropContext"><span class="SyntaxCode"><span class="SyntaxFunction">dropContext</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_NativeRenderOgl_setCurrentContext"><span class="SyntaxCode"><span class="SyntaxFunction">setCurrentContext</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_NativeRenderOgl_swapBuffers"><span class="SyntaxCode"><span class="SyntaxFunction">swapBuffers</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_NativeRenderOgl_createContext"><span class="titletype">func</span> <span class="titlelight">NativeRenderOgl.</span><span class="titlestrong">createContext</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createContext</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, hdc: <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HDC</span>, width: <span class="SyntaxType">s32</span>, height: <span class="SyntaxType">s32</span>, mainRC: <span class="SyntaxConstant">Ogl</span>.<span class="SyntaxConstant">HGLRC</span> = <span class="SyntaxKeyword">null</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span> <span class="SyntaxKeyword">throw</span>
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createContext</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, hWnd: <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HWND</span>, width: <span class="SyntaxType">s32</span>, height: <span class="SyntaxType">s32</span>, mainRC: <span class="SyntaxConstant">Ogl</span>.<span class="SyntaxConstant">HGLRC</span> = <span class="SyntaxKeyword">null</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_NativeRenderOgl_dropContext"><span class="titletype">func</span> <span class="titlelight">NativeRenderOgl.</span><span class="titlestrong">dropContext</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">dropContext</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rc: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_NativeRenderOgl_setCurrentContext"><span class="titletype">func</span> <span class="titlelight">NativeRenderOgl.</span><span class="titlestrong">setCurrentContext</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setCurrentContext</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rc: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_NativeRenderOgl_swapBuffers"><span class="titletype">func</span> <span class="titlelight">NativeRenderOgl.</span><span class="titlestrong">swapBuffers</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L62" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">swapBuffers</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rc: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_PaintParams"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">PaintParams</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Zero</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Antialiased</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Default</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_PaintQuality"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">PaintQuality</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L57" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Draft</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Normal</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_PaintState"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">PaintState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L80" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">paintParams</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">paintQuality</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintQuality">PaintQuality</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">transform</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Transform2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">blendingMode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_BlendingMode">BlendingMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">interpolationMode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_InterpolationMode">InterpolationMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">clippingRect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">stackClipRect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">clippingRectOn</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">colorMask</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ColorMask">ColorMask</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Painter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L136" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">commandBuffer</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Command">Command</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">vertexBuffer</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_VertexLayout">VertexLayout</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">triangulateIdx</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxType">s32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">fake</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">sharedSolidPen</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">sharedSolidBrush</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">sharedLinePath</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">sharedLinePathList</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">sharedRoundTmp</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'({factor: <span class="SyntaxType">f32</span>, pos: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span>})</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">curState</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintState">PaintState</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">stackState</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintState">PaintState</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">stackTransform</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Transform2</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">lastTexture0Type</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_BrushType">BrushType</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">stackRT</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">curRT</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">lastTexture0Handle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">hasTextureFont</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">drawingBegin</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">overlapMode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">canSetTexture0</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Painter_addParams"><span class="SyntaxCode"><span class="SyntaxFunction">addParams</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_begin"><span class="SyntaxCode"><span class="SyntaxFunction">begin</span>()</spa</a></td>
<td>Call this before drawing. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_bindRenderTarget"><span class="SyntaxCode"><span class="SyntaxFunction">bindRenderTarget</span>()</spa</a></td>
<td>Bind a render target. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_capStyleExtent"><span class="SyntaxCode"><span class="SyntaxFunction">capStyleExtent</span>()</spa</a></td>
<td>Returns the extent in both directions of a given LineCapStyle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_clear"><span class="SyntaxCode"><span class="SyntaxFunction">clear</span>()</spa</a></td>
<td>Clear the rendering surface. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_computeLayout"><span class="SyntaxCode"><span class="SyntaxFunction">computeLayout</span>()</spa</a></td>
<td>Recompute the layout for a given rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawArc"><span class="SyntaxCode"><span class="SyntaxFunction">drawArc</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Angle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Angle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Draw an arc between two angles. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawArc"><span class="SyntaxCode"><span class="SyntaxFunction">drawArc</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Angle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Angle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawCircle"><span class="SyntaxCode"><span class="SyntaxFunction">drawCircle</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Draw an empty circle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawCircle"><span class="SyntaxCode"><span class="SyntaxFunction">drawCircle</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawEllipse"><span class="SyntaxCode"><span class="SyntaxFunction">drawEllipse</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Draw an empty ellipse. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawEllipse"><span class="SyntaxCode"><span class="SyntaxFunction">drawEllipse</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawLine"><span class="SyntaxCode"><span class="SyntaxFunction">drawLine</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Vector2</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Vector2</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td>Draw a line. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawLine"><span class="SyntaxCode"><span class="SyntaxFunction">drawLine</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawLine"><span class="SyntaxCode"><span class="SyntaxFunction">drawLine</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawPath"><span class="SyntaxCode"><span class="SyntaxFunction">drawPath</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePath</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Draw the line path. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawPath"><span class="SyntaxCode"><span class="SyntaxFunction">drawPath</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePath</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td>Draw the line path with the given <code class="incode">pen</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawPath"><span class="SyntaxCode"><span class="SyntaxFunction">drawPath</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePathList</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxConstant">DrawPathListMode</span>)</span></a></td>
<td>Draw the line path. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawPath"><span class="SyntaxCode"><span class="SyntaxFunction">drawPath</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePathList</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>, <span class="SyntaxConstant">DrawPathListMode</span>)</span></a></td>
<td>Draw the line path. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawRect"><span class="SyntaxCode"><span class="SyntaxFunction">drawRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawRect"><span class="SyntaxCode"><span class="SyntaxFunction">drawRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawRect"><span class="SyntaxCode"><span class="SyntaxFunction">drawRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Draw an empty rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawRect"><span class="SyntaxCode"><span class="SyntaxFunction">drawRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawRichString"><span class="SyntaxCode"><span class="SyntaxFunction">drawRichString</span>()</spa</a></td>
<td>Draw a rich string. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawRoundRect"><span class="SyntaxCode"><span class="SyntaxFunction">drawRoundRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawRoundRect"><span class="SyntaxCode"><span class="SyntaxFunction">drawRoundRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawRoundRect"><span class="SyntaxCode"><span class="SyntaxFunction">drawRoundRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Draw an empty round rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawRoundRect"><span class="SyntaxCode"><span class="SyntaxFunction">drawRoundRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Pen</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawString"><span class="SyntaxCode"><span class="SyntaxFunction">drawString</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">string</span>, *<span class="SyntaxConstant">Font</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>, <span class="SyntaxConstant">UnderlineStyle</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Draw a simple string. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawString"><span class="SyntaxCode"><span class="SyntaxFunction">drawString</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">string</span>, *<span class="SyntaxConstant">Font</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>, <span class="SyntaxConstant">UnderlineStyle</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Draw a simple string. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawStringCenter"><span class="SyntaxCode"><span class="SyntaxFunction">drawStringCenter</span>()</spa</a></td>
<td>Draw a simple string (centered). </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawTexture"><span class="SyntaxCode"><span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Texture</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawTexture"><span class="SyntaxCode"><span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Texture</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawTexture"><span class="SyntaxCode"><span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Texture</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawTexture"><span class="SyntaxCode"><span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Texture</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawTexture"><span class="SyntaxCode"><span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Texture</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td>Draw a portion of an image. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_drawTexture"><span class="SyntaxCode"><span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Texture</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td>Draw a texture image. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_end"><span class="SyntaxCode"><span class="SyntaxFunction">end</span>()</spa</a></td>
<td>Call this after drawing. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_endClippingRegion"><span class="SyntaxCode"><span class="SyntaxFunction">endClippingRegion</span>()</spa</a></td>
<td>Stop painting in the clipping buffer. Back to normal. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillCircle"><span class="SyntaxCode"><span class="SyntaxFunction">fillCircle</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>)</span></a></td>
<td>Draw a solid circle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillCircle"><span class="SyntaxCode"><span class="SyntaxFunction">fillCircle</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillEllipse"><span class="SyntaxCode"><span class="SyntaxFunction">fillEllipse</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>)</span></a></td>
<td>Draw a solid ellipse. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillEllipse"><span class="SyntaxCode"><span class="SyntaxFunction">fillEllipse</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillPath"><span class="SyntaxCode"><span class="SyntaxFunction">fillPath</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePath</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>)</span></a></td>
<td>Fill the line path with the given <code class="incode">brush</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillPath"><span class="SyntaxCode"><span class="SyntaxFunction">fillPath</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePath</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td>Fill the line path. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillPath"><span class="SyntaxCode"><span class="SyntaxFunction">fillPath</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePathList</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>)</span></a></td>
<td>Fill the list of paths  clockwise = fill, anti-clockwise = hole. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillPath"><span class="SyntaxCode"><span class="SyntaxFunction">fillPath</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePathList</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td>Fill the line path. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillPolygon"><span class="SyntaxCode"><span class="SyntaxFunction">fillPolygon</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePath</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>)</span></a></td>
<td>Fill the polygon with the given <code class="incode">brush</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillPolygon"><span class="SyntaxCode"><span class="SyntaxFunction">fillPolygon</span>(<span class="SyntaxKeyword">self</span>, *<span class="SyntaxConstant">LinePath</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td>Fill the polygon with the given color. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillRect"><span class="SyntaxCode"><span class="SyntaxFunction">fillRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillRect"><span class="SyntaxCode"><span class="SyntaxFunction">fillRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillRect"><span class="SyntaxCode"><span class="SyntaxFunction">fillRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>)</span></a></td>
<td>Draw a filled rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillRect"><span class="SyntaxCode"><span class="SyntaxFunction">fillRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillRoundRect"><span class="SyntaxCode"><span class="SyntaxFunction">fillRoundRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillRoundRect"><span class="SyntaxCode"><span class="SyntaxFunction">fillRoundRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillRoundRect"><span class="SyntaxCode"><span class="SyntaxFunction">fillRoundRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Brush</span>)</span></a></td>
<td>Draw a filled round rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_fillRoundRect"><span class="SyntaxCode"><span class="SyntaxFunction">fillRoundRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Color</span>)</span></a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_getClippingRect"><span class="SyntaxCode"><span class="SyntaxFunction">getClippingRect</span>()</spa</a></td>
<td>Get the current clipping rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_getGlyphs"><span class="SyntaxCode"><span class="SyntaxFunction">getGlyphs</span>(<span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Array</span>'(<span class="SyntaxType">rune</span>), *<span class="SyntaxConstant">Font</span>)</span></a></td>
<td>Returns all the glyphs for a given string and font. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_getGlyphs"><span class="SyntaxCode"><span class="SyntaxFunction">getGlyphs</span>(<span class="SyntaxType">string</span>, *<span class="SyntaxConstant">Font</span>)</span></a></td>
<td>Returns all the glyphs for a given string and font. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_getParams"><span class="SyntaxCode"><span class="SyntaxFunction">getParams</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_getQuality"><span class="SyntaxCode"><span class="SyntaxFunction">getQuality</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_getTransform"><span class="SyntaxCode"><span class="SyntaxFunction">getTransform</span>()</spa</a></td>
<td>Returns the current tranformation. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_isEmptyClippingRect"><span class="SyntaxCode"><span class="SyntaxFunction">isEmptyClippingRect</span>()</spa</a></td>
<td>Returns true if the current clipping rectangle is empty (no drawing !). </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_measureRune"><span class="SyntaxCode"><span class="SyntaxFunction">measureRune</span>()</spa</a></td>
<td>Get a rune size for a given font. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_measureString"><span class="SyntaxCode"><span class="SyntaxFunction">measureString</span>(<span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Array</span>'(<span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">GlyphDesc</span>))</span></a></td>
<td>Returns the bounding rectangle if a given text, at a given position. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_measureString"><span class="SyntaxCode"><span class="SyntaxFunction">measureString</span>(<span class="SyntaxType">string</span>, *<span class="SyntaxConstant">Font</span>)</span></a></td>
<td>Returns the metrics of a given text. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_popClippingRect"><span class="SyntaxCode"><span class="SyntaxFunction">popClippingRect</span>()</spa</a></td>
<td>Restore the original clipping rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_popState"><span class="SyntaxCode"><span class="SyntaxFunction">popState</span>()</spa</a></td>
<td>Restore the last paint state. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_popTransform"><span class="SyntaxCode"><span class="SyntaxFunction">popTransform</span>()</spa</a></td>
<td>Restore the pushed transformation. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_pushClippingRect"><span class="SyntaxCode"><span class="SyntaxFunction">pushClippingRect</span>()</spa</a></td>
<td>Interface the given rectangle with the current clipping rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_pushClippingSurfaceRect"><span class="SyntaxCode"><span class="SyntaxFunction">pushClippingSurfaceRect</span>()</spa</a></td>
<td>Interface the given rectangle with the current clipping rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_pushState"><span class="SyntaxCode"><span class="SyntaxFunction">pushState</span>()</spa</a></td>
<td>Save the current paint state. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_pushTransform"><span class="SyntaxCode"><span class="SyntaxFunction">pushTransform</span>()</spa</a></td>
<td>Push the current transformation. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_releaseRenderTarget"><span class="SyntaxCode"><span class="SyntaxFunction">releaseRenderTarget</span>()</spa</a></td>
<td>Release the render target. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_removeParams"><span class="SyntaxCode"><span class="SyntaxFunction">removeParams</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_resetClippingRect"><span class="SyntaxCode"><span class="SyntaxFunction">resetClippingRect</span>()</spa</a></td>
<td>Reset the current clipping rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_resetClippingRegion"><span class="SyntaxCode"><span class="SyntaxFunction">resetClippingRegion</span>()</spa</a></td>
<td>Reset clipping buffer to its default value (no more clipping). </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_resetState"><span class="SyntaxCode"><span class="SyntaxFunction">resetState</span>()</spa</a></td>
<td>Reset the current painting state. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_resetTransform"><span class="SyntaxCode"><span class="SyntaxFunction">resetTransform</span>()</spa</a></td>
<td>Reset the paint position, rotation, scale. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_rotateTransform"><span class="SyntaxCode"><span class="SyntaxFunction">rotateTransform</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Angle</span>)</span></a></td>
<td>Set the paint rotation. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_rotateTransform"><span class="SyntaxCode"><span class="SyntaxFunction">rotateTransform</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Angle</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Set the paint rotation. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_rotateTransformInPlace"><span class="SyntaxCode"><span class="SyntaxFunction">rotateTransformInPlace</span>()</spa</a></td>
<td>Set the paint rotation. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_scaleTransform"><span class="SyntaxCode"><span class="SyntaxFunction">scaleTransform</span>()</spa</a></td>
<td>Set the paint scale. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setAntialiased"><span class="SyntaxCode"><span class="SyntaxFunction">setAntialiased</span>()</spa</a></td>
<td>Set antialiased mode on/off. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setBlendingMode"><span class="SyntaxCode"><span class="SyntaxFunction">setBlendingMode</span>()</spa</a></td>
<td>Set the current blending mode. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setClippingRect"><span class="SyntaxCode"><span class="SyntaxFunction">setClippingRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Rectangle</span>)</span></a></td>
<td>Set the current clipping rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setClippingRect"><span class="SyntaxCode"><span class="SyntaxFunction">setClippingRect</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Set the current clipping rectangle. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setClippingRegionMode"><span class="SyntaxCode"><span class="SyntaxFunction">setClippingRegionMode</span>()</spa</a></td>
<td>Set the clipping region mode. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setColorMask"><span class="SyntaxCode"><span class="SyntaxFunction">setColorMask</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">bool</span>, <span class="SyntaxType">bool</span>, <span class="SyntaxType">bool</span>, <span class="SyntaxType">bool</span>)</span></a></td>
<td>Set color mask. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setColorMask"><span class="SyntaxCode"><span class="SyntaxFunction">setColorMask</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">ColorMask</span>)</span></a></td>
<td>Set color mask. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setColorMaskAlpha"><span class="SyntaxCode"><span class="SyntaxFunction">setColorMaskAlpha</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setColorMaskColor"><span class="SyntaxCode"><span class="SyntaxFunction">setColorMaskColor</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setColorMaskFull"><span class="SyntaxCode"><span class="SyntaxFunction">setColorMaskFull</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setInterpolationMode"><span class="SyntaxCode"><span class="SyntaxFunction">setInterpolationMode</span>()</spa</a></td>
<td>Set the texture interpolation mode. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setParams"><span class="SyntaxCode"><span class="SyntaxFunction">setParams</span>()</spa</a></td>
<td>Set the paint parameters. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setQuality"><span class="SyntaxCode"><span class="SyntaxFunction">setQuality</span>()</spa</a></td>
<td>Set the paint quality. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setShader"><span class="SyntaxCode"><span class="SyntaxFunction">setShader</span>()</spa</a></td>
<td>Set the current shader. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setShaderParam"><span class="SyntaxCode"><span class="SyntaxFunction">setShaderParam</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxConstant">ShaderParamHandle</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Set a shader parameter. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setShaderParam"><span class="SyntaxCode"><span class="SyntaxFunction">setShaderParam</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxConstant">ShaderParamHandle</span>, <span class="SyntaxType">s32</span>)</span></a></td>
<td>Set a shader parameter. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setState"><span class="SyntaxCode"><span class="SyntaxFunction">setState</span>()</spa</a></td>
<td>Set the current painting state. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_setTransform"><span class="SyntaxCode"><span class="SyntaxFunction">setTransform</span>()</spa</a></td>
<td>Set the paint transformation. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_startClippingRegion"><span class="SyntaxCode"><span class="SyntaxFunction">startClippingRegion</span>()</spa</a></td>
<td>Start painting in the clipping buffer. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_translateTransform"><span class="SyntaxCode"><span class="SyntaxFunction">translateTransform</span>()</spa</a></td>
<td>Set the paint position. </td>
</tr>
<tr>
<td><a href="#Pixel_Painter_unbindRenderTarget"><span class="SyntaxCode"><span class="SyntaxFunction">unbindRenderTarget</span>()</spa</a></td>
<td>Bind a render target. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_addParams"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">addParams</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L480" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">addParams</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, add: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_begin"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">begin</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L399" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Call this before drawing. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">begin</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_bindRenderTarget"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">bindRenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L742" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Bind a render target. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">bindRenderTarget</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, tgt: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span>, paintAlpha = <span class="SyntaxKeyword">false</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_capStyleExtent"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">capStyleExtent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawpath.swg#L753" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the extent in both directions of a given LineCapStyle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">capStyleExtent</span>(capStyle: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LineCapStyle">LineCapStyle</a></span>, lineSize: <span class="SyntaxType">f32</span>, capScale: <span class="SyntaxType">f32</span>)-&gt;{w: <span class="SyntaxType">f32</span>, l: <span class="SyntaxType">f32</span>}</span></code>
</div>
<ul>
<li><code class="incode">lineSize</code> is the supposed line drawing size</li>
<li><code class="incode">capScale</code> is the supposed LineCapStyle scale</li>
</ul>
<p> Returns the width and the length. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_clear"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L423" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear the rendering surface. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">clear</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_computeLayout"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">computeLayout</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L244" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Recompute the layout for a given rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">computeLayout</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, pos: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, text: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RichString">RichString</a></span>, format: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RichStringFormat">RichStringFormat</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawArc"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawArc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an arc between two angles. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawArc</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radius: <span class="SyntaxType">f32</span>, startAngle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>, endAngle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawArc</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radius: <span class="SyntaxType">f32</span>, startAngle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>, endAngle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawCircle"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawCircle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an empty circle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawCircle</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radius: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawCircle</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radius: <span class="SyntaxType">f32</span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawEllipse"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawEllipse</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an empty ellipse. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawEllipse</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawEllipse</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawLine"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawline.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a line. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawLine</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, start: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span>, end: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawLine</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, startX: <span class="SyntaxType">f32</span>, startY: <span class="SyntaxType">f32</span>, endX: <span class="SyntaxType">f32</span>, endY: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawLine</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, startX: <span class="SyntaxType">f32</span>, startY: <span class="SyntaxType">f32</span>, endX: <span class="SyntaxType">f32</span>, endY: <span class="SyntaxType">f32</span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawPath"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawPath</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawpath.swg#L669" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw the line path. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawPath</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, path: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>Draw the line path with the given <code class="incode">pen</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawPath</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, path: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>Draw the line path. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawPath</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, pathList: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span>, mode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DrawPathListMode">DrawPathListMode</a></span> = .<span class="SyntaxConstant">Separate</span>)</span></code>
</div>
<p>Draw the line path. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawPath</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, pathList: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>, mode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DrawPathListMode">DrawPathListMode</a></span> = .<span class="SyntaxConstant">Separate</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawrect.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an empty rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawRichString"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawRichString</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L429" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a rich string. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawRichString</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, pos: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, text: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RichString">RichString</a></span>, format: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RichStringFormat">RichStringFormat</a></span>, forceCompute = <span class="SyntaxKeyword">false</span>)</span></code>
</div>
<p> A rich string can be multiline, and accepts  The layout will be computed at the first call, and each time <code class="incode">pos</code> has changed </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawRoundRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawRoundRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawrect.swg#L168" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an empty round rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawRoundRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawRoundRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawRoundRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawRoundRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, pen: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawString"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawString</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L139" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a simple string. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawString</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, text: <span class="SyntaxType">string</span>, font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>, under = <span class="SyntaxConstant"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span>.<span class="SyntaxConstant">None</span>, zoom = <span class="SyntaxNumber">1.0</span>)</span></code>
</div>
<p>Draw a simple string. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawString</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, text: <span class="SyntaxType">string</span>, font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, under = <span class="SyntaxConstant"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span>.<span class="SyntaxConstant">None</span>, zoom = <span class="SyntaxNumber">1.0</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawStringCenter"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawStringCenter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L120" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a simple string (centered). </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawStringCenter</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>, text: <span class="SyntaxType">string</span>, font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, under = <span class="SyntaxConstant"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span>.<span class="SyntaxConstant">None</span>, zoom = <span class="SyntaxNumber">1.0</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_drawTexture"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">drawTexture</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawtexture.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a portion of an image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, dstRect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, srcRect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span> = <span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>.<span class="SyntaxConstant">White</span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span> = <span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>.<span class="SyntaxConstant">White</span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>, srcRect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span> = <span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>.<span class="SyntaxConstant">White</span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>, texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span> = <span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>.<span class="SyntaxConstant">White</span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, srcRect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span> = <span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>.<span class="SyntaxConstant">White</span>)</span></code>
</div>
<p>Draw a texture image. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">drawTexture</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span> = <span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span>.<span class="SyntaxConstant">White</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_end"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">end</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L416" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Call this after drawing. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">end</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_endClippingRegion"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">endClippingRegion</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L699" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Stop painting in the clipping buffer. Back to normal. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">endClippingRegion</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_fillCircle"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">fillCircle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a solid circle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillCircle</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radius: <span class="SyntaxType">f32</span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillCircle</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radius: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_fillEllipse"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">fillEllipse</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L104" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a solid ellipse. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillEllipse</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillEllipse</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_fillPath"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">fillPath</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\fillpath.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill the line path with the given <code class="incode">brush</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillPath</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, path: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<p>Fill the line path. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillPath</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, path: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>Fill the list of paths  clockwise = fill, anti-clockwise = hole. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillPath</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, pathList: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<p>Fill the line path. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillPath</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, pathList: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_fillPolygon"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">fillPolygon</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\fillpath.swg#L263" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill the polygon with the given <code class="incode">brush</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillPolygon</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, path: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<p>Fill the polygon with the given color. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillPolygon</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, path: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_fillRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">fillRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawrect.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a filled rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_fillRoundRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">fillRoundRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawrect.swg#L185" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a filled round rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillRoundRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillRoundRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)
<span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillRoundRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, brush: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">fillRoundRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, left: <span class="SyntaxType">f32</span>, top: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>, radiusX: <span class="SyntaxType">f32</span>, radiusY: <span class="SyntaxType">f32</span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_getClippingRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">getClippingRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L671" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the current clipping rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getClippingRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_getGlyphs"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">getGlyphs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns all the glyphs for a given string and font. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getGlyphs</span>(text: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxType">rune</span>), font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span>)</span></code>
</div>
<p>Returns all the glyphs for a given string and font. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getGlyphs</span>(text: <span class="SyntaxType">string</span>, font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_getParams"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">getParams</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L500" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getParams</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_getQuality"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">getQuality</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L502" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getQuality</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintQuality">PaintQuality</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_getTransform"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">getTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L589" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the current tranformation. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Transform2</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_isEmptyClippingRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">isEmptyClippingRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L679" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the current clipping rectangle is empty (no drawing !). </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">isEmptyClippingRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_measureRune"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">measureRune</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L86" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get a rune size for a given font. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">measureRune</span>(c: <span class="SyntaxType">rune</span>, font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_measureString"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">measureString</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L103" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the bounding rectangle if a given text, at a given position. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">measureString</span>(text: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span>))-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_StringMetrics">StringMetrics</a></span></span></code>
</div>
<p>Returns the metrics of a given text. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">measureString</span>(text: <span class="SyntaxType">string</span>, font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_StringMetrics">StringMetrics</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_popClippingRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">popClippingRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L657" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Restore the original clipping rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">popClippingRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_popState"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">popState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L467" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Restore the last paint state. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">popState</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_popTransform"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">popTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L568" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Restore the pushed transformation. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">popTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_pushClippingRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">pushClippingRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L636" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Interface the given rectangle with the current clipping rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">pushClippingRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, isSurfacePos = <span class="SyntaxKeyword">false</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_pushClippingSurfaceRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">pushClippingSurfaceRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L630" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Interface the given rectangle with the current clipping rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">pushClippingSurfaceRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_pushState"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">pushState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L461" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Save the current paint state. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">pushState</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_pushTransform"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">pushTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L562" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Push the current transformation. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">pushTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_releaseRenderTarget"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">releaseRenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L771" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the render target. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">releaseRenderTarget</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, tgt: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_removeParams"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">removeParams</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L485" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">removeParams</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, remove: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_resetClippingRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">resetClippingRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L619" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the current clipping rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">resetClippingRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_resetClippingRegion"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">resetClippingRegion</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L705" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset clipping buffer to its default value (no more clipping). </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">resetClippingRegion</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_resetState"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">resetState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L430" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the current painting state. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">resetState</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_resetTransform"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">resetTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L574" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the paint position, rotation, scale. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">resetTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_rotateTransform"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">rotateTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L521" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint rotation. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">rotateTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, angle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>)</span></code>
</div>
<p>Set the paint rotation. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">rotateTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, angle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_rotateTransformInPlace"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">rotateTransformInPlace</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L532" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint rotation. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">rotateTransformInPlace</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, angle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>)</span></code>
</div>
<p> Will rotate around the current transformation position. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_scaleTransform"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">scaleTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L552" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint scale. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">scaleTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setAntialiased"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setAntialiased</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L491" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set antialiased mode on/off. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setAntialiased</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, aa = <span class="SyntaxKeyword">true</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setBlendingMode"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setBlendingMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L711" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current blending mode. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setBlendingMode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, mode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_BlendingMode">BlendingMode</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setClippingRect"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setClippingRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L608" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current clipping rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setClippingRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>)</span></code>
</div>
<p>Set the current clipping rectangle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setClippingRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>, width: <span class="SyntaxType">f32</span>, height: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setClippingRegionMode"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setClippingRegionMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L692" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the clipping region mode. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setClippingRegionMode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, mode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ClippingMode">ClippingMode</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setColorMask"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setColorMask</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L727" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set color mask. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setColorMask</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, r: <span class="SyntaxType">bool</span>, g: <span class="SyntaxType">bool</span>, b: <span class="SyntaxType">bool</span>, a: <span class="SyntaxType">bool</span>)</span></code>
</div>
<p>Set color mask. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setColorMask</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, cm: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ColorMask">ColorMask</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setColorMaskAlpha"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setColorMaskAlpha</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L739" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setColorMaskAlpha</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setColorMaskColor"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setColorMaskColor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L737" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setColorMaskColor</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setColorMaskFull"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setColorMaskFull</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L738" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setColorMaskFull</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setInterpolationMode"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setInterpolationMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L596" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the texture interpolation mode. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setInterpolationMode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, mode: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_InterpolationMode">InterpolationMode</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setParams"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setParams</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L474" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint parameters. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setParams</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, add: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span>, remove: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span> = .<span class="SyntaxConstant">Zero</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setQuality"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setQuality</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L505" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint quality. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setQuality</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, quality: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintQuality">PaintQuality</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setShader"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setShader</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L780" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current shader. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setShader</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, shader: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span> = <span class="SyntaxKeyword">null</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setShaderParam"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setShaderParam</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L796" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set a shader parameter. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setShaderParam</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, param: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>, val: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>Set a shader parameter. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setShaderParam</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, param: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>, val: <span class="SyntaxType">s32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setState"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L446" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current painting state. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setState</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, state: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PaintState">PaintState</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_setTransform"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">setTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L581" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint transformation. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, tr: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Transform2</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_startClippingRegion"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">startClippingRegion</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L685" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Start painting in the clipping buffer. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">startClippingRegion</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, mode = <span class="SyntaxConstant"><a href="#Pixel_ClippingMode">ClippingMode</a></span>.<span class="SyntaxConstant">Set</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_translateTransform"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">translateTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L511" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint position. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">translateTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Painter_unbindRenderTarget"><span class="titletype">func</span> <span class="titlelight">Painter.</span><span class="titlestrong">unbindRenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L756" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Bind a render target. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">unbindRenderTarget</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Pen"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Pen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">size</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">brush</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pattern</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxType">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">dashBeginCapStyle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DashCapStyle">DashCapStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">dashEndCapStyle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DashCapStyle">DashCapStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">borderPos</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_BorderPos">BorderPos</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">capStartScaleX</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">capStartScaleY</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">capEndScaleX</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">capEndScaleY</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">joinMiterLimit</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">plotOffset</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">dashStyle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DashStyle">DashStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">beginCapStyle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LineCapStyle">LineCapStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">endCapStyle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LineCapStyle">LineCapStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">joinStyle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_JoinStyle">JoinStyle</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Pen_createDash"><span class="SyntaxCode"><span class="SyntaxFunction">createDash</span>()</spa</a></td>
<td>Returns a dash pen. </td>
</tr>
<tr>
<td><a href="#Pixel_Pen_createHatch"><span class="SyntaxCode"><span class="SyntaxFunction">createHatch</span>()</spa</a></td>
<td>Return a predefined hatch pen. </td>
</tr>
<tr>
<td><a href="#Pixel_Pen_createSolid"><span class="SyntaxCode"><span class="SyntaxFunction">createSolid</span>()</spa</a></td>
<td>Returns a solid color pen. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Pen_createDash"><span class="titletype">func</span> <span class="titlelight">Pen.</span><span class="titlestrong">createDash</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a dash pen. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createDash</span>(style: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DashStyle">DashStyle</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, size: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Pen_createHatch"><span class="titletype">func</span> <span class="titlelight">Pen.</span><span class="titlestrong">createHatch</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L95" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Return a predefined hatch pen. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createHatch</span>(type: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_HatchStyle">HatchStyle</a></span>, color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, size: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Pen_createSolid"><span class="titletype">func</span> <span class="titlelight">Pen.</span><span class="titlestrong">createSolid</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a solid color pen. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createSolid</span>(color: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>, size: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">1</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_PixelFormat"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">PixelFormat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Desribe the layout format of one image pixel. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BGR8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BGRA8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RGB8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">RGBA8</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_PixelFormat_bitDepth"><span class="titletype">func</span> <span class="titlelight">PixelFormat.</span><span class="titlestrong">bitDepth</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the bit depth of each channel. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">bitDepth</span>(<span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_PixelFormat_bpp"><span class="titletype">func</span> <span class="titlelight">PixelFormat.</span><span class="titlestrong">bpp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the bpp (bits per pixel) of the given pixel format. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">bpp</span>(<span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_PixelFormat_channels"><span class="titletype">func</span> <span class="titlelight">PixelFormat.</span><span class="titlestrong">channels</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the number of image channels. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">channels</span>(<span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">u8</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_PixelFormat_hasAlpha"><span class="titletype">func</span> <span class="titlelight">PixelFormat.</span><span class="titlestrong">hasAlpha</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the given pixel format has an alpha channel. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">hasAlpha</span>(<span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Png_Decoder"><span class="titletype">struct</span> <span class="titlelight">Png.</span><span class="titlestrong">Decoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\png.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Png_Decoder_IImageDecoder_canDecode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">canDecode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\png.swg#L694" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canDecode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Png_Decoder_IImageDecoder_decode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">decode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\png.swg#L699" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">decode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, buffer: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>, options: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Png_EncodeOptions"><span class="titletype">struct</span> <span class="titlelight">Png.</span><span class="titlestrong">EncodeOptions</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\png.swg#L582" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">compressionLevel</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Compress</span>.<span class="SyntaxConstant">Deflate</span>.<span class="SyntaxConstant">CompressionLevel</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">filtering</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Png_Encoder"><span class="titletype">struct</span> <span class="titlelight">Png.</span><span class="titlestrong">Encoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\png.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Png_Encoder_IImageEncoder_canEncode"><span class="titletype">func</span> <span class="titlelight">IImageEncoder.</span><span class="titlestrong">canEncode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\png.swg#L590" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canEncode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Png_Encoder_IImageEncoder_encode"><span class="titletype">func</span> <span class="titlelight">IImageEncoder.</span><span class="titlestrong">encode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\png.swg#L595" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">encode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, result: *<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">ConcatBuffer</span>, image: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>, options: <span class="SyntaxType">any</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Edge"><span class="titletype">struct</span> <span class="titlelight">Poly2Tri.</span><span class="titlestrong">Edge</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">p</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">q</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Point"><span class="titletype">struct</span> <span class="titlelight">Poly2Tri.</span><span class="titlestrong">Point</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">using</span> v</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">edgeList</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Edge">Edge</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">border</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">2</span>] *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Edge">Edge</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Tesselate"><span class="titletype">struct</span> <span class="titlelight">Poly2Tri.</span><span class="titlestrong">Tesselate</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L459" class="src">[src]</a></td>
</tr>
</table>
</p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Poly2Tri_Tesselate_addPoint"><span class="SyntaxCode"><span class="SyntaxFunction">addPoint</span>()</spa</a></td>
<td>Register a new point. </td>
</tr>
<tr>
<td><a href="#Pixel_Poly2Tri_Tesselate_clear"><span class="SyntaxCode"><span class="SyntaxFunction">clear</span>()</spa</a></td>
<td>Clear content. </td>
</tr>
<tr>
<td><a href="#Pixel_Poly2Tri_Tesselate_endPolyLine"><span class="SyntaxCode"><span class="SyntaxFunction">endPolyLine</span>()</spa</a></td>
<td>To call to register the polyline. </td>
</tr>
<tr>
<td><a href="#Pixel_Poly2Tri_Tesselate_getTriangles"><span class="SyntaxCode"><span class="SyntaxFunction">getTriangles</span>()</spa</a></td>
<td>Returns the list of triangles. </td>
</tr>
<tr>
<td><a href="#Pixel_Poly2Tri_Tesselate_startPolyLine"><span class="SyntaxCode"><span class="SyntaxFunction">startPolyLine</span>()</spa</a></td>
<td>To call before adding a polyline. </td>
</tr>
<tr>
<td><a href="#Pixel_Poly2Tri_Tesselate_triangulate"><span class="SyntaxCode"><span class="SyntaxFunction">triangulate</span>()</spa</a></td>
<td>Triangulate. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Tesselate_addPoint"><span class="titletype">func</span> <span class="titlelight">Tesselate.</span><span class="titlestrong">addPoint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L522" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a new point. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">addPoint</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Tesselate_clear"><span class="titletype">func</span> <span class="titlelight">Tesselate.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L485" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear content. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">clear</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Tesselate_endPolyLine"><span class="titletype">func</span> <span class="titlelight">Tesselate.</span><span class="titlestrong">endPolyLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L531" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>To call to register the polyline. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">endPolyLine</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Tesselate_getTriangles"><span class="titletype">func</span> <span class="titlelight">Tesselate.</span><span class="titlestrong">getTriangles</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L510" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the list of triangles. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getTriangles</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;[..] *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Triangle">Triangle</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Tesselate_startPolyLine"><span class="titletype">func</span> <span class="titlelight">Tesselate.</span><span class="titlestrong">startPolyLine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L516" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>To call before adding a polyline. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">startPolyLine</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Tesselate_triangulate"><span class="titletype">func</span> <span class="titlelight">Tesselate.</span><span class="titlestrong">triangulate</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L549" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Triangulate. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">triangulate</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Poly2Tri_Triangle"><span class="titletype">struct</span> <span class="titlelight">Poly2Tri.</span><span class="titlestrong">Triangle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">constrainedEdge</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">3</span>] <span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">delaunayEdge</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">3</span>] <span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">points</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">3</span>] *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">neighbors</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">3</span>] *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant">Poly2Tri</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Triangle">Triangle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">interior</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Polygon</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">points</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">isClockwise</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">isConvex</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">area</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">totalLen</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bvMin</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">bvMax</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Polygon_add"><span class="SyntaxCode"><span class="SyntaxFunction">add</span>()</spa</a></td>
<td>Add a new point. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_clean"><span class="SyntaxCode"><span class="SyntaxFunction">clean</span>()</spa</a></td>
<td>Clean polygon by removing bad/unecessary points. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_clear"><span class="SyntaxCode"><span class="SyntaxFunction">clear</span>()</spa</a></td>
<td>Clear all points. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_compute"><span class="SyntaxCode"><span class="SyntaxFunction">compute</span>()</spa</a></td>
<td>Compute internal values, like convex/clockwise  Call it once the polygon contains all its points. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_getClean"><span class="SyntaxCode"><span class="SyntaxFunction">getClean</span>()</spa</a></td>
<td>Clean polygon by removing bad/unecessary points. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_getOffset"><span class="SyntaxCode"><span class="SyntaxFunction">getOffset</span>()</spa</a></td>
<td>Slow but quality offset with a joinStyle. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_getPoint"><span class="SyntaxCode"><span class="SyntaxFunction">getPoint</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">u64</span>)</span></a></td>
<td>Get the given point. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_getPoint"><span class="SyntaxCode"><span class="SyntaxFunction">getPoint</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">u64</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Get the given point, with an optional offset. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_normalizeDist"><span class="SyntaxCode"><span class="SyntaxFunction">normalizeDist</span>()</spa</a></td>
<td>Divide by adding points if two points are too far away. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_offset"><span class="SyntaxCode"><span class="SyntaxFunction">offset</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Fast expand polygon by a given amount. </td>
</tr>
<tr>
<td><a href="#Pixel_Polygon_offset"><span class="SyntaxCode"><span class="SyntaxFunction">offset</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxConstant">JoinStyle</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Slow but quality offset with a joinStyle. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon_add"><span class="titletype">func</span> <span class="titlelight">Polygon.</span><span class="titlestrong">add</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new point. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">add</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, v: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon_clean"><span class="titletype">func</span> <span class="titlelight">Polygon.</span><span class="titlestrong">clean</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L222" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clean polygon by removing bad/unecessary points. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">clean</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon_clear"><span class="titletype">func</span> <span class="titlelight">Polygon.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L90" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear all points. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">clear</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon_compute"><span class="titletype">func</span> <span class="titlelight">Polygon.</span><span class="titlestrong">compute</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L256" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Compute internal values, like convex/clockwise  Call it once the polygon contains all its points. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">compute</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon_getClean"><span class="titletype">func</span> <span class="titlelight">Polygon.</span><span class="titlestrong">getClean</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L232" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clean polygon by removing bad/unecessary points. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getClean</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon_getOffset"><span class="titletype">func</span> <span class="titlelight">Polygon.</span><span class="titlestrong">getOffset</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Slow but quality offset with a joinStyle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getOffset</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, value: <span class="SyntaxType">f32</span>, joinStyle: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_JoinStyle">JoinStyle</a></span>, toler: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">0.5</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon_getPoint"><span class="titletype">func</span> <span class="titlelight">Polygon.</span><span class="titlestrong">getPoint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the given point. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getPoint</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, i: <span class="SyntaxType">u64</span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></code>
</div>
<p>Get the given point, with an optional offset. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getPoint</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, i: <span class="SyntaxType">u64</span>, offset: <span class="SyntaxType">f32</span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon_normalizeDist"><span class="titletype">func</span> <span class="titlelight">Polygon.</span><span class="titlestrong">normalizeDist</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Divide by adding points if two points are too far away. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">normalizeDist</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, dist: <span class="SyntaxType">f32</span>, closed: <span class="SyntaxType">bool</span> = <span class="SyntaxKeyword">true</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Polygon_offset"><span class="titletype">func</span> <span class="titlelight">Polygon.</span><span class="titlestrong">offset</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L106" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fast expand polygon by a given amount. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">offset</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, value: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>Slow but quality offset with a joinStyle. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">offset</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, value: <span class="SyntaxType">f32</span>, joinStyle: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_JoinStyle">JoinStyle</a></span>, toler: <span class="SyntaxType">f32</span> = <span class="SyntaxNumber">0.5</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">RenderOgl</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">using</span> native</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_NativeRenderOgl">NativeRenderOgl</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">vertexbuffer</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">shaderSimple</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderSimple">ShaderSimple</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">shaderAA</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderAA">ShaderAA</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">shaderBlur</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderBlur">ShaderBlur</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">worldTransform</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Transform2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">orthoMatrix</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Matrix4x4</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">modelMatrix</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Matrix4x4</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">whiteTexture</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">hatchTextures</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">36</span>] <span class="SyntaxConstant">Ogl</span>.<span class="SyntaxConstant">GLuint</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">blendingMode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_BlendingMode">BlendingMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">curRC</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">stackReleasedRT</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">stackRT</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">curRT</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">curShader</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">overlapMode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_RenderOgl_addImage"><span class="SyntaxCode"><span class="SyntaxFunction">addImage</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Image</span>)</span></a></td>
<td>Register an image for rendering. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_addImage"><span class="SyntaxCode"><span class="SyntaxFunction">addImage</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxType">string</span>)</span></a></td>
<td>Load and register an image for rendering. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_begin"><span class="SyntaxCode"><span class="SyntaxFunction">begin</span>()</spa</a></td>
<td>To be called before rendering. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_bindRenderTarget"><span class="SyntaxCode"><span class="SyntaxFunction">bindRenderTarget</span>()</spa</a></td>
<td>Set the current render target. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_clear"><span class="SyntaxCode"><span class="SyntaxFunction">clear</span>()</spa</a></td>
<td>Clear current render buffers. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_computeMatrices"><span class="SyntaxCode"><span class="SyntaxFunction">computeMatrices</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_createRenderTarget"><span class="SyntaxCode"><span class="SyntaxFunction">createRenderTarget</span>()</spa</a></td>
<td>Create a render target. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_createShader"><span class="SyntaxCode"><span class="SyntaxFunction">createShader</span>()</spa</a></td>
<td>Create the given shader. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_deleteRenderTarget"><span class="SyntaxCode"><span class="SyntaxFunction">deleteRenderTarget</span>()</spa</a></td>
<td>Delete a render target. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_draw"><span class="SyntaxCode"><span class="SyntaxFunction">draw</span>()</spa</a></td>
<td>Draw the given painter. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_end"><span class="SyntaxCode"><span class="SyntaxFunction">end</span>()</spa</a></td>
<td>To be called after rendering. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_getRenderTarget"><span class="SyntaxCode"><span class="SyntaxFunction">getRenderTarget</span>()</spa</a></td>
<td>Get a render target of the given size  releaseRenderTarget must be called to release it. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_getTargetSize"><span class="SyntaxCode"><span class="SyntaxFunction">getTargetSize</span>()</spa</a></td>
<td>Get the actual render target size. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_getTexturePixels"><span class="SyntaxCode"><span class="SyntaxFunction">getTexturePixels</span>()</spa</a></td>
<td>Returns the pixels associated to a texture. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_init"><span class="SyntaxCode"><span class="SyntaxFunction">init</span>()</spa</a></td>
<td>First init. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_pixelFormatToNative"><span class="SyntaxCode"><span class="SyntaxFunction">pixelFormatToNative</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_releaseRenderTarget"><span class="SyntaxCode"><span class="SyntaxFunction">releaseRenderTarget</span>()</spa</a></td>
<td>Release the given render target. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_removeTexture"><span class="SyntaxCode"><span class="SyntaxFunction">removeTexture</span>()</spa</a></td>
<td>Unregister a texture created with <code class="incode">addImage</code>. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_resetScissorRect"><span class="SyntaxCode"><span class="SyntaxFunction">resetScissorRect</span>()</spa</a></td>
<td>Reset the scissor. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_resetTransform"><span class="SyntaxCode"><span class="SyntaxFunction">resetTransform</span>()</spa</a></td>
<td>Reset the world transform. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_rotateTransform"><span class="SyntaxCode"><span class="SyntaxFunction">rotateTransform</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Angle</span>)</span></a></td>
<td>Rotate the world transform. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_rotateTransform"><span class="SyntaxCode"><span class="SyntaxFunction">rotateTransform</span>(<span class="SyntaxKeyword">self</span>, <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">ref</span> <span class="SyntaxConstant">Angle</span>, <span class="SyntaxType">f32</span>, <span class="SyntaxType">f32</span>)</span></a></td>
<td>Rotate the world transform. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_scaleTransform"><span class="SyntaxCode"><span class="SyntaxFunction">scaleTransform</span>()</spa</a></td>
<td>Scale the world transform. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_setCurrentContext"><span class="SyntaxCode"><span class="SyntaxFunction">setCurrentContext</span>()</spa</a></td>
<td>Set the current rendering context. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_setScissorRect"><span class="SyntaxCode"><span class="SyntaxFunction">setScissorRect</span>()</spa</a></td>
<td>Force the scissor. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_setShader"><span class="SyntaxCode"><span class="SyntaxFunction">setShader</span>()</spa</a></td>
<td>Set the current shader. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_translateTransform"><span class="SyntaxCode"><span class="SyntaxFunction">translateTransform</span>()</spa</a></td>
<td>Translate the world transform. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_unbindRenderTarget"><span class="SyntaxCode"><span class="SyntaxFunction">unbindRenderTarget</span>()</spa</a></td>
<td>Go back to the previous render target, or frame buffer. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_unbindRenderTargetToImage"><span class="SyntaxCode"><span class="SyntaxFunction">unbindRenderTargetToImage</span>()</spa</a></td>
<td>Get current render target pixels. </td>
</tr>
<tr>
<td><a href="#Pixel_RenderOgl_updateTexture"><span class="SyntaxCode"><span class="SyntaxFunction">updateTexture</span>()</spa</a></td>
<td>Update content of texture. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_addImage"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">addImage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L459" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register an image for rendering. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">addImage</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, image: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span></span></code>
</div>
<p>Load and register an image for rendering. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">addImage</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fullName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_begin"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">begin</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L210" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>To be called before rendering. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">begin</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rc: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_bindRenderTarget"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">bindRenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L353" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current render target. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">bindRenderTarget</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rt: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span>, vp: *<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span> = <span class="SyntaxKeyword">null</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_clear"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">clear</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L233" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear current render buffers. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">clear</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_computeMatrices"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">computeMatrices</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L570" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">computeMatrices</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, mvp: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>, mdl: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_createRenderTarget"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">createRenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L398" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a render target. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createRenderTarget</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, w: <span class="SyntaxType">s32</span>, h: <span class="SyntaxType">s32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_createShader"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">createShader</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L896" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the given shader. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createShader</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, base: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span>, vertex: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>, pixel: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_deleteRenderTarget"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">deleteRenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L432" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Delete a render target. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">deleteRenderTarget</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rt: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_draw"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">draw</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L814" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw the given painter. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">draw</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, dc: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Painter">Painter</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_end"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">end</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L218" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>To be called after rendering. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">end</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_getRenderTarget"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">getRenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L861" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get a render target of the given size  releaseRenderTarget must be called to release it. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getRenderTarget</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, w: <span class="SyntaxType">s32</span>, h: <span class="SyntaxType">s32</span>)-&gt;*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_getTargetSize"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">getTargetSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L242" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the actual render target size. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getTargetSize</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;{width: <span class="SyntaxType">s32</span>, height: <span class="SyntaxType">s32</span>}</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_getTexturePixels"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">getTexturePixels</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L513" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the pixels associated to a texture. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getTexturePixels</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>, pf: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxType">u8</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_init"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">init</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L265" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>First init. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">init</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_pixelFormatToNative"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">pixelFormatToNative</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L489" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">pixelFormatToNative</span>(pf: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)-&gt;<span class="SyntaxConstant">Ogl</span>.<span class="SyntaxConstant">GLenum</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_releaseRenderTarget"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">releaseRenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L890" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the given render target. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">releaseRenderTarget</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, tgt: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_removeTexture"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">removeTexture</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L527" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unregister a texture created with <code class="incode">addImage</code>. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">removeTexture</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, texture: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_resetScissorRect"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">resetScissorRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L259" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the scissor. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">resetScissorRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_resetTransform"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">resetTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L564" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the world transform. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">resetTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_rotateTransform"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">rotateTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L543" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Rotate the world transform. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">rotateTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, angle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>)</span></code>
</div>
<p>Rotate the world transform. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">rotateTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, angle: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Angle</span>, centerX: <span class="SyntaxType">f32</span>, centerY: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_scaleTransform"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">scaleTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L557" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Scale the world transform. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">scaleTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_setCurrentContext"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">setCurrentContext</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L226" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current rendering context. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setCurrentContext</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rc: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_setScissorRect"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">setScissorRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L250" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the scissor. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setScissorRect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_setShader"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">setShader</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L851" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current shader. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">setShader</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, shader: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span> = <span class="SyntaxKeyword">null</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_translateTransform"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">translateTransform</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L536" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Translate the world transform. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">translateTransform</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">f32</span>, y: <span class="SyntaxType">f32</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_unbindRenderTarget"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">unbindRenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L380" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Go back to the previous render target, or frame buffer. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">unbindRenderTarget</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_unbindRenderTargetToImage"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">unbindRenderTargetToImage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L370" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get current render target pixels. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">unbindRenderTargetToImage</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, x: <span class="SyntaxType">s32</span>, y: <span class="SyntaxType">s32</span>, w: <span class="SyntaxType">s32</span>, h: <span class="SyntaxType">s32</span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderOgl_updateTexture"><span class="titletype">func</span> <span class="titlelight">RenderOgl.</span><span class="titlestrong">updateTexture</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L504" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Update content of texture. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">updateTexture</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, texture: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span>, pixels: <span class="SyntaxKeyword">const</span> *<span class="SyntaxType">u8</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderTarget"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">RenderTarget</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\rendertarget.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">handle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">colorBuffer</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">depthStencil</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">paintAlpha</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">paintAlphaIdx</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RenderingContext"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">RenderingContext</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">hDC</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">const</span> *<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">hRC</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">const</span> *<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
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
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichChunk"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">RichChunk</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">slice</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">string</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">fontStyle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">hasFontStyle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">isSpace</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">isEol</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">colorIdx</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">boundRect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pos</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">font</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">under</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichString"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">RichString</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Represents a string with embedded formats. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">raw</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">String</span></span></td>
<td>The string as passed by the user. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">chunks</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RichChunk">RichChunk</a></span>)</span></td>
<td>The corresponding list of chunks. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">layoutRect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">boundRect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">dirty</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td>If dirty, <code class="incode">Compute</code>. </td>
</tr>
</table>
<p> This kind of string is a specific version of [Core.String] but specific for painting. It can contains  some special markers to change it's appearence. </p>
<div class="precode"><code><span class="SyntaxCode">&lt;b&gt; this is bold&lt;b&gt;
 &lt;p1&gt; this will have special color <span class="SyntaxNumber">1</span>&lt;p1&gt;
 ...</span></code>
</div>
<p> See <a href="#Pixel_RichString_set">RichString.set</a> for a list of available markers. </p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_RichString_compute"><span class="SyntaxCode"><span class="SyntaxFunction">compute</span>()</spa</a></td>
<td>Compute layout. </td>
</tr>
<tr>
<td><a href="#Pixel_RichString_getNaked"><span class="SyntaxCode"><span class="SyntaxFunction">getNaked</span>()</spa</a></td>
<td>Transform the given string to a naked one. </td>
</tr>
<tr>
<td><a href="#Pixel_RichString_invalidate"><span class="SyntaxCode"><span class="SyntaxFunction">invalidate</span>()</spa</a></td>
<td>Force the string to be repainted. </td>
</tr>
<tr>
<td><a href="#Pixel_RichString_isEmpty"><span class="SyntaxCode"><span class="SyntaxFunction">isEmpty</span>()</spa</a></td>
<td>Returns <code class="incode">true</code> if the string is empty. </td>
</tr>
<tr>
<td><a href="#Pixel_RichString_set"><span class="SyntaxCode"><span class="SyntaxFunction">set</span>()</spa</a></td>
<td>Associate a string. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_RichString_opAffect"><span class="SyntaxCode"><span class="SyntaxFunction">opAffect</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_RichString_opCount"><span class="SyntaxCode"><span class="SyntaxFunction">opCount</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_RichString_opEquals"><span class="SyntaxCode"><span class="SyntaxFunction">opEquals</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichString_compute"><span class="titletype">func</span> <span class="titlelight">RichString.</span><span class="titlestrong">compute</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L160" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Compute layout. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">compute</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, force = <span class="SyntaxKeyword">false</span>)</span></code>
</div>
<p> This will transform the string in a serie of chunks, each chunk having its one style. The string  will only be recomputed if it's dirty (the text has changed) or if <code class="incode">force</code> is true. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichString_getNaked"><span class="titletype">func</span> <span class="titlelight">RichString.</span><span class="titlestrong">getNaked</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L122" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform the given string to a naked one. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getNaked</span>(str: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">String</span></span></code>
</div>
<p> Will return the string without the format markers. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichString_invalidate"><span class="titletype">func</span> <span class="titlelight">RichString.</span><span class="titlestrong">invalidate</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L95" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the string to be repainted. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">invalidate</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichString_isEmpty"><span class="titletype">func</span> <span class="titlelight">RichString.</span><span class="titlestrong">isEmpty</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns <code class="incode">true</code> if the string is empty. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">isEmpty</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichString_opAffect"><span class="titletype">func</span> <span class="titlelight">RichString.</span><span class="titlestrong">opAffect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L88" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opAffect</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, str: <span class="SyntaxType">string</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichString_opCount"><span class="titletype">func</span> <span class="titlelight">RichString.</span><span class="titlestrong">opCount</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L86" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opCount</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">u64</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichString_opEquals"><span class="titletype">func</span> <span class="titlelight">RichString.</span><span class="titlestrong">opEquals</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">opEquals</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, other: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichString_set"><span class="titletype">func</span> <span class="titlelight">RichString.</span><span class="titlestrong">set</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L112" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a string. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">set</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, text: <span class="SyntaxType">string</span>)</span></code>
</div>
<p> The string can have multiple lines.  Accepted markers are: </p>
<ul>
<li><code class="incode">&lt;u&gt;</code> =&gt; underline</li>
<li><code class="incode">&lt;b&gt;</code> =&gt; bold</li>
<li><code class="incode">&lt;i&gt;</code> =&gt; italic</li>
<li><code class="incode">&lt;p1&gt;</code> =&gt; color palette 1</li>
<li><code class="incode">&lt;p2&gt;</code> =&gt; color palette 2</li>
<li><code class="incode">&lt;p3&gt;</code> =&gt; color palette 3</li>
</ul>
<div class="precode"><code><span class="SyntaxCode">&lt;b&gt;this is bold&lt;b&gt; but this is normal &lt;i&gt;<span class="SyntaxLogic">and</span> this is italic&lt;/i&gt;</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_RichStringFormat"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">RichStringFormat</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">font</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamily">FontFamily</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">fontStyle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">palette</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">4</span>] <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">brush</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">horzAlignment</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_StringHorzAlignment">StringHorzAlignment</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">vertAlignment</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_StringVertAlignment">StringVertAlignment</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">lineGap</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">flags</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_StringFormatFlags">StringFormatFlags</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_ShaderAA"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">ShaderAA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\shader.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">using</span> base</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_ShaderBase"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">ShaderBase</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\shader.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">handle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">mvp</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">mdl</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">boundRect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">textureRect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">uvMode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">copyMode</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">textureW</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">textureH</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_ShaderBlur"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">ShaderBlur</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\shader.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">using</span> base</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">radius</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">axis</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s64</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_ShaderSimple"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">ShaderSimple</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\shader.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">using</span> base</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringFormatFlags"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">StringFormatFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Default</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">WordWrap</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Clip</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">DontAdaptSingleLineHeight</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringHorzAlignment"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">StringHorzAlignment</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Center</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Left</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Right</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Justify</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringMetrics"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">StringMetrics</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">ascent</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td>Maximum size above baseline. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">descent</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td>Maximum size below baseline. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">width</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">height</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td>ascent + descent. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringPainter"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">StringPainter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">str</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RichString">RichString</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">fmt</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_RichStringFormat">RichStringFormat</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_StringPainter_color"><span class="SyntaxCode"><span class="SyntaxFunction">color</span>()</spa</a></td>
<td>Set text color. </td>
</tr>
<tr>
<td><a href="#Pixel_StringPainter_draw"><span class="SyntaxCode"><span class="SyntaxFunction">draw</span>()</spa</a></td>
<td>Draw string. </td>
</tr>
<tr>
<td><a href="#Pixel_StringPainter_flags"><span class="SyntaxCode"><span class="SyntaxFunction">flags</span>()</spa</a></td>
<td>Set drawing flags. </td>
</tr>
<tr>
<td><a href="#Pixel_StringPainter_font"><span class="SyntaxCode"><span class="SyntaxFunction">font</span>()</spa</a></td>
<td>Set font. </td>
</tr>
<tr>
<td><a href="#Pixel_StringPainter_horzAlign"><span class="SyntaxCode"><span class="SyntaxFunction">horzAlign</span>()</spa</a></td>
<td>Set horizontal alignment. </td>
</tr>
<tr>
<td><a href="#Pixel_StringPainter_vertAlign"><span class="SyntaxCode"><span class="SyntaxFunction">vertAlign</span>()</spa</a></td>
<td>Set vertical alignment. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringPainter_color"><span class="titletype">func</span> <span class="titlelight">StringPainter.</span><span class="titlestrong">color</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set text color. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">color</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, col: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringPainter_draw"><span class="titletype">func</span> <span class="titlelight">StringPainter.</span><span class="titlestrong">draw</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw string. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">draw</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, painter: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Painter">Painter</a></span>, rect: <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Rectangle</span>, what: <span class="SyntaxType">string</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringPainter_flags"><span class="titletype">func</span> <span class="titlelight">StringPainter.</span><span class="titlestrong">flags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set drawing flags. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">flags</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, f: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_StringFormatFlags">StringFormatFlags</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringPainter_font"><span class="titletype">func</span> <span class="titlelight">StringPainter.</span><span class="titlestrong">font</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set font. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">font</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, font: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringPainter_horzAlign"><span class="titletype">func</span> <span class="titlelight">StringPainter.</span><span class="titlestrong">horzAlign</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set horizontal alignment. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">horzAlign</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, align: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_StringHorzAlignment">StringHorzAlignment</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringPainter_vertAlign"><span class="titletype">func</span> <span class="titlelight">StringPainter.</span><span class="titlestrong">vertAlign</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set vertical alignment. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">vertAlign</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, align: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_StringVertAlignment">StringVertAlignment</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_StringVertAlignment"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">StringVertAlignment</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Center</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Top</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Bottom</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Texture"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Texture</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">handle</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pf</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span></span></td>
<td></td>
</tr>
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
<td class="codetype"><span class="SyntaxCode">valid</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_Texture_getByteSize"><span class="SyntaxCode"><span class="SyntaxFunction">getByteSize</span>()</spa</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Pixel_Texture_isValid"><span class="SyntaxCode"><span class="SyntaxFunction">isValid</span>()</spa</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Texture_getByteSize"><span class="titletype">func</span> <span class="titlelight">Texture.</span><span class="titlestrong">getByteSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getByteSize</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">s32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Texture_isValid"><span class="titletype">func</span> <span class="titlelight">Texture.</span><span class="titlestrong">isValid</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">isValid</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Tga_Decoder"><span class="titletype">struct</span> <span class="titlelight">Tga.</span><span class="titlestrong">Decoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\tga.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Tga_Decoder_IImageDecoder_canDecode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">canDecode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\tga.swg#L95" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canDecode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Tga_Decoder_IImageDecoder_decode"><span class="titletype">func</span> <span class="titlelight">IImageDecoder.</span><span class="titlestrong">decode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\tga.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">decode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, buffer: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>, options: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Tga_Encoder"><span class="titletype">struct</span> <span class="titlelight">Tga.</span><span class="titlestrong">Encoder</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\tga.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Tga_Encoder_IImageEncoder_canEncode"><span class="titletype">func</span> <span class="titlelight">IImageEncoder.</span><span class="titlestrong">canEncode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\tga.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">canEncode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, fileName: <span class="SyntaxType">string</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_Tga_Encoder_IImageEncoder_encode"><span class="titletype">func</span> <span class="titlelight">IImageEncoder.</span><span class="titlestrong">encode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\tga.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">encode</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, result: *<span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">ConcatBuffer</span>, image: <span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span>, options: <span class="SyntaxType">any</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">TypeFace</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">fullname</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">string</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">buffer</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Array</span>'(<span class="SyntaxType">u8</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">face</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant">FT_FaceRec</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">forceBoldDiv</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">forceItalic</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">forceBold</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">bool</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Pixel_TypeFace_create"><span class="SyntaxCode"><span class="SyntaxFunction">create</span>()</spa</a></td>
<td>Get or create a new typeface from a memory buffer. </td>
</tr>
<tr>
<td><a href="#Pixel_TypeFace_createFromHFONT"><span class="SyntaxCode"><span class="SyntaxFunction">createFromHFONT</span>()</spa</a></td>
<td>Create a new typeface from a HFONT. </td>
</tr>
<tr>
<td><a href="#Pixel_TypeFace_getFamilyName"><span class="SyntaxCode"><span class="SyntaxFunction">getFamilyName</span>()</spa</a></td>
<td>Returns the underlying font family name. </td>
</tr>
<tr>
<td><a href="#Pixel_TypeFace_getRuneOutline"><span class="SyntaxCode"><span class="SyntaxFunction">getRuneOutline</span>()</spa</a></td>
<td>Convert a rune to its outline, at a given size. </td>
</tr>
<tr>
<td><a href="#Pixel_TypeFace_getStringOutline"><span class="SyntaxCode"><span class="SyntaxFunction">getStringOutline</span>()</spa</a></td>
<td>Convert a string to its outline, at a given size. </td>
</tr>
<tr>
<td><a href="#Pixel_TypeFace_isBold"><span class="SyntaxCode"><span class="SyntaxFunction">isBold</span>()</spa</a></td>
<td>Returns true if the typeface is bold. </td>
</tr>
<tr>
<td><a href="#Pixel_TypeFace_isItalic"><span class="SyntaxCode"><span class="SyntaxFunction">isItalic</span>()</spa</a></td>
<td>Returns true if the typeface is italic. </td>
</tr>
<tr>
<td><a href="#Pixel_TypeFace_load"><span class="SyntaxCode"><span class="SyntaxFunction">load</span>()</spa</a></td>
<td>Get or load a new typeface. </td>
</tr>
<tr>
<td><a href="#Pixel_TypeFace_release"><span class="SyntaxCode"><span class="SyntaxFunction">release</span>()</spa</a></td>
<td>Release the given typeface. </td>
</tr>
<tr>
<td><a href="#Pixel_TypeFace_renderGlyph"><span class="SyntaxCode"><span class="SyntaxFunction">renderGlyph</span>()</spa</a></td>
<td>Render one glyph. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_create"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get or create a new typeface from a memory buffer. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">create</span>(fullname: <span class="SyntaxType">string</span>, bytes: <span class="SyntaxKeyword">const</span> [..] <span class="SyntaxType">u8</span>)-&gt;*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_createFromHFONT"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">createFromHFONT</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.win32.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new typeface from a HFONT. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">createFromHFONT</span>(fullname: <span class="SyntaxType">string</span>, hf: <span class="SyntaxConstant">Gdi32</span>.<span class="SyntaxConstant">HFONT</span>)-&gt;*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_getFamilyName"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">getFamilyName</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L76" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the underlying font family name. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getFamilyName</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">string</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_getRuneOutline"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">getRuneOutline</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L161" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a rune to its outline, at a given size. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getRuneOutline</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, pathList: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span>, c: <span class="SyntaxType">rune</span>, size: <span class="SyntaxType">u32</span>, embolden: <span class="SyntaxType">u32</span> = <span class="SyntaxNumber">0</span>)-&gt;<span class="SyntaxType">f32</span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_getStringOutline"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">getStringOutline</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L188" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a string to its outline, at a given size. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">getStringOutline</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, pathList: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span>, text: <span class="SyntaxType">string</span>, size: <span class="SyntaxType">u32</span>, embolden: <span class="SyntaxType">u32</span> = <span class="SyntaxNumber">0</span>) <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_isBold"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">isBold</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L64" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the typeface is bold. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">isBold</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_isItalic"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">isItalic</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L70" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the typeface is italic. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">isItalic</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">const</span> <span class="SyntaxKeyword">self</span>)-&gt;<span class="SyntaxType">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_load"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">load</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L111" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get or load a new typeface. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">load</span>(fullname: <span class="SyntaxType">string</span>)-&gt;*<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_release"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">release</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the given typeface. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">release</span>(typeface: *<span class="SyntaxConstant">Pixel</span>.<span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_TypeFace_renderGlyph"><span class="titletype">func</span> <span class="titlelight">TypeFace.</span><span class="titlestrong">renderGlyph</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L150" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Render one glyph. </p>
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxFunction">renderGlyph</span>(<span class="SyntaxKeyword">using</span> <span class="SyntaxKeyword">self</span>, c: <span class="SyntaxType">rune</span>, size: <span class="SyntaxType">u32</span>, forceBold = <span class="SyntaxKeyword">false</span>, forceItalic = <span class="SyntaxKeyword">false</span>)-&gt;<span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">FreeType</span>.<span class="SyntaxConstant">FT_Bitmap</span> <span class="SyntaxKeyword">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_UVMode"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">UVMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Fill</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FillSubRect</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Tile</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_UnderlineStyle"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">UnderlineStyle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">None</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Underline</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Strikeout</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Pixel_VertexLayout"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">VertexLayout</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SyntaxCode">pos</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">uv</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">color</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">aa</span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">12</span>] <span class="SyntaxConstant">Core</span>.<span class="SyntaxConstant">Math</span>.<span class="SyntaxConstant">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">aanum</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
</table>
</div>
</div>
</div>
</body>
</html>
