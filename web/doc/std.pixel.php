<html>
<body>
<head>
<meta charset="UTF-8">
</head>
<style>
.container {
            display:        flex;
            flex-wrap:      nowrap;
            flex-direction: row;
            height:         100%;
            line-height:    1.3em;
            font-family:    Segoe UI;
        }
        .left {
            display:    block;
            overflow-y: scroll;
            width:      650;
            height:     100%;
        }
        .right {
            display:     block;
            overflow-y:  scroll;
            width:       100%;
            line-height: 1.3em;
            height:      100%;
        }
        .page {
            width:  1000;
            margin: 0 auto;
        }
        blockquote {
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            padding:            10px;
            width:              90%;
        }
        a {
            text-decoration: none;
            color:           DoggerBlue;
        }
        a:hover {
            text-decoration: underline;
        }
        a.src {
            font-size:          90%;
            color:              LightGrey;
        }
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        td.enumeration {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              30%;
        }
        td.tdname {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              20%;
            background-color:   #f8f8f8;
        }
        td.tdtype {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              auto;
        }
        td:last-child {
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
        .code {
            background-color:   #eeeeee;
            border:             1px solid LightGrey;
            padding:            10px;
            width:              94%;
            margin-left:        20px;
        }
    .SyntaxCode { color: #7f7f7f; }
.SyntaxComment { color: #71a35b; }
.SyntaxCompiler { color: #7f7f7f; }
.SyntaxFunction { color: #ff6a00; }
.SyntaxConstant { color: #3173cd; }
.SyntaxIntrinsic { color: #b4b44a; }
.SyntaxType { color: #3bc3a7; }
.SyntaxKeyword { color: #3186cd; }
.SyntaxLogic { color: #b040be; }
.SyntaxNumber { color: #74a35b; }
.SyntaxString { color: #bb6643; }
.SyntaxAttribute { color: #7f7f7f; }
</style>
<div class="container">
<div class="left">
<h1>Module pixel</h1>
<h2>Structs</h2>
<h3>image</h3>
<ul>
<li><a href="#Pixel_Image">Image</a></li>
<li><a href="#Pixel_RenderTarget">RenderTarget</a></li>
<li><a href="#Pixel_Texture">Texture</a></li>
</ul>
<h3>image/decode</h3>
<ul>
<li><a href="#Pixel_Bmp_Decoder">Bmp.Decoder</a></li>
<li><a href="#Pixel_DecodeOptions">DecodeOptions</a></li>
<li><a href="#Pixel_Gif_Decoder">Gif.Decoder</a></li>
<li><a href="#Pixel_Bmp_Header">Header</a></li>
<li><a href="#Pixel_Jpg_Decoder">Jpg.Decoder</a></li>
<li><a href="#Pixel_Png_Decoder">Png.Decoder</a></li>
<li><a href="#Pixel_Tga_Decoder">Tga.Decoder</a></li>
</ul>
<h3>image/encode</h3>
<ul>
<li><a href="#Pixel_Bmp_Encoder">Bmp.Encoder</a></li>
<li><a href="#Pixel_Jpg_EncodeOptions">Jpg.EncodeOptions</a></li>
<li><a href="#Pixel_Jpg_Encoder">Jpg.Encoder</a></li>
<li><a href="#Pixel_Png_EncodeOptions">Png.EncodeOptions</a></li>
<li><a href="#Pixel_Png_Encoder">Png.Encoder</a></li>
<li><a href="#Pixel_Tga_Encoder">Tga.Encoder</a></li>
</ul>
<h3>painter</h3>
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
<h3>poly</h3>
<ul>
<li><a href="#Pixel_Poly2Tri_Edge">Edge</a></li>
<li><a href="#Pixel_Clipper_IntPoint">IntPoint</a></li>
<li><a href="#Pixel_Poly2Tri_Point">Point</a></li>
<li><a href="#Pixel_Polygon">Polygon</a></li>
<li><a href="#Pixel_Poly2Tri_Tesselate">Tesselate</a></li>
<li><a href="#Pixel_Clipper_Transform">Transform</a></li>
<li><a href="#Pixel_Poly2Tri_Triangle">Triangle</a></li>
</ul>
<h3>render/ogl</h3>
<ul>
<li><a href="#Pixel_NativeRenderOgl">NativeRenderOgl</a></li>
<li><a href="#Pixel_RenderOgl">RenderOgl</a></li>
<li><a href="#Pixel_RenderingContext">RenderingContext</a></li>
<li><a href="#Pixel_ShaderAA">ShaderAA</a></li>
<li><a href="#Pixel_ShaderBase">ShaderBase</a></li>
<li><a href="#Pixel_ShaderBlur">ShaderBlur</a></li>
<li><a href="#Pixel_ShaderSimple">ShaderSimple</a></li>
</ul>
<h3>text</h3>
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
<h3>types</h3>
<ul>
<li><a href="#Pixel_Brush">Brush</a></li>
<li><a href="#Pixel_Color">Color</a></li>
<li><a href="#Pixel_Element">Element</a></li>
<li><a href="#Pixel_LinePath">LinePath</a></li>
<li><a href="#Pixel_LinePathList">LinePathList</a></li>
<li><a href="#Pixel_Pen">Pen</a></li>
</ul>
<h2>Interfaces</h2>
<h3>image/decode</h3>
<ul>
<li><a href="#Pixel_IImageDecoder">IImageDecoder</a></li>
</ul>
<h3>image/encode</h3>
<ul>
<li><a href="#Pixel_IImageEncoder">IImageEncoder</a></li>
</ul>
<h2>Enums</h2>
<h3>image</h3>
<ul>
<li><a href="#Pixel_HatchStyle">HatchStyle</a></li>
<li><a href="#Pixel_InterpolationMode">InterpolationMode</a></li>
<li><a href="#Pixel_PixelFormat">PixelFormat</a></li>
<li><a href="#Pixel_UVMode">UVMode</a></li>
</ul>
<h3>image/filter</h3>
<ul>
<li><a href="#Pixel_Image_ChannelMode">ChannelMode</a></li>
<li><a href="#Pixel_Image_ChannelValueMode">ChannelValueMode</a></li>
<li><a href="#Pixel_Image_FillHslType">FillHslType</a></li>
<li><a href="#Pixel_Image_Kernel">Kernel</a></li>
</ul>
<h3>image/transform</h3>
<ul>
<li><a href="#Pixel_Image_MixMode">MixMode</a></li>
<li><a href="#Pixel_Image_ResizeMode">ResizeMode</a></li>
<li><a href="#Pixel_Image_TurnAngle">TurnAngle</a></li>
</ul>
<h3>painter</h3>
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
<h3>poly</h3>
<ul>
<li><a href="#Pixel_Clipper_ClipType">ClipType</a></li>
<li><a href="#Pixel_Clipper_InitOptions">InitOptions</a></li>
<li><a href="#Pixel_Clipper_JoinType">JoinType</a></li>
<li><a href="#Pixel_Clipper_PolyFillType">PolyFillType</a></li>
<li><a href="#Pixel_Clipper_PolyType">PolyType</a></li>
</ul>
<h3>text</h3>
<ul>
<li><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></li>
</ul>
<h3>types</h3>
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
<h2>Constants</h2>
<h3>image/decode</h3>
<ul>
<li><a href="#Pixel_Bmp_BI_BITFIELDS">BI_BITFIELDS</a></li>
<li><a href="#Pixel_Bmp_BI_RGB">BI_RGB</a></li>
<li><a href="#Pixel_Bmp_BI_RLE4">BI_RLE4</a></li>
<li><a href="#Pixel_Bmp_BI_RLE8">BI_RLE8</a></li>
</ul>
<h3>painter</h3>
<ul>
<li><a href="#Pixel_MaxAAEdge">MaxAAEdge</a></li>
</ul>
<h3>poly</h3>
<ul>
<li><a href="#Pixel_ClipperScaleCoords">ClipperScaleCoords</a></li>
</ul>
<h3>render/ogl</h3>
<ul>
<li><a href="#Pixel_InvalidRenderTargetHandle">InvalidRenderTargetHandle</a></li>
<li><a href="#Pixel_InvalidRenderTargetSurfaceHandle">InvalidRenderTargetSurfaceHandle</a></li>
<li><a href="#Pixel_InvalidShaderHandle">InvalidShaderHandle</a></li>
<li><a href="#Pixel_InvalidShaderParamHandle">InvalidShaderParamHandle</a></li>
<li><a href="#Pixel_InvalidTextureHandle">InvalidTextureHandle</a></li>
</ul>
<h2>Functions</h2>
<h3>image</h3>
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
<h3>image/decode</h3>
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
<h3>image/encode</h3>
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
<h3>image/filter</h3>
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
<h3>image/transform</h3>
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
<h3>painter</h3>
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
<h3>poly</h3>
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
<h3>render/ogl</h3>
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
<h3>text</h3>
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
<h3>types</h3>
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
<div class="right">
<div class="page">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.22.0)</blockquote>
<h1>Overview</h1>
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
<td id="Pixel_Bmp_BI_BITFIELDS" class="tdname">
BI_BITFIELDS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_Bmp_BI_RGB" class="tdname">
BI_RGB</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_Bmp_BI_RLE4" class="tdname">
BI_RLE4</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_Bmp_BI_RLE8" class="tdname">
BI_RLE8</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_ClipperScaleCoords" class="tdname">
ClipperScaleCoords</td>
<td class="tdtype">
f64</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_InvalidRenderTargetHandle" class="tdname">
InvalidRenderTargetHandle</td>
<td class="tdtype">
Pixel.RenderTargetHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_InvalidRenderTargetSurfaceHandle" class="tdname">
InvalidRenderTargetSurfaceHandle</td>
<td class="tdtype">
Pixel.RenderTargetSurfaceHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_InvalidShaderHandle" class="tdname">
InvalidShaderHandle</td>
<td class="tdtype">
Pixel.ShaderHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_InvalidShaderParamHandle" class="tdname">
InvalidShaderParamHandle</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_InvalidTextureHandle" class="tdname">
InvalidTextureHandle</td>
<td class="tdtype">
Pixel.TextureHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Pixel_MaxAAEdge" class="tdname">
MaxAAEdge</td>
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
<span class="content" id="Pixel_Argb"><span class="titletype">enum</span> <span class="titlelight">Pixel.</span><span class="titlestrong">Argb</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\argb.swg#L1" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="tdname">
AliceBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
AntiqueWhite</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Aqua</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Aquamarine</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Azure</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Beige</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Bisque</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Black</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BlanchedAlmond</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Blue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BlueViolet</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Brown</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BurlyWood</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
CadetBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Chartreuse</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Chocolate</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Coral</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
CornflowerBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Cornsilk</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Crimson</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Cyan</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkCyan</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkGoldenrod</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkGray</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkKhaki</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkMagenta</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkOliveGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkOrange</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkOrchid</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkRed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkSalmon</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkSeaGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkSlateBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkSlateGray</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkTurquoise</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DarkViolet</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DeepPink</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DeepSkyBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DimGray</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DodgerBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Firebrick</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FloralWhite</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ForestGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Fuchsia</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Gainsboro</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
GhostWhite</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Gold</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Goldenrod</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Gray</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Green</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
GreenYellow</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Honeydew</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
HotPink</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
IndianRed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Indigo</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Ivory</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Khaki</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Lavender</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LavenderBlush</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LawnGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LemonChiffon</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightCoral</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightCyan</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightGoldenrodYellow</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightGray</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightPink</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightSalmon</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightSeaGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightSkyBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightSlateGray</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightSteelBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightYellow</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Lime</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LimeGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Linen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Magenta</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Maroon</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MediumAquamarine</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MediumBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MediumOrchid</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MediumPurple</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MediumSeaGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MediumSlateBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MediumSpringGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MediumTurquoise</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MediumVioletRed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MidnightBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MintCream</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MistyRose</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Moccasin</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
NavajoWhite</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Navy</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
OldLace</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Olive</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
OliveDrab</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Orange</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
OrangeRed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Orchid</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PaleGoldenrod</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PaleGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PaleTurquoise</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PaleVioletRed</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PapayaWhip</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PeachPuff</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Peru</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Pink</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Plum</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PowderBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Purple</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Red</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
RosyBrown</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
RoyalBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SaddleBrown</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Salmon</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SandyBrown</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SeaGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SeaShell</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Sienna</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Silver</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SkyBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SlateBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SlateGray</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Snow</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SpringGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SteelBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Tan</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Teal</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Thistle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Tomato</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Turquoise</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Violet</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Wheat</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
White</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
WhiteSmoke</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Yellow</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
YellowGreen</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Zero</td>
<td class="enumeration">
</td>
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
<p>
Get the Argb value that matches <code class="incode">name</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromName</span><span class="SyntaxCode">(name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
Copy</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Alpha</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Add</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Sub</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SubDst</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Min</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Max</td>
<td class="enumeration">
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canDecode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">decode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, buffer: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, options: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canEncode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">encode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, result: *</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ConcatBuffer</span><span class="SyntaxCode">, image: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">, options: </span><span class="SyntaxType">any</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
biSize</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biWidth</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biHeight</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biPlanes</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biBitCount</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biCompression</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biSizeImage</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biXPelsPerMeter</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biYPelsPerMeter</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biClrUsed</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
biClrImportant</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
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
<td class="tdname">
None</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Inside</td>
<td class="enumeration">
</td>
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
<td class="tdname">
color</td>
<td class="tdtype">
<a href="#Pixel_Color">Pixel.Color</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
type</td>
<td class="tdtype">
<a href="#Pixel_BrushType">Pixel.BrushType</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hatch</td>
<td class="tdtype">
<a href="#Pixel_HatchStyle">Pixel.HatchStyle</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
texture</td>
<td class="tdtype">
<a href="#Pixel_Texture">Pixel.Texture</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uvMode</td>
<td class="tdtype">
<a href="#Pixel_UVMode">Pixel.UVMode</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uvRect</td>
<td class="tdtype">
Core.Math.Rectangle</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Brush_createHatch">createHatch(HatchStyle, const ref Color)</a></td>
<td class="enumeration">
<p>
Returns a hatch brush.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Brush_createSolid">createSolid(const ref Color)</a></td>
<td class="enumeration">
<p>
Returns a solid color brush.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Brush_createTexture">createTexture(const ref Texture)</a></td>
<td class="enumeration">
<p>
Returns a full texture brush.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Brush_createTiled">createTiled(const ref Texture)</a></td>
<td class="enumeration">
<p>
Returns a tiled texture brush.</p>
</td>
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
<p>
Returns a hatch brush.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createHatch</span><span class="SyntaxCode">(style: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_HatchStyle">HatchStyle</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns a solid color brush.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createSolid</span><span class="SyntaxCode">(color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns a full texture brush.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createTexture</span><span class="SyntaxCode">(texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns a tiled texture brush.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createTiled</span><span class="SyntaxCode">(texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
SolidColor</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Texture</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Hatch</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Intersection</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Union</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Difference</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Xor</td>
<td class="enumeration">
</td>
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
<td class="tdname">
ReverseSolution</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
StrictlySimple</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PreserveCollinear</td>
<td class="enumeration">
</td>
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
<td class="tdname">
x</td>
<td class="tdtype">
Pixel.Clipper.CInt</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y</td>
<td class="tdtype">
Pixel.Clipper.CInt</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Clipper_IntPoint_opEquals">opEquals(self, const ref IntPoint)</a></td>
<td class="enumeration">
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opEquals</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Clipper</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Clipper_IntPoint">IntPoint</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
Square</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Round</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Miter</td>
<td class="enumeration">
</td>
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
<td class="tdname">
EvenOdd</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
NonZero</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Positive</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Negative</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Subject</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Clip</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Set</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Clear</td>
<td class="enumeration">
</td>
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
<td class="tdname">
a</td>
<td class="tdtype">
u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
r</td>
<td class="tdtype">
u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
g</td>
<td class="tdtype">
u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
b</td>
<td class="tdtype">
u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
argb</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromAbgr">fromAbgr(u32)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromArgb">fromArgb(Argb)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromArgb">fromArgb(u32)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromArgb">fromArgb(u8, Argb)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromArgb">fromArgb(u8, u8, u8, u8)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromArgbf">fromArgbf(f32, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromHsl">fromHsl(f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromRgb">fromRgb(u8, u8, u8)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromRgbf">fromRgbf(f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_fromVector4">fromVector4(const ref Vector4)</a></td>
<td class="enumeration">
<p>
Create a new Color with specified values.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_getBlend">getBlend(const ref Color, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Blend two colors.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_getBlendKeepAlpha">getBlendKeepAlpha(const ref Color, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Blend two colors, keeping the alpha channel of <code class="incode">col0</code> untouched.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_getDarker">getDarker(const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Returns a darker color.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_getDistanceRgb">getDistanceRgb(self, const ref Color)</a></td>
<td class="enumeration">
<p>
Returns the distance between the other color.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_getLighter">getLighter(const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Returns a lighter color.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_isOpaque">isOpaque(self)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_setArgb">setArgb(self, u8, u8, u8, u8)</a></td>
<td class="enumeration">
<p>
Set all components.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_setArgbf">setArgbf(self, f32, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Set all components with floats in the range [0 1].</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_setHsl">setHsl(self, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Initialize color with hue, saturation, lightness.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_setRgb">setRgb(self, const ref Color)</a></td>
<td class="enumeration">
<p>
Just set r, g and b components.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_setRgb">setRgb(self, u8, u8, u8)</a></td>
<td class="enumeration">
<p>
Just set r, g and b components.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_setRgbf">setRgbf(self, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Set r,g,b with floats in the range [0 1].</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_toArgbf">toArgbf(self)</a></td>
<td class="enumeration">
<p>
Retrieve all components as floating point values between 0 and 1.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_toHsl">toHsl(self)</a></td>
<td class="enumeration">
<p>
Returns hue, saturation, lightness.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_toRgbf">toRgbf(self)</a></td>
<td class="enumeration">
<p>
Retrieve all components as floating point values between 0 and 1.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_toVector4">toVector4(self)</a></td>
<td class="enumeration">
<p>
Get the color as a [Math.Vector4].</p>
</td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Color_opAffect">opAffect(self, Argb)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_opAffect">opAffect(self, string)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_opAffect">opAffect(self, u32)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_opEquals">opEquals(self, Argb)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_opEquals">opEquals(self, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Color_opEquals">opEquals(self, u32)</a></td>
<td class="enumeration">
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">poke</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, buf: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">string</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromAbgr</span><span class="SyntaxCode">(abgr: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromArgb</span><span class="SyntaxCode">(argb: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromArgb</span><span class="SyntaxCode">(argb: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromArgb</span><span class="SyntaxCode">(alpha: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, argb: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromArgb</span><span class="SyntaxCode">(a: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, r: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromArgbf</span><span class="SyntaxCode">(a: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, r: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromHsl</span><span class="SyntaxCode">(h: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, s: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, l: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromRgb</span><span class="SyntaxCode">(r: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromRgbf</span><span class="SyntaxCode">(r: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Create a new Color with specified values.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fromVector4</span><span class="SyntaxCode">(vec: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Vector4</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Blend two colors.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getBlend</span><span class="SyntaxCode">(col0: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, col1: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, factor: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Blend two colors, keeping the alpha channel of <code class="incode">col0</code> untouched.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getBlendKeepAlpha</span><span class="SyntaxCode">(col0: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, col1: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, factor: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns a darker color.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getDarker</span><span class="SyntaxCode">(col: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, factor: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns the distance between the other color.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getDistanceRgb</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, col: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns a lighter color.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getLighter</span><span class="SyntaxCode">(col: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, factor: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isOpaque</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(self, argb: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(self, name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(self, argb: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opEquals</span><span class="SyntaxCode">(self, argb: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opEquals</span><span class="SyntaxCode">(self, other: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opEquals</span><span class="SyntaxCode">(self, argb: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set all components.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setArgb</span><span class="SyntaxCode">(self, a: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, r: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set all components with floats in the range [0 1].</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setArgbf</span><span class="SyntaxCode">(self, a: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, r: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Initialize color with hue, saturation, lightness.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setHsl</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, h: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, s: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, l: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Just set r, g and b components.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRgb</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rgb: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
<p>
Just set r, g and b components.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRgb</span><span class="SyntaxCode">(self, r: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</p>
<p>
 Alpha will be opaque.</p>
<p>
 Alpha will be opaque.</p>
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
<p>
Set r,g,b with floats in the range [0 1].</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRgbf</span><span class="SyntaxCode">(self, r: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
<p>
 Alpha will be opaque.</p>
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
<p>
Retrieve all components as floating point values between 0 and 1.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toArgbf</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
{ 
	a: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
	r: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
	g: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
	b: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
}</code>
</p>
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
<p>
Returns hue, saturation, lightness.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toHsl</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
{ 
	h: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
	s: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
	l: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
}</code>
</p>
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
<p>
Retrieve all components as floating point values between 0 and 1.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toRgbf</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
{ 
	r: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
	g: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
	b: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
}</code>
</p>
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
<p>
Get the color as a [Math.Vector4].</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toVector4</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Vector4</span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
r</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
g</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
b</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
a</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
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
<td class="tdname">
id</td>
<td class="tdtype">
<a href="#Pixel_CommandId">Pixel.CommandId</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
<b>using</b> params</td>
<td class="tdtype">
{clear: {color: Pixel.Color}, drawTriangles: {start: u32, count: u32}, transform: {tr: Core.Math.Transform2}, clippingRect: {rect: Core.Math.Rectangle}, clippingRegion: {mode: Pixel.ClippingMode}, font: {fontRef: *Pixel.Font}, blendingMode: {mode: Pixel.BlendingMode}, textureFont: {font: *Pixel.Font, atlasIndex: s32}, colorMask: Pixel.ColorMask, renderTgt: {tgt: *Pixel.RenderTarget, paintAlpha: bool}, shader: {shader: *Pixel.ShaderBase}, shaderParam: {param: Pixel.ShaderParamHandle, type: const *Swag.TypeInfo, _f32: f32, _s32: s32}, texture0: {boundRect: Core.Math.Vector4, textureRect: Core.Math.Vector4, type: Pixel.BrushType, hatch: Pixel.HatchStyle, uvMode: Pixel.UVMode, interpolationMode: Pixel.InterpolationMode, texture: Pixel.Texture}}</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Clear</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ColorMask</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Transform</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DrawTriangles</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BindTexture0</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ResetTexture0</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BindTextureFont</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ResetTextureFont</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SetClippingRect</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ResetClippingRect</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
StartClippingRegion</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EndClippingRegion</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ResetClippingRegion</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SetClippingRegionMode</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
UpdateFont</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
StartNoOverlap</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
StopNoOverlap</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SetBlendingMode</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BindRenderTarget</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
UnbindRenderTarget</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ReleaseRenderTarget</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SetShader</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SetShaderParam</td>
<td class="enumeration">
</td>
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
<td class="tdname">
None</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Triangle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Round</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Square</td>
<td class="enumeration">
</td>
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
<td class="tdname">
None</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Pattern</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Dot</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Dash</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DashDot</td>
<td class="enumeration">
</td>
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
<td class="tdname">
decodePixels</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Default</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Blur</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Separate</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Merge</td>
<td class="enumeration">
</td>
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
<td class="tdname">
type</td>
<td class="tdtype">
<a href="#Pixel_ElementType">Pixel.ElementType</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
position</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
p1</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
p2</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Line</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Arc</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Bezier1</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Bezier2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Curve</td>
<td class="enumeration">
</td>
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
<td class="tdname">
ascent</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
descent</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
height</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
underlinePos</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
underlineSize</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
forceBold</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
forceItalic</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
fullname</td>
<td class="tdtype">
Core.String</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
typeFace</td>
<td class="tdtype">
*Pixel.TypeFace</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
size</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
atlases</td>
<td class="tdtype">
Core.Array'(Pixel.GlyphAtlas)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
asciiRef</td>
<td class="tdtype">
[128] Pixel.GlyphDesc</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
unicodeRef</td>
<td class="tdtype">
Core.HashTable'(rune, *Pixel.GlyphDesc)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dirty</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Font_create">create(*TypeFace, u32, bool, bool)</a></td>
<td class="enumeration">
<p>
Creates a new Font of the given typeface and size.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Font_create">create(string, u32, bool, bool)</a></td>
<td class="enumeration">
<p>
Creates a new Font with the given filename and size.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Font_getGlyphDesc">getGlyphDesc(self, rune)</a></td>
<td class="enumeration">
<p>
Returns the glyph descriptor of a given rune.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Font_release">release(*Font)</a></td>
<td class="enumeration">
<p>
Release the given font.</p>
</td>
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
<p>
Creates a new Font of the given typeface and size.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(typeFace: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, forceBold = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">, forceItalic = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode"></code>
</p>
<p>
Creates a new Font with the given filename and size.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(fullname: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, forceBold = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">, forceItalic = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns the glyph descriptor of a given rune.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getGlyphDesc</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, c: </span><span class="SyntaxType">rune</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Release the given font.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">release</span><span class="SyntaxCode">(font: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">)</code>
</p>
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
<td class="tdname">
regular</td>
<td class="tdtype">
*Pixel.Font</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bold</td>
<td class="tdtype">
*Pixel.Font</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
italic</td>
<td class="tdtype">
*Pixel.Font</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
boldItalic</td>
<td class="tdtype">
*Pixel.Font</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_FontFamily_createTypeFace">createTypeFace(const ref FontFamilyEnumerate, FontFamilyStyle)</a></td>
<td class="enumeration">
<p>
Creates a typeface from a FontFamilyEnumerate and FontFamilyStyle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_FontFamily_enumerateFromOs">enumerateFromOs()</a></td>
<td class="enumeration">
<p>
Enumerate the font families available in the os.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_FontFamily_getFont">getFont(self, FontFamilyStyle)</a></td>
<td class="enumeration">
<p>
Get the font corresponding to the style.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_FontFamily_getFromOs">getFromOs(string)</a></td>
<td class="enumeration">
<p>
Get a FontFamilyEnumerate from a font family name.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_FontFamily_setFont">setFont(self, *Font, FontFamilyStyle)</a></td>
<td class="enumeration">
<p>
Set the font corresponding to a given style.</p>
</td>
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
<p>
Creates a typeface from a FontFamilyEnumerate and FontFamilyStyle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createTypeFace</span><span class="SyntaxCode">(enumerate: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></span><span class="SyntaxCode">, style: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Enumerate the font families available in the os.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">enumerateFromOs</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Get the font corresponding to the style.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFont</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, style: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode"></code>
</p>
<p>
 If not evailable, get another one.</p>
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
<p>
Get a FontFamilyEnumerate from a font family name.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFromOs</span><span class="SyntaxCode">(fontFamily: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Set the font corresponding to a given style.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFont</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fnt: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">, style: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span><span class="SyntaxCode">)</code>
</p>
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
<td class="tdname">
name</td>
<td class="tdtype">
Core.String</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
faces</td>
<td class="tdtype">
[4] Pixel.FontFamilyStyleEnumerate</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Regular</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Bold</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Italic</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BoldItalic</td>
<td class="enumeration">
</td>
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
<td class="tdname">
name</td>
<td class="tdtype">
Core.String</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
face</td>
<td class="tdtype">
Gdi32.LOGFONTW</td>
<td class="enumeration">
</td>
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
<td class="enumeration">
<a href="#Pixel_Gif_Decoder_getFrame">getFrame(self, *Image, bool)</a></td>
<td class="enumeration">
<p>
Create an image for the current frame.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Gif_Decoder_getNumFrames">getNumFrames(self)</a></td>
<td class="enumeration">
<p>
Returns the number of frames As Gif does not store it, we need to go threw all images (!).</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Gif_Decoder_init">init(self, const [..] u8, bool)</a></td>
<td class="enumeration">
<p>
Initialize the decoder.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Gif_Decoder_nextFrame">nextFrame(self, *Image)</a></td>
<td class="enumeration">
<p>
Get the next frame. Returns false if we have reached the end.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Gif_Decoder_rewind">rewind(self)</a></td>
<td class="enumeration">
<p>
Restart at frame 0.</p>
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canDecode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">decode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, buffer: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, options: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Create an image for the current frame.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFrame</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, image: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">, decodePixels = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Returns the number of frames As Gif does not store it, we need to go threw all images (!).</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getNumFrames</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Initialize the decoder.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">init</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, buffer: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, decodePixels: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Get the next frame. Returns false if we have reached the end.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">nextFrame</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, img: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Restart at frame 0.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">rewind</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<td class="tdname">
texture</td>
<td class="tdtype">
<a href="#Pixel_Texture">Pixel.Texture</a></td>
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
height</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bpp</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
datas</td>
<td class="tdtype">
Core.Array'(u8)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dirty</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
curX</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
curY</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
nextY</td>
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
<span class="content" id="Pixel_GlyphDesc"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">GlyphDesc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="tdname">
uv</td>
<td class="tdtype">
Core.Math.Rectangle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
atlasIndex</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
advanceX</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
advanceY</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
shiftX</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
shiftY</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
width</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
height</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Dot</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Horizontal</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
HorizontalLight</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
HorizontalNarrow</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Vertical</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
VerticalLight</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
VerticalNarrow</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SolidDiamond</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
OutlinedDiamond</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
HorizontalDark</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
VerticalDark</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Grid</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
GridLarge</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
GridDotted</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Cross</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
CrossLarge</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent5</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent10</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent20</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent25</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent30</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent40</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent50</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent60</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent70</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent75</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent80</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Percent90</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
CheckBoard</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
CheckBoardLarge</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Weave</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DiagonalForward</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DiagonalForwardLight</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DiagonalBackward</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DiagonalBackwardLight</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DiagonalCross</td>
<td class="enumeration">
</td>
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
<p>
Interface to decode a buffer.</p>
<table class="enumeration">
<tr>
<td class="tdname">
canDecode</td>
<td class="tdtype">
func(*Pixel.IImageDecoder, string)->bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
decode</td>
<td class="tdtype">
func(*Pixel.IImageDecoder, const [..] u8, Pixel.DecodeOptions)->Pixel.Image throw</td>
<td class="enumeration">
</td>
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
<p>
Interface to decode a buffer.</p>
<table class="enumeration">
<tr>
<td class="tdname">
canEncode</td>
<td class="tdtype">
func(*Pixel.IImageEncoder, string)->bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
encode</td>
<td class="tdtype">
func(*Pixel.IImageEncoder, *Core.ConcatBuffer, Pixel.Image, any) throw</td>
<td class="enumeration">
</td>
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
<p>
An image buffer, in various pixel formats.</p>
<table class="enumeration">
<tr>
<td class="tdname">
pixels</td>
<td class="tdtype">
Core.Array'(u8)</td>
<td class="enumeration">
<p>
All the pixels.</p>
</td>
</tr>
<tr>
<td class="tdname">
size</td>
<td class="tdtype">
u64</td>
<td class="enumeration">
<p>
Size in bytes.</p>
</td>
</tr>
<tr>
<td class="tdname">
width</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
<p>
The width of the image.</p>
</td>
</tr>
<tr>
<td class="tdname">
height</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
<p>
The height of the image.</p>
</td>
</tr>
<tr>
<td class="tdname">
width8</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
<p>
The width, in bytes.</p>
</td>
</tr>
<tr>
<td class="tdname">
pf</td>
<td class="tdtype">
<a href="#Pixel_PixelFormat">Pixel.PixelFormat</a></td>
<td class="enumeration">
<p>
Format of one pixel.</p>
</td>
</tr>
<tr>
<td class="tdname">
bpp</td>
<td class="tdtype">
u8</td>
<td class="enumeration">
<p>
Bits per pixel.</p>
</td>
</tr>
<tr>
<td class="tdname">
bpp8</td>
<td class="tdtype">
u8</td>
<td class="enumeration">
<p>
Bytes per pixel.</p>
</td>
</tr>
<tr>
<td class="tdname">
workingBuffer</td>
<td class="tdtype">
*Pixel.Image</td>
<td class="enumeration">
<p>
Temporary buffer for filters and transformations.</p>
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Image_addDecoder">addDecoder()</a></td>
<td class="enumeration">
<p>
Register an image decoder.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_addEncoder">addEncoder()</a></td>
<td class="enumeration">
<p>
Register an image encoder.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_allocPixels">allocPixels(self)</a></td>
<td class="enumeration">
<p>
Allocate pixels.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_allocWorkingBuffer">allocWorkingBuffer(self)</a></td>
<td class="enumeration">
<p>
Allocate a computing buffer with current image size.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_allocWorkingBuffer">allocWorkingBuffer(self, s32, s32, PixelFormat)</a></td>
<td class="enumeration">
<p>
Allocate a computing buffer with new sizes.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_applyKernel">applyKernel(self, Kernel)</a></td>
<td class="enumeration">
<p>
Apply a kernel to the image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_applyWorkingBuffer">applyWorkingBuffer(self)</a></td>
<td class="enumeration">
<p>
Replace the current image content with the working buffer.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_canLoad">canLoad(string)</a></td>
<td class="enumeration">
<p>
Returns <code class="incode">true</code> if the given filename has a corresponding decoder.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_canSave">canSave(string)</a></td>
<td class="enumeration">
<p>
Returns <code class="incode">true</code> if the given filename has a corresponding encoder.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_clear">clear(self)</a></td>
<td class="enumeration">
<p>
Clear the content of the image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_colorize">colorize(self, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Colorize the image by setting the hue and saturation.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_contrast">contrast(self, f32)</a></td>
<td class="enumeration">
<p>
Change the contrast. <code class="incode">factor</code> is [-1, 1].</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_copyPixel">copyPixel(^u8, ^u8)</a></td>
<td class="enumeration">
<p>
Copy one pixel, depending on bpp.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_create">create(s32, s32, PixelFormat, const [..] u8)</a></td>
<td class="enumeration">
<p>
Creates a new image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_crop">crop(self, const ref Rectangle)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_crop">crop(self, s32, s32, s32, s32)</a></td>
<td class="enumeration">
<p>
Crop image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_decode">decode(string, const [..] u8, const ref DecodeOptions)</a></td>
<td class="enumeration">
<p>
Decode the given image buffer.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_fade">fade(self, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Fade to a given color. <code class="incode">factor</code> is [0, 1].</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_fill">fill(self, const ref Color)</a></td>
<td class="enumeration">
<p>
Fill image with <code class="incode">color</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_fillGradient2">fillGradient2(self, const ref Color, const ref Color)</a></td>
<td class="enumeration">
<p>
Fill with an horizontal gradient from <code class="incode">color0</code> to <code class="incode">color1</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_fillGradient4">fillGradient4(self, const ref Color, const ref Color, const ref Color, const ref Color)</a></td>
<td class="enumeration">
<p>
Fill with a gradient with a different color at each corner.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_fillHsl">fillHsl(self, FillHslType, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_flip">flip(self)</a></td>
<td class="enumeration">
<p>
Flip image vertically.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_freeWorkingBuffer">freeWorkingBuffer(self)</a></td>
<td class="enumeration">
<p>
Free the working buffer if allocated.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_from">from(HBITMAP)</a></td>
<td class="enumeration">
<p>
Creates an image from a windows HBITMAP.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_from">from(HICON)</a></td>
<td class="enumeration">
<p>
Creates an image from a windows HICON.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_gamma">gamma(self, f32)</a></td>
<td class="enumeration">
<p>
Change the gamma.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_getPixelColor">getPixelColor(self, s32, s32)</a></td>
<td class="enumeration">
<p>
Returns the color at the given coordinate.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_grayScale">grayScale(self, f32)</a></td>
<td class="enumeration">
<p>
Transform image to grayscale, with a given factor.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_halfSize">halfSize(self)</a></td>
<td class="enumeration">
<p>
Divide image size by 2 with a bilinear 2x2 filter.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_init">init(self, s32, s32, PixelFormat)</a></td>
<td class="enumeration">
<p>
Initialize image informations <code class="incode">pixels</code> is set to null, and must be initialized after a call to that function This gives the opportunity to set the pixels with an external buffer.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_invert">invert(self)</a></td>
<td class="enumeration">
<p>
Invert colors.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_isValid">isValid(self)</a></td>
<td class="enumeration">
<p>
Returns true if the image is valid.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_lightness">lightness(self, f32)</a></td>
<td class="enumeration">
<p>
Change the lightness. <code class="incode">factor</code> is [-1, 1].</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_load">load(string, const ref DecodeOptions)</a></td>
<td class="enumeration">
<p>
Load the given image file.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_mirror">mirror(self)</a></td>
<td class="enumeration">
<p>
Flip image horizontally.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_mix">mix(self, const ref Image, s32, s32, MixMode)</a></td>
<td class="enumeration">
<p>
Mix with another image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_release">release(self)</a></td>
<td class="enumeration">
<p>
Release the content of the image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_resize">resize(self, s32, s32, ResizeMode)</a></td>
<td class="enumeration">
<p>
Resize image Put <code class="incode">newWidth</code> or <code class="incode">newHeight</code> to 0 to keep aspect ratio.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_save">save(self, string, any)</a></td>
<td class="enumeration">
<p>
Save the image to a file.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_setAlpha">setAlpha(self, u8)</a></td>
<td class="enumeration">
<p>
Change the alpha channel.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_setChannel">setChannel(self, ChannelMode, ChannelValueMode, u8)</a></td>
<td class="enumeration">
<p>
Change specified color channels.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_setPixelFormat">setPixelFormat(self, PixelFormat)</a></td>
<td class="enumeration">
<p>
Change image pixel format.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_setPixelRGBA8">setPixelRGBA8(^u8, u8, u8, u8, u8)</a></td>
<td class="enumeration">
<p>
Set pixel values depending on pixel format.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toApplyKernel">toApplyKernel(self, Kernel)</a></td>
<td class="enumeration">
<p>
Apply a kernel to the image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toCrop">toCrop(self, const ref Rectangle)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toCrop">toCrop(self, s32, s32, s32, s32)</a></td>
<td class="enumeration">
<p>
Crop image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toFlip">toFlip(self)</a></td>
<td class="enumeration">
<p>
Flip image vertically.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toHICON">toHICON(self)</a></td>
<td class="enumeration">
<p>
Transform an image to a windows icon.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toHalfSize">toHalfSize(self)</a></td>
<td class="enumeration">
<p>
Divide image size by 2 with a bilinear 2x2 filter.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toMirror">toMirror(self)</a></td>
<td class="enumeration">
<p>
Flip image horizontally.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toMix">toMix(self, const ref Image, s32, s32, MixMode)</a></td>
<td class="enumeration">
<p>
Mix with another image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toResize">toResize(self, s32, s32, ResizeMode)</a></td>
<td class="enumeration">
<p>
Resize image Put <code class="incode">newWidth</code> or <code class="incode">newHeight</code> to 0 to keep aspect ratio.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toSetPixelFormat">toSetPixelFormat(self, PixelFormat)</a></td>
<td class="enumeration">
<p>
Change image pixel format.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_toTurn">toTurn(self, TurnAngle)</a></td>
<td class="enumeration">
<p>
Turn image by a given predefined angle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_turn">turn(self, TurnAngle)</a></td>
<td class="enumeration">
<p>
Turn image by a given predefined angle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Image_visitPixels">visitPixels(self, *void, s32, code)</a></td>
<td class="enumeration">
<p>
Macro to visit all pixels of the image in parallel chunks.</p>
</td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Image_opVisit">opVisit(self, code)</a></td>
<td class="enumeration">
</td>
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
<td class="tdname">
Red</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Green</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Blue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Alpha</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
RGB</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
RGBA</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Color</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MinRGB</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MaxRGB</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MeanRGB</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Alpha</td>
<td class="enumeration">
</td>
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
<td class="tdname">
HueVert</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
HueHorz</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SaturationVert</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SaturationHorz</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightnessVert</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
LightnessHorz</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
HueSaturation</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
HueLightness</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SaturationLightness</td>
<td class="enumeration">
<p>
(h,0,1) (h,1,0.5) (h,0,0) (h,1,0).</p>
</td>
</tr>
<tr>
<td class="tdname">
GreenBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
RedBlue</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
RedGreen</td>
<td class="enumeration">
</td>
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
<p>
Kernel format is : kernelSize, kernelNormalizeValue, kernelValues...</p>
<table class="enumeration">
<tr>
<td class="tdname">
GaussianBlur3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
GaussianBlur3x3B</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
GaussianBlur5x5A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BoxBlur3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BoxBlur5x5A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Sharpen3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Sharpen3x3B</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Soften3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EmbossLeft3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EmbossLeft3x3B</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EmbossLeft3x3C</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EmbossRight3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EmbossRight3x3B</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EmbossRight3x3C</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EdgeDetect3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EdgeDetect3x3B</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EdgeDetect3x3C</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EdgeDetect3x3D</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EdgeDetect3x3E</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
EdgeDetect3x3F</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SobelHorizontal3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SobelVertical3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PrevitHorizontal3x3A</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PrevitVertical3x3A</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Copy</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
AlphaBlend</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Raw</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Bilinear</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Bicubic</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Gaussian</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Quadratic</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Hermite</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Hamming</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Catrom</td>
<td class="enumeration">
</td>
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
<td class="tdname">
T90</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
T180</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
T270</td>
<td class="enumeration">
</td>
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
<p>
Register an image decoder.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">addDecoder</span><span class="SyntaxCode">()</code>
</p>
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
<p>
Register an image encoder.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">addEncoder</span><span class="SyntaxCode">()</code>
</p>
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
<p>
Allocate pixels.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">allocPixels</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Allocate a computing buffer with current image size.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">allocWorkingBuffer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
<p>
Allocate a computing buffer with new sizes.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">allocWorkingBuffer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, newWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, newHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, newPf: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Apply a kernel to the image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">applyKernel</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, kernel: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image_Kernel">Kernel</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Replace the current image content with the working buffer.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">applyWorkingBuffer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Returns <code class="incode">true</code> if the given filename has a corresponding decoder.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canLoad</span><span class="SyntaxCode">(fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns <code class="incode">true</code> if the given filename has a corresponding encoder.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canSave</span><span class="SyntaxCode">(fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p>
Clear the content of the image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Colorize the image by setting the hue and saturation.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">colorize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, hue: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, saturation: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, strength = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Change the contrast. <code class="incode">factor</code> is [-1, 1].</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">contrast</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, factor: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Copy one pixel, depending on bpp.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">BPP</span><span class="SyntaxCode">: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">copyPixel</span><span class="SyntaxCode">(pixDst: ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, pixSrc: ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Creates a new image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(width: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pf: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span><span class="SyntaxCode">, buffer: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Crop image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">crop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cropRect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">crop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, w: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, h: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Decode the given image buffer.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">decode</span><span class="SyntaxCode">(fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, bytes: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, options: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span><span class="SyntaxCode"> = {})-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Fade to a given color. <code class="incode">factor</code> is [0, 1].</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fade</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, factor: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Fill image with <code class="incode">color</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fill</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Fill with an horizontal gradient from <code class="incode">color0</code> to <code class="incode">color1</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillGradient2</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, color0: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, color1: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Fill with a gradient with a different color at each corner.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillGradient4</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, topLeft: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, topRight: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, bottomLeft: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, bottomRight: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillHsl</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fillType: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image_FillHslType">FillHslType</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"> = </span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">White</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Flip image vertically.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">flip</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Free the working buffer if allocated.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">freeWorkingBuffer</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Creates an image from a windows HBITMAP.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">from</span><span class="SyntaxCode">(hbitmap: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
<p>
Creates an image from a windows HICON.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">from</span><span class="SyntaxCode">(hicon: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Change the gamma.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">gamma</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, level: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">2.2</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Returns the color at the given coordinate.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getPixelColor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pixX: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pixY: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Transform image to grayscale, with a given factor.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">grayScale</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, factor: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Divide image size by 2 with a bilinear 2x2 filter.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">halfSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Initialize image informations <code class="incode">pixels</code> is set to null, and must be initialized after a call to that function This gives the opportunity to set the pixels with an external buffer.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">init</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, width: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, pf: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Invert colors.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">invert</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Returns true if the image is valid.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isValid</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Change the lightness. <code class="incode">factor</code> is [-1, 1].</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">lightness</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, factor: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Load the given image file.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">load</span><span class="SyntaxCode">(fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, options: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span><span class="SyntaxCode"> = {})-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Flip image horizontally.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mirror</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Mix with another image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">mix</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, srcImage: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, mode = </span><span class="SyntaxConstant"><a href="#Pixel_Image_MixMode">MixMode</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Copy</span><span class="SyntaxCode">)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[<a href="swag.runtime.html#Swag_Macro">Swag.Macro</a>]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(ptr: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">opVisit</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, stmt: code)</code>
</p>
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
<p>
Release the content of the image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">release</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Resize image Put <code class="incode">newWidth</code> or <code class="incode">newHeight</code> to 0 to keep aspect ratio.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">resize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, newWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, newHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, mode = </span><span class="SyntaxConstant"><a href="#Pixel_Image_ResizeMode">ResizeMode</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Raw</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Save the image to a file.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">save</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, options: </span><span class="SyntaxType">any</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Change the alpha channel.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setAlpha</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Change specified color channels.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setChannel</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mode: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image_ChannelMode">ChannelMode</a></span><span class="SyntaxCode">, value: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image_ChannelValueMode">ChannelValueMode</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxType">u8</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Change image pixel format.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setPixelFormat</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, newPf: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set pixel values depending on pixel format.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">PF</span><span class="SyntaxCode">: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span><span class="SyntaxCode">) </span><span class="SyntaxFunction">setPixelRGBA8</span><span class="SyntaxCode">(pixDst: ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, r: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, _a: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Apply a kernel to the image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toApplyKernel</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, kernel: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image_Kernel">Kernel</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Crop image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toCrop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self, cropRect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toCrop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, w: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, h: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Flip image vertically.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toFlip</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Transform an image to a windows icon.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toHICON</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Divide image size by 2 with a bilinear 2x2 filter.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toHalfSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Flip image horizontally.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toMirror</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Mix with another image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toMix</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self, srcImage: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, mode = </span><span class="SyntaxConstant"><a href="#Pixel_Image_MixMode">MixMode</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Copy</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Resize image Put <code class="incode">newWidth</code> or <code class="incode">newHeight</code> to 0 to keep aspect ratio.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toResize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self, newWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, newHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, mode = </span><span class="SyntaxConstant"><a href="#Pixel_Image_ResizeMode">ResizeMode</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Raw</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Change image pixel format.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toSetPixelFormat</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self, newPf: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Turn image by a given predefined angle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">toTurn</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self, angle: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image_TurnAngle">TurnAngle</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Turn image by a given predefined angle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">turn</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, angle: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image_TurnAngle">TurnAngle</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Macro to visit all pixels of the image in parallel chunks.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[<a href="swag.runtime.html#Swag_Macro">Swag.Macro</a>]</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">visitPixels</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self, userData: *</span><span class="SyntaxType">void</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">, stride: </span><span class="SyntaxType">s32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">, stmt: code)</code>
</p>
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
<td class="tdname">
Pixel</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Linear</td>
<td class="enumeration">
</td>
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
<td class="tdname">
None</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Bevel</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Round</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Miter</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MiterBevel</td>
<td class="enumeration">
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canDecode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">decode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, buffer: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, options: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
quality</td>
<td class="tdtype">
u8</td>
<td class="enumeration">
<p>
Encoding quality between 1 and 100.</p>
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canEncode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">encode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, result: *</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ConcatBuffer</span><span class="SyntaxCode">, image: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">, options: </span><span class="SyntaxType">any</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
None</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Triangle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Round</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Square</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ArrowAnchor</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SquareAnchor</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
RoundAnchor</td>
<td class="enumeration">
</td>
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
<td class="tdname">
elements</td>
<td class="tdtype">
Core.Array'(Pixel.Element)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
startPoint</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
endPoint</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
isClosed</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
isDirty</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
serial</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
flattenQuality</td>
<td class="tdtype">
<a href="#Pixel_PaintQuality">Pixel.PaintQuality</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
isPolyOnly</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
isFlatten</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bvMin</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bvMax</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
endPosBuffer</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
polygon</td>
<td class="tdtype">
<a href="#Pixel_Polygon">Pixel.Polygon</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
points</td>
<td class="tdtype">
Core.Array'(*Pixel.Poly2Tri.Point)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
triangles</td>
<td class="tdtype">
Core.Array'(*Pixel.Poly2Tri.Triangle)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
edgeList</td>
<td class="tdtype">
Core.Array'(*Pixel.Poly2Tri.Edge)</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_arcTo">arcTo(self, f32, f32, f32, f32, const ref Angle, const ref Angle)</a></td>
<td class="enumeration">
<p>
Add an arc from the previous point.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_bezierTo">bezierTo(self, f32, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Add a curve from the previous point to <code class="incode">pt</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_bezierTo">bezierTo(self, f32, f32, f32, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Add a curve from the previous point to <code class="incode">pt</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_clearCache">clearCache(self)</a></td>
<td class="enumeration">
<p>
Clear internal cache.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_close">close(self)</a></td>
<td class="enumeration">
<p>
Close the figure Will add a line or a curve to the first point if necessary.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_curveTo">curveTo(self, f32, f32)</a></td>
<td class="enumeration">
<p>
Add a curve from the previous point to <code class="incode">pt</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_flatten">flatten(self, PaintQuality)</a></td>
<td class="enumeration">
<p>
Convert the path to a list of points.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_lineTo">lineTo(self, f32, f32)</a></td>
<td class="enumeration">
<p>
Add a line from the previous point to <code class="incode">pt</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_setArc">setArc(self, f32, f32, f32, const ref Angle, const ref Angle)</a></td>
<td class="enumeration">
<p>
Initialize the path with an arc.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_setCircle">setCircle(self, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Initialize the path with a circle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_setEllipse">setEllipse(self, f32, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Initialize the path with an ellipse.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_setRect">setRect(self, f32, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Initialize the path with a rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_setRoundRect">setRoundRect(self, f32, f32, f32, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Initialize the path with a round rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePath_start">start(self, f32, f32)</a></td>
<td class="enumeration">
<p>
Reset the path.</p>
</td>
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
<p>
Add an arc from the previous point.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">arcTo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, initAngle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">, maxAngle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Add a curve from the previous point to <code class="incode">pt</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">bezierTo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, ctx: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, cty: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
<p>
Add a curve from the previous point to <code class="incode">pt</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">bezierTo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, ct1x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, ct1y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, ct2x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, ct2y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Clear internal cache.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clearCache</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Close the figure Will add a line or a curve to the first point if necessary.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">close</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Add a curve from the previous point to <code class="incode">pt</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">curveTo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Convert the path to a list of points.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">flatten</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, quality = </span><span class="SyntaxConstant"><a href="#Pixel_PaintQuality">PaintQuality</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Normal</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Add a line from the previous point to <code class="incode">pt</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">lineTo</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Initialize the path with an arc.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setArc</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radius: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, startAngle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">, endAngle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Initialize the path with a circle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setCircle</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radius: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Initialize the path with an ellipse.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setEllipse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Initialize the path with a rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Initialize the path with a round rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRoundRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Reset the path.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">start</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, startX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, startY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<td class="tdname">
paths</td>
<td class="tdtype">
Core.Array'(*Pixel.LinePath)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bvMin</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bvMax</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_LinePathList_clean">clean(self)</a></td>
<td class="enumeration">
<p>
Clean all paths They must before be flattened.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePathList_clear">clear(self)</a></td>
<td class="enumeration">
<p>
Remove all internal line paths.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePathList_count">count(self)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePathList_flatten">flatten(self, PaintQuality)</a></td>
<td class="enumeration">
<p>
Flatten all paths.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePathList_newPath">newPath(self)</a></td>
<td class="enumeration">
<p>
Returns a new path.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePathList_offset">offset(self, f32)</a></td>
<td class="enumeration">
<p>
Call fast offset on all paths They must before be flattened.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_LinePathList_offset">offset(self, f32, JoinStyle, f32)</a></td>
<td class="enumeration">
<p>
Call quality offset on all paths They must before be flattened.</p>
</td>
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
<p>
Clean all paths They must before be flattened.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clean</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Remove all internal line paths.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">count</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Flatten all paths.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">flatten</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, quality: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PaintQuality">PaintQuality</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Returns a new path.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">newPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Call fast offset on all paths They must before be flattened.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">offset</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
<p>
Call quality offset on all paths They must before be flattened.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">offset</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, joinStyle: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_JoinStyle">JoinStyle</a></span><span class="SyntaxCode">, toler: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">)</code>
</p>
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
<td class="enumeration">
<a href="#Pixel_NativeRenderOgl_createContext">createContext(self, HDC, s32, s32, HGLRC)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_NativeRenderOgl_createContext">createContext(self, HWND, s32, s32, HGLRC)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_NativeRenderOgl_dropContext">dropContext(self, const ref RenderingContext)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_NativeRenderOgl_setCurrentContext">setCurrentContext(self, const ref RenderingContext)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_NativeRenderOgl_swapBuffers">swapBuffers(self, const ref RenderingContext)</a></td>
<td class="enumeration">
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createContext</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, mainRC: </span><span class="SyntaxConstant">Ogl</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLRC</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createContext</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, mainRC: </span><span class="SyntaxConstant">Ogl</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLRC</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">dropContext</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rc: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span><span class="SyntaxCode">)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setCurrentContext</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rc: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span><span class="SyntaxCode">)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">swapBuffers</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rc: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span><span class="SyntaxCode">)</code>
</p>
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
<td class="tdname">
Zero</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Antialiased</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Default</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Draft</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Normal</td>
<td class="enumeration">
</td>
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
<td class="tdname">
paintParams</td>
<td class="tdtype">
<a href="#Pixel_PaintParams">Pixel.PaintParams</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
paintQuality</td>
<td class="tdtype">
<a href="#Pixel_PaintQuality">Pixel.PaintQuality</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
transform</td>
<td class="tdtype">
Core.Math.Transform2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
blendingMode</td>
<td class="tdtype">
<a href="#Pixel_BlendingMode">Pixel.BlendingMode</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
interpolationMode</td>
<td class="tdtype">
<a href="#Pixel_InterpolationMode">Pixel.InterpolationMode</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
clippingRect</td>
<td class="tdtype">
Core.Math.Rectangle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
stackClipRect</td>
<td class="tdtype">
Core.Array'(Core.Math.Rectangle)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
clippingRectOn</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
colorMask</td>
<td class="tdtype">
<a href="#Pixel_ColorMask">Pixel.ColorMask</a></td>
<td class="enumeration">
</td>
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
<td class="tdname">
commandBuffer</td>
<td class="tdtype">
Core.Array'(Pixel.Command)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
vertexBuffer</td>
<td class="tdtype">
Core.Array'(Pixel.VertexLayout)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
triangulateIdx</td>
<td class="tdtype">
Core.Array'(s32)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
fake</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sharedSolidPen</td>
<td class="tdtype">
<a href="#Pixel_Pen">Pixel.Pen</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sharedSolidBrush</td>
<td class="tdtype">
<a href="#Pixel_Brush">Pixel.Brush</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sharedLinePath</td>
<td class="tdtype">
<a href="#Pixel_LinePath">Pixel.LinePath</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sharedLinePathList</td>
<td class="tdtype">
<a href="#Pixel_LinePathList">Pixel.LinePathList</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
sharedRoundTmp</td>
<td class="tdtype">
Core.Array'({factor: f32, pos: Core.Math.Vector2})</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
curState</td>
<td class="tdtype">
<a href="#Pixel_PaintState">Pixel.PaintState</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
stackState</td>
<td class="tdtype">
Core.Array'(Pixel.PaintState)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
stackTransform</td>
<td class="tdtype">
Core.Array'(Core.Math.Transform2)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lastTexture0Type</td>
<td class="tdtype">
<a href="#Pixel_BrushType">Pixel.BrushType</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
stackRT</td>
<td class="tdtype">
Core.Array'(*Pixel.RenderTarget)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
curRT</td>
<td class="tdtype">
*Pixel.RenderTarget</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lastTexture0Handle</td>
<td class="tdtype">
Pixel.TextureHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hasTextureFont</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
drawingBegin</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
overlapMode</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
canSetTexture0</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_addParams">addParams(self, PaintParams)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_begin">begin(self)</a></td>
<td class="enumeration">
<p>
Call this before drawing.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_bindRenderTarget">bindRenderTarget(self, *RenderTarget, bool)</a></td>
<td class="enumeration">
<p>
Bind a render target.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_capStyleExtent">capStyleExtent(LineCapStyle, f32, f32)</a></td>
<td class="enumeration">
<p>
Returns the extent in both directions of a given LineCapStyle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_clear">clear(self, const ref Color)</a></td>
<td class="enumeration">
<p>
Clear the rendering surface.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_computeLayout">computeLayout(self, const ref Rectangle, *RichString, const ref RichStringFormat)</a></td>
<td class="enumeration">
<p>
Recompute the layout for a given rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawArc">drawArc(self, f32, f32, f32, const ref Angle, const ref Angle, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Draw an arc between two angles.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawArc">drawArc(self, f32, f32, f32, const ref Angle, const ref Angle, const ref Pen)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawCircle">drawCircle(self, f32, f32, f32, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Draw an empty circle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawCircle">drawCircle(self, f32, f32, f32, const ref Pen)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawEllipse">drawEllipse(self, f32, f32, f32, f32, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Draw an empty ellipse.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawEllipse">drawEllipse(self, f32, f32, f32, f32, const ref Pen)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawLine">drawLine(self, const ref Vector2, const ref Vector2, const ref Pen)</a></td>
<td class="enumeration">
<p>
Draw a line.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawLine">drawLine(self, f32, f32, f32, f32, const ref Color, f32)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawLine">drawLine(self, f32, f32, f32, f32, const ref Pen)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawPath">drawPath(self, *LinePath, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Draw the line path.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawPath">drawPath(self, *LinePath, const ref Pen)</a></td>
<td class="enumeration">
<p>
Draw the line path with the given <code class="incode">pen</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawPath">drawPath(self, *LinePathList, const ref Color, f32, DrawPathListMode)</a></td>
<td class="enumeration">
<p>
Draw the line path.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawPath">drawPath(self, *LinePathList, const ref Pen, DrawPathListMode)</a></td>
<td class="enumeration">
<p>
Draw the line path.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawRect">drawRect(self, const ref Rectangle, const ref Color, f32)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawRect">drawRect(self, const ref Rectangle, const ref Pen)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawRect">drawRect(self, f32, f32, f32, f32, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Draw an empty rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawRect">drawRect(self, f32, f32, f32, f32, const ref Pen)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawRichString">drawRichString(self, const ref Rectangle, *RichString, const ref RichStringFormat, bool)</a></td>
<td class="enumeration">
<p>
Draw a rich string.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawRoundRect">drawRoundRect(self, const ref Rectangle, f32, f32, const ref Color, f32)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawRoundRect">drawRoundRect(self, const ref Rectangle, f32, f32, const ref Pen)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawRoundRect">drawRoundRect(self, f32, f32, f32, f32, f32, f32, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Draw an empty round rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawRoundRect">drawRoundRect(self, f32, f32, f32, f32, f32, f32, const ref Pen)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawString">drawString(self, f32, f32, string, *Font, const ref Brush, UnderlineStyle, f32)</a></td>
<td class="enumeration">
<p>
Draw a simple string.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawString">drawString(self, f32, f32, string, *Font, const ref Color, UnderlineStyle, f32)</a></td>
<td class="enumeration">
<p>
Draw a simple string.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawStringCenter">drawStringCenter(self, f32, f32, string, *Font, const ref Color, UnderlineStyle, f32)</a></td>
<td class="enumeration">
<p>
Draw a simple string (centered).</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawTexture">drawTexture(self, const ref Rectangle, const ref Rectangle, const ref Texture, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawTexture">drawTexture(self, const ref Rectangle, const ref Texture, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawTexture">drawTexture(self, f32, f32, const ref Rectangle, const ref Texture, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawTexture">drawTexture(self, f32, f32, const ref Texture, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawTexture">drawTexture(self, f32, f32, f32, f32, const ref Rectangle, const ref Texture, const ref Color)</a></td>
<td class="enumeration">
<p>
Draw a portion of an image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_drawTexture">drawTexture(self, f32, f32, f32, f32, const ref Texture, const ref Color)</a></td>
<td class="enumeration">
<p>
Draw a texture image.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_end">end(self)</a></td>
<td class="enumeration">
<p>
Call this after drawing.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_endClippingRegion">endClippingRegion(self)</a></td>
<td class="enumeration">
<p>
Stop painting in the clipping buffer. Back to normal.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillCircle">fillCircle(self, f32, f32, f32, const ref Brush)</a></td>
<td class="enumeration">
<p>
Draw a solid circle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillCircle">fillCircle(self, f32, f32, f32, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillEllipse">fillEllipse(self, f32, f32, f32, f32, const ref Brush)</a></td>
<td class="enumeration">
<p>
Draw a solid ellipse.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillEllipse">fillEllipse(self, f32, f32, f32, f32, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillPath">fillPath(self, *LinePath, const ref Brush)</a></td>
<td class="enumeration">
<p>
Fill the line path with the given <code class="incode">brush</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillPath">fillPath(self, *LinePath, const ref Color)</a></td>
<td class="enumeration">
<p>
Fill the line path.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillPath">fillPath(self, *LinePathList, const ref Brush)</a></td>
<td class="enumeration">
<p>
Fill the list of paths clockwise = fill, anti-clockwise = hole.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillPath">fillPath(self, *LinePathList, const ref Color)</a></td>
<td class="enumeration">
<p>
Fill the line path.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillPolygon">fillPolygon(self, *LinePath, const ref Brush)</a></td>
<td class="enumeration">
<p>
Fill the polygon with the given <code class="incode">brush</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillPolygon">fillPolygon(self, *LinePath, const ref Color)</a></td>
<td class="enumeration">
<p>
Fill the polygon with the given color.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillRect">fillRect(self, const ref Rectangle, const ref Brush)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillRect">fillRect(self, const ref Rectangle, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillRect">fillRect(self, f32, f32, f32, f32, const ref Brush)</a></td>
<td class="enumeration">
<p>
Draw a filled rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillRect">fillRect(self, f32, f32, f32, f32, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillRoundRect">fillRoundRect(self, const ref Rectangle, f32, f32, const ref Brush)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillRoundRect">fillRoundRect(self, const ref Rectangle, f32, f32, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillRoundRect">fillRoundRect(self, f32, f32, f32, f32, f32, f32, const ref Brush)</a></td>
<td class="enumeration">
<p>
Draw a filled round rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_fillRoundRect">fillRoundRect(self, f32, f32, f32, f32, f32, f32, const ref Color)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_getClippingRect">getClippingRect(self)</a></td>
<td class="enumeration">
<p>
Get the current clipping rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_getGlyphs">getGlyphs(const ref Array'(rune), *Font)</a></td>
<td class="enumeration">
<p>
Returns all the glyphs for a given string and font.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_getGlyphs">getGlyphs(string, *Font)</a></td>
<td class="enumeration">
<p>
Returns all the glyphs for a given string and font.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_getParams">getParams(self)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_getQuality">getQuality(self)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_getTransform">getTransform(self)</a></td>
<td class="enumeration">
<p>
Returns the current tranformation.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_isEmptyClippingRect">isEmptyClippingRect(self)</a></td>
<td class="enumeration">
<p>
Returns true if the current clipping rectangle is empty (no drawing !).</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_measureRune">measureRune(rune, *Font)</a></td>
<td class="enumeration">
<p>
Get a rune size for a given font.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_measureString">measureString(const ref Array'(const *GlyphDesc))</a></td>
<td class="enumeration">
<p>
Returns the bounding rectangle if a given text, at a given position.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_measureString">measureString(string, *Font)</a></td>
<td class="enumeration">
<p>
Returns the metrics of a given text.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_popClippingRect">popClippingRect(self)</a></td>
<td class="enumeration">
<p>
Restore the original clipping rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_popState">popState(self)</a></td>
<td class="enumeration">
<p>
Restore the last paint state.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_popTransform">popTransform(self)</a></td>
<td class="enumeration">
<p>
Restore the pushed transformation.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_pushClippingRect">pushClippingRect(self, const ref Rectangle, bool)</a></td>
<td class="enumeration">
<p>
Interface the given rectangle with the current clipping rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_pushClippingSurfaceRect">pushClippingSurfaceRect(self, const ref Rectangle)</a></td>
<td class="enumeration">
<p>
Interface the given rectangle with the current clipping rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_pushState">pushState(self)</a></td>
<td class="enumeration">
<p>
Save the current paint state.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_pushTransform">pushTransform(self)</a></td>
<td class="enumeration">
<p>
Push the current transformation.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_releaseRenderTarget">releaseRenderTarget(self, *RenderTarget)</a></td>
<td class="enumeration">
<p>
Release the render target.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_removeParams">removeParams(self, PaintParams)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_resetClippingRect">resetClippingRect(self)</a></td>
<td class="enumeration">
<p>
Reset the current clipping rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_resetClippingRegion">resetClippingRegion(self)</a></td>
<td class="enumeration">
<p>
Reset clipping buffer to its default value (no more clipping).</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_resetState">resetState(self)</a></td>
<td class="enumeration">
<p>
Reset the current painting state.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_resetTransform">resetTransform(self)</a></td>
<td class="enumeration">
<p>
Reset the paint position, rotation, scale.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_rotateTransform">rotateTransform(self, const ref Angle)</a></td>
<td class="enumeration">
<p>
Set the paint rotation.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_rotateTransform">rotateTransform(self, const ref Angle, f32, f32)</a></td>
<td class="enumeration">
<p>
Set the paint rotation.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_rotateTransformInPlace">rotateTransformInPlace(self, const ref Angle)</a></td>
<td class="enumeration">
<p>
Set the paint rotation.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_scaleTransform">scaleTransform(self, f32, f32)</a></td>
<td class="enumeration">
<p>
Set the paint scale.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setAntialiased">setAntialiased(self, bool)</a></td>
<td class="enumeration">
<p>
Set antialiased mode on/off.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setBlendingMode">setBlendingMode(self, BlendingMode)</a></td>
<td class="enumeration">
<p>
Set the current blending mode.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setClippingRect">setClippingRect(self, const ref Rectangle)</a></td>
<td class="enumeration">
<p>
Set the current clipping rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setClippingRect">setClippingRect(self, f32, f32, f32, f32)</a></td>
<td class="enumeration">
<p>
Set the current clipping rectangle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setClippingRegionMode">setClippingRegionMode(self, ClippingMode)</a></td>
<td class="enumeration">
<p>
Set the clipping region mode.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setColorMask">setColorMask(self, bool, bool, bool, bool)</a></td>
<td class="enumeration">
<p>
Set color mask.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setColorMask">setColorMask(self, const ref ColorMask)</a></td>
<td class="enumeration">
<p>
Set color mask.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setColorMaskAlpha">setColorMaskAlpha(self)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setColorMaskColor">setColorMaskColor(self)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setColorMaskFull">setColorMaskFull(self)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setInterpolationMode">setInterpolationMode(self, InterpolationMode)</a></td>
<td class="enumeration">
<p>
Set the texture interpolation mode.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setParams">setParams(self, PaintParams, PaintParams)</a></td>
<td class="enumeration">
<p>
Set the paint parameters.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setQuality">setQuality(self, PaintQuality)</a></td>
<td class="enumeration">
<p>
Set the paint quality.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setShader">setShader(self, *ShaderBase)</a></td>
<td class="enumeration">
<p>
Set the current shader.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setShaderParam">setShaderParam(self, ShaderParamHandle, f32)</a></td>
<td class="enumeration">
<p>
Set a shader parameter.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setShaderParam">setShaderParam(self, ShaderParamHandle, s32)</a></td>
<td class="enumeration">
<p>
Set a shader parameter.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setState">setState(self, const ref PaintState)</a></td>
<td class="enumeration">
<p>
Set the current painting state.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_setTransform">setTransform(self, const ref Transform2)</a></td>
<td class="enumeration">
<p>
Set the paint transformation.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_startClippingRegion">startClippingRegion(self, ClippingMode)</a></td>
<td class="enumeration">
<p>
Start painting in the clipping buffer.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_translateTransform">translateTransform(self, f32, f32)</a></td>
<td class="enumeration">
<p>
Set the paint position.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Painter_unbindRenderTarget">unbindRenderTarget(self)</a></td>
<td class="enumeration">
<p>
Bind a render target.</p>
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addParams</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, add: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Call this before drawing.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">begin</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Bind a render target.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">bindRenderTarget</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, tgt: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span><span class="SyntaxCode">, paintAlpha = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Returns the extent in both directions of a given LineCapStyle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">capStyleExtent</span><span class="SyntaxCode">(capStyle: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LineCapStyle">LineCapStyle</a></span><span class="SyntaxCode">, lineSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, capScale: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
{ 
	w: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
	l: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">
}</code>
</p>
<p>
<ul>
<li> <code class="incode">lineSize</code> is the supposed line drawing size</li>
<li> <code class="incode">capScale</code> is the supposed LineCapStyle scale</li>
</ul>
 Returns the width and the length.</p>
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
<p>
Clear the rendering surface.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Recompute the layout for a given rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">computeLayout</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, text: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RichString">RichString</a></span><span class="SyntaxCode">, format: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RichStringFormat">RichStringFormat</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw an arc between two angles.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawArc</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radius: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, startAngle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">, endAngle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawArc</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radius: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, startAngle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">, endAngle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw an empty circle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawCircle</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radius: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawCircle</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radius: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw an empty ellipse.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawEllipse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawEllipse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw a line.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, start: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Vector2</span><span class="SyntaxCode">, end: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Vector2</span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, startX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, startY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, endX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, endY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, startX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, startY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, endX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, endY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw the line path.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, path: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
<p>
Draw the line path with the given <code class="incode">pen</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, path: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)</code>
</p>
<p>
Draw the line path.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pathList: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, mode: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DrawPathListMode">DrawPathListMode</a></span><span class="SyntaxCode"> = .</span><span class="SyntaxConstant">Separate</span><span class="SyntaxCode">)</code>
</p>
<p>
Draw the line path.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pathList: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">, mode: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DrawPathListMode">DrawPathListMode</a></span><span class="SyntaxCode"> = .</span><span class="SyntaxConstant">Separate</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw an empty rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw a rich string.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawRichString</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pos: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, text: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RichString">RichString</a></span><span class="SyntaxCode">, format: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RichStringFormat">RichStringFormat</a></span><span class="SyntaxCode">, forceCompute = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)</code>
</p>
<p>
 A rich string can be multiline, and accepts The layout will be computed at the first call, and each time <code class="incode">pos</code> has changed</p>
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
<p>
Draw an empty round rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawRoundRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawRoundRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawRoundRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, penSize: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawRoundRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, pen: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw a simple string.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawString</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, font: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">, under = </span><span class="SyntaxConstant"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">None</span><span class="SyntaxCode">, zoom = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)</code>
</p>
<p>
Draw a simple string.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawString</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, font: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, under = </span><span class="SyntaxConstant"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">None</span><span class="SyntaxCode">, zoom = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw a simple string (centered).</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawStringCenter</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, font: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, under = </span><span class="SyntaxConstant"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">None</span><span class="SyntaxCode">, zoom = </span><span class="SyntaxNumber">1.0</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw a portion of an image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawTexture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, dstRect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, srcRect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"> = </span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">White</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawTexture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"> = </span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">White</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawTexture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, srcRect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"> = </span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">White</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawTexture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"> = </span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">White</span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawTexture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, srcRect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"> = </span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">White</span><span class="SyntaxCode">)</code>
</p>
<p>
Draw a texture image.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">drawTexture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode"> = </span><span class="SyntaxConstant"><a href="#Pixel_Argb">Argb</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">White</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Call this after drawing.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">end</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Stop painting in the clipping buffer. Back to normal.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">endClippingRegion</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Draw a solid circle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillCircle</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radius: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillCircle</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radius: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw a solid ellipse.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillEllipse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillEllipse</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Fill the line path with the given <code class="incode">brush</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, path: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">)</code>
</p>
<p>
Fill the line path.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, path: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
<p>
Fill the list of paths clockwise = fill, anti-clockwise = hole.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pathList: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">)</code>
</p>
<p>
Fill the line path.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillPath</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pathList: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Fill the polygon with the given <code class="incode">brush</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillPolygon</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, path: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">)</code>
</p>
<p>
Fill the polygon with the given color.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillPolygon</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, path: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePath">LinePath</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw a filled rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw a filled round rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillRoundRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillRoundRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillRoundRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, brush: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Brush">Brush</a></span><span class="SyntaxCode">)</code>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">fillRoundRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, left: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, top: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, radiusY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Get the current clipping rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getClippingRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns all the glyphs for a given string and font.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getGlyphs</span><span class="SyntaxCode">(text: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxType">rune</span><span class="SyntaxCode">), font: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span><span class="SyntaxCode">)</code>
</p>
<p>
Returns all the glyphs for a given string and font.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getGlyphs</span><span class="SyntaxCode">(text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, font: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span><span class="SyntaxCode">)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getParams</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getQuality</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Returns the current tranformation.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Transform2</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns true if the current clipping rectangle is empty (no drawing !).</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isEmptyClippingRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p>
Get a rune size for a given font.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">measureRune</span><span class="SyntaxCode">(c: </span><span class="SyntaxType">rune</span><span class="SyntaxCode">, font: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Point">Point</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns the bounding rectangle if a given text, at a given position.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">measureString</span><span class="SyntaxCode">(text: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span><span class="SyntaxCode">))-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_StringMetrics">StringMetrics</a></span><span class="SyntaxCode"></code>
</p>
<p>
Returns the metrics of a given text.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">measureString</span><span class="SyntaxCode">(text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, font: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_StringMetrics">StringMetrics</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Restore the original clipping rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">popClippingRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Restore the last paint state.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">popState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Restore the pushed transformation.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">popTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Interface the given rectangle with the current clipping rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pushClippingRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, isSurfacePos = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Interface the given rectangle with the current clipping rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pushClippingSurfaceRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Save the current paint state.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pushState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Push the current transformation.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pushTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Release the render target.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">releaseRenderTarget</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, tgt: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span><span class="SyntaxCode">)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">removeParams</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, remove: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Reset the current clipping rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">resetClippingRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Reset clipping buffer to its default value (no more clipping).</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">resetClippingRegion</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Reset the current painting state.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">resetState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Reset the paint position, rotation, scale.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">resetTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Set the paint rotation.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">rotateTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, angle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">)</code>
</p>
<p>
Set the paint rotation.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">rotateTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, angle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the paint rotation.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">rotateTransformInPlace</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, angle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">)</code>
</p>
<p>
 Will rotate around the current transformation position.</p>
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
<p>
Set the paint scale.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">scaleTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set antialiased mode on/off.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setAntialiased</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, aa = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the current blending mode.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setBlendingMode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mode: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_BlendingMode">BlendingMode</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the current clipping rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setClippingRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">)</code>
</p>
<p>
Set the current clipping rectangle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setClippingRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, width: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, height: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the clipping region mode.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setClippingRegionMode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mode: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_ClippingMode">ClippingMode</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set color mask.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColorMask</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, r: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">, g: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">, b: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">, a: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</p>
<p>
Set color mask.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColorMask</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, cm: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_ColorMask">ColorMask</a></span><span class="SyntaxCode">)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColorMaskAlpha</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColorMaskColor</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setColorMaskFull</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Set the texture interpolation mode.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setInterpolationMode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mode: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_InterpolationMode">InterpolationMode</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the paint parameters.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setParams</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, add: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span><span class="SyntaxCode">, remove: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PaintParams">PaintParams</a></span><span class="SyntaxCode"> = .</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the paint quality.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setQuality</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, quality: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PaintQuality">PaintQuality</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the current shader.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setShader</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, shader: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set a shader parameter.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setShaderParam</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, param: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ShaderParamHandle</span><span class="SyntaxCode">, val: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
<p>
Set a shader parameter.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setShaderParam</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, param: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ShaderParamHandle</span><span class="SyntaxCode">, val: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the current painting state.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setState</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, state: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PaintState">PaintState</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the paint transformation.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, tr: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Transform2</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Start painting in the clipping buffer.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">startClippingRegion</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mode = </span><span class="SyntaxConstant"><a href="#Pixel_ClippingMode">ClippingMode</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Set</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the paint position.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">translateTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Bind a render target.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">unbindRenderTarget</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<td class="tdname">
size</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
brush</td>
<td class="tdtype">
<a href="#Pixel_Brush">Pixel.Brush</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
pattern</td>
<td class="tdtype">
Core.Array'(f32)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dashBeginCapStyle</td>
<td class="tdtype">
<a href="#Pixel_DashCapStyle">Pixel.DashCapStyle</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dashEndCapStyle</td>
<td class="tdtype">
<a href="#Pixel_DashCapStyle">Pixel.DashCapStyle</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
borderPos</td>
<td class="tdtype">
<a href="#Pixel_BorderPos">Pixel.BorderPos</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
capStartScaleX</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
capStartScaleY</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
capEndScaleX</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
capEndScaleY</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
joinMiterLimit</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
plotOffset</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dashStyle</td>
<td class="tdtype">
<a href="#Pixel_DashStyle">Pixel.DashStyle</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
beginCapStyle</td>
<td class="tdtype">
<a href="#Pixel_LineCapStyle">Pixel.LineCapStyle</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
endCapStyle</td>
<td class="tdtype">
<a href="#Pixel_LineCapStyle">Pixel.LineCapStyle</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
joinStyle</td>
<td class="tdtype">
<a href="#Pixel_JoinStyle">Pixel.JoinStyle</a></td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Pen_createDash">createDash(DashStyle, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Returns a dash pen.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Pen_createHatch">createHatch(HatchStyle, const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Return a predefined hatch pen.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Pen_createSolid">createSolid(const ref Color, f32)</a></td>
<td class="enumeration">
<p>
Returns a solid color pen.</p>
</td>
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
<p>
Returns a dash pen.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createDash</span><span class="SyntaxCode">(style: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DashStyle">DashStyle</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, size: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Return a predefined hatch pen.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createHatch</span><span class="SyntaxCode">(type: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_HatchStyle">HatchStyle</a></span><span class="SyntaxCode">, color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, size: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns a solid color pen.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createSolid</span><span class="SyntaxCode">(color: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">, size: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Pen">Pen</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Desribe the layout format of one image pixel.</p>
<table class="enumeration">
<tr>
<td class="tdname">
BGR8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BGRA8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
RGB8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
RGBA8</td>
<td class="enumeration">
</td>
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
<p>
Returns the bit depth of each channel.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">bitDepth</span><span class="SyntaxCode">(self)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns the bpp (bits per pixel) of the given pixel format.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">bpp</span><span class="SyntaxCode">(self)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns the number of image channels.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">channels</span><span class="SyntaxCode">(self)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns true if the given pixel format has an alpha channel.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">hasAlpha</span><span class="SyntaxCode">(self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canDecode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">decode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, buffer: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, options: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
compressionLevel</td>
<td class="tdtype">
Core.Compress.Deflate.CompressionLevel</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
filtering</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canEncode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">encode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, result: *</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ConcatBuffer</span><span class="SyntaxCode">, image: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">, options: </span><span class="SyntaxType">any</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
p</td>
<td class="tdtype">
*Pixel.Poly2Tri.Point</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
q</td>
<td class="tdtype">
*Pixel.Poly2Tri.Point</td>
<td class="enumeration">
</td>
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
<td class="tdname">
<b>using</b> v</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
edgeList</td>
<td class="tdtype">
Core.Array'(*Pixel.Poly2Tri.Edge)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
border</td>
<td class="tdtype">
[2] *Pixel.Poly2Tri.Edge</td>
<td class="enumeration">
</td>
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
<td class="enumeration">
<a href="#Pixel_Poly2Tri_Tesselate_addPoint">addPoint(self, f32, f32)</a></td>
<td class="enumeration">
<p>
Register a new point.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Poly2Tri_Tesselate_clear">clear(self)</a></td>
<td class="enumeration">
<p>
Clear content.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Poly2Tri_Tesselate_endPolyLine">endPolyLine(self)</a></td>
<td class="enumeration">
<p>
To call to register the polyline.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Poly2Tri_Tesselate_getTriangles">getTriangles(self)</a></td>
<td class="enumeration">
<p>
Returns the list of triangles.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Poly2Tri_Tesselate_startPolyLine">startPolyLine(self)</a></td>
<td class="enumeration">
<p>
To call before adding a polyline.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Poly2Tri_Tesselate_triangulate">triangulate(self)</a></td>
<td class="enumeration">
<p>
Triangulate.</p>
</td>
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
<p>
Register a new point.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addPoint</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Clear content.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
To call to register the polyline.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">endPolyLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Returns the list of triangles.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTriangles</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;[..] *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Poly2Tri</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Poly2Tri_Triangle">Triangle</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
To call before adding a polyline.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">startPolyLine</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Triangulate.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">triangulate</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<td class="tdname">
constrainedEdge</td>
<td class="tdtype">
[3] bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
delaunayEdge</td>
<td class="tdtype">
[3] bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
points</td>
<td class="tdtype">
[3] *Pixel.Poly2Tri.Point</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
neighbors</td>
<td class="tdtype">
[3] *Pixel.Poly2Tri.Triangle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
interior</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
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
<td class="tdname">
points</td>
<td class="tdtype">
Core.Array'(Core.Math.Vector2)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
isClockwise</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
isConvex</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
area</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
totalLen</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bvMin</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bvMax</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_add">add(self, const ref Vector2)</a></td>
<td class="enumeration">
<p>
Add a new point.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_clean">clean(self)</a></td>
<td class="enumeration">
<p>
Clean polygon by removing bad/unecessary points.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_clear">clear(self)</a></td>
<td class="enumeration">
<p>
Clear all points.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_compute">compute(self)</a></td>
<td class="enumeration">
<p>
Compute internal values, like convex/clockwise Call it once the polygon contains all its points.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_getClean">getClean(self)</a></td>
<td class="enumeration">
<p>
Clean polygon by removing bad/unecessary points.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_getOffset">getOffset(self, f32, JoinStyle, f32)</a></td>
<td class="enumeration">
<p>
Slow but quality offset with a joinStyle.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_getPoint">getPoint(self, u64)</a></td>
<td class="enumeration">
<p>
Get the given point.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_getPoint">getPoint(self, u64, f32)</a></td>
<td class="enumeration">
<p>
Get the given point, with an optional offset.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_normalizeDist">normalizeDist(self, f32, bool)</a></td>
<td class="enumeration">
<p>
Divide by adding points if two points are too far away.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_offset">offset(self, f32)</a></td>
<td class="enumeration">
<p>
Fast expand polygon by a given amount.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Polygon_offset">offset(self, f32, JoinStyle, f32)</a></td>
<td class="enumeration">
<p>
Slow but quality offset with a joinStyle.</p>
</td>
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
<p>
Add a new point.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">add</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, v: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Vector2</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Clean polygon by removing bad/unecessary points.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clean</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Clear all points.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Compute internal values, like convex/clockwise Call it once the polygon contains all its points.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">compute</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Clean polygon by removing bad/unecessary points.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getClean</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Slow but quality offset with a joinStyle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getOffset</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, joinStyle: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_JoinStyle">JoinStyle</a></span><span class="SyntaxCode">, toler: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Get the given point.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getPoint</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, i: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Vector2</span><span class="SyntaxCode"></code>
</p>
<p>
Get the given point, with an optional offset.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getPoint</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, i: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, offset: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Vector2</span><span class="SyntaxCode"></code>
</p>
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
<p>
Divide by adding points if two points are too far away.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">normalizeDist</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, dist: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, closed: </span><span class="SyntaxType">bool</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Fast expand polygon by a given amount.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">offset</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
<p>
Slow but quality offset with a joinStyle.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">offset</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, joinStyle: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_JoinStyle">JoinStyle</a></span><span class="SyntaxCode">, toler: </span><span class="SyntaxType">f32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">)</code>
</p>
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
<td class="tdname">
<b>using</b> native</td>
<td class="tdtype">
<a href="#Pixel_NativeRenderOgl">Pixel.NativeRenderOgl</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
vertexbuffer</td>
<td class="tdtype">
Ogl.GLuint</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
shaderSimple</td>
<td class="tdtype">
<a href="#Pixel_ShaderSimple">Pixel.ShaderSimple</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
shaderAA</td>
<td class="tdtype">
<a href="#Pixel_ShaderAA">Pixel.ShaderAA</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
shaderBlur</td>
<td class="tdtype">
<a href="#Pixel_ShaderBlur">Pixel.ShaderBlur</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
worldTransform</td>
<td class="tdtype">
Core.Math.Transform2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
orthoMatrix</td>
<td class="tdtype">
Core.Math.Matrix4x4</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
modelMatrix</td>
<td class="tdtype">
Core.Math.Matrix4x4</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
whiteTexture</td>
<td class="tdtype">
Ogl.GLuint</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hatchTextures</td>
<td class="tdtype">
[36] Ogl.GLuint</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
blendingMode</td>
<td class="tdtype">
<a href="#Pixel_BlendingMode">Pixel.BlendingMode</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
curRC</td>
<td class="tdtype">
<a href="#Pixel_RenderingContext">Pixel.RenderingContext</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
stackReleasedRT</td>
<td class="tdtype">
Core.Array'(*Pixel.RenderTarget)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
stackRT</td>
<td class="tdtype">
Core.Array'(*Pixel.RenderTarget)</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
curRT</td>
<td class="tdtype">
*Pixel.RenderTarget</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
curShader</td>
<td class="tdtype">
*Pixel.ShaderBase</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
overlapMode</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_addImage">addImage(self, const ref Image)</a></td>
<td class="enumeration">
<p>
Register an image for rendering.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_addImage">addImage(self, string)</a></td>
<td class="enumeration">
<p>
Load and register an image for rendering.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_begin">begin(self, const ref RenderingContext)</a></td>
<td class="enumeration">
<p>
To be called before rendering.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_bindRenderTarget">bindRenderTarget(self, *RenderTarget, *Rectangle)</a></td>
<td class="enumeration">
<p>
Set the current render target.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_clear">clear(self)</a></td>
<td class="enumeration">
<p>
Clear current render buffers.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_computeMatrices">computeMatrices(self, ShaderParamHandle, ShaderParamHandle)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_createRenderTarget">createRenderTarget(self, s32, s32)</a></td>
<td class="enumeration">
<p>
Create a render target.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_createShader">createShader(self, *ShaderBase, const [..] u8, const [..] u8)</a></td>
<td class="enumeration">
<p>
Create the given shader.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_deleteRenderTarget">deleteRenderTarget(self, *RenderTarget)</a></td>
<td class="enumeration">
<p>
Delete a render target.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_draw">draw(self, *Painter)</a></td>
<td class="enumeration">
<p>
Draw the given painter.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_end">end(self)</a></td>
<td class="enumeration">
<p>
To be called after rendering.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_getRenderTarget">getRenderTarget(self, s32, s32)</a></td>
<td class="enumeration">
<p>
Get a render target of the given size releaseRenderTarget must be called to release it.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_getTargetSize">getTargetSize(self)</a></td>
<td class="enumeration">
<p>
Get the actual render target size.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_getTexturePixels">getTexturePixels(self, const ref Texture, PixelFormat)</a></td>
<td class="enumeration">
<p>
Returns the pixels associated to a texture.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_init">init(self)</a></td>
<td class="enumeration">
<p>
First init.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_pixelFormatToNative">pixelFormatToNative(PixelFormat)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_releaseRenderTarget">releaseRenderTarget(self, *RenderTarget)</a></td>
<td class="enumeration">
<p>
Release the given render target.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_removeTexture">removeTexture(self, *Texture)</a></td>
<td class="enumeration">
<p>
Unregister a texture created with <code class="incode">addImage</code>.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_resetScissorRect">resetScissorRect(self)</a></td>
<td class="enumeration">
<p>
Reset the scissor.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_resetTransform">resetTransform(self)</a></td>
<td class="enumeration">
<p>
Reset the world transform.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_rotateTransform">rotateTransform(self, const ref Angle)</a></td>
<td class="enumeration">
<p>
Rotate the world transform.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_rotateTransform">rotateTransform(self, const ref Angle, f32, f32)</a></td>
<td class="enumeration">
<p>
Rotate the world transform.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_scaleTransform">scaleTransform(self, f32, f32)</a></td>
<td class="enumeration">
<p>
Scale the world transform.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_setCurrentContext">setCurrentContext(self, const ref RenderingContext)</a></td>
<td class="enumeration">
<p>
Set the current rendering context.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_setScissorRect">setScissorRect(self, const ref Rectangle)</a></td>
<td class="enumeration">
<p>
Force the scissor.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_setShader">setShader(self, *ShaderBase)</a></td>
<td class="enumeration">
<p>
Set the current shader.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_translateTransform">translateTransform(self, f32, f32)</a></td>
<td class="enumeration">
<p>
Translate the world transform.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_unbindRenderTarget">unbindRenderTarget(self)</a></td>
<td class="enumeration">
<p>
Go back to the previous render target, or frame buffer.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_unbindRenderTargetToImage">unbindRenderTargetToImage(self, s32, s32, s32, s32)</a></td>
<td class="enumeration">
<p>
Get current render target pixels.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RenderOgl_updateTexture">updateTexture(self, const ref Texture, const *u8)</a></td>
<td class="enumeration">
<p>
Update content of texture.</p>
</td>
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
<p>
Register an image for rendering.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addImage</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, image: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode"></code>
</p>
<p>
Load and register an image for rendering.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">addImage</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fullName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
To be called before rendering.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">begin</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rc: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the current render target.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">bindRenderTarget</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rt: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span><span class="SyntaxCode">, vp: *</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Clear current render buffers.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">clear</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">computeMatrices</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, mvp: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ShaderParamHandle</span><span class="SyntaxCode">, mdl: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ShaderParamHandle</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Create a render target.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createRenderTarget</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, w: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, h: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Create the given shader.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createShader</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, base: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span><span class="SyntaxCode">, vertex: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, pixel: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Delete a render target.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">deleteRenderTarget</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rt: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw the given painter.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">draw</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, dc: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Painter">Painter</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
To be called after rendering.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">end</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Get a render target of the given size releaseRenderTarget must be called to release it.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getRenderTarget</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, w: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, h: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Get the actual render target size.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTargetSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxKeyword">struct</span><span class="SyntaxCode">
{ 
	width: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
	height: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">
}</code>
</p>
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
<p>
Returns the pixels associated to a texture.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getTexturePixels</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">, pf: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Array</span><span class="SyntaxCode">'(</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</p>
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
<p>
First init.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">init</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pixelFormatToNative</span><span class="SyntaxCode">(pf: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_PixelFormat">PixelFormat</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Ogl</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">GLenum</span><span class="SyntaxCode"></code>
</p>
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
<p>
Release the given render target.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">releaseRenderTarget</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, tgt: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderTarget">RenderTarget</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Unregister a texture created with <code class="incode">addImage</code>.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">removeTexture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, texture: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Reset the scissor.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">resetScissorRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Reset the world transform.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">resetTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Rotate the world transform.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">rotateTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, angle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">)</code>
</p>
<p>
Rotate the world transform.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">rotateTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, angle: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Angle</span><span class="SyntaxCode">, centerX: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, centerY: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Scale the world transform.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">scaleTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the current rendering context.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setCurrentContext</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rc: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_RenderingContext">RenderingContext</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Force the scissor.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setScissorRect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set the current shader.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setShader</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, shader: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_ShaderBase">ShaderBase</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Translate the world transform.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">translateTransform</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Go back to the previous render target, or frame buffer.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">unbindRenderTarget</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Get current render target pixels.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">unbindRenderTargetToImage</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, w: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, h: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"></code>
</p>
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
<p>
Update content of texture.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">updateTexture</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, texture: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Texture">Texture</a></span><span class="SyntaxCode">, pixels: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</p>
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
<td class="tdname">
handle</td>
<td class="tdtype">
Pixel.RenderTargetHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
colorBuffer</td>
<td class="tdtype">
<a href="#Pixel_Texture">Pixel.Texture</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
depthStencil</td>
<td class="tdtype">
Pixel.RenderTargetSurfaceHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
paintAlpha</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
paintAlphaIdx</td>
<td class="tdtype">
u64</td>
<td class="enumeration">
</td>
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
<td class="tdname">
hDC</td>
<td class="tdtype">
Win32.HDC</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hRC</td>
<td class="tdtype">
Ogl.HGLRC</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
width</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
height</td>
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
<span class="content" id="Pixel_RichChunk"><span class="titletype">struct</span> <span class="titlelight">Pixel.</span><span class="titlestrong">RichChunk</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="tdname">
slice</td>
<td class="tdtype">
string</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
fontStyle</td>
<td class="tdtype">
<a href="#Pixel_FontFamilyStyle">Pixel.FontFamilyStyle</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hasFontStyle</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
isSpace</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
isEol</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
colorIdx</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
boundRect</td>
<td class="tdtype">
Core.Math.Rectangle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
pos</td>
<td class="tdtype">
Core.Math.Point</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
font</td>
<td class="tdtype">
*Pixel.Font</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
under</td>
<td class="tdtype">
<a href="#Pixel_UnderlineStyle">Pixel.UnderlineStyle</a></td>
<td class="enumeration">
</td>
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
<p>
Represents a string with embedded formats.</p>
<table class="enumeration">
<tr>
<td class="tdname">
raw</td>
<td class="tdtype">
Core.String</td>
<td class="enumeration">
<p>
The string as passed by the user.</p>
</td>
</tr>
<tr>
<td class="tdname">
chunks</td>
<td class="tdtype">
Core.Array'(Pixel.RichChunk)</td>
<td class="enumeration">
<p>
The corresponding list of chunks.</p>
</td>
</tr>
<tr>
<td class="tdname">
layoutRect</td>
<td class="tdtype">
Core.Math.Rectangle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
boundRect</td>
<td class="tdtype">
Core.Math.Rectangle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dirty</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
<p>
If dirty, <code class="incode">Compute</code>.</p>
</td>
</tr>
</table>
<p>
 This kind of string is a specific version of [Core.String] but specific for painting. It can contains some special markers to change it's appearence.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode">&lt;b&gt; this is bold&lt;b&gt;
 &lt;p1&gt; this will have special color </span><span class="SyntaxNumber">1</span><span class="SyntaxCode">&lt;p1&gt;
 ...</code>
</p>
<p>
 See <a href="#Pixel_RichString_set">RichString.set</a> for a list of available markers.</p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_RichString_compute">compute(self, bool)</a></td>
<td class="enumeration">
<p>
Compute layout.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RichString_getNaked">getNaked(string)</a></td>
<td class="enumeration">
<p>
Transform the given string to a naked one.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RichString_invalidate">invalidate(self)</a></td>
<td class="enumeration">
<p>
Force the string to be repainted.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RichString_isEmpty">isEmpty(self)</a></td>
<td class="enumeration">
<p>
Returns <code class="incode">true</code> if the string is empty.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RichString_set">set(self, string)</a></td>
<td class="enumeration">
<p>
Associate a string.</p>
</td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_RichString_opAffect">opAffect(self, string)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RichString_opCount">opCount(self)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_RichString_opEquals">opEquals(self, string)</a></td>
<td class="enumeration">
</td>
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
<p>
Compute layout.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">compute</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, force = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)</code>
</p>
<p>
 This will transform the string in a serie of chunks, each chunk having its one style. The string will only be recomputed if it's dirty (the text has changed) or if <code class="incode">force</code> is true.</p>
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
<p>
Transform the given string to a naked one.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getNaked</span><span class="SyntaxCode">(str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">String</span><span class="SyntaxCode"></code>
</p>
<p>
 Will return the string without the format markers.</p>
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
<p>
Force the string to be repainted.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">invalidate</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p>
Returns <code class="incode">true</code> if the string is empty.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isEmpty</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opAffect</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opCount</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opEquals</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, other: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p>
Associate a string.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">set</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</p>
<p>
 The string can have multiple lines. Accepted markers are:<ul>
<li> <code class="incode">&lt;u&gt;</code> =&gt; underline</li>
<li> <code class="incode">&lt;b&gt;</code> =&gt; bold</li>
<li> <code class="incode">&lt;i&gt;</code> =&gt; italic</li>
<li> <code class="incode">&lt;p1&gt;</code> =&gt; color palette 1</li>
<li> <code class="incode">&lt;p2&gt;</code> =&gt; color palette 2</li>
<li> <code class="incode">&lt;p3&gt;</code> =&gt; color palette 3</li>
</ul>
</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode">&lt;b&gt;this is bold&lt;b&gt; but this is normal &lt;i&gt;</span><span class="SyntaxLogic">and</span><span class="SyntaxCode"> this is italic&lt;/i&gt;</code>
</p>
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
<td class="tdname">
font</td>
<td class="tdtype">
<a href="#Pixel_FontFamily">Pixel.FontFamily</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
fontStyle</td>
<td class="tdtype">
<a href="#Pixel_FontFamilyStyle">Pixel.FontFamilyStyle</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
palette</td>
<td class="tdtype">
[4] Pixel.Color</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
brush</td>
<td class="tdtype">
<a href="#Pixel_Brush">Pixel.Brush</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
horzAlignment</td>
<td class="tdtype">
<a href="#Pixel_StringHorzAlignment">Pixel.StringHorzAlignment</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
vertAlignment</td>
<td class="tdtype">
<a href="#Pixel_StringVertAlignment">Pixel.StringVertAlignment</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lineGap</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
flags</td>
<td class="tdtype">
<a href="#Pixel_StringFormatFlags">Pixel.StringFormatFlags</a></td>
<td class="enumeration">
</td>
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
<td class="tdname">
<b>using</b> base</td>
<td class="tdtype">
<a href="#Pixel_ShaderBase">Pixel.ShaderBase</a></td>
<td class="enumeration">
</td>
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
<td class="tdname">
handle</td>
<td class="tdtype">
Pixel.ShaderHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
mvp</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
mdl</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
boundRect</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
textureRect</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uvMode</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
copyMode</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
textureW</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
textureH</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
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
<td class="tdname">
<b>using</b> base</td>
<td class="tdtype">
<a href="#Pixel_ShaderBase">Pixel.ShaderBase</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
radius</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
axis</td>
<td class="tdtype">
Pixel.ShaderParamHandle</td>
<td class="enumeration">
</td>
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
<td class="tdname">
<b>using</b> base</td>
<td class="tdtype">
<a href="#Pixel_ShaderBase">Pixel.ShaderBase</a></td>
<td class="enumeration">
</td>
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
<td class="tdname">
Default</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
WordWrap</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Clip</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
DontAdaptSingleLineHeight</td>
<td class="enumeration">
</td>
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
<td class="tdname">
Center</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Left</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Right</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Justify</td>
<td class="enumeration">
</td>
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
<td class="tdname">
ascent</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
<p>
Maximum size above baseline.</p>
</td>
</tr>
<tr>
<td class="tdname">
descent</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
<p>
Maximum size below baseline.</p>
</td>
</tr>
<tr>
<td class="tdname">
width</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
height</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
<p>
ascent + descent.</p>
</td>
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
<td class="tdname">
str</td>
<td class="tdtype">
<a href="#Pixel_RichString">Pixel.RichString</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
fmt</td>
<td class="tdtype">
<a href="#Pixel_RichStringFormat">Pixel.RichStringFormat</a></td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_StringPainter_color">color(self, const ref Color)</a></td>
<td class="enumeration">
<p>
Set text color.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_StringPainter_draw">draw(self, *Painter, const ref Rectangle, string)</a></td>
<td class="enumeration">
<p>
Draw string.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_StringPainter_flags">flags(self, StringFormatFlags)</a></td>
<td class="enumeration">
<p>
Set drawing flags.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_StringPainter_font">font(self, *Font)</a></td>
<td class="enumeration">
<p>
Set font.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_StringPainter_horzAlign">horzAlign(self, StringHorzAlignment)</a></td>
<td class="enumeration">
<p>
Set horizontal alignment.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_StringPainter_vertAlign">vertAlign(self, StringVertAlignment)</a></td>
<td class="enumeration">
<p>
Set vertical alignment.</p>
</td>
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
<p>
Set text color.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">color</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, col: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Color">Color</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Draw string.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">draw</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, painter: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Painter">Painter</a></span><span class="SyntaxCode">, rect: </span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Math</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Rectangle</span><span class="SyntaxCode">, what: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set drawing flags.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">flags</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, f: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_StringFormatFlags">StringFormatFlags</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set font.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">font</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, font: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Font">Font</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set horizontal alignment.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">horzAlign</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, align: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_StringHorzAlignment">StringHorzAlignment</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Set vertical alignment.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">vertAlign</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, align: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_StringVertAlignment">StringVertAlignment</a></span><span class="SyntaxCode">)</code>
</p>
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
<td class="tdname">
Center</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Top</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Bottom</td>
<td class="enumeration">
</td>
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
<td class="tdname">
handle</td>
<td class="tdtype">
Pixel.TextureHandle</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
pf</td>
<td class="tdtype">
<a href="#Pixel_PixelFormat">Pixel.PixelFormat</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
width</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
height</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
valid</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_Texture_getByteSize">getByteSize(self)</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_Texture_isValid">isValid(self)</a></td>
<td class="enumeration">
</td>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getByteSize</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isValid</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)</code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canDecode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">decode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, buffer: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, options: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">canEncode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, fileName: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">encode</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, result: *</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ConcatBuffer</span><span class="SyntaxCode">, image: </span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_Image">Image</a></span><span class="SyntaxCode">, options: </span><span class="SyntaxType">any</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
fullname</td>
<td class="tdtype">
string</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
buffer</td>
<td class="tdtype">
Core.Array'(u8)</td>
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
forceBoldDiv</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
forceItalic</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
forceBold</td>
<td class="tdtype">
bool</td>
<td class="enumeration">
</td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_create">create(string, const [..] u8)</a></td>
<td class="enumeration">
<p>
Get or create a new typeface from a memory buffer.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_createFromHFONT">createFromHFONT(string, HFONT)</a></td>
<td class="enumeration">
<p>
Create a new typeface from a HFONT.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_getFamilyName">getFamilyName(self)</a></td>
<td class="enumeration">
<p>
Returns the underlying font family name.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_getRuneOutline">getRuneOutline(self, *LinePathList, rune, u32, u32)</a></td>
<td class="enumeration">
<p>
Convert a rune to its outline, at a given size.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_getStringOutline">getStringOutline(self, *LinePathList, string, u32, u32)</a></td>
<td class="enumeration">
<p>
Convert a string to its outline, at a given size.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_isBold">isBold(self)</a></td>
<td class="enumeration">
<p>
Returns true if the typeface is bold.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_isItalic">isItalic(self)</a></td>
<td class="enumeration">
<p>
Returns true if the typeface is italic.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_load">load(string)</a></td>
<td class="enumeration">
<p>
Get or load a new typeface.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_release">release(*TypeFace)</a></td>
<td class="enumeration">
<p>
Release the given typeface.</p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Pixel_TypeFace_renderGlyph">renderGlyph(self, rune, u32, bool, bool)</a></td>
<td class="enumeration">
<p>
Render one glyph.</p>
</td>
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
<p>
Get or create a new typeface from a memory buffer.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(fullname: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, bytes: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Create a new typeface from a HFONT.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createFromHFONT</span><span class="SyntaxCode">(fullname: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, hf: </span><span class="SyntaxConstant">Gdi32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HFONT</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns the underlying font family name.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getFamilyName</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">string</span><span class="SyntaxCode"></code>
</p>
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
<p>
Convert a rune to its outline, at a given size.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getRuneOutline</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pathList: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span><span class="SyntaxCode">, c: </span><span class="SyntaxType">rune</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, embolden: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Convert a string to its outline, at a given size.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getStringOutline</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, pathList: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_LinePathList">LinePathList</a></span><span class="SyntaxCode">, text: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, embolden: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns true if the typeface is bold.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isBold</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p>
Returns true if the typeface is italic.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isItalic</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</p>
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
<p>
Get or load a new typeface.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">load</span><span class="SyntaxCode">(fullname: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<p>
Release the given typeface.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">release</span><span class="SyntaxCode">(typeface: *</span><span class="SyntaxConstant">Pixel</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Pixel_TypeFace">TypeFace</a></span><span class="SyntaxCode">)</code>
</p>
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
<p>
Render one glyph.</p>
<p class="code">
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">renderGlyph</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, c: </span><span class="SyntaxType">rune</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, forceBold = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">, forceItalic = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">FreeType</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FT_Bitmap</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</p>
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
<td class="tdname">
Fill</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
FillSubRect</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Tile</td>
<td class="enumeration">
</td>
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
<td class="tdname">
None</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Underline</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Strikeout</td>
<td class="enumeration">
</td>
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
<td class="tdname">
pos</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uv</td>
<td class="tdtype">
Core.Math.Vector2</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
color</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
aa</td>
<td class="tdtype">
[12] Core.Math.Vector4</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
aanum</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
</td>
</tr>
</table>
</div>
</div>
</div>
</body>
</html>
