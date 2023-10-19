<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module pixel</title>
<link rel="stylesheet" type="text/css" href="css/style.css">
<script src="https://kit.fontawesome.com/f76be2b3ee.js" crossorigin="anonymous"></script>
<style>

        .container  { display: flex; flex-wrap: nowrap; flex-direction: row; margin: 0px auto; padding: 0px; }
        .left       { display: block; overflow-y: scroll; width: 500px; }
        .left-page  { margin: 10px; }
        .right      { display: block; width: 100%; }
        .right-page { max-width: 1024px; margin: 10px auto; }
        
        @media(min-width: 640px)  { .container { max-width: 640px; }}
        @media(min-width: 768px)  { .container { max-width: 768px; }}
        @media(min-width: 1024px) { .container { max-width: 1024px; }}
        @media(min-width: 1280px) { .container { max-width: 1280px; }}
        @media(min-width: 1536px) { .container { max-width: 1536px; }}
        
        @media screen and (max-width: 600px) {
            .left       { display: none; }
            .right-page { margin:  10px; }
        }

            .container { height: 100vh; }
            .right     { overflow-y: scroll; }

        body { margin: 0px; line-height: 1.3em; }
        
        .container a        { color: DoggerBlue; }
        .container a:hover  { text-decoration: underline; }
        .container img      { margin: 0 auto; }
        
        .left a     { text-decoration: none; }
        .left ul    { list-style-type: none; margin-left: -20px; }
        .left h3    { background-color: Black; color: White; padding: 6px; }
        .right h1   { margin-top: 50px; margin-bottom: 50px; }
        .right h2   { margin-top: 35px; }
        
        .strikethrough-text { text-decoration: line-through; }
        .swag-watermark     { text-align:right; font-size: 80%; margin-top: 30px; }
        .swag-watermark a   { text-decoration: none; color: inherit; }
        
        .blockquote               { border-radius: 5px; border: 1px solid; margin: 20px; padding: 10px; }
        .blockquote-default       { border-color: Orange; border-left: 6px solid Orange; background-color: LightYellow; }
        .blockquote-note          { border-color: #ADCEDD; background-color: #CDEEFD; }
        .blockquote-tip           { border-color: #BCCFBC; background-color: #DCEFDC; }
        .blockquote-warning       { border-color: #DFBDB3; background-color: #FFDDD3; }
        .blockquote-attention     { border-color: #DDBAB8; background-color: #FDDAD8; }
        .blockquote-example       { border: 2px solid LightGrey; }
        .blockquote-title-block   { margin-bottom: 10px; }
        .blockquote-title         { font-weight: bold; }
        .blockquote-default       p:first-child { margin-top: 0px; }
        .blockquote-default       p:last-child  { margin-bottom: 0px; }
        .blockquote               p:last-child  { margin-bottom: 0px; }
        
        .description-list-title   { font-weight: bold; font-style: italic; }
        .description-list-block   { margin-left: 30px; }
        
        .container table          { border: 1px solid LightGrey; border-collapse: collapse; font-size: 90%; margin-left: 20px; margin-right: 20px; }
        .container td             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; }
        .container th             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; background-color: #eeeeee; }
        
        table.api-item            { border-collapse: separate; background-color: Black; color: White; width: 100%; margin-top: 70px; margin-right: 0px; font-size: 110%; }
        .api-item td              { font-size: revert; border: 0; }
        .api-item td:first-child  { width: 33%; white-space: nowrap; }
        .api-item-title-src-ref   { text-align:  right; }
        .api-item-title-src-ref a { color:       inherit; }
        .api-item-title-kind      { font-weight: normal; font-size: 80%; }
        .api-item-title-light     { font-weight: normal; }
        .api-item-title-strong    { font-weight: bold; font-size: 100%; }
        .api-additional-infos     { font-size: 90%; white-space: break-spaces; overflow-wrap: break-word; }
        
        table.table-enumeration           { width: calc(100% - 40px); }
        .table-enumeration td:first-child { background-color: #f8f8f8; white-space: nowrap; }
        .table-enumeration td:last-child  { width: 100%; }
        .table-enumeration td.code-type   { background-color: #eeeeee; }
        .table-enumeration a              { text-decoration: none; color: inherit; }
        
        .code-inline  { background-color: #eeeeee; border-radius: 5px; border: 1px dotted #cccccc; padding: 0px 8px; font-size: 110%; font-family: monospace; display: inline-block; }
        .code-block   { background-color: #eeeeee; border-radius: 5px; border: 1px solid LightGrey; padding: 10px; margin: 20px; white-space: pre; overflow-x: auto; }
        .code-block a { color: inherit; }
        
    .SCde { color: #222222; }
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
    .SInv { color: #ff0000; }
</style>
<?php include('common/end-head.php'); ?></head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="left">
<div class="left-page">
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
<div class="right-page">
<h1>Module pixel</h1>
<h1>Content</h1>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Bmp_BI_BITFIELDS"><span class="api-item-title-kind">const</span> <span class="api-item-title-light">Bmp.</span><span class="api-item-title-strong">Constants</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td id="Pixel_Bmp_BI_BITFIELDS">BI_BITFIELDS</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_Bmp_BI_RGB">BI_RGB</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_Bmp_BI_RLE4">BI_RLE4</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_Bmp_BI_RLE8">BI_RLE8</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_ClipperScaleCoords">ClipperScaleCoords</td>
<td class="code-type"><span class="STpe">f64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidRenderTargetHandle">InvalidRenderTargetHandle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidRenderTargetSurfaceHandle">InvalidRenderTargetSurfaceHandle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidShaderHandle">InvalidShaderHandle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidShaderParamHandle">InvalidShaderParamHandle</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_InvalidTextureHandle">InvalidTextureHandle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_MaxAAEdge">MaxAAEdge</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Clipper_CInt"><span class="api-item-title-kind">type alias</span> <span class="api-item-title-light">Clipper.</span><span class="api-item-title-strong">Type Aliases</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td id="Pixel_Clipper_CInt">CInt</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_RenderTargetHandle">RenderTargetHandle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_RenderTargetSurfaceHandle">RenderTargetSurfaceHandle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_ShaderHandle">ShaderHandle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_ShaderParamHandle">ShaderParamHandle</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td id="Pixel_TextureHandle">TextureHandle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Argb"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Argb</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\argb.swg#L1" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>AliceBlue</td>
<td></td>
</tr>
<tr>
<td>AntiqueWhite</td>
<td></td>
</tr>
<tr>
<td>Aqua</td>
<td></td>
</tr>
<tr>
<td>Aquamarine</td>
<td></td>
</tr>
<tr>
<td>Azure</td>
<td></td>
</tr>
<tr>
<td>Beige</td>
<td></td>
</tr>
<tr>
<td>Bisque</td>
<td></td>
</tr>
<tr>
<td>Black</td>
<td></td>
</tr>
<tr>
<td>BlanchedAlmond</td>
<td></td>
</tr>
<tr>
<td>Blue</td>
<td></td>
</tr>
<tr>
<td>BlueViolet</td>
<td></td>
</tr>
<tr>
<td>Brown</td>
<td></td>
</tr>
<tr>
<td>BurlyWood</td>
<td></td>
</tr>
<tr>
<td>CadetBlue</td>
<td></td>
</tr>
<tr>
<td>Chartreuse</td>
<td></td>
</tr>
<tr>
<td>Chocolate</td>
<td></td>
</tr>
<tr>
<td>Coral</td>
<td></td>
</tr>
<tr>
<td>CornflowerBlue</td>
<td></td>
</tr>
<tr>
<td>Cornsilk</td>
<td></td>
</tr>
<tr>
<td>Crimson</td>
<td></td>
</tr>
<tr>
<td>Cyan</td>
<td></td>
</tr>
<tr>
<td>DarkBlue</td>
<td></td>
</tr>
<tr>
<td>DarkCyan</td>
<td></td>
</tr>
<tr>
<td>DarkGoldenrod</td>
<td></td>
</tr>
<tr>
<td>DarkGray</td>
<td></td>
</tr>
<tr>
<td>DarkGreen</td>
<td></td>
</tr>
<tr>
<td>DarkKhaki</td>
<td></td>
</tr>
<tr>
<td>DarkMagenta</td>
<td></td>
</tr>
<tr>
<td>DarkOliveGreen</td>
<td></td>
</tr>
<tr>
<td>DarkOrange</td>
<td></td>
</tr>
<tr>
<td>DarkOrchid</td>
<td></td>
</tr>
<tr>
<td>DarkRed</td>
<td></td>
</tr>
<tr>
<td>DarkSalmon</td>
<td></td>
</tr>
<tr>
<td>DarkSeaGreen</td>
<td></td>
</tr>
<tr>
<td>DarkSlateBlue</td>
<td></td>
</tr>
<tr>
<td>DarkSlateGray</td>
<td></td>
</tr>
<tr>
<td>DarkTurquoise</td>
<td></td>
</tr>
<tr>
<td>DarkViolet</td>
<td></td>
</tr>
<tr>
<td>DeepPink</td>
<td></td>
</tr>
<tr>
<td>DeepSkyBlue</td>
<td></td>
</tr>
<tr>
<td>DimGray</td>
<td></td>
</tr>
<tr>
<td>DodgerBlue</td>
<td></td>
</tr>
<tr>
<td>Firebrick</td>
<td></td>
</tr>
<tr>
<td>FloralWhite</td>
<td></td>
</tr>
<tr>
<td>ForestGreen</td>
<td></td>
</tr>
<tr>
<td>Fuchsia</td>
<td></td>
</tr>
<tr>
<td>Gainsboro</td>
<td></td>
</tr>
<tr>
<td>GhostWhite</td>
<td></td>
</tr>
<tr>
<td>Gold</td>
<td></td>
</tr>
<tr>
<td>Goldenrod</td>
<td></td>
</tr>
<tr>
<td>Gray</td>
<td></td>
</tr>
<tr>
<td>Green</td>
<td></td>
</tr>
<tr>
<td>GreenYellow</td>
<td></td>
</tr>
<tr>
<td>Honeydew</td>
<td></td>
</tr>
<tr>
<td>HotPink</td>
<td></td>
</tr>
<tr>
<td>IndianRed</td>
<td></td>
</tr>
<tr>
<td>Indigo</td>
<td></td>
</tr>
<tr>
<td>Ivory</td>
<td></td>
</tr>
<tr>
<td>Khaki</td>
<td></td>
</tr>
<tr>
<td>Lavender</td>
<td></td>
</tr>
<tr>
<td>LavenderBlush</td>
<td></td>
</tr>
<tr>
<td>LawnGreen</td>
<td></td>
</tr>
<tr>
<td>LemonChiffon</td>
<td></td>
</tr>
<tr>
<td>LightBlue</td>
<td></td>
</tr>
<tr>
<td>LightCoral</td>
<td></td>
</tr>
<tr>
<td>LightCyan</td>
<td></td>
</tr>
<tr>
<td>LightGoldenrodYellow</td>
<td></td>
</tr>
<tr>
<td>LightGray</td>
<td></td>
</tr>
<tr>
<td>LightGreen</td>
<td></td>
</tr>
<tr>
<td>LightPink</td>
<td></td>
</tr>
<tr>
<td>LightSalmon</td>
<td></td>
</tr>
<tr>
<td>LightSeaGreen</td>
<td></td>
</tr>
<tr>
<td>LightSkyBlue</td>
<td></td>
</tr>
<tr>
<td>LightSlateGray</td>
<td></td>
</tr>
<tr>
<td>LightSteelBlue</td>
<td></td>
</tr>
<tr>
<td>LightYellow</td>
<td></td>
</tr>
<tr>
<td>Lime</td>
<td></td>
</tr>
<tr>
<td>LimeGreen</td>
<td></td>
</tr>
<tr>
<td>Linen</td>
<td></td>
</tr>
<tr>
<td>Magenta</td>
<td></td>
</tr>
<tr>
<td>Maroon</td>
<td></td>
</tr>
<tr>
<td>MediumAquamarine</td>
<td></td>
</tr>
<tr>
<td>MediumBlue</td>
<td></td>
</tr>
<tr>
<td>MediumOrchid</td>
<td></td>
</tr>
<tr>
<td>MediumPurple</td>
<td></td>
</tr>
<tr>
<td>MediumSeaGreen</td>
<td></td>
</tr>
<tr>
<td>MediumSlateBlue</td>
<td></td>
</tr>
<tr>
<td>MediumSpringGreen</td>
<td></td>
</tr>
<tr>
<td>MediumTurquoise</td>
<td></td>
</tr>
<tr>
<td>MediumVioletRed</td>
<td></td>
</tr>
<tr>
<td>MidnightBlue</td>
<td></td>
</tr>
<tr>
<td>MintCream</td>
<td></td>
</tr>
<tr>
<td>MistyRose</td>
<td></td>
</tr>
<tr>
<td>Moccasin</td>
<td></td>
</tr>
<tr>
<td>NavajoWhite</td>
<td></td>
</tr>
<tr>
<td>Navy</td>
<td></td>
</tr>
<tr>
<td>OldLace</td>
<td></td>
</tr>
<tr>
<td>Olive</td>
<td></td>
</tr>
<tr>
<td>OliveDrab</td>
<td></td>
</tr>
<tr>
<td>Orange</td>
<td></td>
</tr>
<tr>
<td>OrangeRed</td>
<td></td>
</tr>
<tr>
<td>Orchid</td>
<td></td>
</tr>
<tr>
<td>PaleGoldenrod</td>
<td></td>
</tr>
<tr>
<td>PaleGreen</td>
<td></td>
</tr>
<tr>
<td>PaleTurquoise</td>
<td></td>
</tr>
<tr>
<td>PaleVioletRed</td>
<td></td>
</tr>
<tr>
<td>PapayaWhip</td>
<td></td>
</tr>
<tr>
<td>PeachPuff</td>
<td></td>
</tr>
<tr>
<td>Peru</td>
<td></td>
</tr>
<tr>
<td>Pink</td>
<td></td>
</tr>
<tr>
<td>Plum</td>
<td></td>
</tr>
<tr>
<td>PowderBlue</td>
<td></td>
</tr>
<tr>
<td>Purple</td>
<td></td>
</tr>
<tr>
<td>Red</td>
<td></td>
</tr>
<tr>
<td>RosyBrown</td>
<td></td>
</tr>
<tr>
<td>RoyalBlue</td>
<td></td>
</tr>
<tr>
<td>SaddleBrown</td>
<td></td>
</tr>
<tr>
<td>Salmon</td>
<td></td>
</tr>
<tr>
<td>SandyBrown</td>
<td></td>
</tr>
<tr>
<td>SeaGreen</td>
<td></td>
</tr>
<tr>
<td>SeaShell</td>
<td></td>
</tr>
<tr>
<td>Sienna</td>
<td></td>
</tr>
<tr>
<td>Silver</td>
<td></td>
</tr>
<tr>
<td>SkyBlue</td>
<td></td>
</tr>
<tr>
<td>SlateBlue</td>
<td></td>
</tr>
<tr>
<td>SlateGray</td>
<td></td>
</tr>
<tr>
<td>Snow</td>
<td></td>
</tr>
<tr>
<td>SpringGreen</td>
<td></td>
</tr>
<tr>
<td>SteelBlue</td>
<td></td>
</tr>
<tr>
<td>Tan</td>
<td></td>
</tr>
<tr>
<td>Teal</td>
<td></td>
</tr>
<tr>
<td>Thistle</td>
<td></td>
</tr>
<tr>
<td>Tomato</td>
<td></td>
</tr>
<tr>
<td>Turquoise</td>
<td></td>
</tr>
<tr>
<td>Violet</td>
<td></td>
</tr>
<tr>
<td>Wheat</td>
<td></td>
</tr>
<tr>
<td>White</td>
<td></td>
</tr>
<tr>
<td>WhiteSmoke</td>
<td></td>
</tr>
<tr>
<td>Yellow</td>
<td></td>
</tr>
<tr>
<td>YellowGreen</td>
<td></td>
</tr>
<tr>
<td>Zero</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Argb_fromName"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Argb.</span><span class="api-item-title-strong">fromName</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\argb.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the Argb value that matches <span class="code-inline">name</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromName</span>(name: <span class="STpe">string</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Argb">Argb</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_BlendingMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">BlendingMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Copy</td>
<td></td>
</tr>
<tr>
<td>Alpha</td>
<td></td>
</tr>
<tr>
<td>Add</td>
<td></td>
</tr>
<tr>
<td>Sub</td>
<td></td>
</tr>
<tr>
<td>SubDst</td>
<td></td>
</tr>
<tr>
<td>Min</td>
<td></td>
</tr>
<tr>
<td>Max</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Bmp_Decoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Bmp.</span><span class="api-item-title-strong">Decoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Bmp_Decoder_IImageDecoder_canDecode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">canDecode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canDecode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Bmp_Decoder_IImageDecoder_decode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">decode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L61" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">decode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, buffer: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>, options: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Bmp_Encoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Bmp.</span><span class="api-item-title-strong">Encoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\bmp.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Bmp_Encoder_IImageEncoder_canEncode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageEncoder.</span><span class="api-item-title-strong">canEncode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\bmp.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canEncode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Bmp_Encoder_IImageEncoder_encode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageEncoder.</span><span class="api-item-title-strong">encode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\bmp.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">encode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, result: *<span class="SCst">Core</span>.<span class="SCst">ConcatBuffer</span>, image: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>, options: <span class="STpe">any</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Bmp_Header"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Bmp.</span><span class="api-item-title-strong">Header</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\bmp.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>biSize</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>biWidth</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>biHeight</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>biPlanes</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td>biBitCount</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td>biCompression</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>biSizeImage</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>biXPelsPerMeter</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>biYPelsPerMeter</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>biClrUsed</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>biClrImportant</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_BorderPos"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">BorderPos</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Inside</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Brush"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Brush</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>color</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></td>
<td></td>
</tr>
<tr>
<td>type</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_BrushType">BrushType</a></span></span></td>
<td></td>
</tr>
<tr>
<td>hatch</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_HatchStyle">HatchStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td>texture</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span></span></td>
<td></td>
</tr>
<tr>
<td>uvMode</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_UVMode">UVMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td>uvRect</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Brush_createHatch">createHatch</a></span></td>
<td>Returns a hatch brush. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Brush_createSolid">createSolid</a></span></td>
<td>Returns a solid color brush. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Brush_createTexture">createTexture</a></span></td>
<td>Returns a full texture brush. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Brush_createTiled">createTiled</a></span></td>
<td>Returns a tiled texture brush. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Brush_createHatch"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Brush.</span><span class="api-item-title-strong">createHatch</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L55" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a hatch brush. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createHatch</span>(style: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_HatchStyle">HatchStyle</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Brush_createSolid"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Brush.</span><span class="api-item-title-strong">createSolid</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a solid color brush. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createSolid</span>(color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Brush_createTexture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Brush.</span><span class="api-item-title-strong">createTexture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a full texture brush. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createTexture</span>(texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Brush_createTiled"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Brush.</span><span class="api-item-title-strong">createTiled</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a tiled texture brush. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createTiled</span>(texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_BrushType"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">BrushType</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\brush.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>SolidColor</td>
<td></td>
</tr>
<tr>
<td>Texture</td>
<td></td>
</tr>
<tr>
<td>Hatch</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Clipper_ClipType"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Clipper.</span><span class="api-item-title-strong">ClipType</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Intersection</td>
<td></td>
</tr>
<tr>
<td>Union</td>
<td></td>
</tr>
<tr>
<td>Difference</td>
<td></td>
</tr>
<tr>
<td>Xor</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Clipper_InitOptions"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Clipper.</span><span class="api-item-title-strong">InitOptions</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>ReverseSolution</td>
<td></td>
</tr>
<tr>
<td>StrictlySimple</td>
<td></td>
</tr>
<tr>
<td>PreserveCollinear</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Clipper_IntPoint"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Clipper.</span><span class="api-item-title-strong">IntPoint</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>x</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td>y</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Clipper_IntPoint_opEquals">opEquals</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Clipper_IntPoint_opEquals"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IntPoint.</span><span class="api-item-title-strong">opEquals</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, other: <span class="SCst">Pixel</span>.<span class="SCst">Clipper</span>.<span class="SCst"><a href="#Pixel_Clipper_IntPoint">IntPoint</a></span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Clipper_JoinType"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Clipper.</span><span class="api-item-title-strong">JoinType</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Square</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
<tr>
<td>Miter</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Clipper_PolyFillType"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Clipper.</span><span class="api-item-title-strong">PolyFillType</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>EvenOdd</td>
<td></td>
</tr>
<tr>
<td>NonZero</td>
<td></td>
</tr>
<tr>
<td>Positive</td>
<td></td>
</tr>
<tr>
<td>Negative</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Clipper_PolyType"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Clipper.</span><span class="api-item-title-strong">PolyType</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Subject</td>
<td></td>
</tr>
<tr>
<td>Clip</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Clipper_Transform"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Clipper.</span><span class="api-item-title-strong">Transform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\clipper.swg#L142" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_ClippingMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">ClippingMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Set</td>
<td></td>
</tr>
<tr>
<td>Clear</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Color</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>a</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td>r</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td>g</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td>b</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td>argb</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromAbgr">fromAbgr</a></span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromArgb">fromArgb</a></span><span class="SCde">(<span class="SCst">Argb</span>)</span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromArgb">fromArgb</a></span><span class="SCde">(<span class="STpe">u32</span>)</span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromArgb">fromArgb</a></span><span class="SCde">(<span class="STpe">u8</span>, <span class="SCst">Argb</span>)</span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromArgb">fromArgb</a></span><span class="SCde">(<span class="STpe">u8</span>, <span class="STpe">u8</span>, <span class="STpe">u8</span>, <span class="STpe">u8</span>)</span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromArgbf">fromArgbf</a></span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromHsl">fromHsl</a></span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromRgb">fromRgb</a></span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromRgbf">fromRgbf</a></span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_fromVector4">fromVector4</a></span></td>
<td>Create a new Color with specified values. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_getBlend">getBlend</a></span></td>
<td>Blend two colors. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_getBlendKeepAlpha">getBlendKeepAlpha</a></span></td>
<td>Blend two colors, keeping the alpha channel of <span class="code-inline">col0</span> untouched. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_getDarker">getDarker</a></span></td>
<td>Returns a darker color. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_getDistanceRgb">getDistanceRgb</a></span></td>
<td>Returns the distance between the other color. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_getLighter">getLighter</a></span></td>
<td>Returns a lighter color. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_isOpaque">isOpaque</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_setArgb">setArgb</a></span></td>
<td>Set all components. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_setArgbf">setArgbf</a></span></td>
<td>Set all components with floats in the range [0 1]. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_setHsl">setHsl</a></span></td>
<td>Initialize color with hue, saturation, lightness. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_setRgb">setRgb</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td>Just set r, g and b components. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_setRgb">setRgb</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">u8</span>, <span class="STpe">u8</span>, <span class="STpe">u8</span>)</span></td>
<td>Just set r, g and b components. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_setRgbf">setRgbf</a></span></td>
<td>Set r,g,b with floats in the range [0 1]. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_toArgbf">toArgbf</a></span></td>
<td>Retrieve all components as floating point values between 0 and 1. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_toHsl">toHsl</a></span></td>
<td>Returns hue, saturation, lightness. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_toRgbf">toRgbf</a></span></td>
<td>Retrieve all components as floating point values between 0 and 1. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_toVector4">toVector4</a></span></td>
<td>Get the color as a [Math.Vector4]. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_opAffect">opAffect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SCst">Argb</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_opAffect">opAffect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">string</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_opAffect">opAffect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">u32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_opEquals">opEquals</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SCst">Argb</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_opEquals">opEquals</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Color_opEquals">opEquals</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">u32</span>)</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_IPokeValue_poke"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IPokeValue.</span><span class="api-item-title-strong">poke</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L353" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">poke</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, buf: <span class="STpe">string</span>)-&gt;<span class="STpe">string</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_fromAbgr"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">fromAbgr</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromAbgr</span>(abgr: <span class="STpe">u32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_fromArgb"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">fromArgb</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L156" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromArgb</span>(argb: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Argb">Argb</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromArgb</span>(argb: <span class="STpe">u32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromArgb</span>(alpha: <span class="STpe">u8</span>, argb: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Argb">Argb</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromArgb</span>(a: <span class="STpe">u8</span>, r: <span class="STpe">u8</span>, g: <span class="STpe">u8</span>, b: <span class="STpe">u8</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_fromArgbf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">fromArgbf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L193" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromArgbf</span>(a: <span class="STpe">f32</span>, r: <span class="STpe">f32</span>, g: <span class="STpe">f32</span>, b: <span class="STpe">f32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_fromHsl"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">fromHsl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L211" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromHsl</span>(h: <span class="STpe">f32</span>, s: <span class="STpe">f32</span>, l: <span class="STpe">f32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_fromRgb"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">fromRgb</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L129" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromRgb</span>(r: <span class="STpe">u8</span>, g: <span class="STpe">u8</span>, b: <span class="STpe">u8</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_fromRgbf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">fromRgbf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L138" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromRgbf</span>(r: <span class="STpe">f32</span>, g: <span class="STpe">f32</span>, b: <span class="STpe">f32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_fromVector4"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">fromVector4</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L202" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new Color with specified values. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fromVector4</span>(vec: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_getBlend"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">getBlend</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L315" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Blend two colors. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getBlend</span>(col0: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, col1: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, factor: <span class="STpe">f32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_getBlendKeepAlpha"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">getBlendKeepAlpha</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L303" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Blend two colors, keeping the alpha channel of <span class="code-inline">col0</span> untouched. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getBlendKeepAlpha</span>(col0: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, col1: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, factor: <span class="STpe">f32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_getDarker"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">getDarker</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L327" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a darker color. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getDarker</span>(col: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, factor: <span class="STpe">f32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_getDistanceRgb"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">getDistanceRgb</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L340" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the distance between the other color. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getDistanceRgb</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, col: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_getLighter"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">getLighter</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L334" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a lighter color. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getLighter</span>(col: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, factor: <span class="STpe">f32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_isOpaque"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">isOpaque</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isOpaque</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_opAffect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">opAffect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">self</span>, argb: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Argb">Argb</a></span>)
<span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">self</span>, name: <span class="STpe">string</span>)
<span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">self</span>, argb: <span class="STpe">u32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_opEquals"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">opEquals</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">self</span>, argb: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Argb">Argb</a></span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">self</span>, other: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)-&gt;<span class="STpe">bool</span>
<span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">self</span>, argb: <span class="STpe">u32</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_setArgb"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">setArgb</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set all components. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setArgb</span>(<span class="SKwd">self</span>, a: <span class="STpe">u8</span>, r: <span class="STpe">u8</span>, g: <span class="STpe">u8</span>, b: <span class="STpe">u8</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_setArgbf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">setArgbf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set all components with floats in the range [0 1]. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setArgbf</span>(<span class="SKwd">self</span>, a: <span class="STpe">f32</span>, r: <span class="STpe">f32</span>, g: <span class="STpe">f32</span>, b: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_setHsl"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">setHsl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L219" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize color with hue, saturation, lightness. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setHsl</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, h: <span class="STpe">f32</span>, s: <span class="STpe">f32</span>, l: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_setRgb"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">setRgb</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Just set r, g and b components. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRgb</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rgb: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>Just set r, g and b components. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRgb</span>(<span class="SKwd">self</span>, r: <span class="STpe">u8</span>, g: <span class="STpe">u8</span>, b: <span class="STpe">u8</span>)</span></code>
</div>
<p> Alpha will be opaque. </p>
<p> Alpha will be opaque. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_setRgbf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">setRgbf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set r,g,b with floats in the range [0 1]. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRgbf</span>(<span class="SKwd">self</span>, r: <span class="STpe">f32</span>, g: <span class="STpe">f32</span>, b: <span class="STpe">f32</span>)</span></code>
</div>
<p> Alpha will be opaque. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_toArgbf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">toArgbf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L94" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Retrieve all components as floating point values between 0 and 1. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toArgbf</span>(<span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;{a: <span class="STpe">f32</span>, r: <span class="STpe">f32</span>, g: <span class="STpe">f32</span>, b: <span class="STpe">f32</span>}</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_toHsl"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">toHsl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L261" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns hue, saturation, lightness. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toHsl</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;{h: <span class="STpe">f32</span>, s: <span class="STpe">f32</span>, l: <span class="STpe">f32</span>}</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_toRgbf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">toRgbf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L118" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Retrieve all components as floating point values between 0 and 1. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toRgbf</span>(<span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;{r: <span class="STpe">f32</span>, g: <span class="STpe">f32</span>, b: <span class="STpe">f32</span>}</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Color_toVector4"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Color.</span><span class="api-item-title-strong">toVector4</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\color.swg#L106" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the color as a [Math.Vector4]. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toVector4</span>(<span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_ColorMask"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">ColorMask</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>r</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>g</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>b</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>a</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Command"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Command</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>id</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_CommandId">CommandId</a></span></span></td>
<td></td>
</tr>
<tr>
<td>using params</td>
<td class="code-type"><span class="SCde">{clear: {color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>}, drawTriangles: {start: <span class="STpe">u32</span>, count: <span class="STpe">u32</span>}, transform: {tr: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Transform2</span>}, clippingRect: {rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>}, clippingRegion: {mode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ClippingMode">ClippingMode</a></span>}, font: {fontRef: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>}, blendingMode: {mode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_BlendingMode">BlendingMode</a></span>}, textureFont: {font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>, atlasIndex: <span class="STpe">s32</span>}, colorMask: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ColorMask">ColorMask</a></span>, renderTgt: {tgt: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span>, paintAlpha: <span class="STpe">bool</span>}, shader: {shader: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderBase">ShaderBase</a></span>}, shaderParam: {param: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>, type: <span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfo</span>, _f32: <span class="STpe">f32</span>, _s32: <span class="STpe">s32</span>}, texture0: {boundRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span>, textureRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span>, type: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_BrushType">BrushType</a></span>, hatch: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_HatchStyle">HatchStyle</a></span>, uvMode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_UVMode">UVMode</a></span>, interpolationMode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_InterpolationMode">InterpolationMode</a></span>, texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>}}</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_CommandId"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">CommandId</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Clear</td>
<td></td>
</tr>
<tr>
<td>ColorMask</td>
<td></td>
</tr>
<tr>
<td>Transform</td>
<td></td>
</tr>
<tr>
<td>DrawTriangles</td>
<td></td>
</tr>
<tr>
<td>BindTexture0</td>
<td></td>
</tr>
<tr>
<td>ResetTexture0</td>
<td></td>
</tr>
<tr>
<td>BindTextureFont</td>
<td></td>
</tr>
<tr>
<td>ResetTextureFont</td>
<td></td>
</tr>
<tr>
<td>SetClippingRect</td>
<td></td>
</tr>
<tr>
<td>ResetClippingRect</td>
<td></td>
</tr>
<tr>
<td>StartClippingRegion</td>
<td></td>
</tr>
<tr>
<td>EndClippingRegion</td>
<td></td>
</tr>
<tr>
<td>ResetClippingRegion</td>
<td></td>
</tr>
<tr>
<td>SetClippingRegionMode</td>
<td></td>
</tr>
<tr>
<td>UpdateFont</td>
<td></td>
</tr>
<tr>
<td>StartNoOverlap</td>
<td></td>
</tr>
<tr>
<td>StopNoOverlap</td>
<td></td>
</tr>
<tr>
<td>SetBlendingMode</td>
<td></td>
</tr>
<tr>
<td>BindRenderTarget</td>
<td></td>
</tr>
<tr>
<td>UnbindRenderTarget</td>
<td></td>
</tr>
<tr>
<td>ReleaseRenderTarget</td>
<td></td>
</tr>
<tr>
<td>SetShader</td>
<td></td>
</tr>
<tr>
<td>SetShaderParam</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_DashCapStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">DashCapStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Triangle</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
<tr>
<td>Square</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_DashStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">DashStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Pattern</td>
<td></td>
</tr>
<tr>
<td>Dot</td>
<td></td>
</tr>
<tr>
<td>Dash</td>
<td></td>
</tr>
<tr>
<td>DashDot</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_DecodeOptions"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">DecodeOptions</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>decodePixels</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_DefaultShaderId"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">DefaultShaderId</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Default</td>
<td></td>
</tr>
<tr>
<td>Blur</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_DrawPathListMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">DrawPathListMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawpath.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Separate</td>
<td></td>
</tr>
<tr>
<td>Merge</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Element"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Element</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>type</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ElementType">ElementType</a></span></span></td>
<td></td>
</tr>
<tr>
<td>position</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>p1</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>p2</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_ElementType"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">ElementType</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Line</td>
<td></td>
</tr>
<tr>
<td>Arc</td>
<td></td>
</tr>
<tr>
<td>Bezier1</td>
<td></td>
</tr>
<tr>
<td>Bezier2</td>
<td></td>
</tr>
<tr>
<td>Curve</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Font"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Font</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>ascent</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>descent</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>height</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>underlinePos</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>underlineSize</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>forceBold</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>forceItalic</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>fullname</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td>typeFace</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_TypeFace">TypeFace</a></span></span></td>
<td></td>
</tr>
<tr>
<td>size</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>atlases</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_GlyphAtlas">GlyphAtlas</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>asciiRef</td>
<td class="code-type"><span class="SCde">[<span class="SNum">128</span>] <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span></span></td>
<td></td>
</tr>
<tr>
<td>unicodeRef</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">HashTable</span>'(<span class="STpe">rune</span>, *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>dirty</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Font_create">create</a></span><span class="SCde">(*<span class="SCst">TypeFace</span>, <span class="STpe">u32</span>, <span class="STpe">bool</span>, <span class="STpe">bool</span>)</span></td>
<td>Creates a new Font of the given typeface and size. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Font_create">create</a></span><span class="SCde">(<span class="STpe">string</span>, <span class="STpe">u32</span>, <span class="STpe">bool</span>, <span class="STpe">bool</span>)</span></td>
<td>Creates a new Font with the given filename and size. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Font_getGlyphDesc">getGlyphDesc</a></span></td>
<td>Returns the glyph descriptor of a given rune. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Font_release">release</a></span></td>
<td>Release the given font. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Font_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Font.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new Font of the given typeface and size. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(typeFace: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_TypeFace">TypeFace</a></span>, size: <span class="STpe">u32</span>, forceBold = <span class="SKwd">false</span>, forceItalic = <span class="SKwd">false</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span></span></code>
</div>
<p>Creates a new Font with the given filename and size. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(fullname: <span class="STpe">string</span>, size: <span class="STpe">u32</span>, forceBold = <span class="SKwd">false</span>, forceItalic = <span class="SKwd">false</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Font_getGlyphDesc"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Font.</span><span class="api-item-title-strong">getGlyphDesc</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L126" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the glyph descriptor of a given rune. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getGlyphDesc</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, c: <span class="STpe">rune</span>)-&gt;<span class="SKwd">const</span> *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Font_release"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Font.</span><span class="api-item-title-strong">release</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L73" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the given font. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">release</span>(font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_FontFamily"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">FontFamily</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>regular</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
<tr>
<td>bold</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
<tr>
<td>italic</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
<tr>
<td>boldItalic</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_FontFamily_createTypeFace">createTypeFace</a></span></td>
<td>Creates a typeface from a FontFamilyEnumerate and FontFamilyStyle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_FontFamily_enumerateFromOs">enumerateFromOs</a></span></td>
<td>Enumerate the font families available in the os. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_FontFamily_getFont">getFont</a></span></td>
<td>Get the font corresponding to the style. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_FontFamily_getFromOs">getFromOs</a></span></td>
<td>Get a FontFamilyEnumerate from a font family name. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_FontFamily_setFont">setFont</a></span></td>
<td>Set the font corresponding to a given style. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_FontFamily_createTypeFace"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FontFamily.</span><span class="api-item-title-strong">createTypeFace</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L109" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a typeface from a FontFamilyEnumerate and FontFamilyStyle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createTypeFace</span>(enumerate: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></span>, style: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_TypeFace">TypeFace</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_FontFamily_enumerateFromOs"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FontFamily.</span><span class="api-item-title-strong">enumerateFromOs</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enumerate the font families available in the os. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">enumerateFromOs</span>()-&gt;<span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_FontFamily_getFont"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FontFamily.</span><span class="api-item-title-strong">getFont</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the font corresponding to the style. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFont</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, style: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span></span></code>
</div>
<p> If not evailable, get another one. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_FontFamily_getFromOs"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FontFamily.</span><span class="api-item-title-strong">getFromOs</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get a FontFamilyEnumerate from a font family name. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFromOs</span>(fontFamily: <span class="STpe">string</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamilyEnumerate">FontFamilyEnumerate</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_FontFamily_setFont"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">FontFamily.</span><span class="api-item-title-strong">setFont</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the font corresponding to a given style. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFont</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fnt: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>, style: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_FontFamilyEnumerate"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">FontFamilyEnumerate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>name</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td>faces</td>
<td class="code-type"><span class="SCde">[<span class="SNum">4</span>] <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamilyStyleEnumerate">FontFamilyStyleEnumerate</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_FontFamilyStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">FontFamilyStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Regular</td>
<td></td>
</tr>
<tr>
<td>Bold</td>
<td></td>
</tr>
<tr>
<td>Italic</td>
<td></td>
</tr>
<tr>
<td>BoldItalic</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_FontFamilyStyleEnumerate"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">FontFamilyStyleEnumerate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\fontfamily.win32.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>name</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td>face</td>
<td class="code-type"><span class="SCde"><span class="SCst">Gdi32</span>.<span class="SCst">LOGFONTW</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Gif_Decoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Gif.</span><span class="api-item-title-strong">Decoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Gif_Decoder_getFrame">getFrame</a></span></td>
<td>Create an image for the current frame. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Gif_Decoder_getNumFrames">getNumFrames</a></span></td>
<td>Returns the number of frames  As Gif does not store it, we need to go threw all images (!). </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Gif_Decoder_init">init</a></span></td>
<td>Initialize the decoder. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Gif_Decoder_nextFrame">nextFrame</a></span></td>
<td>Get the next frame. Returns false if we have reached the end. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Gif_Decoder_rewind">rewind</a></span></td>
<td>Restart at frame 0. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Gif_Decoder_IImageDecoder_canDecode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">canDecode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L555" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canDecode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Gif_Decoder_IImageDecoder_decode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">decode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L561" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">decode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, buffer: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>, options: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Gif_Decoder_getFrame"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">getFrame</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L501" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create an image for the current frame. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFrame</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, image: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>, decodePixels = <span class="SKwd">true</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Gif_Decoder_getNumFrames"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">getNumFrames</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L529" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the number of frames  As Gif does not store it, we need to go threw all images (!). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getNumFrames</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">s32</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Gif_Decoder_init"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">init</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L439" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the decoder. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">init</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, buffer: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>, decodePixels: <span class="STpe">bool</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Gif_Decoder_nextFrame"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">nextFrame</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L511" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the next frame. Returns false if we have reached the end. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">nextFrame</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, img: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>)-&gt;<span class="STpe">bool</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Gif_Decoder_rewind"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">rewind</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\gif.swg#L544" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Restart at frame 0. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rewind</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_GlyphAtlas"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">GlyphAtlas</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>texture</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span></span></td>
<td></td>
</tr>
<tr>
<td>width</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>height</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>bpp</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>datas</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>)</span></td>
<td></td>
</tr>
<tr>
<td>dirty</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>curX</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>curY</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>nextY</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_GlyphDesc"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">GlyphDesc</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\font.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>uv</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td>atlasIndex</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>advanceX</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>advanceY</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>shiftX</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>shiftY</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>width</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>height</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_HatchStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">HatchStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\hatch.swg#L2" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Dot</td>
<td></td>
</tr>
<tr>
<td>Horizontal</td>
<td></td>
</tr>
<tr>
<td>HorizontalLight</td>
<td></td>
</tr>
<tr>
<td>HorizontalNarrow</td>
<td></td>
</tr>
<tr>
<td>Vertical</td>
<td></td>
</tr>
<tr>
<td>VerticalLight</td>
<td></td>
</tr>
<tr>
<td>VerticalNarrow</td>
<td></td>
</tr>
<tr>
<td>SolidDiamond</td>
<td></td>
</tr>
<tr>
<td>OutlinedDiamond</td>
<td></td>
</tr>
<tr>
<td>HorizontalDark</td>
<td></td>
</tr>
<tr>
<td>VerticalDark</td>
<td></td>
</tr>
<tr>
<td>Grid</td>
<td></td>
</tr>
<tr>
<td>GridLarge</td>
<td></td>
</tr>
<tr>
<td>GridDotted</td>
<td></td>
</tr>
<tr>
<td>Cross</td>
<td></td>
</tr>
<tr>
<td>CrossLarge</td>
<td></td>
</tr>
<tr>
<td>Percent5</td>
<td></td>
</tr>
<tr>
<td>Percent10</td>
<td></td>
</tr>
<tr>
<td>Percent20</td>
<td></td>
</tr>
<tr>
<td>Percent25</td>
<td></td>
</tr>
<tr>
<td>Percent30</td>
<td></td>
</tr>
<tr>
<td>Percent40</td>
<td></td>
</tr>
<tr>
<td>Percent50</td>
<td></td>
</tr>
<tr>
<td>Percent60</td>
<td></td>
</tr>
<tr>
<td>Percent70</td>
<td></td>
</tr>
<tr>
<td>Percent75</td>
<td></td>
</tr>
<tr>
<td>Percent80</td>
<td></td>
</tr>
<tr>
<td>Percent90</td>
<td></td>
</tr>
<tr>
<td>CheckBoard</td>
<td></td>
</tr>
<tr>
<td>CheckBoardLarge</td>
<td></td>
</tr>
<tr>
<td>Weave</td>
<td></td>
</tr>
<tr>
<td>DiagonalForward</td>
<td></td>
</tr>
<tr>
<td>DiagonalForwardLight</td>
<td></td>
</tr>
<tr>
<td>DiagonalBackward</td>
<td></td>
</tr>
<tr>
<td>DiagonalBackwardLight</td>
<td></td>
</tr>
<tr>
<td>DiagonalCross</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_IImageDecoder"><span class="api-item-title-kind">interface</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">IImageDecoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Interface to decode a buffer. </p>
<table class="table-enumeration">
<tr>
<td>canDecode</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_IImageDecoder">IImageDecoder</a></span>, <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td>decode</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_IImageDecoder">IImageDecoder</a></span>, <span class="SKwd">const</span> [..] <span class="STpe">u8</span>, <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_IImageEncoder"><span class="api-item-title-kind">interface</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">IImageEncoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\encode.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Interface to decode a buffer. </p>
<table class="table-enumeration">
<tr>
<td>canEncode</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_IImageEncoder">IImageEncoder</a></span>, <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td>encode</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_IImageEncoder">IImageEncoder</a></span>, *<span class="SCst">Core</span>.<span class="SCst">ConcatBuffer</span>, <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>, <span class="STpe">any</span>) <span class="SKwd">throw</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Image</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>An image buffer, in various pixel formats. </p>
<table class="table-enumeration">
<tr>
<td>pixels</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>)</span></td>
<td>All the pixels. </td>
</tr>
<tr>
<td>size</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td>Size in bytes. </td>
</tr>
<tr>
<td>width</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>The width of the image. </td>
</tr>
<tr>
<td>height</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>The height of the image. </td>
</tr>
<tr>
<td>width8</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>The width, in bytes. </td>
</tr>
<tr>
<td>pf</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span></span></td>
<td>Format of one pixel. </td>
</tr>
<tr>
<td>bpp</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td>Bits per pixel. </td>
</tr>
<tr>
<td>bpp8</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td>Bytes per pixel. </td>
</tr>
<tr>
<td>workingBuffer</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></td>
<td>Temporary buffer for filters and transformations. </td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_addDecoder">addDecoder</a></span></td>
<td>Register an image decoder. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_addEncoder">addEncoder</a></span></td>
<td>Register an image encoder. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_allocPixels">allocPixels</a></span></td>
<td>Allocate pixels. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_allocWorkingBuffer">allocWorkingBuffer</a></span><span class="SCde">(<span class="SKwd">self</span>)</span></td>
<td>Allocate a computing buffer with current image size. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_allocWorkingBuffer">allocWorkingBuffer</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">s32</span>, <span class="STpe">s32</span>, <span class="SCst">PixelFormat</span>)</span></td>
<td>Allocate a computing buffer with new sizes. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_applyKernel">applyKernel</a></span></td>
<td>Apply a kernel to the image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_applyWorkingBuffer">applyWorkingBuffer</a></span></td>
<td>Replace the current image content with the working buffer. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_canLoad">canLoad</a></span></td>
<td>Returns <span class="code-inline">true</span> if the given filename has a corresponding decoder. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_canSave">canSave</a></span></td>
<td>Returns <span class="code-inline">true</span> if the given filename has a corresponding encoder. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_clear">clear</a></span></td>
<td>Clear the content of the image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_colorize">colorize</a></span></td>
<td>Colorize the image by setting the hue and saturation. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_contrast">contrast</a></span></td>
<td>Change the contrast. <span class="code-inline">factor</span> is [-1, 1]. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_copyPixel">copyPixel</a></span></td>
<td>Copy one pixel, depending on bpp. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_create">create</a></span></td>
<td>Creates a new image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_crop">crop</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_crop">crop</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">s32</span>, <span class="STpe">s32</span>, <span class="STpe">s32</span>, <span class="STpe">s32</span>)</span></td>
<td>Crop image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_decode">decode</a></span></td>
<td>Decode the given image buffer. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_fade">fade</a></span></td>
<td>Fade to a given color. <span class="code-inline">factor</span> is [0, 1]. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_fill">fill</a></span></td>
<td>Fill image with <span class="code-inline">color</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_fillGradient2">fillGradient2</a></span></td>
<td>Fill with an horizontal gradient from <span class="code-inline">color0</span> to <span class="code-inline">color1</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_fillGradient4">fillGradient4</a></span></td>
<td>Fill with a gradient with a different color at each corner. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_fillHsl">fillHsl</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_flip">flip</a></span></td>
<td>Flip image vertically. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_freeWorkingBuffer">freeWorkingBuffer</a></span></td>
<td>Free the working buffer if allocated. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_from">from</a></span><span class="SCde">(<span class="SCst">HBITMAP</span>)</span></td>
<td>Creates an image from a windows HBITMAP. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_from">from</a></span><span class="SCde">(<span class="SCst">HICON</span>)</span></td>
<td>Creates an image from a windows HICON. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_gamma">gamma</a></span></td>
<td>Change the gamma. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_getPixelColor">getPixelColor</a></span></td>
<td>Returns the color at the given coordinate. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_grayScale">grayScale</a></span></td>
<td>Transform image to grayscale, with a given factor. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_halfSize">halfSize</a></span></td>
<td>Divide image size by 2 with a bilinear 2x2 filter. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_init">init</a></span></td>
<td>Initialize image informations  <span class="code-inline">pixels</span> is set to null, and must be initialized after a call to that function  This gives the opportunity to set the pixels with an external buffer. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_invert">invert</a></span></td>
<td>Invert colors. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_isValid">isValid</a></span></td>
<td>Returns true if the image is valid. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_lightness">lightness</a></span></td>
<td>Change the lightness. <span class="code-inline">factor</span> is [-1, 1]. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_load">load</a></span></td>
<td>Load the given image file. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_mirror">mirror</a></span></td>
<td>Flip image horizontally. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_mix">mix</a></span></td>
<td>Mix with another image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_release">release</a></span></td>
<td>Release the content of the image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_resize">resize</a></span></td>
<td>Resize image  Put <span class="code-inline">newWidth</span> or <span class="code-inline">newHeight</span> to 0 to keep aspect ratio. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_save">save</a></span></td>
<td>Save the image to a file. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_setAlpha">setAlpha</a></span></td>
<td>Change the alpha channel. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_setChannel">setChannel</a></span></td>
<td>Change specified color channels. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_setPixelFormat">setPixelFormat</a></span></td>
<td>Change image pixel format. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_setPixelRGBA8">setPixelRGBA8</a></span></td>
<td>Set pixel values depending on pixel format. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toApplyKernel">toApplyKernel</a></span></td>
<td>Apply a kernel to the image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toCrop">toCrop</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toCrop">toCrop</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">s32</span>, <span class="STpe">s32</span>, <span class="STpe">s32</span>, <span class="STpe">s32</span>)</span></td>
<td>Crop image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toFlip">toFlip</a></span></td>
<td>Flip image vertically. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toHICON">toHICON</a></span></td>
<td>Transform an image to a windows icon. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toHalfSize">toHalfSize</a></span></td>
<td>Divide image size by 2 with a bilinear 2x2 filter. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toMirror">toMirror</a></span></td>
<td>Flip image horizontally. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toMix">toMix</a></span></td>
<td>Mix with another image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toResize">toResize</a></span></td>
<td>Resize image  Put <span class="code-inline">newWidth</span> or <span class="code-inline">newHeight</span> to 0 to keep aspect ratio. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toSetPixelFormat">toSetPixelFormat</a></span></td>
<td>Change image pixel format. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_toTurn">toTurn</a></span></td>
<td>Turn image by a given predefined angle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_turn">turn</a></span></td>
<td>Turn image by a given predefined angle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_visitPixels">visitPixels</a></span></td>
<td>Macro to visit all pixels of the image in parallel chunks. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Image_opVisit">opVisit</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_ChannelMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">ChannelMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\setchannel.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Red</td>
<td></td>
</tr>
<tr>
<td>Green</td>
<td></td>
</tr>
<tr>
<td>Blue</td>
<td></td>
</tr>
<tr>
<td>Alpha</td>
<td></td>
</tr>
<tr>
<td>RGB</td>
<td></td>
</tr>
<tr>
<td>RGBA</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_ChannelValueMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">ChannelValueMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\setchannel.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Color</td>
<td></td>
</tr>
<tr>
<td>MinRGB</td>
<td></td>
</tr>
<tr>
<td>MaxRGB</td>
<td></td>
</tr>
<tr>
<td>MeanRGB</td>
<td></td>
</tr>
<tr>
<td>Alpha</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_FillHslType"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">FillHslType</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fillhsl.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>HueVert</td>
<td></td>
</tr>
<tr>
<td>HueHorz</td>
<td></td>
</tr>
<tr>
<td>SaturationVert</td>
<td></td>
</tr>
<tr>
<td>SaturationHorz</td>
<td></td>
</tr>
<tr>
<td>LightnessVert</td>
<td></td>
</tr>
<tr>
<td>LightnessHorz</td>
<td></td>
</tr>
<tr>
<td>HueSaturation</td>
<td></td>
</tr>
<tr>
<td>HueLightness</td>
<td></td>
</tr>
<tr>
<td>SaturationLightness</td>
<td>(h,0,1) (h,1,0.5) (h,0,0) (h,1,0). </td>
</tr>
<tr>
<td>GreenBlue</td>
<td></td>
</tr>
<tr>
<td>RedBlue</td>
<td></td>
</tr>
<tr>
<td>RedGreen</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_Kernel"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">Kernel</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\applykernel.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Kernel format is : kernelSize, kernelNormalizeValue, kernelValues... </p>
<table class="table-enumeration">
<tr>
<td>GaussianBlur3x3A</td>
<td></td>
</tr>
<tr>
<td>GaussianBlur3x3B</td>
<td></td>
</tr>
<tr>
<td>GaussianBlur5x5A</td>
<td></td>
</tr>
<tr>
<td>BoxBlur3x3A</td>
<td></td>
</tr>
<tr>
<td>BoxBlur5x5A</td>
<td></td>
</tr>
<tr>
<td>Sharpen3x3A</td>
<td></td>
</tr>
<tr>
<td>Sharpen3x3B</td>
<td></td>
</tr>
<tr>
<td>Soften3x3A</td>
<td></td>
</tr>
<tr>
<td>EmbossLeft3x3A</td>
<td></td>
</tr>
<tr>
<td>EmbossLeft3x3B</td>
<td></td>
</tr>
<tr>
<td>EmbossLeft3x3C</td>
<td></td>
</tr>
<tr>
<td>EmbossRight3x3A</td>
<td></td>
</tr>
<tr>
<td>EmbossRight3x3B</td>
<td></td>
</tr>
<tr>
<td>EmbossRight3x3C</td>
<td></td>
</tr>
<tr>
<td>EdgeDetect3x3A</td>
<td></td>
</tr>
<tr>
<td>EdgeDetect3x3B</td>
<td></td>
</tr>
<tr>
<td>EdgeDetect3x3C</td>
<td></td>
</tr>
<tr>
<td>EdgeDetect3x3D</td>
<td></td>
</tr>
<tr>
<td>EdgeDetect3x3E</td>
<td></td>
</tr>
<tr>
<td>EdgeDetect3x3F</td>
<td></td>
</tr>
<tr>
<td>SobelHorizontal3x3A</td>
<td></td>
</tr>
<tr>
<td>SobelVertical3x3A</td>
<td></td>
</tr>
<tr>
<td>PrevitHorizontal3x3A</td>
<td></td>
</tr>
<tr>
<td>PrevitVertical3x3A</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_MixMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">MixMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mix.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Copy</td>
<td></td>
</tr>
<tr>
<td>AlphaBlend</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_ResizeMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">ResizeMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\resize.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Raw</td>
<td></td>
</tr>
<tr>
<td>Bilinear</td>
<td></td>
</tr>
<tr>
<td>Bicubic</td>
<td></td>
</tr>
<tr>
<td>Gaussian</td>
<td></td>
</tr>
<tr>
<td>Quadratic</td>
<td></td>
</tr>
<tr>
<td>Hermite</td>
<td></td>
</tr>
<tr>
<td>Hamming</td>
<td></td>
</tr>
<tr>
<td>Catrom</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_TurnAngle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">TurnAngle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\turn.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>T90</td>
<td></td>
</tr>
<tr>
<td>T180</td>
<td></td>
</tr>
<tr>
<td>T270</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_addDecoder"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">addDecoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register an image decoder. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">addDecoder</span>()</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_addEncoder"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">addEncoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\encode.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register an image encoder. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">addEncoder</span>()</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_allocPixels"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">allocPixels</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Allocate pixels. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">allocPixels</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_allocWorkingBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">allocWorkingBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L90" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Allocate a computing buffer with current image size. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">allocWorkingBuffer</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>Allocate a computing buffer with new sizes. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">allocWorkingBuffer</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newWidth: <span class="STpe">s32</span>, newHeight: <span class="STpe">s32</span>, newPf: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_applyKernel"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">applyKernel</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\applykernel.swg#L112" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Apply a kernel to the image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">applyKernel</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, kernel: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>.<span class="SCst"><a href="#Pixel_Image_Kernel">Kernel</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_applyWorkingBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">applyWorkingBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L117" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Replace the current image content with the working buffer. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">applyWorkingBuffer</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_canLoad"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">canLoad</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L77" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns <span class="code-inline">true</span> if the given filename has a corresponding decoder. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canLoad</span>(fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_canSave"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">canSave</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\encode.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns <span class="code-inline">true</span> if the given filename has a corresponding encoder. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canSave</span>(fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L173" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear the content of the image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_colorize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">colorize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\colorize.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Colorize the image by setting the hue and saturation. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">colorize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, hue: <span class="STpe">f32</span>, saturation: <span class="STpe">f32</span>, strength = <span class="SNum">1.0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_contrast"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">contrast</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\contrast.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the contrast. <span class="code-inline">factor</span> is [-1, 1]. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">contrast</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, factor: <span class="STpe">f32</span> = <span class="SNum">0.5</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_copyPixel"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">copyPixel</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Copy one pixel, depending on bpp. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">BPP</span>: <span class="STpe">u8</span>) <span class="SFct">copyPixel</span>(pixDst: ^<span class="STpe">u8</span>, pixSrc: ^<span class="STpe">u8</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(width: <span class="STpe">s32</span>, height: <span class="STpe">s32</span>, pf: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span>, buffer: <span class="SKwd">const</span> [..] <span class="STpe">u8</span> = <span class="SKwd">null</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_crop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">crop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\crop.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Crop image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">crop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cropRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)
<span class="SKwd">func</span> <span class="SFct">crop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, w: <span class="STpe">s32</span>, h: <span class="STpe">s32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_decode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">decode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Decode the given image buffer. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">decode</span>(fileName: <span class="STpe">string</span>, bytes: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>, options: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span> = {})-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_fade"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">fade</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fade.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fade to a given color. <span class="code-inline">factor</span> is [0, 1]. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fade</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, factor: <span class="STpe">f32</span> = <span class="SNum">0.5</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_fill"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">fill</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fill.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill image with <span class="code-inline">color</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fill</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_fillGradient2"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">fillGradient2</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fillgradient.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill with an horizontal gradient from <span class="code-inline">color0</span> to <span class="code-inline">color1</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillGradient2</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, color0: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, color1: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_fillGradient4"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">fillGradient4</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fillgradient.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill with a gradient with a different color at each corner. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillGradient4</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, topLeft: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, topRight: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, bottomLeft: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, bottomRight: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_fillHsl"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">fillHsl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\fillhsl.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillHsl</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fillType: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>.<span class="SCst"><a href="#Pixel_Image_FillHslType">FillHslType</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span> = <span class="SCst"><a href="#Pixel_Argb">Argb</a></span>.<span class="SCst">White</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_flip"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">flip</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\flip.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flip image vertically. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">flip</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_freeWorkingBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">freeWorkingBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L110" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Free the working buffer if allocated. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">freeWorkingBuffer</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_from"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">from</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.win32.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates an image from a windows HBITMAP. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">from</span>(hbitmap: <span class="SCst">Win32</span>.<span class="SCst">HBITMAP</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>Creates an image from a windows HICON. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">from</span>(hicon: <span class="SCst">Win32</span>.<span class="SCst">HICON</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_gamma"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">gamma</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\gamma.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the gamma. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">gamma</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, level: <span class="STpe">f32</span> = <span class="SNum">2.2</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_getPixelColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">getPixelColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L208" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the color at the given coordinate. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getPixelColor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pixX: <span class="STpe">s32</span>, pixY: <span class="STpe">s32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_grayScale"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">grayScale</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\grayscale.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform image to grayscale, with a given factor. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">grayScale</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, factor: <span class="STpe">f32</span> = <span class="SNum">1</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_halfSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">halfSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\halfsize.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Divide image size by 2 with a bilinear 2x2 filter. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">halfSize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_init"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">init</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize image informations  <span class="code-inline">pixels</span> is set to null, and must be initialized after a call to that function  This gives the opportunity to set the pixels with an external buffer. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">init</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, width: <span class="STpe">s32</span>, height: <span class="STpe">s32</span>, pf: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_invert"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">invert</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\invert.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Invert colors. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">invert</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_isValid"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">isValid</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the image is valid. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isValid</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_lightness"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">lightness</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\lightness.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the lightness. <span class="code-inline">factor</span> is [-1, 1]. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lightness</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, factor: <span class="STpe">f32</span> = <span class="SNum">0.5</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_load"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">load</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\decode.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Load the given image file. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">load</span>(fileName: <span class="STpe">string</span>, options: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span> = {})-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_mirror"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">mirror</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mirror.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flip image horizontally. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">mirror</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_mix"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">mix</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mix.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Mix with another image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">mix</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, srcImage: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, mode = <span class="SCst"><a href="#Pixel_Image_MixMode">MixMode</a></span>.<span class="SCst">Copy</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_opVisit"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">opVisit</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SAtr">#[<a href="swag.runtime.php#Swag_Macro">Swag.Macro</a>]</span>
<span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, stmt: <span class="STpe">code</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_release"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">release</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L166" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the content of the image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">release</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_resize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">resize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\resize.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Resize image  Put <span class="code-inline">newWidth</span> or <span class="code-inline">newHeight</span> to 0 to keep aspect ratio. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newWidth: <span class="STpe">s32</span>, newHeight: <span class="STpe">s32</span>, mode = <span class="SCst"><a href="#Pixel_Image_ResizeMode">ResizeMode</a></span>.<span class="SCst">Raw</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_save"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">save</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\encode.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Save the image to a file. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">save</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>, options: <span class="STpe">any</span> = <span class="SKwd">null</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_setAlpha"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">setAlpha</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\setchannel.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the alpha channel. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setAlpha</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">u8</span> = <span class="SNum">0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_setChannel"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">setChannel</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\setchannel.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change specified color channels. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setChannel</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>.<span class="SCst"><a href="#Pixel_Image_ChannelMode">ChannelMode</a></span>, value: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>.<span class="SCst"><a href="#Pixel_Image_ChannelValueMode">ChannelValueMode</a></span>, color: <span class="STpe">u8</span> = <span class="SNum">0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_setPixelFormat"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">setPixelFormat</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\setpixelformat.swg#L154" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change image pixel format. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setPixelFormat</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newPf: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_setPixelRGBA8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">setPixelRGBA8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L199" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set pixel values depending on pixel format. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">PF</span>: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span>) <span class="SFct">setPixelRGBA8</span>(pixDst: ^<span class="STpe">u8</span>, r: <span class="STpe">u8</span>, g: <span class="STpe">u8</span>, b: <span class="STpe">u8</span>, _a: <span class="STpe">u8</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toApplyKernel"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toApplyKernel</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\filter\applykernel.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Apply a kernel to the image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toApplyKernel</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, kernel: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>.<span class="SCst"><a href="#Pixel_Image_Kernel">Kernel</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toCrop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toCrop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\crop.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Crop image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toCrop</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, cropRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>
<span class="SKwd">func</span> <span class="SFct">toCrop</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, w: <span class="STpe">s32</span>, h: <span class="STpe">s32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toFlip"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toFlip</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\flip.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flip image vertically. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toFlip</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toHICON"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toHICON</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.win32.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform an image to a windows icon. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toHICON</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HICON</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toHalfSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toHalfSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\halfsize.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Divide image size by 2 with a bilinear 2x2 filter. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toHalfSize</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toMirror"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toMirror</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mirror.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flip image horizontally. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toMirror</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toMix"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toMix</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\mix.swg#L81" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Mix with another image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toMix</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, srcImage: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, mode = <span class="SCst"><a href="#Pixel_Image_MixMode">MixMode</a></span>.<span class="SCst">Copy</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toResize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toResize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\resize.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Resize image  Put <span class="code-inline">newWidth</span> or <span class="code-inline">newHeight</span> to 0 to keep aspect ratio. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toResize</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, newWidth: <span class="STpe">s32</span>, newHeight: <span class="STpe">s32</span>, mode = <span class="SCst"><a href="#Pixel_Image_ResizeMode">ResizeMode</a></span>.<span class="SCst">Raw</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toSetPixelFormat"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toSetPixelFormat</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\setpixelformat.swg#L167" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change image pixel format. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toSetPixelFormat</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, newPf: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_toTurn"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">toTurn</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\turn.swg#L80" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Turn image by a given predefined angle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toTurn</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, angle: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>.<span class="SCst"><a href="#Pixel_Image_TurnAngle">TurnAngle</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_turn"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">turn</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\transform\turn.swg#L68" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Turn image by a given predefined angle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">turn</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, angle: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>.<span class="SCst"><a href="#Pixel_Image_TurnAngle">TurnAngle</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Image_visitPixels"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Image.</span><span class="api-item-title-strong">visitPixels</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\image.swg#L142" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Macro to visit all pixels of the image in parallel chunks. </p>
<div class="code-block"><code><span class="SCde"><span class="SAtr">#[<a href="swag.runtime.php#Swag_Macro">Swag.Macro</a>]</span>
<span class="SKwd">func</span> <span class="SFct">visitPixels</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, userData: *<span class="STpe">void</span> = <span class="SKwd">null</span>, stride: <span class="STpe">s32</span> = <span class="SNum">1</span>, stmt: <span class="STpe">code</span>)</span></code>
</div>
<p style="white-space: break-spaces"> Exported variables:
 'pix'        address of the pixel
 'image'      the processed image
 'index'      the pixel index
 'x'          the pixel x coordinates
 'y'          the pixel y coordinates
 'userData'</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_InterpolationMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">InterpolationMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Pixel</td>
<td></td>
</tr>
<tr>
<td>Linear</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_JoinStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">JoinStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Bevel</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
<tr>
<td>Miter</td>
<td></td>
</tr>
<tr>
<td>MiterBevel</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Jpg_Decoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Jpg.</span><span class="api-item-title-strong">Decoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\jpg.swg#L141" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Jpg_Decoder_IImageDecoder_canDecode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">canDecode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\jpg.swg#L2044" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canDecode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Jpg_Decoder_IImageDecoder_decode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">decode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\jpg.swg#L2050" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">decode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, buffer: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>, options: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Jpg_EncodeOptions"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Jpg.</span><span class="api-item-title-strong">EncodeOptions</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\jpg.swg#L174" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>quality</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td>Encoding quality between 1 and 100. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Jpg_Encoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Jpg.</span><span class="api-item-title-strong">Encoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\jpg.swg#L180" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Jpg_Encoder_IImageEncoder_canEncode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageEncoder.</span><span class="api-item-title-strong">canEncode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\jpg.swg#L731" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canEncode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Jpg_Encoder_IImageEncoder_encode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageEncoder.</span><span class="api-item-title-strong">encode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\jpg.swg#L737" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">encode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, result: *<span class="SCst">Core</span>.<span class="SCst">ConcatBuffer</span>, image: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>, options: <span class="STpe">any</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LineCapStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">LineCapStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Triangle</td>
<td></td>
</tr>
<tr>
<td>Round</td>
<td></td>
</tr>
<tr>
<td>Square</td>
<td></td>
</tr>
<tr>
<td>ArrowAnchor</td>
<td></td>
</tr>
<tr>
<td>SquareAnchor</td>
<td></td>
</tr>
<tr>
<td>RoundAnchor</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">LinePath</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>elements</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Element">Element</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>startPoint</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>endPoint</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>isClosed</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>isDirty</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>serial</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>flattenQuality</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintQuality">PaintQuality</a></span></span></td>
<td></td>
</tr>
<tr>
<td>isPolyOnly</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>isFlatten</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>bvMin</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>bvMax</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>endPosBuffer</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>polygon</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Polygon">Polygon</a></span></span></td>
<td></td>
</tr>
<tr>
<td>points</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Point">Point</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>triangles</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Triangle">Triangle</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>edgeList</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Edge">Edge</a></span>)</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_arcTo">arcTo</a></span></td>
<td>Add an arc from the previous point. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_bezierTo">bezierTo</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>)</span></td>
<td>Add a curve from the previous point to <span class="code-inline">pt</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_bezierTo">bezierTo</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>)</span></td>
<td>Add a curve from the previous point to <span class="code-inline">pt</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_clearCache">clearCache</a></span></td>
<td>Clear internal cache. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_close">close</a></span></td>
<td>Close the figure  Will add a line or a curve to the first point if necessary. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_curveTo">curveTo</a></span></td>
<td>Add a curve from the previous point to <span class="code-inline">pt</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_flatten">flatten</a></span></td>
<td>Convert the path to a list of points. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_lineTo">lineTo</a></span></td>
<td>Add a line from the previous point to <span class="code-inline">pt</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_setArc">setArc</a></span></td>
<td>Initialize the path with an arc. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_setCircle">setCircle</a></span></td>
<td>Initialize the path with a circle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_setEllipse">setEllipse</a></span></td>
<td>Initialize the path with an ellipse. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_setRect">setRect</a></span></td>
<td>Initialize the path with a rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_setRoundRect">setRoundRect</a></span></td>
<td>Initialize the path with a round rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePath_start">start</a></span></td>
<td>Reset the path. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_arcTo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">arcTo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L145" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add an arc from the previous point. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">arcTo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, initAngle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>, maxAngle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_bezierTo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">bezierTo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L112" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a curve from the previous point to <span class="code-inline">pt</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bezierTo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, ctx: <span class="STpe">f32</span>, cty: <span class="STpe">f32</span>)</span></code>
</div>
<p>Add a curve from the previous point to <span class="code-inline">pt</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bezierTo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, ct1x: <span class="STpe">f32</span>, ct1y: <span class="STpe">f32</span>, ct2x: <span class="STpe">f32</span>, ct2y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_clearCache"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">clearCache</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear internal cache. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clearCache</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_close"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">close</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L174" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Close the figure  Will add a line or a curve to the first point if necessary. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">close</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_curveTo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">curveTo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a curve from the previous point to <span class="code-inline">pt</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">curveTo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_flatten"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">flatten</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L324" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert the path to a list of points. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">flatten</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, quality = <span class="SCst"><a href="#Pixel_PaintQuality">PaintQuality</a></span>.<span class="SCst">Normal</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_lineTo"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">lineTo</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a line from the previous point to <span class="code-inline">pt</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">lineTo</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_setArc"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">setArc</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L279" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with an arc. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setArc</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radius: <span class="STpe">f32</span>, startAngle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>, endAngle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_setCircle"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">setCircle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L264" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with a circle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setCircle</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radius: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_setEllipse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">setEllipse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L253" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with an ellipse. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setEllipse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_setRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">setRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with a rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_setRoundRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">setRoundRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L216" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initialize the path with a round rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRoundRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePath_start"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePath.</span><span class="api-item-title-strong">start</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepath.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the path. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">start</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, startX: <span class="STpe">f32</span>, startY: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePathList"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">LinePathList</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>paths</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePath">LinePath</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>bvMin</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>bvMax</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePathList_clean">clean</a></span></td>
<td>Clean all paths  They must before be flattened. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePathList_clear">clear</a></span></td>
<td>Remove all internal line paths. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePathList_count">count</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePathList_flatten">flatten</a></span></td>
<td>Flatten all paths. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePathList_newPath">newPath</a></span></td>
<td>Returns a new path. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePathList_offset">offset</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>)</span></td>
<td>Call fast offset on all paths  They must before be flattened. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_LinePathList_offset">offset</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="SCst">JoinStyle</span>, <span class="STpe">f32</span>)</span></td>
<td>Call quality offset on all paths  They must before be flattened. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePathList_clean"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePathList.</span><span class="api-item-title-strong">clean</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clean all paths  They must before be flattened. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clean</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePathList_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePathList.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove all internal line paths. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePathList_count"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePathList.</span><span class="api-item-title-strong">count</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">count</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">u64</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePathList_flatten"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePathList.</span><span class="api-item-title-strong">flatten</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flatten all paths. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">flatten</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, quality: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintQuality">PaintQuality</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePathList_newPath"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePathList.</span><span class="api-item-title-strong">newPath</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a new path. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">newPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePath">LinePath</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_LinePathList_offset"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">LinePathList.</span><span class="api-item-title-strong">offset</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\linepathlist.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Call fast offset on all paths  They must before be flattened. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">offset</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f32</span>)</span></code>
</div>
<p>Call quality offset on all paths  They must before be flattened. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">offset</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f32</span>, joinStyle: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_JoinStyle">JoinStyle</a></span>, toler: <span class="STpe">f32</span> = <span class="SNum">0.5</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_NativeRenderOgl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">NativeRenderOgl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_NativeRenderOgl_createContext">createContext</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SCst">HDC</span>, <span class="STpe">s32</span>, <span class="STpe">s32</span>, <span class="SCst">HGLRC</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_NativeRenderOgl_createContext">createContext</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SCst">HWND</span>, <span class="STpe">s32</span>, <span class="STpe">s32</span>, <span class="SCst">HGLRC</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_NativeRenderOgl_dropContext">dropContext</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_NativeRenderOgl_setCurrentContext">setCurrentContext</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_NativeRenderOgl_swapBuffers">swapBuffers</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_NativeRenderOgl_createContext"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NativeRenderOgl.</span><span class="api-item-title-strong">createContext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createContext</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, hdc: <span class="SCst">Win32</span>.<span class="SCst">HDC</span>, width: <span class="STpe">s32</span>, height: <span class="STpe">s32</span>, mainRC: <span class="SCst">Ogl</span>.<span class="SCst">HGLRC</span> = <span class="SKwd">null</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderingContext">RenderingContext</a></span> <span class="SKwd">throw</span>
<span class="SKwd">func</span> <span class="SFct">createContext</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, hWnd: <span class="SCst">Win32</span>.<span class="SCst">HWND</span>, width: <span class="STpe">s32</span>, height: <span class="STpe">s32</span>, mainRC: <span class="SCst">Ogl</span>.<span class="SCst">HGLRC</span> = <span class="SKwd">null</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderingContext">RenderingContext</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_NativeRenderOgl_dropContext"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NativeRenderOgl.</span><span class="api-item-title-strong">dropContext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">dropContext</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rc: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_NativeRenderOgl_setCurrentContext"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NativeRenderOgl.</span><span class="api-item-title-strong">setCurrentContext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setCurrentContext</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rc: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_NativeRenderOgl_swapBuffers"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NativeRenderOgl.</span><span class="api-item-title-strong">swapBuffers</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L62" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">swapBuffers</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rc: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_PaintParams"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">PaintParams</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>Antialiased</td>
<td></td>
</tr>
<tr>
<td>Default</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_PaintQuality"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">PaintQuality</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L57" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Draft</td>
<td></td>
</tr>
<tr>
<td>Normal</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_PaintState"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">PaintState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L80" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>paintParams</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintParams">PaintParams</a></span></span></td>
<td></td>
</tr>
<tr>
<td>paintQuality</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintQuality">PaintQuality</a></span></span></td>
<td></td>
</tr>
<tr>
<td>transform</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Transform2</span></span></td>
<td></td>
</tr>
<tr>
<td>blendingMode</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_BlendingMode">BlendingMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td>interpolationMode</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_InterpolationMode">InterpolationMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td>clippingRect</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td>stackClipRect</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)</span></td>
<td></td>
</tr>
<tr>
<td>clippingRectOn</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>colorMask</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ColorMask">ColorMask</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Painter</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L136" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>commandBuffer</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Command">Command</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>vertexBuffer</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_VertexLayout">VertexLayout</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>triangulateIdx</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">s32</span>)</span></td>
<td></td>
</tr>
<tr>
<td>fake</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>sharedSolidPen</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span></span></td>
<td></td>
</tr>
<tr>
<td>sharedSolidBrush</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span></span></td>
<td></td>
</tr>
<tr>
<td>sharedLinePath</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePath">LinePath</a></span></span></td>
<td></td>
</tr>
<tr>
<td>sharedLinePathList</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePathList">LinePathList</a></span></span></td>
<td></td>
</tr>
<tr>
<td>sharedRoundTmp</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'({factor: <span class="STpe">f32</span>, pos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span>})</span></td>
<td></td>
</tr>
<tr>
<td>curState</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintState">PaintState</a></span></span></td>
<td></td>
</tr>
<tr>
<td>stackState</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintState">PaintState</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>stackTransform</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Transform2</span>)</span></td>
<td></td>
</tr>
<tr>
<td>lastTexture0Type</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_BrushType">BrushType</a></span></span></td>
<td></td>
</tr>
<tr>
<td>stackRT</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>curRT</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span></span></td>
<td></td>
</tr>
<tr>
<td>lastTexture0Handle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td>hasTextureFont</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>drawingBegin</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>overlapMode</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>canSetTexture0</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_addParams">addParams</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_begin">begin</a></span></td>
<td>Call this before drawing. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_bindRenderTarget">bindRenderTarget</a></span></td>
<td>Bind a render target. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_capStyleExtent">capStyleExtent</a></span></td>
<td>Returns the extent in both directions of a given LineCapStyle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_clear">clear</a></span></td>
<td>Clear the rendering surface. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_computeLayout">computeLayout</a></span></td>
<td>Recompute the layout for a given rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawArc">drawArc</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Angle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Angle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>)</span></td>
<td>Draw an arc between two angles. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawArc">drawArc</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Angle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Angle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawCircle">drawCircle</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>)</span></td>
<td>Draw an empty circle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawCircle">drawCircle</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawEllipse">drawEllipse</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>)</span></td>
<td>Draw an empty ellipse. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawEllipse">drawEllipse</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawLine">drawLine</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Vector2</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Vector2</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td>Draw a line. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawLine">drawLine</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawLine">drawLine</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawPath">drawPath</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePath</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>)</span></td>
<td>Draw the line path. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawPath">drawPath</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePath</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td>Draw the line path with the given <span class="code-inline">pen</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawPath">drawPath</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePathList</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>, <span class="SCst">DrawPathListMode</span>)</span></td>
<td>Draw the line path. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawPath">drawPath</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePathList</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>, <span class="SCst">DrawPathListMode</span>)</span></td>
<td>Draw the line path. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawRect">drawRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawRect">drawRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawRect">drawRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>)</span></td>
<td>Draw an empty rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawRect">drawRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawRichString">drawRichString</a></span></td>
<td>Draw a rich string. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawRoundRect">drawRoundRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawRoundRect">drawRoundRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawRoundRect">drawRoundRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="STpe">f32</span>)</span></td>
<td>Draw an empty round rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawRoundRect">drawRoundRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Pen</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawString">drawString</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">string</span>, *<span class="SCst">Font</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>, <span class="SCst">UnderlineStyle</span>, <span class="STpe">f32</span>)</span></td>
<td>Draw a simple string. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawString">drawString</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">string</span>, *<span class="SCst">Font</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>, <span class="SCst">UnderlineStyle</span>, <span class="STpe">f32</span>)</span></td>
<td>Draw a simple string. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawStringCenter">drawStringCenter</a></span></td>
<td>Draw a simple string (centered). </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawTexture">drawTexture</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Texture</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawTexture">drawTexture</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Texture</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawTexture">drawTexture</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Texture</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawTexture">drawTexture</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Texture</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawTexture">drawTexture</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Texture</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td>Draw a portion of an image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_drawTexture">drawTexture</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Texture</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td>Draw a texture image. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_end">end</a></span></td>
<td>Call this after drawing. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_endClippingRegion">endClippingRegion</a></span></td>
<td>Stop painting in the clipping buffer. Back to normal. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillCircle">fillCircle</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>)</span></td>
<td>Draw a solid circle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillCircle">fillCircle</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillEllipse">fillEllipse</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>)</span></td>
<td>Draw a solid ellipse. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillEllipse">fillEllipse</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillPath">fillPath</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePath</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>)</span></td>
<td>Fill the line path with the given <span class="code-inline">brush</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillPath">fillPath</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePath</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td>Fill the line path. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillPath">fillPath</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePathList</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>)</span></td>
<td>Fill the list of paths  clockwise = fill, anti-clockwise = hole. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillPath">fillPath</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePathList</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td>Fill the line path. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillPolygon">fillPolygon</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePath</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>)</span></td>
<td>Fill the polygon with the given <span class="code-inline">brush</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillPolygon">fillPolygon</a></span><span class="SCde">(<span class="SKwd">self</span>, *<span class="SCst">LinePath</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td>Fill the polygon with the given color. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillRect">fillRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillRect">fillRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillRect">fillRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>)</span></td>
<td>Draw a filled rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillRect">fillRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillRoundRect">fillRoundRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillRoundRect">fillRoundRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillRoundRect">fillRoundRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Brush</span>)</span></td>
<td>Draw a filled round rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_fillRoundRect">fillRoundRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Color</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_getClippingRect">getClippingRect</a></span></td>
<td>Get the current clipping rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_getGlyphs">getGlyphs</a></span><span class="SCde">(<span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Array</span>'(<span class="STpe">rune</span>), *<span class="SCst">Font</span>)</span></td>
<td>Returns all the glyphs for a given string and font. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_getGlyphs">getGlyphs</a></span><span class="SCde">(<span class="STpe">string</span>, *<span class="SCst">Font</span>)</span></td>
<td>Returns all the glyphs for a given string and font. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_getParams">getParams</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_getQuality">getQuality</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_getTransform">getTransform</a></span></td>
<td>Returns the current tranformation. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_isEmptyClippingRect">isEmptyClippingRect</a></span></td>
<td>Returns true if the current clipping rectangle is empty (no drawing !). </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_measureRune">measureRune</a></span></td>
<td>Get a rune size for a given font. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_measureString">measureString</a></span><span class="SCde">(<span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Array</span>'(<span class="SKwd">const</span> *<span class="SCst">GlyphDesc</span>))</span></td>
<td>Returns the bounding rectangle if a given text, at a given position. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_measureString">measureString</a></span><span class="SCde">(<span class="STpe">string</span>, *<span class="SCst">Font</span>)</span></td>
<td>Returns the metrics of a given text. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_popClippingRect">popClippingRect</a></span></td>
<td>Restore the original clipping rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_popState">popState</a></span></td>
<td>Restore the last paint state. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_popTransform">popTransform</a></span></td>
<td>Restore the pushed transformation. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_pushClippingRect">pushClippingRect</a></span></td>
<td>Interface the given rectangle with the current clipping rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_pushClippingSurfaceRect">pushClippingSurfaceRect</a></span></td>
<td>Interface the given rectangle with the current clipping rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_pushState">pushState</a></span></td>
<td>Save the current paint state. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_pushTransform">pushTransform</a></span></td>
<td>Push the current transformation. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_releaseRenderTarget">releaseRenderTarget</a></span></td>
<td>Release the render target. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_removeParams">removeParams</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_resetClippingRect">resetClippingRect</a></span></td>
<td>Reset the current clipping rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_resetClippingRegion">resetClippingRegion</a></span></td>
<td>Reset clipping buffer to its default value (no more clipping). </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_resetState">resetState</a></span></td>
<td>Reset the current painting state. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_resetTransform">resetTransform</a></span></td>
<td>Reset the paint position, rotation, scale. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_rotateTransform">rotateTransform</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Angle</span>)</span></td>
<td>Set the paint rotation. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_rotateTransform">rotateTransform</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Angle</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>)</span></td>
<td>Set the paint rotation. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_rotateTransformInPlace">rotateTransformInPlace</a></span></td>
<td>Set the paint rotation. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_scaleTransform">scaleTransform</a></span></td>
<td>Set the paint scale. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setAntialiased">setAntialiased</a></span></td>
<td>Set antialiased mode on/off. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setBlendingMode">setBlendingMode</a></span></td>
<td>Set the current blending mode. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setClippingRect">setClippingRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Rectangle</span>)</span></td>
<td>Set the current clipping rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setClippingRect">setClippingRect</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>)</span></td>
<td>Set the current clipping rectangle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setClippingRegionMode">setClippingRegionMode</a></span></td>
<td>Set the clipping region mode. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setColorMask">setColorMask</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">bool</span>, <span class="STpe">bool</span>, <span class="STpe">bool</span>, <span class="STpe">bool</span>)</span></td>
<td>Set color mask. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setColorMask">setColorMask</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">ColorMask</span>)</span></td>
<td>Set color mask. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setColorMaskAlpha">setColorMaskAlpha</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setColorMaskColor">setColorMaskColor</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setColorMaskFull">setColorMaskFull</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setInterpolationMode">setInterpolationMode</a></span></td>
<td>Set the texture interpolation mode. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setParams">setParams</a></span></td>
<td>Set the paint parameters. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setQuality">setQuality</a></span></td>
<td>Set the paint quality. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setShader">setShader</a></span></td>
<td>Set the current shader. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setShaderParam">setShaderParam</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SCst">ShaderParamHandle</span>, <span class="STpe">f32</span>)</span></td>
<td>Set a shader parameter. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setShaderParam">setShaderParam</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SCst">ShaderParamHandle</span>, <span class="STpe">s32</span>)</span></td>
<td>Set a shader parameter. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setState">setState</a></span></td>
<td>Set the current painting state. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_setTransform">setTransform</a></span></td>
<td>Set the paint transformation. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_startClippingRegion">startClippingRegion</a></span></td>
<td>Start painting in the clipping buffer. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_translateTransform">translateTransform</a></span></td>
<td>Set the paint position. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Painter_unbindRenderTarget">unbindRenderTarget</a></span></td>
<td>Bind a render target. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_addParams"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">addParams</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L480" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addParams</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, add: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintParams">PaintParams</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_begin"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">begin</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L399" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Call this before drawing. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">begin</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_bindRenderTarget"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">bindRenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L742" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Bind a render target. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bindRenderTarget</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, tgt: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span>, paintAlpha = <span class="SKwd">false</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_capStyleExtent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">capStyleExtent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawpath.swg#L753" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the extent in both directions of a given LineCapStyle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">capStyleExtent</span>(capStyle: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LineCapStyle">LineCapStyle</a></span>, lineSize: <span class="STpe">f32</span>, capScale: <span class="STpe">f32</span>)-&gt;{w: <span class="STpe">f32</span>, l: <span class="STpe">f32</span>}</span></code>
</div>
<ul>
<li><span class="code-inline">lineSize</span> is the supposed line drawing size</li>
<li><span class="code-inline">capScale</span> is the supposed LineCapStyle scale</li>
</ul>
<p> Returns the width and the length. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L423" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear the rendering surface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_computeLayout"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">computeLayout</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L244" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Recompute the layout for a given rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">computeLayout</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, text: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RichString">RichString</a></span>, format: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RichStringFormat">RichStringFormat</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawArc"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawArc</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an arc between two angles. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawArc</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radius: <span class="STpe">f32</span>, startAngle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>, endAngle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span> = <span class="SNum">1</span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawArc</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radius: <span class="STpe">f32</span>, startAngle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>, endAngle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawCircle"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawCircle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an empty circle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawCircle</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radius: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span> = <span class="SNum">1</span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawCircle</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radius: <span class="STpe">f32</span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawEllipse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawEllipse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an empty ellipse. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawEllipse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span> = <span class="SNum">1</span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawEllipse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawline.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a line. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, start: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span>, end: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, startX: <span class="STpe">f32</span>, startY: <span class="STpe">f32</span>, endX: <span class="STpe">f32</span>, endY: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span> = <span class="SNum">1</span>)
<span class="SKwd">func</span> <span class="SFct">drawLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, startX: <span class="STpe">f32</span>, startY: <span class="STpe">f32</span>, endX: <span class="STpe">f32</span>, endY: <span class="STpe">f32</span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawPath"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawPath</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawpath.swg#L669" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw the line path. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, path: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePath">LinePath</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span>)</span></code>
</div>
<p>Draw the line path with the given <span class="code-inline">pen</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, path: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePath">LinePath</a></span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>Draw the line path. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pathList: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePathList">LinePathList</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span>, mode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DrawPathListMode">DrawPathListMode</a></span> = .<span class="SCst">Separate</span>)</span></code>
</div>
<p>Draw the line path. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pathList: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePathList">LinePathList</a></span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>, mode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DrawPathListMode">DrawPathListMode</a></span> = .<span class="SCst">Separate</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawrect.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an empty rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span> = <span class="SNum">1</span>)
<span class="SKwd">func</span> <span class="SFct">drawRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)
<span class="SKwd">func</span> <span class="SFct">drawRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span> = <span class="SNum">1</span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawRichString"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawRichString</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L429" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a rich string. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawRichString</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pos: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, text: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RichString">RichString</a></span>, format: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RichStringFormat">RichStringFormat</a></span>, forceCompute = <span class="SKwd">false</span>)</span></code>
</div>
<p> A rich string can be multiline, and accepts  The layout will be computed at the first call, and each time <span class="code-inline">pos</span> has changed </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawRoundRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawRoundRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawrect.swg#L168" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw an empty round rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawRoundRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span> = <span class="SNum">1</span>)
<span class="SKwd">func</span> <span class="SFct">drawRoundRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)
<span class="SKwd">func</span> <span class="SFct">drawRoundRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, penSize: <span class="STpe">f32</span> = <span class="SNum">1</span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawRoundRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, pen: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawString"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawString</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L139" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a simple string. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawString</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, text: <span class="STpe">string</span>, font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>, under = <span class="SCst"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span>.<span class="SCst">None</span>, zoom = <span class="SNum">1.0</span>)</span></code>
</div>
<p>Draw a simple string. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawString</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, text: <span class="STpe">string</span>, font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, under = <span class="SCst"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span>.<span class="SCst">None</span>, zoom = <span class="SNum">1.0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawStringCenter"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawStringCenter</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L120" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a simple string (centered). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawStringCenter</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, text: <span class="STpe">string</span>, font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, under = <span class="SCst"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span>.<span class="SCst">None</span>, zoom = <span class="SNum">1.0</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_drawTexture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">drawTexture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawtexture.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a portion of an image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawTexture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, dstRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, srcRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span> = <span class="SCst"><a href="#Pixel_Argb">Argb</a></span>.<span class="SCst">White</span>)
<span class="SKwd">func</span> <span class="SFct">drawTexture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span> = <span class="SCst"><a href="#Pixel_Argb">Argb</a></span>.<span class="SCst">White</span>)
<span class="SKwd">func</span> <span class="SFct">drawTexture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, srcRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span> = <span class="SCst"><a href="#Pixel_Argb">Argb</a></span>.<span class="SCst">White</span>)
<span class="SKwd">func</span> <span class="SFct">drawTexture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span> = <span class="SCst"><a href="#Pixel_Argb">Argb</a></span>.<span class="SCst">White</span>)
<span class="SKwd">func</span> <span class="SFct">drawTexture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, srcRect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span> = <span class="SCst"><a href="#Pixel_Argb">Argb</a></span>.<span class="SCst">White</span>)</span></code>
</div>
<p>Draw a texture image. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drawTexture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span> = <span class="SCst"><a href="#Pixel_Argb">Argb</a></span>.<span class="SCst">White</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_end"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">end</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L416" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Call this after drawing. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">end</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_endClippingRegion"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">endClippingRegion</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L699" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Stop painting in the clipping buffer. Back to normal. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endClippingRegion</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_fillCircle"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">fillCircle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a solid circle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillCircle</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radius: <span class="STpe">f32</span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillCircle</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radius: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_fillEllipse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">fillEllipse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawcircle.swg#L104" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a solid ellipse. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillEllipse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillEllipse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_fillPath"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">fillPath</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\fillpath.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill the line path with the given <span class="code-inline">brush</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, path: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePath">LinePath</a></span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<p>Fill the line path. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, path: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePath">LinePath</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>Fill the list of paths  clockwise = fill, anti-clockwise = hole. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pathList: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePathList">LinePathList</a></span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<p>Fill the line path. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillPath</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pathList: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePathList">LinePathList</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_fillPolygon"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">fillPolygon</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\fillpath.swg#L263" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill the polygon with the given <span class="code-inline">brush</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillPolygon</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, path: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePath">LinePath</a></span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<p>Fill the polygon with the given color. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillPolygon</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, path: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePath">LinePath</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_fillRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">fillRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawrect.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a filled rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>)
<span class="SKwd">func</span> <span class="SFct">fillRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)
<span class="SKwd">func</span> <span class="SFct">fillRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_fillRoundRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">fillRoundRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawrect.swg#L185" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw a filled round rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillRoundRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>)
<span class="SKwd">func</span> <span class="SFct">fillRoundRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)
<span class="SKwd">func</span> <span class="SFct">fillRoundRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, brush: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span>)</span></code>
</div>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">fillRoundRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, left: <span class="STpe">f32</span>, top: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>, radiusX: <span class="STpe">f32</span>, radiusY: <span class="STpe">f32</span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_getClippingRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">getClippingRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L671" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the current clipping rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getClippingRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_getGlyphs"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">getGlyphs</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns all the glyphs for a given string and font. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getGlyphs</span>(text: <span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">rune</span>), font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SKwd">const</span> *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span>)</span></code>
</div>
<p>Returns all the glyphs for a given string and font. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getGlyphs</span>(text: <span class="STpe">string</span>, font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SKwd">const</span> *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_getParams"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">getParams</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L500" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getParams</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintParams">PaintParams</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_getQuality"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">getQuality</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L502" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getQuality</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintQuality">PaintQuality</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_getTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">getTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L589" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the current tranformation. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Transform2</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_isEmptyClippingRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">isEmptyClippingRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L679" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the current clipping rectangle is empty (no drawing !). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isEmptyClippingRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_measureRune"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">measureRune</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L86" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get a rune size for a given font. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">measureRune</span>(c: <span class="STpe">rune</span>, font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_measureString"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">measureString</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L103" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the bounding rectangle if a given text, at a given position. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">measureString</span>(text: <span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SKwd">const</span> *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_GlyphDesc">GlyphDesc</a></span>))-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_StringMetrics">StringMetrics</a></span></span></code>
</div>
<p>Returns the metrics of a given text. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">measureString</span>(text: <span class="STpe">string</span>, font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_StringMetrics">StringMetrics</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_popClippingRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">popClippingRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L657" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Restore the original clipping rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">popClippingRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_popState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">popState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L467" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Restore the last paint state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">popState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_popTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">popTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L568" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Restore the pushed transformation. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">popTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_pushClippingRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">pushClippingRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L636" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Interface the given rectangle with the current clipping rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pushClippingRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, isSurfacePos = <span class="SKwd">false</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_pushClippingSurfaceRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">pushClippingSurfaceRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L630" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Interface the given rectangle with the current clipping rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pushClippingSurfaceRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_pushState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">pushState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L461" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Save the current paint state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pushState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_pushTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">pushTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L562" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Push the current transformation. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pushTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_releaseRenderTarget"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">releaseRenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L771" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the render target. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">releaseRenderTarget</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, tgt: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_removeParams"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">removeParams</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L485" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">removeParams</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, remove: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintParams">PaintParams</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_resetClippingRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">resetClippingRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L619" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the current clipping rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resetClippingRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_resetClippingRegion"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">resetClippingRegion</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L705" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset clipping buffer to its default value (no more clipping). </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resetClippingRegion</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_resetState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">resetState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L430" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the current painting state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resetState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_resetTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">resetTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L574" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the paint position, rotation, scale. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resetTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_rotateTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">rotateTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L521" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint rotation. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rotateTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, angle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>)</span></code>
</div>
<p>Set the paint rotation. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rotateTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, angle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_rotateTransformInPlace"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">rotateTransformInPlace</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L532" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint rotation. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rotateTransformInPlace</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, angle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>)</span></code>
</div>
<p> Will rotate around the current transformation position. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_scaleTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">scaleTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L552" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint scale. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">scaleTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setAntialiased"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setAntialiased</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L491" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set antialiased mode on/off. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setAntialiased</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, aa = <span class="SKwd">true</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setBlendingMode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setBlendingMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L711" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current blending mode. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setBlendingMode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_BlendingMode">BlendingMode</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setClippingRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setClippingRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L608" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current clipping rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setClippingRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)</span></code>
</div>
<p>Set the current clipping rectangle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setClippingRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>, width: <span class="STpe">f32</span>, height: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setClippingRegionMode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setClippingRegionMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L692" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the clipping region mode. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setClippingRegionMode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ClippingMode">ClippingMode</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setColorMask"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setColorMask</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L727" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set color mask. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColorMask</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, r: <span class="STpe">bool</span>, g: <span class="STpe">bool</span>, b: <span class="STpe">bool</span>, a: <span class="STpe">bool</span>)</span></code>
</div>
<p>Set color mask. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColorMask</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cm: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ColorMask">ColorMask</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setColorMaskAlpha"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setColorMaskAlpha</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L739" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColorMaskAlpha</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setColorMaskColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setColorMaskColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L737" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColorMaskColor</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setColorMaskFull"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setColorMaskFull</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L738" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setColorMaskFull</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setInterpolationMode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setInterpolationMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L596" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the texture interpolation mode. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setInterpolationMode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mode: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_InterpolationMode">InterpolationMode</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setParams"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setParams</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L474" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint parameters. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setParams</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, add: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintParams">PaintParams</a></span>, remove: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintParams">PaintParams</a></span> = .<span class="SCst">Zero</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setQuality"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setQuality</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L505" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint quality. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setQuality</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, quality: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintQuality">PaintQuality</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L780" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current shader. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setShader</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, shader: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderBase">ShaderBase</a></span> = <span class="SKwd">null</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setShaderParam"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setShaderParam</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L796" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set a shader parameter. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setShaderParam</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, param: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>, val: <span class="STpe">f32</span>)</span></code>
</div>
<p>Set a shader parameter. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setShaderParam</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, param: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>, val: <span class="STpe">s32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L446" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current painting state. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setState</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, state: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PaintState">PaintState</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_setTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">setTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L581" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint transformation. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, tr: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Transform2</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_startClippingRegion"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">startClippingRegion</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L685" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Start painting in the clipping buffer. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">startClippingRegion</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mode = <span class="SCst"><a href="#Pixel_ClippingMode">ClippingMode</a></span>.<span class="SCst">Set</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_translateTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">translateTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L511" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the paint position. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">translateTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Painter_unbindRenderTarget"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Painter.</span><span class="api-item-title-strong">unbindRenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L756" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Bind a render target. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">unbindRenderTarget</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Pen"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Pen</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>size</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>brush</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span></span></td>
<td></td>
</tr>
<tr>
<td>pattern</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td>dashBeginCapStyle</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DashCapStyle">DashCapStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td>dashEndCapStyle</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DashCapStyle">DashCapStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td>borderPos</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_BorderPos">BorderPos</a></span></span></td>
<td></td>
</tr>
<tr>
<td>capStartScaleX</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>capStartScaleY</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>capEndScaleX</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>capEndScaleY</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>joinMiterLimit</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>plotOffset</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>dashStyle</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DashStyle">DashStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td>beginCapStyle</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LineCapStyle">LineCapStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td>endCapStyle</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LineCapStyle">LineCapStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td>joinStyle</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_JoinStyle">JoinStyle</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Pen_createDash">createDash</a></span></td>
<td>Returns a dash pen. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Pen_createHatch">createHatch</a></span></td>
<td>Return a predefined hatch pen. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Pen_createSolid">createSolid</a></span></td>
<td>Returns a solid color pen. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Pen_createDash"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Pen.</span><span class="api-item-title-strong">createDash</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a dash pen. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createDash</span>(style: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DashStyle">DashStyle</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, size: <span class="STpe">f32</span> = <span class="SNum">1</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Pen_createHatch"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Pen.</span><span class="api-item-title-strong">createHatch</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L95" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Return a predefined hatch pen. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createHatch</span>(type: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_HatchStyle">HatchStyle</a></span>, color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, size: <span class="STpe">f32</span> = <span class="SNum">1</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Pen_createSolid"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Pen.</span><span class="api-item-title-strong">createSolid</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\types\pen.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a solid color pen. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createSolid</span>(color: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>, size: <span class="STpe">f32</span> = <span class="SNum">1</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Pen">Pen</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_PixelFormat"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">PixelFormat</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Desribe the layout format of one image pixel. </p>
<table class="table-enumeration">
<tr>
<td>BGR8</td>
<td></td>
</tr>
<tr>
<td>BGRA8</td>
<td></td>
</tr>
<tr>
<td>RGB8</td>
<td></td>
</tr>
<tr>
<td>RGBA8</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_PixelFormat_bitDepth"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PixelFormat.</span><span class="api-item-title-strong">bitDepth</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the bit depth of each channel. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bitDepth</span>(<span class="SKwd">self</span>)-&gt;<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_PixelFormat_bpp"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PixelFormat.</span><span class="api-item-title-strong">bpp</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the bpp (bits per pixel) of the given pixel format. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bpp</span>(<span class="SKwd">self</span>)-&gt;<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_PixelFormat_channels"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PixelFormat.</span><span class="api-item-title-strong">channels</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the number of image channels. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">channels</span>(<span class="SKwd">self</span>)-&gt;<span class="STpe">u8</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_PixelFormat_hasAlpha"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">PixelFormat.</span><span class="api-item-title-strong">hasAlpha</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\pixelformat.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the given pixel format has an alpha channel. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hasAlpha</span>(<span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Png_Decoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Png.</span><span class="api-item-title-strong">Decoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\png.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Png_Decoder_IImageDecoder_canDecode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">canDecode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\png.swg#L694" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canDecode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Png_Decoder_IImageDecoder_decode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">decode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\png.swg#L699" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">decode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, buffer: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>, options: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Png_EncodeOptions"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Png.</span><span class="api-item-title-strong">EncodeOptions</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\png.swg#L582" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>compressionLevel</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Compress</span>.<span class="SCst">Deflate</span>.<span class="SCst">CompressionLevel</span></span></td>
<td></td>
</tr>
<tr>
<td>filtering</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Png_Encoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Png.</span><span class="api-item-title-strong">Encoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\png.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Png_Encoder_IImageEncoder_canEncode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageEncoder.</span><span class="api-item-title-strong">canEncode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\png.swg#L590" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canEncode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Png_Encoder_IImageEncoder_encode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageEncoder.</span><span class="api-item-title-strong">encode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\png.swg#L595" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">encode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, result: *<span class="SCst">Core</span>.<span class="SCst">ConcatBuffer</span>, image: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>, options: <span class="STpe">any</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Edge"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Poly2Tri.</span><span class="api-item-title-strong">Edge</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>p</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></td>
<td></td>
</tr>
<tr>
<td>q</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Point"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Poly2Tri.</span><span class="api-item-title-strong">Point</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>using v</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>edgeList</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Edge">Edge</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>border</td>
<td class="code-type"><span class="SCde">[<span class="SNum">2</span>] *<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Edge">Edge</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Tesselate"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Poly2Tri.</span><span class="api-item-title-strong">Tesselate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L459" class="src">[src]</a></td>
</tr>
</table>
</p>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Poly2Tri_Tesselate_addPoint">addPoint</a></span></td>
<td>Register a new point. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Poly2Tri_Tesselate_clear">clear</a></span></td>
<td>Clear content. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Poly2Tri_Tesselate_endPolyLine">endPolyLine</a></span></td>
<td>To call to register the polyline. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Poly2Tri_Tesselate_getTriangles">getTriangles</a></span></td>
<td>Returns the list of triangles. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Poly2Tri_Tesselate_startPolyLine">startPolyLine</a></span></td>
<td>To call before adding a polyline. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Poly2Tri_Tesselate_triangulate">triangulate</a></span></td>
<td>Triangulate. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Tesselate_addPoint"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tesselate.</span><span class="api-item-title-strong">addPoint</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L522" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a new point. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addPoint</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Tesselate_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tesselate.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L485" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear content. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Tesselate_endPolyLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tesselate.</span><span class="api-item-title-strong">endPolyLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L531" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>To call to register the polyline. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endPolyLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Tesselate_getTriangles"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tesselate.</span><span class="api-item-title-strong">getTriangles</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L510" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the list of triangles. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTriangles</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;[..] *<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Triangle">Triangle</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Tesselate_startPolyLine"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tesselate.</span><span class="api-item-title-strong">startPolyLine</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L516" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>To call before adding a polyline. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">startPolyLine</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Tesselate_triangulate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Tesselate.</span><span class="api-item-title-strong">triangulate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L549" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Triangulate. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">triangulate</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Poly2Tri_Triangle"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Poly2Tri.</span><span class="api-item-title-strong">Triangle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\poly2tri.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>constrainedEdge</td>
<td class="code-type"><span class="SCde">[<span class="SNum">3</span>] <span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td>delaunayEdge</td>
<td class="code-type"><span class="SCde">[<span class="SNum">3</span>] <span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td>points</td>
<td class="code-type"><span class="SCde">[<span class="SNum">3</span>] *<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></td>
<td></td>
</tr>
<tr>
<td>neighbors</td>
<td class="code-type"><span class="SCde">[<span class="SNum">3</span>] *<span class="SCst">Pixel</span>.<span class="SCst">Poly2Tri</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Triangle">Triangle</a></span></span></td>
<td></td>
</tr>
<tr>
<td>interior</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Polygon</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>points</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span>)</span></td>
<td></td>
</tr>
<tr>
<td>isClockwise</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>isConvex</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>area</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>totalLen</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>bvMin</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>bvMax</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_add">add</a></span></td>
<td>Add a new point. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_clean">clean</a></span></td>
<td>Clean polygon by removing bad/unecessary points. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_clear">clear</a></span></td>
<td>Clear all points. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_compute">compute</a></span></td>
<td>Compute internal values, like convex/clockwise  Call it once the polygon contains all its points. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_getClean">getClean</a></span></td>
<td>Clean polygon by removing bad/unecessary points. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_getOffset">getOffset</a></span></td>
<td>Slow but quality offset with a joinStyle. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_getPoint">getPoint</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">u64</span>)</span></td>
<td>Get the given point. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_getPoint">getPoint</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">u64</span>, <span class="STpe">f32</span>)</span></td>
<td>Get the given point, with an optional offset. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_normalizeDist">normalizeDist</a></span></td>
<td>Divide by adding points if two points are too far away. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_offset">offset</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>)</span></td>
<td>Fast expand polygon by a given amount. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Polygon_offset">offset</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">f32</span>, <span class="SCst">JoinStyle</span>, <span class="STpe">f32</span>)</span></td>
<td>Slow but quality offset with a joinStyle. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon_add"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Polygon.</span><span class="api-item-title-strong">add</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new point. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, v: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon_clean"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Polygon.</span><span class="api-item-title-strong">clean</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L222" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clean polygon by removing bad/unecessary points. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clean</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Polygon.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L90" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear all points. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon_compute"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Polygon.</span><span class="api-item-title-strong">compute</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L256" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Compute internal values, like convex/clockwise  Call it once the polygon contains all its points. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">compute</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon_getClean"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Polygon.</span><span class="api-item-title-strong">getClean</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L232" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clean polygon by removing bad/unecessary points. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getClean</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePathList">LinePathList</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon_getOffset"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Polygon.</span><span class="api-item-title-strong">getOffset</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Slow but quality offset with a joinStyle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getOffset</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f32</span>, joinStyle: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_JoinStyle">JoinStyle</a></span>, toler: <span class="STpe">f32</span> = <span class="SNum">0.5</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePathList">LinePathList</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon_getPoint"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Polygon.</span><span class="api-item-title-strong">getPoint</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the given point. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getPoint</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, i: <span class="STpe">u64</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></code>
</div>
<p>Get the given point, with an optional offset. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getPoint</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, i: <span class="STpe">u64</span>, offset: <span class="STpe">f32</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon_normalizeDist"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Polygon.</span><span class="api-item-title-strong">normalizeDist</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Divide by adding points if two points are too far away. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">normalizeDist</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, dist: <span class="STpe">f32</span>, closed: <span class="STpe">bool</span> = <span class="SKwd">true</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Polygon_offset"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Polygon.</span><span class="api-item-title-strong">offset</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\poly\polygon.swg#L106" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fast expand polygon by a given amount. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">offset</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f32</span>)</span></code>
</div>
<p>Slow but quality offset with a joinStyle. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">offset</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f32</span>, joinStyle: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_JoinStyle">JoinStyle</a></span>, toler: <span class="STpe">f32</span> = <span class="SNum">0.5</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">RenderOgl</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>using native</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_NativeRenderOgl">NativeRenderOgl</a></span></span></td>
<td></td>
</tr>
<tr>
<td>vertexbuffer</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>shaderSimple</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderSimple">ShaderSimple</a></span></span></td>
<td></td>
</tr>
<tr>
<td>shaderAA</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderAA">ShaderAA</a></span></span></td>
<td></td>
</tr>
<tr>
<td>shaderBlur</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderBlur">ShaderBlur</a></span></span></td>
<td></td>
</tr>
<tr>
<td>worldTransform</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Transform2</span></span></td>
<td></td>
</tr>
<tr>
<td>orthoMatrix</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Matrix4x4</span></span></td>
<td></td>
</tr>
<tr>
<td>modelMatrix</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Matrix4x4</span></span></td>
<td></td>
</tr>
<tr>
<td>whiteTexture</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>hatchTextures</td>
<td class="code-type"><span class="SCde">[<span class="SNum">36</span>] <span class="SCst">Ogl</span>.<span class="SCst">GLuint</span></span></td>
<td></td>
</tr>
<tr>
<td>blendingMode</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_BlendingMode">BlendingMode</a></span></span></td>
<td></td>
</tr>
<tr>
<td>curRC</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderingContext">RenderingContext</a></span></span></td>
<td></td>
</tr>
<tr>
<td>stackReleasedRT</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>stackRT</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>curRT</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span></span></td>
<td></td>
</tr>
<tr>
<td>curShader</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderBase">ShaderBase</a></span></span></td>
<td></td>
</tr>
<tr>
<td>overlapMode</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_addImage">addImage</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Image</span>)</span></td>
<td>Register an image for rendering. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_addImage">addImage</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="STpe">string</span>)</span></td>
<td>Load and register an image for rendering. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_begin">begin</a></span></td>
<td>To be called before rendering. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_bindRenderTarget">bindRenderTarget</a></span></td>
<td>Set the current render target. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_clear">clear</a></span></td>
<td>Clear current render buffers. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_computeMatrices">computeMatrices</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_createRenderTarget">createRenderTarget</a></span></td>
<td>Create a render target. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_createShader">createShader</a></span></td>
<td>Create the given shader. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_deleteRenderTarget">deleteRenderTarget</a></span></td>
<td>Delete a render target. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_draw">draw</a></span></td>
<td>Draw the given painter. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_end">end</a></span></td>
<td>To be called after rendering. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_getRenderTarget">getRenderTarget</a></span></td>
<td>Get a render target of the given size  releaseRenderTarget must be called to release it. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_getTargetSize">getTargetSize</a></span></td>
<td>Get the actual render target size. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_getTexturePixels">getTexturePixels</a></span></td>
<td>Returns the pixels associated to a texture. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_init">init</a></span></td>
<td>First init. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_pixelFormatToNative">pixelFormatToNative</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_releaseRenderTarget">releaseRenderTarget</a></span></td>
<td>Release the given render target. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_removeTexture">removeTexture</a></span></td>
<td>Unregister a texture created with <span class="code-inline">addImage</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_resetScissorRect">resetScissorRect</a></span></td>
<td>Reset the scissor. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_resetTransform">resetTransform</a></span></td>
<td>Reset the world transform. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_rotateTransform">rotateTransform</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Angle</span>)</span></td>
<td>Rotate the world transform. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_rotateTransform">rotateTransform</a></span><span class="SCde">(<span class="SKwd">self</span>, <span class="SKwd">const</span> <span class="SKwd">ref</span> <span class="SCst">Angle</span>, <span class="STpe">f32</span>, <span class="STpe">f32</span>)</span></td>
<td>Rotate the world transform. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_scaleTransform">scaleTransform</a></span></td>
<td>Scale the world transform. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_setCurrentContext">setCurrentContext</a></span></td>
<td>Set the current rendering context. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_setScissorRect">setScissorRect</a></span></td>
<td>Force the scissor. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_setShader">setShader</a></span></td>
<td>Set the current shader. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_translateTransform">translateTransform</a></span></td>
<td>Translate the world transform. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_unbindRenderTarget">unbindRenderTarget</a></span></td>
<td>Go back to the previous render target, or frame buffer. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_unbindRenderTargetToImage">unbindRenderTargetToImage</a></span></td>
<td>Get current render target pixels. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RenderOgl_updateTexture">updateTexture</a></span></td>
<td>Update content of texture. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_addImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">addImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L460" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register an image for rendering. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addImage</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, image: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span></span></code>
</div>
<p>Load and register an image for rendering. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addImage</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fullName: <span class="STpe">string</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_begin"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">begin</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L211" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>To be called before rendering. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">begin</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rc: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_bindRenderTarget"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">bindRenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L354" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current render target. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">bindRenderTarget</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rt: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span>, vp: *<span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span> = <span class="SKwd">null</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L234" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear current render buffers. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_computeMatrices"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">computeMatrices</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L571" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">computeMatrices</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, mvp: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>, mdl: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderParamHandle">ShaderParamHandle</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_createRenderTarget"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">createRenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L399" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a render target. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createRenderTarget</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, w: <span class="STpe">s32</span>, h: <span class="STpe">s32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_createShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">createShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L897" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create the given shader. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createShader</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, base: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderBase">ShaderBase</a></span>, vertex: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>, pixel: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_deleteRenderTarget"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">deleteRenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L433" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Delete a render target. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deleteRenderTarget</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rt: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_draw"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">draw</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L815" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw the given painter. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">draw</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, dc: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Painter">Painter</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_end"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">end</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L219" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>To be called after rendering. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">end</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_getRenderTarget"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">getRenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L862" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get a render target of the given size  releaseRenderTarget must be called to release it. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getRenderTarget</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, w: <span class="STpe">s32</span>, h: <span class="STpe">s32</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_getTargetSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">getTargetSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L243" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the actual render target size. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTargetSize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;{width: <span class="STpe">s32</span>, height: <span class="STpe">s32</span>}</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_getTexturePixels"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">getTexturePixels</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L514" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the pixels associated to a texture. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getTexturePixels</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>, pf: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_init"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">init</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L266" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>First init. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">init</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_pixelFormatToNative"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">pixelFormatToNative</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L490" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pixelFormatToNative</span>(pf: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span>)-&gt;<span class="SCst">Ogl</span>.<span class="SCst">GLenum</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_releaseRenderTarget"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">releaseRenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L891" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the given render target. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">releaseRenderTarget</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, tgt: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderTarget">RenderTarget</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_removeTexture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">removeTexture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L528" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Unregister a texture created with <span class="code-inline">addImage</span>. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">removeTexture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, texture: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_resetScissorRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">resetScissorRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L260" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the scissor. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resetScissorRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_resetTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">resetTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L565" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reset the world transform. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resetTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_rotateTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">rotateTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L544" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Rotate the world transform. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rotateTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, angle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>)</span></code>
</div>
<p>Rotate the world transform. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">rotateTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, angle: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Angle</span>, centerX: <span class="STpe">f32</span>, centerY: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_scaleTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">scaleTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L558" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Scale the world transform. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">scaleTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_setCurrentContext"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">setCurrentContext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L227" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current rendering context. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setCurrentContext</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rc: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RenderingContext">RenderingContext</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_setScissorRect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">setScissorRect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L251" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the scissor. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setScissorRect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_setShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">setShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L852" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the current shader. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setShader</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, shader: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderBase">ShaderBase</a></span> = <span class="SKwd">null</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_translateTransform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">translateTransform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L537" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Translate the world transform. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">translateTransform</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">f32</span>, y: <span class="STpe">f32</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_unbindRenderTarget"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">unbindRenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L381" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Go back to the previous render target, or frame buffer. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">unbindRenderTarget</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_unbindRenderTargetToImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">unbindRenderTargetToImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L371" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get current render target pixels. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">unbindRenderTargetToImage</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, w: <span class="STpe">s32</span>, h: <span class="STpe">s32</span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderOgl_updateTexture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RenderOgl.</span><span class="api-item-title-strong">updateTexture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.swg#L505" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Update content of texture. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">updateTexture</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, texture: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span>, pixels: <span class="SKwd">const</span> *<span class="STpe">u8</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderTarget"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">RenderTarget</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\rendertarget.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>handle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td>colorBuffer</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Texture">Texture</a></span></span></td>
<td></td>
</tr>
<tr>
<td>depthStencil</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td>paintAlpha</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>paintAlphaIdx</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RenderingContext"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">RenderingContext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\renderogl.win32.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>hDC</td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td>hRC</td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td>width</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>height</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichChunk"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">RichChunk</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>slice</td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td>fontStyle</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td>hasFontStyle</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>isSpace</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>isEol</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>colorIdx</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>boundRect</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td>pos</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst"><a href="#Pixel_Poly2Tri_Point">Point</a></span></span></td>
<td></td>
</tr>
<tr>
<td>font</td>
<td class="code-type"><span class="SCde">*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span></span></td>
<td></td>
</tr>
<tr>
<td>under</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_UnderlineStyle">UnderlineStyle</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichString"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">RichString</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Represents a string with embedded formats. </p>
<table class="table-enumeration">
<tr>
<td>raw</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td>The string as passed by the user. </td>
</tr>
<tr>
<td>chunks</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RichChunk">RichChunk</a></span>)</span></td>
<td>The corresponding list of chunks. </td>
</tr>
<tr>
<td>layoutRect</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td>boundRect</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span></span></td>
<td></td>
</tr>
<tr>
<td>dirty</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td>If dirty, <span class="code-inline">Compute</span>. </td>
</tr>
</table>
<p> This kind of string is a specific version of [Core.String] but specific for painting. It can contains  some special markers to change it's appearence. </p>
<div class="code-block"><code><span class="SCde">&lt;b&gt; this <span class="SInv">is</span> bold&lt;b&gt;
 &lt;p1&gt; this will have special color <span class="SNum">1</span>&lt;p1&gt;
 ...</span></code>
</div>
<p> See <a href="#Pixel_RichString_set">RichString.set</a> for a list of available markers. </p>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RichString_compute">compute</a></span></td>
<td>Compute layout. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RichString_getNaked">getNaked</a></span></td>
<td>Transform the given string to a naked one. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RichString_invalidate">invalidate</a></span></td>
<td>Force the string to be repainted. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RichString_isEmpty">isEmpty</a></span></td>
<td>Returns <span class="code-inline">true</span> if the string is empty. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RichString_set">set</a></span></td>
<td>Associate a string. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RichString_opAffect">opAffect</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RichString_opCount">opCount</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_RichString_opEquals">opEquals</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichString_compute"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichString.</span><span class="api-item-title-strong">compute</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L160" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Compute layout. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">compute</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, force = <span class="SKwd">false</span>)</span></code>
</div>
<p> This will transform the string in a serie of chunks, each chunk having its one style. The string  will only be recomputed if it's dirty (the text has changed) or if <span class="code-inline">force</span> is true. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichString_getNaked"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichString.</span><span class="api-item-title-strong">getNaked</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L122" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform the given string to a naked one. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getNaked</span>(str: <span class="STpe">string</span>)-&gt;<span class="SCst">Core</span>.<span class="SCst">String</span></span></code>
</div>
<p> Will return the string without the format markers. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichString_invalidate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichString.</span><span class="api-item-title-strong">invalidate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L95" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force the string to be repainted. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">invalidate</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichString_isEmpty"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichString.</span><span class="api-item-title-strong">isEmpty</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns <span class="code-inline">true</span> if the string is empty. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isEmpty</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichString_opAffect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichString.</span><span class="api-item-title-strong">opAffect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L88" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, str: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichString_opCount"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichString.</span><span class="api-item-title-strong">opCount</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L86" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opCount</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">u64</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichString_opEquals"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichString.</span><span class="api-item-title-strong">opEquals</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, other: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichString_set"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">RichString.</span><span class="api-item-title-strong">set</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\richstring.swg#L112" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Associate a string. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">set</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, text: <span class="STpe">string</span>)</span></code>
</div>
<p> The string can have multiple lines.  Accepted markers are: </p>
<ul>
<li><span class="code-inline">&lt;u&gt;</span> =&gt; underline</li>
<li><span class="code-inline">&lt;b&gt;</span> =&gt; bold</li>
<li><span class="code-inline">&lt;i&gt;</span> =&gt; italic</li>
<li><span class="code-inline">&lt;p1&gt;</span> =&gt; color palette 1</li>
<li><span class="code-inline">&lt;p2&gt;</span> =&gt; color palette 2</li>
<li><span class="code-inline">&lt;p3&gt;</span> =&gt; color palette 3</li>
</ul>
<div class="code-block"><code><span class="SCde">&lt;b&gt;this <span class="SInv">is</span> bold&lt;b&gt; but this <span class="SInv">is</span> normal &lt;i&gt;<span class="SLgc">and</span> this <span class="SInv">is</span> italic&lt;/i&gt;</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_RichStringFormat"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">RichStringFormat</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>font</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamily">FontFamily</a></span></span></td>
<td></td>
</tr>
<tr>
<td>fontStyle</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_FontFamilyStyle">FontFamilyStyle</a></span></span></td>
<td></td>
</tr>
<tr>
<td>palette</td>
<td class="code-type"><span class="SCde">[<span class="SNum">4</span>] <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span></span></td>
<td></td>
</tr>
<tr>
<td>brush</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Brush">Brush</a></span></span></td>
<td></td>
</tr>
<tr>
<td>horzAlignment</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_StringHorzAlignment">StringHorzAlignment</a></span></span></td>
<td></td>
</tr>
<tr>
<td>vertAlignment</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_StringVertAlignment">StringVertAlignment</a></span></span></td>
<td></td>
</tr>
<tr>
<td>lineGap</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>flags</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_StringFormatFlags">StringFormatFlags</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_ShaderAA"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">ShaderAA</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\shader.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>using base</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderBase">ShaderBase</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_ShaderBase"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">ShaderBase</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\shader.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>handle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td>mvp</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td>mdl</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td>boundRect</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td>textureRect</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td>uvMode</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td>copyMode</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td>textureW</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td>textureH</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_ShaderBlur"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">ShaderBlur</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\shader.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>using base</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderBase">ShaderBase</a></span></span></td>
<td></td>
</tr>
<tr>
<td>radius</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
<tr>
<td>axis</td>
<td class="code-type"><span class="STpe">s64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_ShaderSimple"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">ShaderSimple</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\render\ogl\shader.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>using base</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_ShaderBase">ShaderBase</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringFormatFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">StringFormatFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Default</td>
<td></td>
</tr>
<tr>
<td>WordWrap</td>
<td></td>
</tr>
<tr>
<td>Clip</td>
<td></td>
</tr>
<tr>
<td>DontAdaptSingleLineHeight</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringHorzAlignment"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">StringHorzAlignment</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Center</td>
<td></td>
</tr>
<tr>
<td>Left</td>
<td></td>
</tr>
<tr>
<td>Right</td>
<td></td>
</tr>
<tr>
<td>Justify</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringMetrics"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">StringMetrics</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>ascent</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td>Maximum size above baseline. </td>
</tr>
<tr>
<td>descent</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td>Maximum size below baseline. </td>
</tr>
<tr>
<td>width</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>height</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td>ascent + descent. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringPainter"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">StringPainter</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>str</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RichString">RichString</a></span></span></td>
<td></td>
</tr>
<tr>
<td>fmt</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_RichStringFormat">RichStringFormat</a></span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_StringPainter_color">color</a></span></td>
<td>Set text color. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_StringPainter_draw">draw</a></span></td>
<td>Draw string. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_StringPainter_flags">flags</a></span></td>
<td>Set drawing flags. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_StringPainter_font">font</a></span></td>
<td>Set font. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_StringPainter_horzAlign">horzAlign</a></span></td>
<td>Set horizontal alignment. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_StringPainter_vertAlign">vertAlign</a></span></td>
<td>Set vertical alignment. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringPainter_color"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StringPainter.</span><span class="api-item-title-strong">color</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set text color. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">color</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, col: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Color">Color</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringPainter_draw"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StringPainter.</span><span class="api-item-title-strong">draw</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Draw string. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">draw</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, painter: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Painter">Painter</a></span>, rect: <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Rectangle</span>, what: <span class="STpe">string</span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringPainter_flags"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StringPainter.</span><span class="api-item-title-strong">flags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set drawing flags. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">flags</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, f: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_StringFormatFlags">StringFormatFlags</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringPainter_font"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StringPainter.</span><span class="api-item-title-strong">font</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set font. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">font</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, font: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Font">Font</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringPainter_horzAlign"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StringPainter.</span><span class="api-item-title-strong">horzAlign</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set horizontal alignment. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">horzAlign</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, align: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_StringHorzAlignment">StringHorzAlignment</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringPainter_vertAlign"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StringPainter.</span><span class="api-item-title-strong">vertAlign</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\stringpainter.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set vertical alignment. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">vertAlign</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, align: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_StringVertAlignment">StringVertAlignment</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_StringVertAlignment"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">StringVertAlignment</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Center</td>
<td></td>
</tr>
<tr>
<td>Top</td>
<td></td>
</tr>
<tr>
<td>Bottom</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Texture"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">Texture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>handle</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td>pf</td>
<td class="code-type"><span class="SCde"><span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_PixelFormat">PixelFormat</a></span></span></td>
<td></td>
</tr>
<tr>
<td>width</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>height</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>valid</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Texture_getByteSize">getByteSize</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_Texture_isValid">isValid</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Texture_getByteSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Texture.</span><span class="api-item-title-strong">getByteSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getByteSize</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">s32</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Texture_isValid"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Texture.</span><span class="api-item-title-strong">isValid</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isValid</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Tga_Decoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Tga.</span><span class="api-item-title-strong">Decoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\tga.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Tga_Decoder_IImageDecoder_canDecode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">canDecode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\tga.swg#L95" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canDecode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Tga_Decoder_IImageDecoder_decode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageDecoder.</span><span class="api-item-title-strong">decode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\decode\tga.swg#L100" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">decode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, buffer: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>, options: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_DecodeOptions">DecodeOptions</a></span>)-&gt;<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Tga_Encoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Tga.</span><span class="api-item-title-strong">Encoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\tga.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Tga_Encoder_IImageEncoder_canEncode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageEncoder.</span><span class="api-item-title-strong">canEncode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\tga.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">canEncode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, fileName: <span class="STpe">string</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_Tga_Encoder_IImageEncoder_encode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IImageEncoder.</span><span class="api-item-title-strong">encode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\encode\tga.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">encode</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, result: *<span class="SCst">Core</span>.<span class="SCst">ConcatBuffer</span>, image: <span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_Image">Image</a></span>, options: <span class="STpe">any</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">TypeFace</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>fullname</td>
<td class="code-type"><span class="STpe">string</span></td>
<td></td>
</tr>
<tr>
<td>buffer</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>)</span></td>
<td></td>
</tr>
<tr>
<td>face</td>
<td class="code-type"><span class="SCde">*<span class="SCst">FreeType</span>.<span class="SCst">FT_FaceRec</span></span></td>
<td></td>
</tr>
<tr>
<td>forceBoldDiv</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>forceItalic</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
<tr>
<td>forceBold</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_create">create</a></span></td>
<td>Get or create a new typeface from a memory buffer. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_createFromHFONT">createFromHFONT</a></span></td>
<td>Create a new typeface from a HFONT. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_getFamilyName">getFamilyName</a></span></td>
<td>Returns the underlying font family name. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_getRuneOutline">getRuneOutline</a></span></td>
<td>Convert a rune to its outline, at a given size. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_getStringOutline">getStringOutline</a></span></td>
<td>Convert a string to its outline, at a given size. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_isBold">isBold</a></span></td>
<td>Returns true if the typeface is bold. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_isItalic">isItalic</a></span></td>
<td>Returns true if the typeface is italic. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_load">load</a></span></td>
<td>Get or load a new typeface. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_release">release</a></span></td>
<td>Release the given typeface. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Pixel_TypeFace_renderGlyph">renderGlyph</a></span></td>
<td>Render one glyph. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get or create a new typeface from a memory buffer. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(fullname: <span class="STpe">string</span>, bytes: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_TypeFace">TypeFace</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_createFromHFONT"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">createFromHFONT</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.win32.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a new typeface from a HFONT. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createFromHFONT</span>(fullname: <span class="STpe">string</span>, hf: <span class="SCst">Gdi32</span>.<span class="SCst">HFONT</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_TypeFace">TypeFace</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_getFamilyName"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">getFamilyName</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L76" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the underlying font family name. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getFamilyName</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">string</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_getRuneOutline"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">getRuneOutline</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L161" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a rune to its outline, at a given size. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getRuneOutline</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pathList: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePathList">LinePathList</a></span>, c: <span class="STpe">rune</span>, size: <span class="STpe">u32</span>, embolden: <span class="STpe">u32</span> = <span class="SNum">0</span>)-&gt;<span class="STpe">f32</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_getStringOutline"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">getStringOutline</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L188" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a string to its outline, at a given size. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getStringOutline</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, pathList: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_LinePathList">LinePathList</a></span>, text: <span class="STpe">string</span>, size: <span class="STpe">u32</span>, embolden: <span class="STpe">u32</span> = <span class="SNum">0</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_isBold"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">isBold</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L64" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the typeface is bold. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isBold</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_isItalic"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">isItalic</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L70" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the typeface is italic. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isItalic</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_load"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">load</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L111" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get or load a new typeface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">load</span>(fullname: <span class="STpe">string</span>)-&gt;*<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_TypeFace">TypeFace</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_release"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">release</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the given typeface. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">release</span>(typeface: *<span class="SCst">Pixel</span>.<span class="SCst"><a href="#Pixel_TypeFace">TypeFace</a></span>)</span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_TypeFace_renderGlyph"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">TypeFace.</span><span class="api-item-title-strong">renderGlyph</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\text\typeface.swg#L150" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Render one glyph. </p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">renderGlyph</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, c: <span class="STpe">rune</span>, size: <span class="STpe">u32</span>, forceBold = <span class="SKwd">false</span>, forceItalic = <span class="SKwd">false</span>)-&gt;<span class="SKwd">const</span> *<span class="SCst">FreeType</span>.<span class="SCst">FT_Bitmap</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_UVMode"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">UVMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\image\texture.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Fill</td>
<td></td>
</tr>
<tr>
<td>FillSubRect</td>
<td></td>
</tr>
<tr>
<td>Tile</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_UnderlineStyle"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">UnderlineStyle</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\drawstring.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Underline</td>
<td></td>
</tr>
<tr>
<td>Strikeout</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Pixel_VertexLayout"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Pixel.</span><span class="api-item-title-strong">VertexLayout</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/pixel\src\painter\painter.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>pos</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>uv</td>
<td class="code-type"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector2</span></span></td>
<td></td>
</tr>
<tr>
<td>color</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>aa</td>
<td class="code-type"><span class="SCde">[<span class="SNum">12</span>] <span class="SCst">Core</span>.<span class="SCst">Math</span>.<span class="SCst">Vector4</span></span></td>
<td></td>
</tr>
<tr>
<td>aanum</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<div class="swag-watermark">
Generated on 19-10-2023 with <a href="https://swag-lang.org/index.php">swag</a> 0.26.0</div>
</div>
</div>
</div>
</body>
</html>
