<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module ogl</title>
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
        
    html { font-family: ui-sans-serif, system-ui, "Segoe UI", Roboto, "Helvetica Neue", Arial, "Noto Sans", sans-serif; }
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
    .code-block   { font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace; }
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
<?php include('common/end-head.php'); ?>
</head>
<body>
<?php include('common/start-body.php'); ?>
<div class="container">
<div class="left">
<div class="left-page">
<h2>Table of Contents</h2>
<h3>Constants</h3>
<h4></h4>
<ul>
<li><a href="#Ogl_GL_2D">GL_2D</a></li>
<li><a href="#Ogl_GL_2_BYTES">GL_2_BYTES</a></li>
<li><a href="#Ogl_GL_3D">GL_3D</a></li>
<li><a href="#Ogl_GL_3D_COLOR">GL_3D_COLOR</a></li>
<li><a href="#Ogl_GL_3D_COLOR_TEXTURE">GL_3D_COLOR_TEXTURE</a></li>
<li><a href="#Ogl_GL_3_BYTES">GL_3_BYTES</a></li>
<li><a href="#Ogl_GL_4D_COLOR_TEXTURE">GL_4D_COLOR_TEXTURE</a></li>
<li><a href="#Ogl_GL_4_BYTES">GL_4_BYTES</a></li>
<li><a href="#Ogl_GL_ACCUM">GL_ACCUM</a></li>
<li><a href="#Ogl_GL_ACCUM_ALPHA_BITS">GL_ACCUM_ALPHA_BITS</a></li>
<li><a href="#Ogl_GL_ACCUM_BLUE_BITS">GL_ACCUM_BLUE_BITS</a></li>
<li><a href="#Ogl_GL_ACCUM_BUFFER_BIT">GL_ACCUM_BUFFER_BIT</a></li>
<li><a href="#Ogl_GL_ACCUM_CLEAR_VALUE">GL_ACCUM_CLEAR_VALUE</a></li>
<li><a href="#Ogl_GL_ACCUM_GREEN_BITS">GL_ACCUM_GREEN_BITS</a></li>
<li><a href="#Ogl_GL_ACCUM_RED_BITS">GL_ACCUM_RED_BITS</a></li>
<li><a href="#Ogl_GL_ACTIVE_ATTRIBUTES">GL_ACTIVE_ATTRIBUTES</a></li>
<li><a href="#Ogl_GL_ACTIVE_ATTRIBUTE_MAX_LENGTH">GL_ACTIVE_ATTRIBUTE_MAX_LENGTH</a></li>
<li><a href="#Ogl_GL_ACTIVE_TEXTURE">GL_ACTIVE_TEXTURE</a></li>
<li><a href="#Ogl_GL_ACTIVE_UNIFORMS">GL_ACTIVE_UNIFORMS</a></li>
<li><a href="#Ogl_GL_ACTIVE_UNIFORM_MAX_LENGTH">GL_ACTIVE_UNIFORM_MAX_LENGTH</a></li>
<li><a href="#Ogl_GL_ADD">GL_ADD</a></li>
<li><a href="#Ogl_GL_ADD_SIGNED">GL_ADD_SIGNED</a></li>
<li><a href="#Ogl_GL_ALIASED_LINE_WIDTH_RANGE">GL_ALIASED_LINE_WIDTH_RANGE</a></li>
<li><a href="#Ogl_GL_ALIASED_POINT_SIZE_RANGE">GL_ALIASED_POINT_SIZE_RANGE</a></li>
<li><a href="#Ogl_GL_ALL_ATTRIB_BITS">GL_ALL_ATTRIB_BITS</a></li>
<li><a href="#Ogl_GL_ALPHA">GL_ALPHA</a></li>
<li><a href="#Ogl_GL_ALPHA12">GL_ALPHA12</a></li>
<li><a href="#Ogl_GL_ALPHA16">GL_ALPHA16</a></li>
<li><a href="#Ogl_GL_ALPHA4">GL_ALPHA4</a></li>
<li><a href="#Ogl_GL_ALPHA8">GL_ALPHA8</a></li>
<li><a href="#Ogl_GL_ALPHA_BIAS">GL_ALPHA_BIAS</a></li>
<li><a href="#Ogl_GL_ALPHA_BITS">GL_ALPHA_BITS</a></li>
<li><a href="#Ogl_GL_ALPHA_INTEGER">GL_ALPHA_INTEGER</a></li>
<li><a href="#Ogl_GL_ALPHA_SCALE">GL_ALPHA_SCALE</a></li>
<li><a href="#Ogl_GL_ALPHA_TEST">GL_ALPHA_TEST</a></li>
<li><a href="#Ogl_GL_ALPHA_TEST_FUNC">GL_ALPHA_TEST_FUNC</a></li>
<li><a href="#Ogl_GL_ALPHA_TEST_REF">GL_ALPHA_TEST_REF</a></li>
<li><a href="#Ogl_GL_ALWAYS">GL_ALWAYS</a></li>
<li><a href="#Ogl_GL_AMBIENT">GL_AMBIENT</a></li>
<li><a href="#Ogl_GL_AMBIENT_AND_DIFFUSE">GL_AMBIENT_AND_DIFFUSE</a></li>
<li><a href="#Ogl_GL_AND">GL_AND</a></li>
<li><a href="#Ogl_GL_AND_INVERTED">GL_AND_INVERTED</a></li>
<li><a href="#Ogl_GL_AND_REVERSE">GL_AND_REVERSE</a></li>
<li><a href="#Ogl_GL_ARRAY_BUFFER">GL_ARRAY_BUFFER</a></li>
<li><a href="#Ogl_GL_ARRAY_BUFFER_BINDING">GL_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_ATTACHED_SHADERS">GL_ATTACHED_SHADERS</a></li>
<li><a href="#Ogl_GL_ATTRIB_STACK_DEPTH">GL_ATTRIB_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_AUTO_NORMAL">GL_AUTO_NORMAL</a></li>
<li><a href="#Ogl_GL_AUX0">GL_AUX0</a></li>
<li><a href="#Ogl_GL_AUX1">GL_AUX1</a></li>
<li><a href="#Ogl_GL_AUX2">GL_AUX2</a></li>
<li><a href="#Ogl_GL_AUX3">GL_AUX3</a></li>
<li><a href="#Ogl_GL_AUX_BUFFERS">GL_AUX_BUFFERS</a></li>
<li><a href="#Ogl_GL_BACK">GL_BACK</a></li>
<li><a href="#Ogl_GL_BACK_LEFT">GL_BACK_LEFT</a></li>
<li><a href="#Ogl_GL_BACK_RIGHT">GL_BACK_RIGHT</a></li>
<li><a href="#Ogl_GL_BGR">GL_BGR</a></li>
<li><a href="#Ogl_GL_BGRA">GL_BGRA</a></li>
<li><a href="#Ogl_GL_BGRA_EXT">GL_BGRA_EXT</a></li>
<li><a href="#Ogl_GL_BGRA_INTEGER">GL_BGRA_INTEGER</a></li>
<li><a href="#Ogl_GL_BGR_EXT">GL_BGR_EXT</a></li>
<li><a href="#Ogl_GL_BGR_INTEGER">GL_BGR_INTEGER</a></li>
<li><a href="#Ogl_GL_BITMAP">GL_BITMAP</a></li>
<li><a href="#Ogl_GL_BITMAP_TOKEN">GL_BITMAP_TOKEN</a></li>
<li><a href="#Ogl_GL_BLEND">GL_BLEND</a></li>
<li><a href="#Ogl_GL_BLEND_COLOR">GL_BLEND_COLOR</a></li>
<li><a href="#Ogl_GL_BLEND_DST">GL_BLEND_DST</a></li>
<li><a href="#Ogl_GL_BLEND_DST_ALPHA">GL_BLEND_DST_ALPHA</a></li>
<li><a href="#Ogl_GL_BLEND_DST_RGB">GL_BLEND_DST_RGB</a></li>
<li><a href="#Ogl_GL_BLEND_EQUATION">GL_BLEND_EQUATION</a></li>
<li><a href="#Ogl_GL_BLEND_EQUATION_ALPHA">GL_BLEND_EQUATION_ALPHA</a></li>
<li><a href="#Ogl_GL_BLEND_EQUATION_RGB">GL_BLEND_EQUATION_RGB</a></li>
<li><a href="#Ogl_GL_BLEND_SRC">GL_BLEND_SRC</a></li>
<li><a href="#Ogl_GL_BLEND_SRC_ALPHA">GL_BLEND_SRC_ALPHA</a></li>
<li><a href="#Ogl_GL_BLEND_SRC_RGB">GL_BLEND_SRC_RGB</a></li>
<li><a href="#Ogl_GL_BLUE">GL_BLUE</a></li>
<li><a href="#Ogl_GL_BLUE_BIAS">GL_BLUE_BIAS</a></li>
<li><a href="#Ogl_GL_BLUE_BITS">GL_BLUE_BITS</a></li>
<li><a href="#Ogl_GL_BLUE_INTEGER">GL_BLUE_INTEGER</a></li>
<li><a href="#Ogl_GL_BLUE_SCALE">GL_BLUE_SCALE</a></li>
<li><a href="#Ogl_GL_BOOL">GL_BOOL</a></li>
<li><a href="#Ogl_GL_BOOL_VEC2">GL_BOOL_VEC2</a></li>
<li><a href="#Ogl_GL_BOOL_VEC3">GL_BOOL_VEC3</a></li>
<li><a href="#Ogl_GL_BOOL_VEC4">GL_BOOL_VEC4</a></li>
<li><a href="#Ogl_GL_BUFFER_ACCESS">GL_BUFFER_ACCESS</a></li>
<li><a href="#Ogl_GL_BUFFER_ACCESS_FLAGS">GL_BUFFER_ACCESS_FLAGS</a></li>
<li><a href="#Ogl_GL_BUFFER_MAPPED">GL_BUFFER_MAPPED</a></li>
<li><a href="#Ogl_GL_BUFFER_MAP_LENGTH">GL_BUFFER_MAP_LENGTH</a></li>
<li><a href="#Ogl_GL_BUFFER_MAP_OFFSET">GL_BUFFER_MAP_OFFSET</a></li>
<li><a href="#Ogl_GL_BUFFER_MAP_POINTER">GL_BUFFER_MAP_POINTER</a></li>
<li><a href="#Ogl_GL_BUFFER_SIZE">GL_BUFFER_SIZE</a></li>
<li><a href="#Ogl_GL_BUFFER_USAGE">GL_BUFFER_USAGE</a></li>
<li><a href="#Ogl_GL_BYTE">GL_BYTE</a></li>
<li><a href="#Ogl_GL_C3F_V3F">GL_C3F_V3F</a></li>
<li><a href="#Ogl_GL_C4F_N3F_V3F">GL_C4F_N3F_V3F</a></li>
<li><a href="#Ogl_GL_C4UB_V2F">GL_C4UB_V2F</a></li>
<li><a href="#Ogl_GL_C4UB_V3F">GL_C4UB_V3F</a></li>
<li><a href="#Ogl_GL_CCW">GL_CCW</a></li>
<li><a href="#Ogl_GL_CLAMP">GL_CLAMP</a></li>
<li><a href="#Ogl_GL_CLAMP_FRAGMENT_COLOR">GL_CLAMP_FRAGMENT_COLOR</a></li>
<li><a href="#Ogl_GL_CLAMP_READ_COLOR">GL_CLAMP_READ_COLOR</a></li>
<li><a href="#Ogl_GL_CLAMP_TO_BORDER">GL_CLAMP_TO_BORDER</a></li>
<li><a href="#Ogl_GL_CLAMP_TO_EDGE">GL_CLAMP_TO_EDGE</a></li>
<li><a href="#Ogl_GL_CLAMP_VERTEX_COLOR">GL_CLAMP_VERTEX_COLOR</a></li>
<li><a href="#Ogl_GL_CLEAR">GL_CLEAR</a></li>
<li><a href="#Ogl_GL_CLIENT_ACTIVE_TEXTURE">GL_CLIENT_ACTIVE_TEXTURE</a></li>
<li><a href="#Ogl_GL_CLIENT_ALL_ATTRIB_BITS">GL_CLIENT_ALL_ATTRIB_BITS</a></li>
<li><a href="#Ogl_GL_CLIENT_ATTRIB_STACK_DEPTH">GL_CLIENT_ATTRIB_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_CLIENT_PIXEL_STORE_BIT">GL_CLIENT_PIXEL_STORE_BIT</a></li>
<li><a href="#Ogl_GL_CLIENT_VERTEX_ARRAY_BIT">GL_CLIENT_VERTEX_ARRAY_BIT</a></li>
<li><a href="#Ogl_GL_CLIPPING_INPUT_PRIMITIVES">GL_CLIPPING_INPUT_PRIMITIVES</a></li>
<li><a href="#Ogl_GL_CLIPPING_OUTPUT_PRIMITIVES">GL_CLIPPING_OUTPUT_PRIMITIVES</a></li>
<li><a href="#Ogl_GL_CLIP_DISTANCE0">GL_CLIP_DISTANCE0</a></li>
<li><a href="#Ogl_GL_CLIP_DISTANCE1">GL_CLIP_DISTANCE1</a></li>
<li><a href="#Ogl_GL_CLIP_DISTANCE2">GL_CLIP_DISTANCE2</a></li>
<li><a href="#Ogl_GL_CLIP_DISTANCE3">GL_CLIP_DISTANCE3</a></li>
<li><a href="#Ogl_GL_CLIP_DISTANCE4">GL_CLIP_DISTANCE4</a></li>
<li><a href="#Ogl_GL_CLIP_DISTANCE5">GL_CLIP_DISTANCE5</a></li>
<li><a href="#Ogl_GL_CLIP_PLANE0">GL_CLIP_PLANE0</a></li>
<li><a href="#Ogl_GL_CLIP_PLANE1">GL_CLIP_PLANE1</a></li>
<li><a href="#Ogl_GL_CLIP_PLANE2">GL_CLIP_PLANE2</a></li>
<li><a href="#Ogl_GL_CLIP_PLANE3">GL_CLIP_PLANE3</a></li>
<li><a href="#Ogl_GL_CLIP_PLANE4">GL_CLIP_PLANE4</a></li>
<li><a href="#Ogl_GL_CLIP_PLANE5">GL_CLIP_PLANE5</a></li>
<li><a href="#Ogl_GL_COEFF">GL_COEFF</a></li>
<li><a href="#Ogl_GL_COLOR">GL_COLOR</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY">GL_COLOR_ARRAY</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_BUFFER_BINDING">GL_COLOR_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_COUNT_EXT">GL_COLOR_ARRAY_COUNT_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_EXT">GL_COLOR_ARRAY_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_POINTER">GL_COLOR_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_POINTER_EXT">GL_COLOR_ARRAY_POINTER_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_SIZE">GL_COLOR_ARRAY_SIZE</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_SIZE_EXT">GL_COLOR_ARRAY_SIZE_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_STRIDE">GL_COLOR_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_STRIDE_EXT">GL_COLOR_ARRAY_STRIDE_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_TYPE">GL_COLOR_ARRAY_TYPE</a></li>
<li><a href="#Ogl_GL_COLOR_ARRAY_TYPE_EXT">GL_COLOR_ARRAY_TYPE_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT0">GL_COLOR_ATTACHMENT0</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT1">GL_COLOR_ATTACHMENT1</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT10">GL_COLOR_ATTACHMENT10</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT11">GL_COLOR_ATTACHMENT11</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT12">GL_COLOR_ATTACHMENT12</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT13">GL_COLOR_ATTACHMENT13</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT14">GL_COLOR_ATTACHMENT14</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT15">GL_COLOR_ATTACHMENT15</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT2">GL_COLOR_ATTACHMENT2</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT3">GL_COLOR_ATTACHMENT3</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT4">GL_COLOR_ATTACHMENT4</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT5">GL_COLOR_ATTACHMENT5</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT6">GL_COLOR_ATTACHMENT6</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT7">GL_COLOR_ATTACHMENT7</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT8">GL_COLOR_ATTACHMENT8</a></li>
<li><a href="#Ogl_GL_COLOR_ATTACHMENT9">GL_COLOR_ATTACHMENT9</a></li>
<li><a href="#Ogl_GL_COLOR_BUFFER_BIT">GL_COLOR_BUFFER_BIT</a></li>
<li><a href="#Ogl_GL_COLOR_CLEAR_VALUE">GL_COLOR_CLEAR_VALUE</a></li>
<li><a href="#Ogl_GL_COLOR_INDEX">GL_COLOR_INDEX</a></li>
<li><a href="#Ogl_GL_COLOR_INDEX12_EXT">GL_COLOR_INDEX12_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_INDEX16_EXT">GL_COLOR_INDEX16_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_INDEX1_EXT">GL_COLOR_INDEX1_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_INDEX2_EXT">GL_COLOR_INDEX2_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_INDEX4_EXT">GL_COLOR_INDEX4_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_INDEX8_EXT">GL_COLOR_INDEX8_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_INDEXES">GL_COLOR_INDEXES</a></li>
<li><a href="#Ogl_GL_COLOR_LOGIC_OP">GL_COLOR_LOGIC_OP</a></li>
<li><a href="#Ogl_GL_COLOR_MATERIAL">GL_COLOR_MATERIAL</a></li>
<li><a href="#Ogl_GL_COLOR_MATERIAL_FACE">GL_COLOR_MATERIAL_FACE</a></li>
<li><a href="#Ogl_GL_COLOR_MATERIAL_PARAMETER">GL_COLOR_MATERIAL_PARAMETER</a></li>
<li><a href="#Ogl_GL_COLOR_MATRIX">GL_COLOR_MATRIX</a></li>
<li><a href="#Ogl_GL_COLOR_MATRIX_STACK_DEPTH">GL_COLOR_MATRIX_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_COLOR_SUM">GL_COLOR_SUM</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE">GL_COLOR_TABLE</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_ALPHA_SIZE">GL_COLOR_TABLE_ALPHA_SIZE</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_ALPHA_SIZE_EXT">GL_COLOR_TABLE_ALPHA_SIZE_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_BIAS">GL_COLOR_TABLE_BIAS</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_BLUE_SIZE">GL_COLOR_TABLE_BLUE_SIZE</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_BLUE_SIZE_EXT">GL_COLOR_TABLE_BLUE_SIZE_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_FORMAT">GL_COLOR_TABLE_FORMAT</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_FORMAT_EXT">GL_COLOR_TABLE_FORMAT_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_GREEN_SIZE">GL_COLOR_TABLE_GREEN_SIZE</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_GREEN_SIZE_EXT">GL_COLOR_TABLE_GREEN_SIZE_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_INTENSITY_SIZE">GL_COLOR_TABLE_INTENSITY_SIZE</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_INTENSITY_SIZE_EXT">GL_COLOR_TABLE_INTENSITY_SIZE_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_LUMINANCE_SIZE">GL_COLOR_TABLE_LUMINANCE_SIZE</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_LUMINANCE_SIZE_EXT">GL_COLOR_TABLE_LUMINANCE_SIZE_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_RED_SIZE">GL_COLOR_TABLE_RED_SIZE</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_RED_SIZE_EXT">GL_COLOR_TABLE_RED_SIZE_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_SCALE">GL_COLOR_TABLE_SCALE</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_WIDTH">GL_COLOR_TABLE_WIDTH</a></li>
<li><a href="#Ogl_GL_COLOR_TABLE_WIDTH_EXT">GL_COLOR_TABLE_WIDTH_EXT</a></li>
<li><a href="#Ogl_GL_COLOR_WRITEMASK">GL_COLOR_WRITEMASK</a></li>
<li><a href="#Ogl_GL_COMBINE">GL_COMBINE</a></li>
<li><a href="#Ogl_GL_COMBINE_ALPHA">GL_COMBINE_ALPHA</a></li>
<li><a href="#Ogl_GL_COMBINE_RGB">GL_COMBINE_RGB</a></li>
<li><a href="#Ogl_GL_COMPARE_REF_TO_TEXTURE">GL_COMPARE_REF_TO_TEXTURE</a></li>
<li><a href="#Ogl_GL_COMPARE_R_TO_TEXTURE">GL_COMPARE_R_TO_TEXTURE</a></li>
<li><a href="#Ogl_GL_COMPARE_R_TO_TEXTURE_ARB">GL_COMPARE_R_TO_TEXTURE_ARB</a></li>
<li><a href="#Ogl_GL_COMPILE">GL_COMPILE</a></li>
<li><a href="#Ogl_GL_COMPILE_AND_EXECUTE">GL_COMPILE_AND_EXECUTE</a></li>
<li><a href="#Ogl_GL_COMPILE_STATUS">GL_COMPILE_STATUS</a></li>
<li><a href="#Ogl_GL_COMPRESSED_ALPHA">GL_COMPRESSED_ALPHA</a></li>
<li><a href="#Ogl_GL_COMPRESSED_INTENSITY">GL_COMPRESSED_INTENSITY</a></li>
<li><a href="#Ogl_GL_COMPRESSED_LUMINANCE">GL_COMPRESSED_LUMINANCE</a></li>
<li><a href="#Ogl_GL_COMPRESSED_LUMINANCE_ALPHA">GL_COMPRESSED_LUMINANCE_ALPHA</a></li>
<li><a href="#Ogl_GL_COMPRESSED_RGB">GL_COMPRESSED_RGB</a></li>
<li><a href="#Ogl_GL_COMPRESSED_RGBA">GL_COMPRESSED_RGBA</a></li>
<li><a href="#Ogl_GL_COMPRESSED_RGBA_BPTC_UNORM">GL_COMPRESSED_RGBA_BPTC_UNORM</a></li>
<li><a href="#Ogl_GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT">GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT</a></li>
<li><a href="#Ogl_GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT">GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT</a></li>
<li><a href="#Ogl_GL_COMPRESSED_SLUMINANCE">GL_COMPRESSED_SLUMINANCE</a></li>
<li><a href="#Ogl_GL_COMPRESSED_SLUMINANCE_ALPHA">GL_COMPRESSED_SLUMINANCE_ALPHA</a></li>
<li><a href="#Ogl_GL_COMPRESSED_SRGB">GL_COMPRESSED_SRGB</a></li>
<li><a href="#Ogl_GL_COMPRESSED_SRGB_ALPHA">GL_COMPRESSED_SRGB_ALPHA</a></li>
<li><a href="#Ogl_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM">GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM</a></li>
<li><a href="#Ogl_GL_COMPRESSED_TEXTURE_FORMATS">GL_COMPRESSED_TEXTURE_FORMATS</a></li>
<li><a href="#Ogl_GL_COMPUTE_SHADER_INVOCATIONS">GL_COMPUTE_SHADER_INVOCATIONS</a></li>
<li><a href="#Ogl_GL_CONSTANT">GL_CONSTANT</a></li>
<li><a href="#Ogl_GL_CONSTANT_ALPHA">GL_CONSTANT_ALPHA</a></li>
<li><a href="#Ogl_GL_CONSTANT_ATTENUATION">GL_CONSTANT_ATTENUATION</a></li>
<li><a href="#Ogl_GL_CONSTANT_BORDER">GL_CONSTANT_BORDER</a></li>
<li><a href="#Ogl_GL_CONSTANT_COLOR">GL_CONSTANT_COLOR</a></li>
<li><a href="#Ogl_GL_CONTEXT_COMPATIBILITY_PROFILE_BIT">GL_CONTEXT_COMPATIBILITY_PROFILE_BIT</a></li>
<li><a href="#Ogl_GL_CONTEXT_CORE_PROFILE_BIT">GL_CONTEXT_CORE_PROFILE_BIT</a></li>
<li><a href="#Ogl_GL_CONTEXT_FLAGS">GL_CONTEXT_FLAGS</a></li>
<li><a href="#Ogl_GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT">GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT</a></li>
<li><a href="#Ogl_GL_CONTEXT_FLAG_NO_ERROR_BIT">GL_CONTEXT_FLAG_NO_ERROR_BIT</a></li>
<li><a href="#Ogl_GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT">GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT</a></li>
<li><a href="#Ogl_GL_CONTEXT_PROFILE_MASK">GL_CONTEXT_PROFILE_MASK</a></li>
<li><a href="#Ogl_GL_CONVOLUTION_1D">GL_CONVOLUTION_1D</a></li>
<li><a href="#Ogl_GL_CONVOLUTION_2D">GL_CONVOLUTION_2D</a></li>
<li><a href="#Ogl_GL_CONVOLUTION_BORDER_COLOR">GL_CONVOLUTION_BORDER_COLOR</a></li>
<li><a href="#Ogl_GL_CONVOLUTION_BORDER_MODE">GL_CONVOLUTION_BORDER_MODE</a></li>
<li><a href="#Ogl_GL_CONVOLUTION_FILTER_BIAS">GL_CONVOLUTION_FILTER_BIAS</a></li>
<li><a href="#Ogl_GL_CONVOLUTION_FILTER_SCALE">GL_CONVOLUTION_FILTER_SCALE</a></li>
<li><a href="#Ogl_GL_CONVOLUTION_FORMAT">GL_CONVOLUTION_FORMAT</a></li>
<li><a href="#Ogl_GL_CONVOLUTION_HEIGHT">GL_CONVOLUTION_HEIGHT</a></li>
<li><a href="#Ogl_GL_CONVOLUTION_WIDTH">GL_CONVOLUTION_WIDTH</a></li>
<li><a href="#Ogl_GL_COORD_REPLACE">GL_COORD_REPLACE</a></li>
<li><a href="#Ogl_GL_COPY">GL_COPY</a></li>
<li><a href="#Ogl_GL_COPY_INVERTED">GL_COPY_INVERTED</a></li>
<li><a href="#Ogl_GL_COPY_PIXEL_TOKEN">GL_COPY_PIXEL_TOKEN</a></li>
<li><a href="#Ogl_GL_COPY_READ_BUFFER_BINDING">GL_COPY_READ_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_COPY_WRITE_BUFFER_BINDING">GL_COPY_WRITE_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_CULL_FACE">GL_CULL_FACE</a></li>
<li><a href="#Ogl_GL_CULL_FACE_MODE">GL_CULL_FACE_MODE</a></li>
<li><a href="#Ogl_GL_CURRENT_BIT">GL_CURRENT_BIT</a></li>
<li><a href="#Ogl_GL_CURRENT_COLOR">GL_CURRENT_COLOR</a></li>
<li><a href="#Ogl_GL_CURRENT_FOG_COORD">GL_CURRENT_FOG_COORD</a></li>
<li><a href="#Ogl_GL_CURRENT_FOG_COORDINATE">GL_CURRENT_FOG_COORDINATE</a></li>
<li><a href="#Ogl_GL_CURRENT_INDEX">GL_CURRENT_INDEX</a></li>
<li><a href="#Ogl_GL_CURRENT_NORMAL">GL_CURRENT_NORMAL</a></li>
<li><a href="#Ogl_GL_CURRENT_PROGRAM">GL_CURRENT_PROGRAM</a></li>
<li><a href="#Ogl_GL_CURRENT_QUERY">GL_CURRENT_QUERY</a></li>
<li><a href="#Ogl_GL_CURRENT_RASTER_COLOR">GL_CURRENT_RASTER_COLOR</a></li>
<li><a href="#Ogl_GL_CURRENT_RASTER_DISTANCE">GL_CURRENT_RASTER_DISTANCE</a></li>
<li><a href="#Ogl_GL_CURRENT_RASTER_INDEX">GL_CURRENT_RASTER_INDEX</a></li>
<li><a href="#Ogl_GL_CURRENT_RASTER_POSITION">GL_CURRENT_RASTER_POSITION</a></li>
<li><a href="#Ogl_GL_CURRENT_RASTER_POSITION_VALID">GL_CURRENT_RASTER_POSITION_VALID</a></li>
<li><a href="#Ogl_GL_CURRENT_RASTER_SECONDARY_COLOR">GL_CURRENT_RASTER_SECONDARY_COLOR</a></li>
<li><a href="#Ogl_GL_CURRENT_RASTER_TEXTURE_COORDS">GL_CURRENT_RASTER_TEXTURE_COORDS</a></li>
<li><a href="#Ogl_GL_CURRENT_SECONDARY_COLOR">GL_CURRENT_SECONDARY_COLOR</a></li>
<li><a href="#Ogl_GL_CURRENT_TEXTURE_COORDS">GL_CURRENT_TEXTURE_COORDS</a></li>
<li><a href="#Ogl_GL_CURRENT_VERTEX_ATTRIB">GL_CURRENT_VERTEX_ATTRIB</a></li>
<li><a href="#Ogl_GL_CW">GL_CW</a></li>
<li><a href="#Ogl_GL_DECAL">GL_DECAL</a></li>
<li><a href="#Ogl_GL_DECR">GL_DECR</a></li>
<li><a href="#Ogl_GL_DECR_WRAP">GL_DECR_WRAP</a></li>
<li><a href="#Ogl_GL_DELETE_STATUS">GL_DELETE_STATUS</a></li>
<li><a href="#Ogl_GL_DEPTH">GL_DEPTH</a></li>
<li><a href="#Ogl_GL_DEPTH24_STENCIL8">GL_DEPTH24_STENCIL8</a></li>
<li><a href="#Ogl_GL_DEPTH_ATTACHMENT">GL_DEPTH_ATTACHMENT</a></li>
<li><a href="#Ogl_GL_DEPTH_BIAS">GL_DEPTH_BIAS</a></li>
<li><a href="#Ogl_GL_DEPTH_BITS">GL_DEPTH_BITS</a></li>
<li><a href="#Ogl_GL_DEPTH_BUFFER">GL_DEPTH_BUFFER</a></li>
<li><a href="#Ogl_GL_DEPTH_BUFFER_BIT">GL_DEPTH_BUFFER_BIT</a></li>
<li><a href="#Ogl_GL_DEPTH_CLEAR_VALUE">GL_DEPTH_CLEAR_VALUE</a></li>
<li><a href="#Ogl_GL_DEPTH_COMPONENT">GL_DEPTH_COMPONENT</a></li>
<li><a href="#Ogl_GL_DEPTH_COMPONENT16">GL_DEPTH_COMPONENT16</a></li>
<li><a href="#Ogl_GL_DEPTH_COMPONENT24">GL_DEPTH_COMPONENT24</a></li>
<li><a href="#Ogl_GL_DEPTH_COMPONENT32">GL_DEPTH_COMPONENT32</a></li>
<li><a href="#Ogl_GL_DEPTH_FUNC">GL_DEPTH_FUNC</a></li>
<li><a href="#Ogl_GL_DEPTH_RANGE">GL_DEPTH_RANGE</a></li>
<li><a href="#Ogl_GL_DEPTH_SCALE">GL_DEPTH_SCALE</a></li>
<li><a href="#Ogl_GL_DEPTH_STENCIL">GL_DEPTH_STENCIL</a></li>
<li><a href="#Ogl_GL_DEPTH_STENCIL_ATTACHMENT">GL_DEPTH_STENCIL_ATTACHMENT</a></li>
<li><a href="#Ogl_GL_DEPTH_TEST">GL_DEPTH_TEST</a></li>
<li><a href="#Ogl_GL_DEPTH_TEXTURE_MODE">GL_DEPTH_TEXTURE_MODE</a></li>
<li><a href="#Ogl_GL_DEPTH_WRITEMASK">GL_DEPTH_WRITEMASK</a></li>
<li><a href="#Ogl_GL_DIFFUSE">GL_DIFFUSE</a></li>
<li><a href="#Ogl_GL_DITHER">GL_DITHER</a></li>
<li><a href="#Ogl_GL_DOMAIN">GL_DOMAIN</a></li>
<li><a href="#Ogl_GL_DONT_CARE">GL_DONT_CARE</a></li>
<li><a href="#Ogl_GL_DOT3_RGB">GL_DOT3_RGB</a></li>
<li><a href="#Ogl_GL_DOT3_RGBA">GL_DOT3_RGBA</a></li>
<li><a href="#Ogl_GL_DOUBLE">GL_DOUBLE</a></li>
<li><a href="#Ogl_GL_DOUBLEBUFFER">GL_DOUBLEBUFFER</a></li>
<li><a href="#Ogl_GL_DOUBLE_EXT">GL_DOUBLE_EXT</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER">GL_DRAW_BUFFER</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER0">GL_DRAW_BUFFER0</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER1">GL_DRAW_BUFFER1</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER10">GL_DRAW_BUFFER10</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER11">GL_DRAW_BUFFER11</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER12">GL_DRAW_BUFFER12</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER13">GL_DRAW_BUFFER13</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER14">GL_DRAW_BUFFER14</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER15">GL_DRAW_BUFFER15</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER2">GL_DRAW_BUFFER2</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER3">GL_DRAW_BUFFER3</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER4">GL_DRAW_BUFFER4</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER5">GL_DRAW_BUFFER5</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER6">GL_DRAW_BUFFER6</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER7">GL_DRAW_BUFFER7</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER8">GL_DRAW_BUFFER8</a></li>
<li><a href="#Ogl_GL_DRAW_BUFFER9">GL_DRAW_BUFFER9</a></li>
<li><a href="#Ogl_GL_DRAW_FRAMEBUFFER">GL_DRAW_FRAMEBUFFER</a></li>
<li><a href="#Ogl_GL_DRAW_FRAMEBUFFER_BINDING">GL_DRAW_FRAMEBUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_DRAW_PIXEL_TOKEN">GL_DRAW_PIXEL_TOKEN</a></li>
<li><a href="#Ogl_GL_DST_ALPHA">GL_DST_ALPHA</a></li>
<li><a href="#Ogl_GL_DST_COLOR">GL_DST_COLOR</a></li>
<li><a href="#Ogl_GL_DYNAMIC_COPY">GL_DYNAMIC_COPY</a></li>
<li><a href="#Ogl_GL_DYNAMIC_DRAW">GL_DYNAMIC_DRAW</a></li>
<li><a href="#Ogl_GL_DYNAMIC_READ">GL_DYNAMIC_READ</a></li>
<li><a href="#Ogl_GL_EDGE_FLAG">GL_EDGE_FLAG</a></li>
<li><a href="#Ogl_GL_EDGE_FLAG_ARRAY">GL_EDGE_FLAG_ARRAY</a></li>
<li><a href="#Ogl_GL_EDGE_FLAG_ARRAY_BUFFER_BINDING">GL_EDGE_FLAG_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_EDGE_FLAG_ARRAY_COUNT_EXT">GL_EDGE_FLAG_ARRAY_COUNT_EXT</a></li>
<li><a href="#Ogl_GL_EDGE_FLAG_ARRAY_EXT">GL_EDGE_FLAG_ARRAY_EXT</a></li>
<li><a href="#Ogl_GL_EDGE_FLAG_ARRAY_POINTER">GL_EDGE_FLAG_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_EDGE_FLAG_ARRAY_POINTER_EXT">GL_EDGE_FLAG_ARRAY_POINTER_EXT</a></li>
<li><a href="#Ogl_GL_EDGE_FLAG_ARRAY_STRIDE">GL_EDGE_FLAG_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_EDGE_FLAG_ARRAY_STRIDE_EXT">GL_EDGE_FLAG_ARRAY_STRIDE_EXT</a></li>
<li><a href="#Ogl_GL_ELEMENT_ARRAY_BUFFER">GL_ELEMENT_ARRAY_BUFFER</a></li>
<li><a href="#Ogl_GL_ELEMENT_ARRAY_BUFFER_BINDING">GL_ELEMENT_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_EMISSION">GL_EMISSION</a></li>
<li><a href="#Ogl_GL_ENABLE_BIT">GL_ENABLE_BIT</a></li>
<li><a href="#Ogl_GL_EQUAL">GL_EQUAL</a></li>
<li><a href="#Ogl_GL_EQUIV">GL_EQUIV</a></li>
<li><a href="#Ogl_GL_EVAL_BIT">GL_EVAL_BIT</a></li>
<li><a href="#Ogl_GL_EXP">GL_EXP</a></li>
<li><a href="#Ogl_GL_EXP2">GL_EXP2</a></li>
<li><a href="#Ogl_GL_EXTENSIONS">GL_EXTENSIONS</a></li>
<li><a href="#Ogl_GL_EXT_bgra">GL_EXT_bgra</a></li>
<li><a href="#Ogl_GL_EXT_paletted_texture">GL_EXT_paletted_texture</a></li>
<li><a href="#Ogl_GL_EXT_vertex_array">GL_EXT_vertex_array</a></li>
<li><a href="#Ogl_GL_EYE_LINEAR">GL_EYE_LINEAR</a></li>
<li><a href="#Ogl_GL_EYE_PLANE">GL_EYE_PLANE</a></li>
<li><a href="#Ogl_GL_FALSE">GL_FALSE</a></li>
<li><a href="#Ogl_GL_FASTEST">GL_FASTEST</a></li>
<li><a href="#Ogl_GL_FEEDBACK">GL_FEEDBACK</a></li>
<li><a href="#Ogl_GL_FEEDBACK_BUFFER_POINTER">GL_FEEDBACK_BUFFER_POINTER</a></li>
<li><a href="#Ogl_GL_FEEDBACK_BUFFER_SIZE">GL_FEEDBACK_BUFFER_SIZE</a></li>
<li><a href="#Ogl_GL_FEEDBACK_BUFFER_TYPE">GL_FEEDBACK_BUFFER_TYPE</a></li>
<li><a href="#Ogl_GL_FILL">GL_FILL</a></li>
<li><a href="#Ogl_GL_FIXED_ONLY">GL_FIXED_ONLY</a></li>
<li><a href="#Ogl_GL_FLAT">GL_FLAT</a></li>
<li><a href="#Ogl_GL_FLOAT">GL_FLOAT</a></li>
<li><a href="#Ogl_GL_FLOAT_MAT2">GL_FLOAT_MAT2</a></li>
<li><a href="#Ogl_GL_FLOAT_MAT2x3">GL_FLOAT_MAT2x3</a></li>
<li><a href="#Ogl_GL_FLOAT_MAT2x4">GL_FLOAT_MAT2x4</a></li>
<li><a href="#Ogl_GL_FLOAT_MAT3">GL_FLOAT_MAT3</a></li>
<li><a href="#Ogl_GL_FLOAT_MAT3x2">GL_FLOAT_MAT3x2</a></li>
<li><a href="#Ogl_GL_FLOAT_MAT3x4">GL_FLOAT_MAT3x4</a></li>
<li><a href="#Ogl_GL_FLOAT_MAT4">GL_FLOAT_MAT4</a></li>
<li><a href="#Ogl_GL_FLOAT_MAT4x2">GL_FLOAT_MAT4x2</a></li>
<li><a href="#Ogl_GL_FLOAT_MAT4x3">GL_FLOAT_MAT4x3</a></li>
<li><a href="#Ogl_GL_FLOAT_VEC2">GL_FLOAT_VEC2</a></li>
<li><a href="#Ogl_GL_FLOAT_VEC3">GL_FLOAT_VEC3</a></li>
<li><a href="#Ogl_GL_FLOAT_VEC4">GL_FLOAT_VEC4</a></li>
<li><a href="#Ogl_GL_FOG">GL_FOG</a></li>
<li><a href="#Ogl_GL_FOG_BIT">GL_FOG_BIT</a></li>
<li><a href="#Ogl_GL_FOG_COLOR">GL_FOG_COLOR</a></li>
<li><a href="#Ogl_GL_FOG_COORD">GL_FOG_COORD</a></li>
<li><a href="#Ogl_GL_FOG_COORDINATE">GL_FOG_COORDINATE</a></li>
<li><a href="#Ogl_GL_FOG_COORDINATE_ARRAY">GL_FOG_COORDINATE_ARRAY</a></li>
<li><a href="#Ogl_GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING">GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_FOG_COORDINATE_ARRAY_POINTER">GL_FOG_COORDINATE_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_FOG_COORDINATE_ARRAY_STRIDE">GL_FOG_COORDINATE_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_FOG_COORDINATE_ARRAY_TYPE">GL_FOG_COORDINATE_ARRAY_TYPE</a></li>
<li><a href="#Ogl_GL_FOG_COORDINATE_SOURCE">GL_FOG_COORDINATE_SOURCE</a></li>
<li><a href="#Ogl_GL_FOG_COORD_ARRAY">GL_FOG_COORD_ARRAY</a></li>
<li><a href="#Ogl_GL_FOG_COORD_ARRAY_BUFFER_BINDING">GL_FOG_COORD_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_FOG_COORD_ARRAY_POINTER">GL_FOG_COORD_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_FOG_COORD_ARRAY_STRIDE">GL_FOG_COORD_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_FOG_COORD_ARRAY_TYPE">GL_FOG_COORD_ARRAY_TYPE</a></li>
<li><a href="#Ogl_GL_FOG_COORD_SRC">GL_FOG_COORD_SRC</a></li>
<li><a href="#Ogl_GL_FOG_DENSITY">GL_FOG_DENSITY</a></li>
<li><a href="#Ogl_GL_FOG_END">GL_FOG_END</a></li>
<li><a href="#Ogl_GL_FOG_HINT">GL_FOG_HINT</a></li>
<li><a href="#Ogl_GL_FOG_INDEX">GL_FOG_INDEX</a></li>
<li><a href="#Ogl_GL_FOG_MODE">GL_FOG_MODE</a></li>
<li><a href="#Ogl_GL_FOG_SPECULAR_TEXTURE_WIN">GL_FOG_SPECULAR_TEXTURE_WIN</a></li>
<li><a href="#Ogl_GL_FOG_START">GL_FOG_START</a></li>
<li><a href="#Ogl_GL_FRAGMENT_DEPTH">GL_FRAGMENT_DEPTH</a></li>
<li><a href="#Ogl_GL_FRAGMENT_SHADER">GL_FRAGMENT_SHADER</a></li>
<li><a href="#Ogl_GL_FRAGMENT_SHADER_DERIVATIVE_HINT">GL_FRAGMENT_SHADER_DERIVATIVE_HINT</a></li>
<li><a href="#Ogl_GL_FRAGMENT_SHADER_INVOCATIONS">GL_FRAGMENT_SHADER_INVOCATIONS</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER">GL_FRAMEBUFFER</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE">GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE">GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING">GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE">GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE">GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE">GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_LAYERED">GL_FRAMEBUFFER_ATTACHMENT_LAYERED</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME">GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE">GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE">GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE">GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE">GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER">GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL">GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_BINDING">GL_FRAMEBUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_COMPLETE">GL_FRAMEBUFFER_COMPLETE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_DEFAULT">GL_FRAMEBUFFER_DEFAULT</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS">GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_DEFAULT_HEIGHT">GL_FRAMEBUFFER_DEFAULT_HEIGHT</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_DEFAULT_LAYERS">GL_FRAMEBUFFER_DEFAULT_LAYERS</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_DEFAULT_SAMPLES">GL_FRAMEBUFFER_DEFAULT_SAMPLES</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_DEFAULT_WIDTH">GL_FRAMEBUFFER_DEFAULT_WIDTH</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT">GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER">GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS">GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT">GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE">GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER">GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_SRGB">GL_FRAMEBUFFER_SRGB</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_UNDEFINED">GL_FRAMEBUFFER_UNDEFINED</a></li>
<li><a href="#Ogl_GL_FRAMEBUFFER_UNSUPPORTED">GL_FRAMEBUFFER_UNSUPPORTED</a></li>
<li><a href="#Ogl_GL_FRONT">GL_FRONT</a></li>
<li><a href="#Ogl_GL_FRONT_AND_BACK">GL_FRONT_AND_BACK</a></li>
<li><a href="#Ogl_GL_FRONT_FACE">GL_FRONT_FACE</a></li>
<li><a href="#Ogl_GL_FRONT_LEFT">GL_FRONT_LEFT</a></li>
<li><a href="#Ogl_GL_FRONT_RIGHT">GL_FRONT_RIGHT</a></li>
<li><a href="#Ogl_GL_FUNC_ADD">GL_FUNC_ADD</a></li>
<li><a href="#Ogl_GL_FUNC_REVERSE_SUBTRACT">GL_FUNC_REVERSE_SUBTRACT</a></li>
<li><a href="#Ogl_GL_FUNC_SUBTRACT">GL_FUNC_SUBTRACT</a></li>
<li><a href="#Ogl_GL_GENERATE_MIPMAP">GL_GENERATE_MIPMAP</a></li>
<li><a href="#Ogl_GL_GENERATE_MIPMAP_HINT">GL_GENERATE_MIPMAP_HINT</a></li>
<li><a href="#Ogl_GL_GEOMETRY_INPUT_TYPE">GL_GEOMETRY_INPUT_TYPE</a></li>
<li><a href="#Ogl_GL_GEOMETRY_OUTPUT_TYPE">GL_GEOMETRY_OUTPUT_TYPE</a></li>
<li><a href="#Ogl_GL_GEOMETRY_SHADER">GL_GEOMETRY_SHADER</a></li>
<li><a href="#Ogl_GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED">GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED</a></li>
<li><a href="#Ogl_GL_GEOMETRY_VERTICES_OUT">GL_GEOMETRY_VERTICES_OUT</a></li>
<li><a href="#Ogl_GL_GEQUAL">GL_GEQUAL</a></li>
<li><a href="#Ogl_GL_GREATER">GL_GREATER</a></li>
<li><a href="#Ogl_GL_GREEN">GL_GREEN</a></li>
<li><a href="#Ogl_GL_GREEN_BIAS">GL_GREEN_BIAS</a></li>
<li><a href="#Ogl_GL_GREEN_BITS">GL_GREEN_BITS</a></li>
<li><a href="#Ogl_GL_GREEN_INTEGER">GL_GREEN_INTEGER</a></li>
<li><a href="#Ogl_GL_GREEN_SCALE">GL_GREEN_SCALE</a></li>
<li><a href="#Ogl_GL_HINT_BIT">GL_HINT_BIT</a></li>
<li><a href="#Ogl_GL_HISTOGRAM">GL_HISTOGRAM</a></li>
<li><a href="#Ogl_GL_HISTOGRAM_ALPHA_SIZE">GL_HISTOGRAM_ALPHA_SIZE</a></li>
<li><a href="#Ogl_GL_HISTOGRAM_BLUE_SIZE">GL_HISTOGRAM_BLUE_SIZE</a></li>
<li><a href="#Ogl_GL_HISTOGRAM_FORMAT">GL_HISTOGRAM_FORMAT</a></li>
<li><a href="#Ogl_GL_HISTOGRAM_GREEN_SIZE">GL_HISTOGRAM_GREEN_SIZE</a></li>
<li><a href="#Ogl_GL_HISTOGRAM_LUMINANCE_SIZE">GL_HISTOGRAM_LUMINANCE_SIZE</a></li>
<li><a href="#Ogl_GL_HISTOGRAM_RED_SIZE">GL_HISTOGRAM_RED_SIZE</a></li>
<li><a href="#Ogl_GL_HISTOGRAM_SINK">GL_HISTOGRAM_SINK</a></li>
<li><a href="#Ogl_GL_HISTOGRAM_WIDTH">GL_HISTOGRAM_WIDTH</a></li>
<li><a href="#Ogl_GL_IGNORE_BORDER">GL_IGNORE_BORDER</a></li>
<li><a href="#Ogl_GL_INCR">GL_INCR</a></li>
<li><a href="#Ogl_GL_INCR_WRAP">GL_INCR_WRAP</a></li>
<li><a href="#Ogl_GL_INDEX">GL_INDEX</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY">GL_INDEX_ARRAY</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY_BUFFER_BINDING">GL_INDEX_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY_COUNT_EXT">GL_INDEX_ARRAY_COUNT_EXT</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY_EXT">GL_INDEX_ARRAY_EXT</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY_POINTER">GL_INDEX_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY_POINTER_EXT">GL_INDEX_ARRAY_POINTER_EXT</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY_STRIDE">GL_INDEX_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY_STRIDE_EXT">GL_INDEX_ARRAY_STRIDE_EXT</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY_TYPE">GL_INDEX_ARRAY_TYPE</a></li>
<li><a href="#Ogl_GL_INDEX_ARRAY_TYPE_EXT">GL_INDEX_ARRAY_TYPE_EXT</a></li>
<li><a href="#Ogl_GL_INDEX_BITS">GL_INDEX_BITS</a></li>
<li><a href="#Ogl_GL_INDEX_CLEAR_VALUE">GL_INDEX_CLEAR_VALUE</a></li>
<li><a href="#Ogl_GL_INDEX_LOGIC_OP">GL_INDEX_LOGIC_OP</a></li>
<li><a href="#Ogl_GL_INDEX_MODE">GL_INDEX_MODE</a></li>
<li><a href="#Ogl_GL_INDEX_OFFSET">GL_INDEX_OFFSET</a></li>
<li><a href="#Ogl_GL_INDEX_SHIFT">GL_INDEX_SHIFT</a></li>
<li><a href="#Ogl_GL_INDEX_WRITEMASK">GL_INDEX_WRITEMASK</a></li>
<li><a href="#Ogl_GL_INFO_LOG_LENGTH">GL_INFO_LOG_LENGTH</a></li>
<li><a href="#Ogl_GL_INT">GL_INT</a></li>
<li><a href="#Ogl_GL_INTENSITY">GL_INTENSITY</a></li>
<li><a href="#Ogl_GL_INTENSITY12">GL_INTENSITY12</a></li>
<li><a href="#Ogl_GL_INTENSITY16">GL_INTENSITY16</a></li>
<li><a href="#Ogl_GL_INTENSITY4">GL_INTENSITY4</a></li>
<li><a href="#Ogl_GL_INTENSITY8">GL_INTENSITY8</a></li>
<li><a href="#Ogl_GL_INTERLEAVED_ATTRIBS">GL_INTERLEAVED_ATTRIBS</a></li>
<li><a href="#Ogl_GL_INTERPOLATE">GL_INTERPOLATE</a></li>
<li><a href="#Ogl_GL_INT_SAMPLER_1D">GL_INT_SAMPLER_1D</a></li>
<li><a href="#Ogl_GL_INT_SAMPLER_1D_ARRAY">GL_INT_SAMPLER_1D_ARRAY</a></li>
<li><a href="#Ogl_GL_INT_SAMPLER_2D">GL_INT_SAMPLER_2D</a></li>
<li><a href="#Ogl_GL_INT_SAMPLER_2D_ARRAY">GL_INT_SAMPLER_2D_ARRAY</a></li>
<li><a href="#Ogl_GL_INT_SAMPLER_2D_RECT">GL_INT_SAMPLER_2D_RECT</a></li>
<li><a href="#Ogl_GL_INT_SAMPLER_3D">GL_INT_SAMPLER_3D</a></li>
<li><a href="#Ogl_GL_INT_SAMPLER_BUFFER">GL_INT_SAMPLER_BUFFER</a></li>
<li><a href="#Ogl_GL_INT_SAMPLER_CUBE">GL_INT_SAMPLER_CUBE</a></li>
<li><a href="#Ogl_GL_INT_SAMPLER_CUBE_MAP_ARRAY">GL_INT_SAMPLER_CUBE_MAP_ARRAY</a></li>
<li><a href="#Ogl_GL_INT_VEC2">GL_INT_VEC2</a></li>
<li><a href="#Ogl_GL_INT_VEC3">GL_INT_VEC3</a></li>
<li><a href="#Ogl_GL_INT_VEC4">GL_INT_VEC4</a></li>
<li><a href="#Ogl_GL_INVALID_ENUM">GL_INVALID_ENUM</a></li>
<li><a href="#Ogl_GL_INVALID_FRAMEBUFFER_OPERATION">GL_INVALID_FRAMEBUFFER_OPERATION</a></li>
<li><a href="#Ogl_GL_INVALID_OPERATION">GL_INVALID_OPERATION</a></li>
<li><a href="#Ogl_GL_INVALID_VALUE">GL_INVALID_VALUE</a></li>
<li><a href="#Ogl_GL_INVERT">GL_INVERT</a></li>
<li><a href="#Ogl_GL_KEEP">GL_KEEP</a></li>
<li><a href="#Ogl_GL_LEFT">GL_LEFT</a></li>
<li><a href="#Ogl_GL_LEQUAL">GL_LEQUAL</a></li>
<li><a href="#Ogl_GL_LESS">GL_LESS</a></li>
<li><a href="#Ogl_GL_LIGHT0">GL_LIGHT0</a></li>
<li><a href="#Ogl_GL_LIGHT1">GL_LIGHT1</a></li>
<li><a href="#Ogl_GL_LIGHT2">GL_LIGHT2</a></li>
<li><a href="#Ogl_GL_LIGHT3">GL_LIGHT3</a></li>
<li><a href="#Ogl_GL_LIGHT4">GL_LIGHT4</a></li>
<li><a href="#Ogl_GL_LIGHT5">GL_LIGHT5</a></li>
<li><a href="#Ogl_GL_LIGHT6">GL_LIGHT6</a></li>
<li><a href="#Ogl_GL_LIGHT7">GL_LIGHT7</a></li>
<li><a href="#Ogl_GL_LIGHTING">GL_LIGHTING</a></li>
<li><a href="#Ogl_GL_LIGHTING_BIT">GL_LIGHTING_BIT</a></li>
<li><a href="#Ogl_GL_LIGHT_MODEL_AMBIENT">GL_LIGHT_MODEL_AMBIENT</a></li>
<li><a href="#Ogl_GL_LIGHT_MODEL_COLOR_CONTROL">GL_LIGHT_MODEL_COLOR_CONTROL</a></li>
<li><a href="#Ogl_GL_LIGHT_MODEL_LOCAL_VIEWER">GL_LIGHT_MODEL_LOCAL_VIEWER</a></li>
<li><a href="#Ogl_GL_LIGHT_MODEL_TWO_SIDE">GL_LIGHT_MODEL_TWO_SIDE</a></li>
<li><a href="#Ogl_GL_LINE">GL_LINE</a></li>
<li><a href="#Ogl_GL_LINEAR">GL_LINEAR</a></li>
<li><a href="#Ogl_GL_LINEAR_ATTENUATION">GL_LINEAR_ATTENUATION</a></li>
<li><a href="#Ogl_GL_LINEAR_MIPMAP_LINEAR">GL_LINEAR_MIPMAP_LINEAR</a></li>
<li><a href="#Ogl_GL_LINEAR_MIPMAP_NEAREST">GL_LINEAR_MIPMAP_NEAREST</a></li>
<li><a href="#Ogl_GL_LINES">GL_LINES</a></li>
<li><a href="#Ogl_GL_LINES_ADJACENCY">GL_LINES_ADJACENCY</a></li>
<li><a href="#Ogl_GL_LINE_BIT">GL_LINE_BIT</a></li>
<li><a href="#Ogl_GL_LINE_LOOP">GL_LINE_LOOP</a></li>
<li><a href="#Ogl_GL_LINE_RESET_TOKEN">GL_LINE_RESET_TOKEN</a></li>
<li><a href="#Ogl_GL_LINE_SMOOTH">GL_LINE_SMOOTH</a></li>
<li><a href="#Ogl_GL_LINE_SMOOTH_HINT">GL_LINE_SMOOTH_HINT</a></li>
<li><a href="#Ogl_GL_LINE_STIPPLE">GL_LINE_STIPPLE</a></li>
<li><a href="#Ogl_GL_LINE_STIPPLE_PATTERN">GL_LINE_STIPPLE_PATTERN</a></li>
<li><a href="#Ogl_GL_LINE_STIPPLE_REPEAT">GL_LINE_STIPPLE_REPEAT</a></li>
<li><a href="#Ogl_GL_LINE_STRIP">GL_LINE_STRIP</a></li>
<li><a href="#Ogl_GL_LINE_STRIP_ADJACENCY">GL_LINE_STRIP_ADJACENCY</a></li>
<li><a href="#Ogl_GL_LINE_TOKEN">GL_LINE_TOKEN</a></li>
<li><a href="#Ogl_GL_LINE_WIDTH">GL_LINE_WIDTH</a></li>
<li><a href="#Ogl_GL_LINE_WIDTH_GRANULARITY">GL_LINE_WIDTH_GRANULARITY</a></li>
<li><a href="#Ogl_GL_LINE_WIDTH_RANGE">GL_LINE_WIDTH_RANGE</a></li>
<li><a href="#Ogl_GL_LINK_STATUS">GL_LINK_STATUS</a></li>
<li><a href="#Ogl_GL_LIST_BASE">GL_LIST_BASE</a></li>
<li><a href="#Ogl_GL_LIST_BIT">GL_LIST_BIT</a></li>
<li><a href="#Ogl_GL_LIST_INDEX">GL_LIST_INDEX</a></li>
<li><a href="#Ogl_GL_LIST_MODE">GL_LIST_MODE</a></li>
<li><a href="#Ogl_GL_LOAD">GL_LOAD</a></li>
<li><a href="#Ogl_GL_LOGIC_OP">GL_LOGIC_OP</a></li>
<li><a href="#Ogl_GL_LOGIC_OP_MODE">GL_LOGIC_OP_MODE</a></li>
<li><a href="#Ogl_GL_LOWER_LEFT">GL_LOWER_LEFT</a></li>
<li><a href="#Ogl_GL_LUMINANCE">GL_LUMINANCE</a></li>
<li><a href="#Ogl_GL_LUMINANCE12">GL_LUMINANCE12</a></li>
<li><a href="#Ogl_GL_LUMINANCE12_ALPHA12">GL_LUMINANCE12_ALPHA12</a></li>
<li><a href="#Ogl_GL_LUMINANCE12_ALPHA4">GL_LUMINANCE12_ALPHA4</a></li>
<li><a href="#Ogl_GL_LUMINANCE16">GL_LUMINANCE16</a></li>
<li><a href="#Ogl_GL_LUMINANCE16_ALPHA16">GL_LUMINANCE16_ALPHA16</a></li>
<li><a href="#Ogl_GL_LUMINANCE4">GL_LUMINANCE4</a></li>
<li><a href="#Ogl_GL_LUMINANCE4_ALPHA4">GL_LUMINANCE4_ALPHA4</a></li>
<li><a href="#Ogl_GL_LUMINANCE6_ALPHA2">GL_LUMINANCE6_ALPHA2</a></li>
<li><a href="#Ogl_GL_LUMINANCE8">GL_LUMINANCE8</a></li>
<li><a href="#Ogl_GL_LUMINANCE8_ALPHA8">GL_LUMINANCE8_ALPHA8</a></li>
<li><a href="#Ogl_GL_LUMINANCE_ALPHA">GL_LUMINANCE_ALPHA</a></li>
<li><a href="#Ogl_GL_MAJOR_VERSION">GL_MAJOR_VERSION</a></li>
<li><a href="#Ogl_GL_MAP1_COLOR_4">GL_MAP1_COLOR_4</a></li>
<li><a href="#Ogl_GL_MAP1_GRID_DOMAIN">GL_MAP1_GRID_DOMAIN</a></li>
<li><a href="#Ogl_GL_MAP1_GRID_SEGMENTS">GL_MAP1_GRID_SEGMENTS</a></li>
<li><a href="#Ogl_GL_MAP1_INDEX">GL_MAP1_INDEX</a></li>
<li><a href="#Ogl_GL_MAP1_NORMAL">GL_MAP1_NORMAL</a></li>
<li><a href="#Ogl_GL_MAP1_TEXTURE_COORD_1">GL_MAP1_TEXTURE_COORD_1</a></li>
<li><a href="#Ogl_GL_MAP1_TEXTURE_COORD_2">GL_MAP1_TEXTURE_COORD_2</a></li>
<li><a href="#Ogl_GL_MAP1_TEXTURE_COORD_3">GL_MAP1_TEXTURE_COORD_3</a></li>
<li><a href="#Ogl_GL_MAP1_TEXTURE_COORD_4">GL_MAP1_TEXTURE_COORD_4</a></li>
<li><a href="#Ogl_GL_MAP1_VERTEX_3">GL_MAP1_VERTEX_3</a></li>
<li><a href="#Ogl_GL_MAP1_VERTEX_4">GL_MAP1_VERTEX_4</a></li>
<li><a href="#Ogl_GL_MAP2_COLOR_4">GL_MAP2_COLOR_4</a></li>
<li><a href="#Ogl_GL_MAP2_GRID_DOMAIN">GL_MAP2_GRID_DOMAIN</a></li>
<li><a href="#Ogl_GL_MAP2_GRID_SEGMENTS">GL_MAP2_GRID_SEGMENTS</a></li>
<li><a href="#Ogl_GL_MAP2_INDEX">GL_MAP2_INDEX</a></li>
<li><a href="#Ogl_GL_MAP2_NORMAL">GL_MAP2_NORMAL</a></li>
<li><a href="#Ogl_GL_MAP2_TEXTURE_COORD_1">GL_MAP2_TEXTURE_COORD_1</a></li>
<li><a href="#Ogl_GL_MAP2_TEXTURE_COORD_2">GL_MAP2_TEXTURE_COORD_2</a></li>
<li><a href="#Ogl_GL_MAP2_TEXTURE_COORD_3">GL_MAP2_TEXTURE_COORD_3</a></li>
<li><a href="#Ogl_GL_MAP2_TEXTURE_COORD_4">GL_MAP2_TEXTURE_COORD_4</a></li>
<li><a href="#Ogl_GL_MAP2_VERTEX_3">GL_MAP2_VERTEX_3</a></li>
<li><a href="#Ogl_GL_MAP2_VERTEX_4">GL_MAP2_VERTEX_4</a></li>
<li><a href="#Ogl_GL_MAP_COLOR">GL_MAP_COLOR</a></li>
<li><a href="#Ogl_GL_MAP_STENCIL">GL_MAP_STENCIL</a></li>
<li><a href="#Ogl_GL_MATRIX_MODE">GL_MATRIX_MODE</a></li>
<li><a href="#Ogl_GL_MAX">GL_MAX</a></li>
<li><a href="#Ogl_GL_MAX_3D_TEXTURE_SIZE">GL_MAX_3D_TEXTURE_SIZE</a></li>
<li><a href="#Ogl_GL_MAX_ARRAY_TEXTURE_LAYERS">GL_MAX_ARRAY_TEXTURE_LAYERS</a></li>
<li><a href="#Ogl_GL_MAX_ATTRIB_STACK_DEPTH">GL_MAX_ATTRIB_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_MAX_CLIENT_ATTRIB_STACK_DEPTH">GL_MAX_CLIENT_ATTRIB_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_MAX_CLIP_DISTANCES">GL_MAX_CLIP_DISTANCES</a></li>
<li><a href="#Ogl_GL_MAX_CLIP_PLANES">GL_MAX_CLIP_PLANES</a></li>
<li><a href="#Ogl_GL_MAX_COLOR_ATTACHMENTS">GL_MAX_COLOR_ATTACHMENTS</a></li>
<li><a href="#Ogl_GL_MAX_COLOR_MATRIX_STACK_DEPTH">GL_MAX_COLOR_MATRIX_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS">GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS</a></li>
<li><a href="#Ogl_GL_MAX_CONVOLUTION_HEIGHT">GL_MAX_CONVOLUTION_HEIGHT</a></li>
<li><a href="#Ogl_GL_MAX_CONVOLUTION_WIDTH">GL_MAX_CONVOLUTION_WIDTH</a></li>
<li><a href="#Ogl_GL_MAX_CUBE_MAP_TEXTURE_SIZE">GL_MAX_CUBE_MAP_TEXTURE_SIZE</a></li>
<li><a href="#Ogl_GL_MAX_DRAW_BUFFERS">GL_MAX_DRAW_BUFFERS</a></li>
<li><a href="#Ogl_GL_MAX_ELEMENTS_INDICES">GL_MAX_ELEMENTS_INDICES</a></li>
<li><a href="#Ogl_GL_MAX_ELEMENTS_INDICES_WIN">GL_MAX_ELEMENTS_INDICES_WIN</a></li>
<li><a href="#Ogl_GL_MAX_ELEMENTS_VERTICES">GL_MAX_ELEMENTS_VERTICES</a></li>
<li><a href="#Ogl_GL_MAX_ELEMENTS_VERTICES_WIN">GL_MAX_ELEMENTS_VERTICES_WIN</a></li>
<li><a href="#Ogl_GL_MAX_EVAL_ORDER">GL_MAX_EVAL_ORDER</a></li>
<li><a href="#Ogl_GL_MAX_FRAGMENT_INPUT_COMPONENTS">GL_MAX_FRAGMENT_INPUT_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_FRAGMENT_UNIFORM_COMPONENTS">GL_MAX_FRAGMENT_UNIFORM_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_FRAMEBUFFER_HEIGHT">GL_MAX_FRAMEBUFFER_HEIGHT</a></li>
<li><a href="#Ogl_GL_MAX_FRAMEBUFFER_LAYERS">GL_MAX_FRAMEBUFFER_LAYERS</a></li>
<li><a href="#Ogl_GL_MAX_FRAMEBUFFER_SAMPLES">GL_MAX_FRAMEBUFFER_SAMPLES</a></li>
<li><a href="#Ogl_GL_MAX_FRAMEBUFFER_WIDTH">GL_MAX_FRAMEBUFFER_WIDTH</a></li>
<li><a href="#Ogl_GL_MAX_GEOMETRY_INPUT_COMPONENTS">GL_MAX_GEOMETRY_INPUT_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_GEOMETRY_OUTPUT_COMPONENTS">GL_MAX_GEOMETRY_OUTPUT_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_GEOMETRY_OUTPUT_VERTICES">GL_MAX_GEOMETRY_OUTPUT_VERTICES</a></li>
<li><a href="#Ogl_GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS">GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS</a></li>
<li><a href="#Ogl_GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS">GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_GEOMETRY_UNIFORM_COMPONENTS">GL_MAX_GEOMETRY_UNIFORM_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_LIGHTS">GL_MAX_LIGHTS</a></li>
<li><a href="#Ogl_GL_MAX_LIST_NESTING">GL_MAX_LIST_NESTING</a></li>
<li><a href="#Ogl_GL_MAX_MODELVIEW_STACK_DEPTH">GL_MAX_MODELVIEW_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_MAX_NAME_STACK_DEPTH">GL_MAX_NAME_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_MAX_PIXEL_MAP_TABLE">GL_MAX_PIXEL_MAP_TABLE</a></li>
<li><a href="#Ogl_GL_MAX_PROGRAM_TEXEL_OFFSET">GL_MAX_PROGRAM_TEXEL_OFFSET</a></li>
<li><a href="#Ogl_GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS">GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET">GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET</a></li>
<li><a href="#Ogl_GL_MAX_PROJECTION_STACK_DEPTH">GL_MAX_PROJECTION_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_MAX_RECTANGLE_TEXTURE_SIZE">GL_MAX_RECTANGLE_TEXTURE_SIZE</a></li>
<li><a href="#Ogl_GL_MAX_RENDERBUFFER_SIZE">GL_MAX_RENDERBUFFER_SIZE</a></li>
<li><a href="#Ogl_GL_MAX_SAMPLES">GL_MAX_SAMPLES</a></li>
<li><a href="#Ogl_GL_MAX_TEXTURE_BUFFER_SIZE">GL_MAX_TEXTURE_BUFFER_SIZE</a></li>
<li><a href="#Ogl_GL_MAX_TEXTURE_COORDS">GL_MAX_TEXTURE_COORDS</a></li>
<li><a href="#Ogl_GL_MAX_TEXTURE_IMAGE_UNITS">GL_MAX_TEXTURE_IMAGE_UNITS</a></li>
<li><a href="#Ogl_GL_MAX_TEXTURE_LOD_BIAS">GL_MAX_TEXTURE_LOD_BIAS</a></li>
<li><a href="#Ogl_GL_MAX_TEXTURE_MAX_ANISOTROPY">GL_MAX_TEXTURE_MAX_ANISOTROPY</a></li>
<li><a href="#Ogl_GL_MAX_TEXTURE_SIZE">GL_MAX_TEXTURE_SIZE</a></li>
<li><a href="#Ogl_GL_MAX_TEXTURE_STACK_DEPTH">GL_MAX_TEXTURE_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_MAX_TEXTURE_UNITS">GL_MAX_TEXTURE_UNITS</a></li>
<li><a href="#Ogl_GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS">GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS">GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS</a></li>
<li><a href="#Ogl_GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS">GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_VARYING_COMPONENTS">GL_MAX_VARYING_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_VARYING_FLOATS">GL_MAX_VARYING_FLOATS</a></li>
<li><a href="#Ogl_GL_MAX_VERTEX_ATTRIBS">GL_MAX_VERTEX_ATTRIBS</a></li>
<li><a href="#Ogl_GL_MAX_VERTEX_ATTRIB_STRIDE">GL_MAX_VERTEX_ATTRIB_STRIDE</a></li>
<li><a href="#Ogl_GL_MAX_VERTEX_OUTPUT_COMPONENTS">GL_MAX_VERTEX_OUTPUT_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS">GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS</a></li>
<li><a href="#Ogl_GL_MAX_VERTEX_UNIFORM_COMPONENTS">GL_MAX_VERTEX_UNIFORM_COMPONENTS</a></li>
<li><a href="#Ogl_GL_MAX_VIEWPORT_DIMS">GL_MAX_VIEWPORT_DIMS</a></li>
<li><a href="#Ogl_GL_MIN">GL_MIN</a></li>
<li><a href="#Ogl_GL_MINMAX">GL_MINMAX</a></li>
<li><a href="#Ogl_GL_MINMAX_FORMAT">GL_MINMAX_FORMAT</a></li>
<li><a href="#Ogl_GL_MINMAX_SINK">GL_MINMAX_SINK</a></li>
<li><a href="#Ogl_GL_MINOR_VERSION">GL_MINOR_VERSION</a></li>
<li><a href="#Ogl_GL_MIN_PROGRAM_TEXEL_OFFSET">GL_MIN_PROGRAM_TEXEL_OFFSET</a></li>
<li><a href="#Ogl_GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET">GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET</a></li>
<li><a href="#Ogl_GL_MIN_SAMPLE_SHADING_VALUE">GL_MIN_SAMPLE_SHADING_VALUE</a></li>
<li><a href="#Ogl_GL_MIRRORED_REPEAT">GL_MIRRORED_REPEAT</a></li>
<li><a href="#Ogl_GL_MODELVIEW">GL_MODELVIEW</a></li>
<li><a href="#Ogl_GL_MODELVIEW_MATRIX">GL_MODELVIEW_MATRIX</a></li>
<li><a href="#Ogl_GL_MODELVIEW_STACK_DEPTH">GL_MODELVIEW_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_MODULATE">GL_MODULATE</a></li>
<li><a href="#Ogl_GL_MULT">GL_MULT</a></li>
<li><a href="#Ogl_GL_MULTISAMPLE">GL_MULTISAMPLE</a></li>
<li><a href="#Ogl_GL_MULTISAMPLE_BIT">GL_MULTISAMPLE_BIT</a></li>
<li><a href="#Ogl_GL_N3F_V3F">GL_N3F_V3F</a></li>
<li><a href="#Ogl_GL_NAMED_STRING_LENGTH_ARB">GL_NAMED_STRING_LENGTH_ARB</a></li>
<li><a href="#Ogl_GL_NAMED_STRING_TYPE_ARB">GL_NAMED_STRING_TYPE_ARB</a></li>
<li><a href="#Ogl_GL_NAME_STACK_DEPTH">GL_NAME_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_NAND">GL_NAND</a></li>
<li><a href="#Ogl_GL_NEAREST">GL_NEAREST</a></li>
<li><a href="#Ogl_GL_NEAREST_MIPMAP_LINEAR">GL_NEAREST_MIPMAP_LINEAR</a></li>
<li><a href="#Ogl_GL_NEAREST_MIPMAP_NEAREST">GL_NEAREST_MIPMAP_NEAREST</a></li>
<li><a href="#Ogl_GL_NEVER">GL_NEVER</a></li>
<li><a href="#Ogl_GL_NICEST">GL_NICEST</a></li>
<li><a href="#Ogl_GL_NONE">GL_NONE</a></li>
<li><a href="#Ogl_GL_NOOP">GL_NOOP</a></li>
<li><a href="#Ogl_GL_NOR">GL_NOR</a></li>
<li><a href="#Ogl_GL_NORMALIZE">GL_NORMALIZE</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY">GL_NORMAL_ARRAY</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY_BUFFER_BINDING">GL_NORMAL_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY_COUNT_EXT">GL_NORMAL_ARRAY_COUNT_EXT</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY_EXT">GL_NORMAL_ARRAY_EXT</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY_POINTER">GL_NORMAL_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY_POINTER_EXT">GL_NORMAL_ARRAY_POINTER_EXT</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY_STRIDE">GL_NORMAL_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY_STRIDE_EXT">GL_NORMAL_ARRAY_STRIDE_EXT</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY_TYPE">GL_NORMAL_ARRAY_TYPE</a></li>
<li><a href="#Ogl_GL_NORMAL_ARRAY_TYPE_EXT">GL_NORMAL_ARRAY_TYPE_EXT</a></li>
<li><a href="#Ogl_GL_NORMAL_MAP">GL_NORMAL_MAP</a></li>
<li><a href="#Ogl_GL_NOTEQUAL">GL_NOTEQUAL</a></li>
<li><a href="#Ogl_GL_NO_ERROR">GL_NO_ERROR</a></li>
<li><a href="#Ogl_GL_NUM_COMPRESSED_TEXTURE_FORMATS">GL_NUM_COMPRESSED_TEXTURE_FORMATS</a></li>
<li><a href="#Ogl_GL_NUM_EXTENSIONS">GL_NUM_EXTENSIONS</a></li>
<li><a href="#Ogl_GL_NUM_SHADING_LANGUAGE_VERSIONS">GL_NUM_SHADING_LANGUAGE_VERSIONS</a></li>
<li><a href="#Ogl_GL_NUM_SPIR_V_EXTENSIONS">GL_NUM_SPIR_V_EXTENSIONS</a></li>
<li><a href="#Ogl_GL_OBJECT_LINEAR">GL_OBJECT_LINEAR</a></li>
<li><a href="#Ogl_GL_OBJECT_PLANE">GL_OBJECT_PLANE</a></li>
<li><a href="#Ogl_GL_ONE">GL_ONE</a></li>
<li><a href="#Ogl_GL_ONE_MINUS_CONSTANT_ALPHA">GL_ONE_MINUS_CONSTANT_ALPHA</a></li>
<li><a href="#Ogl_GL_ONE_MINUS_CONSTANT_COLOR">GL_ONE_MINUS_CONSTANT_COLOR</a></li>
<li><a href="#Ogl_GL_ONE_MINUS_DST_ALPHA">GL_ONE_MINUS_DST_ALPHA</a></li>
<li><a href="#Ogl_GL_ONE_MINUS_DST_COLOR">GL_ONE_MINUS_DST_COLOR</a></li>
<li><a href="#Ogl_GL_ONE_MINUS_SRC_ALPHA">GL_ONE_MINUS_SRC_ALPHA</a></li>
<li><a href="#Ogl_GL_ONE_MINUS_SRC_COLOR">GL_ONE_MINUS_SRC_COLOR</a></li>
<li><a href="#Ogl_GL_OPERAND0_ALPHA">GL_OPERAND0_ALPHA</a></li>
<li><a href="#Ogl_GL_OPERAND0_RGB">GL_OPERAND0_RGB</a></li>
<li><a href="#Ogl_GL_OPERAND1_ALPHA">GL_OPERAND1_ALPHA</a></li>
<li><a href="#Ogl_GL_OPERAND1_RGB">GL_OPERAND1_RGB</a></li>
<li><a href="#Ogl_GL_OPERAND2_ALPHA">GL_OPERAND2_ALPHA</a></li>
<li><a href="#Ogl_GL_OPERAND2_RGB">GL_OPERAND2_RGB</a></li>
<li><a href="#Ogl_GL_OR">GL_OR</a></li>
<li><a href="#Ogl_GL_ORDER">GL_ORDER</a></li>
<li><a href="#Ogl_GL_OR_INVERTED">GL_OR_INVERTED</a></li>
<li><a href="#Ogl_GL_OR_REVERSE">GL_OR_REVERSE</a></li>
<li><a href="#Ogl_GL_OUT_OF_MEMORY">GL_OUT_OF_MEMORY</a></li>
<li><a href="#Ogl_GL_PACK_ALIGNMENT">GL_PACK_ALIGNMENT</a></li>
<li><a href="#Ogl_GL_PACK_IMAGE_HEIGHT">GL_PACK_IMAGE_HEIGHT</a></li>
<li><a href="#Ogl_GL_PACK_LSB_FIRST">GL_PACK_LSB_FIRST</a></li>
<li><a href="#Ogl_GL_PACK_ROW_LENGTH">GL_PACK_ROW_LENGTH</a></li>
<li><a href="#Ogl_GL_PACK_SKIP_IMAGES">GL_PACK_SKIP_IMAGES</a></li>
<li><a href="#Ogl_GL_PACK_SKIP_PIXELS">GL_PACK_SKIP_PIXELS</a></li>
<li><a href="#Ogl_GL_PACK_SKIP_ROWS">GL_PACK_SKIP_ROWS</a></li>
<li><a href="#Ogl_GL_PACK_SWAP_BYTES">GL_PACK_SWAP_BYTES</a></li>
<li><a href="#Ogl_GL_PARAMETER_BUFFER">GL_PARAMETER_BUFFER</a></li>
<li><a href="#Ogl_GL_PARAMETER_BUFFER_BINDING">GL_PARAMETER_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_PASS_THROUGH_TOKEN">GL_PASS_THROUGH_TOKEN</a></li>
<li><a href="#Ogl_GL_PERSPECTIVE_CORRECTION_HINT">GL_PERSPECTIVE_CORRECTION_HINT</a></li>
<li><a href="#Ogl_GL_PHONG_HINT_WIN">GL_PHONG_HINT_WIN</a></li>
<li><a href="#Ogl_GL_PHONG_WIN">GL_PHONG_WIN</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_A_TO_A">GL_PIXEL_MAP_A_TO_A</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_A_TO_A_SIZE">GL_PIXEL_MAP_A_TO_A_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_B_TO_B">GL_PIXEL_MAP_B_TO_B</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_B_TO_B_SIZE">GL_PIXEL_MAP_B_TO_B_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_G_TO_G">GL_PIXEL_MAP_G_TO_G</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_G_TO_G_SIZE">GL_PIXEL_MAP_G_TO_G_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_A">GL_PIXEL_MAP_I_TO_A</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_A_SIZE">GL_PIXEL_MAP_I_TO_A_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_B">GL_PIXEL_MAP_I_TO_B</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_B_SIZE">GL_PIXEL_MAP_I_TO_B_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_G">GL_PIXEL_MAP_I_TO_G</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_G_SIZE">GL_PIXEL_MAP_I_TO_G_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_I">GL_PIXEL_MAP_I_TO_I</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_I_SIZE">GL_PIXEL_MAP_I_TO_I_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_R">GL_PIXEL_MAP_I_TO_R</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_I_TO_R_SIZE">GL_PIXEL_MAP_I_TO_R_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_R_TO_R">GL_PIXEL_MAP_R_TO_R</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_R_TO_R_SIZE">GL_PIXEL_MAP_R_TO_R_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_S_TO_S">GL_PIXEL_MAP_S_TO_S</a></li>
<li><a href="#Ogl_GL_PIXEL_MAP_S_TO_S_SIZE">GL_PIXEL_MAP_S_TO_S_SIZE</a></li>
<li><a href="#Ogl_GL_PIXEL_MODE_BIT">GL_PIXEL_MODE_BIT</a></li>
<li><a href="#Ogl_GL_PIXEL_PACK_BUFFER">GL_PIXEL_PACK_BUFFER</a></li>
<li><a href="#Ogl_GL_PIXEL_PACK_BUFFER_BINDING">GL_PIXEL_PACK_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_PIXEL_UNPACK_BUFFER">GL_PIXEL_UNPACK_BUFFER</a></li>
<li><a href="#Ogl_GL_PIXEL_UNPACK_BUFFER_BINDING">GL_PIXEL_UNPACK_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_POINT">GL_POINT</a></li>
<li><a href="#Ogl_GL_POINTS">GL_POINTS</a></li>
<li><a href="#Ogl_GL_POINT_BIT">GL_POINT_BIT</a></li>
<li><a href="#Ogl_GL_POINT_DISTANCE_ATTENUATION">GL_POINT_DISTANCE_ATTENUATION</a></li>
<li><a href="#Ogl_GL_POINT_FADE_THRESHOLD_SIZE">GL_POINT_FADE_THRESHOLD_SIZE</a></li>
<li><a href="#Ogl_GL_POINT_SIZE">GL_POINT_SIZE</a></li>
<li><a href="#Ogl_GL_POINT_SIZE_GRANULARITY">GL_POINT_SIZE_GRANULARITY</a></li>
<li><a href="#Ogl_GL_POINT_SIZE_MAX">GL_POINT_SIZE_MAX</a></li>
<li><a href="#Ogl_GL_POINT_SIZE_MIN">GL_POINT_SIZE_MIN</a></li>
<li><a href="#Ogl_GL_POINT_SIZE_RANGE">GL_POINT_SIZE_RANGE</a></li>
<li><a href="#Ogl_GL_POINT_SMOOTH">GL_POINT_SMOOTH</a></li>
<li><a href="#Ogl_GL_POINT_SMOOTH_HINT">GL_POINT_SMOOTH_HINT</a></li>
<li><a href="#Ogl_GL_POINT_SPRITE">GL_POINT_SPRITE</a></li>
<li><a href="#Ogl_GL_POINT_SPRITE_COORD_ORIGIN">GL_POINT_SPRITE_COORD_ORIGIN</a></li>
<li><a href="#Ogl_GL_POINT_TOKEN">GL_POINT_TOKEN</a></li>
<li><a href="#Ogl_GL_POLYGON">GL_POLYGON</a></li>
<li><a href="#Ogl_GL_POLYGON_BIT">GL_POLYGON_BIT</a></li>
<li><a href="#Ogl_GL_POLYGON_MODE">GL_POLYGON_MODE</a></li>
<li><a href="#Ogl_GL_POLYGON_OFFSET_CLAMP">GL_POLYGON_OFFSET_CLAMP</a></li>
<li><a href="#Ogl_GL_POLYGON_OFFSET_FACTOR">GL_POLYGON_OFFSET_FACTOR</a></li>
<li><a href="#Ogl_GL_POLYGON_OFFSET_FILL">GL_POLYGON_OFFSET_FILL</a></li>
<li><a href="#Ogl_GL_POLYGON_OFFSET_LINE">GL_POLYGON_OFFSET_LINE</a></li>
<li><a href="#Ogl_GL_POLYGON_OFFSET_POINT">GL_POLYGON_OFFSET_POINT</a></li>
<li><a href="#Ogl_GL_POLYGON_OFFSET_UNITS">GL_POLYGON_OFFSET_UNITS</a></li>
<li><a href="#Ogl_GL_POLYGON_SMOOTH">GL_POLYGON_SMOOTH</a></li>
<li><a href="#Ogl_GL_POLYGON_SMOOTH_HINT">GL_POLYGON_SMOOTH_HINT</a></li>
<li><a href="#Ogl_GL_POLYGON_STIPPLE">GL_POLYGON_STIPPLE</a></li>
<li><a href="#Ogl_GL_POLYGON_STIPPLE_BIT">GL_POLYGON_STIPPLE_BIT</a></li>
<li><a href="#Ogl_GL_POLYGON_TOKEN">GL_POLYGON_TOKEN</a></li>
<li><a href="#Ogl_GL_POSITION">GL_POSITION</a></li>
<li><a href="#Ogl_GL_POST_COLOR_MATRIX_ALPHA_BIAS">GL_POST_COLOR_MATRIX_ALPHA_BIAS</a></li>
<li><a href="#Ogl_GL_POST_COLOR_MATRIX_ALPHA_SCALE">GL_POST_COLOR_MATRIX_ALPHA_SCALE</a></li>
<li><a href="#Ogl_GL_POST_COLOR_MATRIX_BLUE_BIAS">GL_POST_COLOR_MATRIX_BLUE_BIAS</a></li>
<li><a href="#Ogl_GL_POST_COLOR_MATRIX_BLUE_SCALE">GL_POST_COLOR_MATRIX_BLUE_SCALE</a></li>
<li><a href="#Ogl_GL_POST_COLOR_MATRIX_COLOR_TABLE">GL_POST_COLOR_MATRIX_COLOR_TABLE</a></li>
<li><a href="#Ogl_GL_POST_COLOR_MATRIX_GREEN_BIAS">GL_POST_COLOR_MATRIX_GREEN_BIAS</a></li>
<li><a href="#Ogl_GL_POST_COLOR_MATRIX_GREEN_SCALE">GL_POST_COLOR_MATRIX_GREEN_SCALE</a></li>
<li><a href="#Ogl_GL_POST_COLOR_MATRIX_RED_BIAS">GL_POST_COLOR_MATRIX_RED_BIAS</a></li>
<li><a href="#Ogl_GL_POST_COLOR_MATRIX_RED_SCALE">GL_POST_COLOR_MATRIX_RED_SCALE</a></li>
<li><a href="#Ogl_GL_POST_CONVOLUTION_ALPHA_BIAS">GL_POST_CONVOLUTION_ALPHA_BIAS</a></li>
<li><a href="#Ogl_GL_POST_CONVOLUTION_ALPHA_SCALE">GL_POST_CONVOLUTION_ALPHA_SCALE</a></li>
<li><a href="#Ogl_GL_POST_CONVOLUTION_BLUE_BIAS">GL_POST_CONVOLUTION_BLUE_BIAS</a></li>
<li><a href="#Ogl_GL_POST_CONVOLUTION_BLUE_SCALE">GL_POST_CONVOLUTION_BLUE_SCALE</a></li>
<li><a href="#Ogl_GL_POST_CONVOLUTION_COLOR_TABLE">GL_POST_CONVOLUTION_COLOR_TABLE</a></li>
<li><a href="#Ogl_GL_POST_CONVOLUTION_GREEN_BIAS">GL_POST_CONVOLUTION_GREEN_BIAS</a></li>
<li><a href="#Ogl_GL_POST_CONVOLUTION_GREEN_SCALE">GL_POST_CONVOLUTION_GREEN_SCALE</a></li>
<li><a href="#Ogl_GL_POST_CONVOLUTION_RED_BIAS">GL_POST_CONVOLUTION_RED_BIAS</a></li>
<li><a href="#Ogl_GL_POST_CONVOLUTION_RED_SCALE">GL_POST_CONVOLUTION_RED_SCALE</a></li>
<li><a href="#Ogl_GL_PREVIOUS">GL_PREVIOUS</a></li>
<li><a href="#Ogl_GL_PRIMARY_COLOR">GL_PRIMARY_COLOR</a></li>
<li><a href="#Ogl_GL_PRIMITIVES_GENERATED">GL_PRIMITIVES_GENERATED</a></li>
<li><a href="#Ogl_GL_PRIMITIVES_SUBMITTED">GL_PRIMITIVES_SUBMITTED</a></li>
<li><a href="#Ogl_GL_PRIMITIVE_RESTART">GL_PRIMITIVE_RESTART</a></li>
<li><a href="#Ogl_GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED">GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED</a></li>
<li><a href="#Ogl_GL_PRIMITIVE_RESTART_INDEX">GL_PRIMITIVE_RESTART_INDEX</a></li>
<li><a href="#Ogl_GL_PROGRAM_POINT_SIZE">GL_PROGRAM_POINT_SIZE</a></li>
<li><a href="#Ogl_GL_PROJECTION">GL_PROJECTION</a></li>
<li><a href="#Ogl_GL_PROJECTION_MATRIX">GL_PROJECTION_MATRIX</a></li>
<li><a href="#Ogl_GL_PROJECTION_STACK_DEPTH">GL_PROJECTION_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_PROXY_COLOR_TABLE">GL_PROXY_COLOR_TABLE</a></li>
<li><a href="#Ogl_GL_PROXY_HISTOGRAM">GL_PROXY_HISTOGRAM</a></li>
<li><a href="#Ogl_GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE">GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE</a></li>
<li><a href="#Ogl_GL_PROXY_POST_CONVOLUTION_COLOR_TABLE">GL_PROXY_POST_CONVOLUTION_COLOR_TABLE</a></li>
<li><a href="#Ogl_GL_PROXY_TEXTURE_1D">GL_PROXY_TEXTURE_1D</a></li>
<li><a href="#Ogl_GL_PROXY_TEXTURE_1D_ARRAY">GL_PROXY_TEXTURE_1D_ARRAY</a></li>
<li><a href="#Ogl_GL_PROXY_TEXTURE_2D">GL_PROXY_TEXTURE_2D</a></li>
<li><a href="#Ogl_GL_PROXY_TEXTURE_2D_ARRAY">GL_PROXY_TEXTURE_2D_ARRAY</a></li>
<li><a href="#Ogl_GL_PROXY_TEXTURE_3D">GL_PROXY_TEXTURE_3D</a></li>
<li><a href="#Ogl_GL_PROXY_TEXTURE_CUBE_MAP">GL_PROXY_TEXTURE_CUBE_MAP</a></li>
<li><a href="#Ogl_GL_PROXY_TEXTURE_CUBE_MAP_ARRAY">GL_PROXY_TEXTURE_CUBE_MAP_ARRAY</a></li>
<li><a href="#Ogl_GL_PROXY_TEXTURE_RECTANGLE">GL_PROXY_TEXTURE_RECTANGLE</a></li>
<li><a href="#Ogl_GL_Q">GL_Q</a></li>
<li><a href="#Ogl_GL_QUADRATIC_ATTENUATION">GL_QUADRATIC_ATTENUATION</a></li>
<li><a href="#Ogl_GL_QUADS">GL_QUADS</a></li>
<li><a href="#Ogl_GL_QUAD_STRIP">GL_QUAD_STRIP</a></li>
<li><a href="#Ogl_GL_QUERY_BY_REGION_NO_WAIT">GL_QUERY_BY_REGION_NO_WAIT</a></li>
<li><a href="#Ogl_GL_QUERY_BY_REGION_WAIT">GL_QUERY_BY_REGION_WAIT</a></li>
<li><a href="#Ogl_GL_QUERY_COUNTER_BITS">GL_QUERY_COUNTER_BITS</a></li>
<li><a href="#Ogl_GL_QUERY_NO_WAIT">GL_QUERY_NO_WAIT</a></li>
<li><a href="#Ogl_GL_QUERY_RESULT">GL_QUERY_RESULT</a></li>
<li><a href="#Ogl_GL_QUERY_RESULT_AVAILABLE">GL_QUERY_RESULT_AVAILABLE</a></li>
<li><a href="#Ogl_GL_QUERY_WAIT">GL_QUERY_WAIT</a></li>
<li><a href="#Ogl_GL_R">GL_R</a></li>
<li><a href="#Ogl_GL_R11F_G11F_B10F">GL_R11F_G11F_B10F</a></li>
<li><a href="#Ogl_GL_R16_SNORM">GL_R16_SNORM</a></li>
<li><a href="#Ogl_GL_R3_G3_B2">GL_R3_G3_B2</a></li>
<li><a href="#Ogl_GL_R8_SNORM">GL_R8_SNORM</a></li>
<li><a href="#Ogl_GL_RASTERIZER_DISCARD">GL_RASTERIZER_DISCARD</a></li>
<li><a href="#Ogl_GL_READ_BUFFER">GL_READ_BUFFER</a></li>
<li><a href="#Ogl_GL_READ_FRAMEBUFFER">GL_READ_FRAMEBUFFER</a></li>
<li><a href="#Ogl_GL_READ_FRAMEBUFFER_BINDING">GL_READ_FRAMEBUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_READ_ONLY">GL_READ_ONLY</a></li>
<li><a href="#Ogl_GL_READ_WRITE">GL_READ_WRITE</a></li>
<li><a href="#Ogl_GL_RED">GL_RED</a></li>
<li><a href="#Ogl_GL_REDUCE">GL_REDUCE</a></li>
<li><a href="#Ogl_GL_RED_BIAS">GL_RED_BIAS</a></li>
<li><a href="#Ogl_GL_RED_BITS">GL_RED_BITS</a></li>
<li><a href="#Ogl_GL_RED_INTEGER">GL_RED_INTEGER</a></li>
<li><a href="#Ogl_GL_RED_SCALE">GL_RED_SCALE</a></li>
<li><a href="#Ogl_GL_RED_SNORM">GL_RED_SNORM</a></li>
<li><a href="#Ogl_GL_REFLECTION_MAP">GL_REFLECTION_MAP</a></li>
<li><a href="#Ogl_GL_RENDER">GL_RENDER</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER">GL_RENDERBUFFER</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_ALPHA_SIZE">GL_RENDERBUFFER_ALPHA_SIZE</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_BINDING">GL_RENDERBUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_BLUE_SIZE">GL_RENDERBUFFER_BLUE_SIZE</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_DEPTH_SIZE">GL_RENDERBUFFER_DEPTH_SIZE</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_GREEN_SIZE">GL_RENDERBUFFER_GREEN_SIZE</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_HEIGHT">GL_RENDERBUFFER_HEIGHT</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_INTERNAL_FORMAT">GL_RENDERBUFFER_INTERNAL_FORMAT</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_RED_SIZE">GL_RENDERBUFFER_RED_SIZE</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_SAMPLES">GL_RENDERBUFFER_SAMPLES</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_STENCIL_SIZE">GL_RENDERBUFFER_STENCIL_SIZE</a></li>
<li><a href="#Ogl_GL_RENDERBUFFER_WIDTH">GL_RENDERBUFFER_WIDTH</a></li>
<li><a href="#Ogl_GL_RENDERER">GL_RENDERER</a></li>
<li><a href="#Ogl_GL_RENDER_MODE">GL_RENDER_MODE</a></li>
<li><a href="#Ogl_GL_REPEAT">GL_REPEAT</a></li>
<li><a href="#Ogl_GL_REPLACE">GL_REPLACE</a></li>
<li><a href="#Ogl_GL_REPLICATE_BORDER">GL_REPLICATE_BORDER</a></li>
<li><a href="#Ogl_GL_RESCALE_NORMAL">GL_RESCALE_NORMAL</a></li>
<li><a href="#Ogl_GL_RETURN">GL_RETURN</a></li>
<li><a href="#Ogl_GL_RG16_SNORM">GL_RG16_SNORM</a></li>
<li><a href="#Ogl_GL_RG8_SNORM">GL_RG8_SNORM</a></li>
<li><a href="#Ogl_GL_RGB">GL_RGB</a></li>
<li><a href="#Ogl_GL_RGB10">GL_RGB10</a></li>
<li><a href="#Ogl_GL_RGB10_A2">GL_RGB10_A2</a></li>
<li><a href="#Ogl_GL_RGB10_A2UI">GL_RGB10_A2UI</a></li>
<li><a href="#Ogl_GL_RGB12">GL_RGB12</a></li>
<li><a href="#Ogl_GL_RGB16">GL_RGB16</a></li>
<li><a href="#Ogl_GL_RGB16F">GL_RGB16F</a></li>
<li><a href="#Ogl_GL_RGB16I">GL_RGB16I</a></li>
<li><a href="#Ogl_GL_RGB16UI">GL_RGB16UI</a></li>
<li><a href="#Ogl_GL_RGB16_SNORM">GL_RGB16_SNORM</a></li>
<li><a href="#Ogl_GL_RGB32F">GL_RGB32F</a></li>
<li><a href="#Ogl_GL_RGB32I">GL_RGB32I</a></li>
<li><a href="#Ogl_GL_RGB32UI">GL_RGB32UI</a></li>
<li><a href="#Ogl_GL_RGB4">GL_RGB4</a></li>
<li><a href="#Ogl_GL_RGB5">GL_RGB5</a></li>
<li><a href="#Ogl_GL_RGB5_A1">GL_RGB5_A1</a></li>
<li><a href="#Ogl_GL_RGB8">GL_RGB8</a></li>
<li><a href="#Ogl_GL_RGB8I">GL_RGB8I</a></li>
<li><a href="#Ogl_GL_RGB8UI">GL_RGB8UI</a></li>
<li><a href="#Ogl_GL_RGB8_SNORM">GL_RGB8_SNORM</a></li>
<li><a href="#Ogl_GL_RGB9_E5">GL_RGB9_E5</a></li>
<li><a href="#Ogl_GL_RGBA">GL_RGBA</a></li>
<li><a href="#Ogl_GL_RGBA12">GL_RGBA12</a></li>
<li><a href="#Ogl_GL_RGBA16">GL_RGBA16</a></li>
<li><a href="#Ogl_GL_RGBA16F">GL_RGBA16F</a></li>
<li><a href="#Ogl_GL_RGBA16I">GL_RGBA16I</a></li>
<li><a href="#Ogl_GL_RGBA16UI">GL_RGBA16UI</a></li>
<li><a href="#Ogl_GL_RGBA16_SNORM">GL_RGBA16_SNORM</a></li>
<li><a href="#Ogl_GL_RGBA2">GL_RGBA2</a></li>
<li><a href="#Ogl_GL_RGBA32F">GL_RGBA32F</a></li>
<li><a href="#Ogl_GL_RGBA32I">GL_RGBA32I</a></li>
<li><a href="#Ogl_GL_RGBA32UI">GL_RGBA32UI</a></li>
<li><a href="#Ogl_GL_RGBA4">GL_RGBA4</a></li>
<li><a href="#Ogl_GL_RGBA8">GL_RGBA8</a></li>
<li><a href="#Ogl_GL_RGBA8I">GL_RGBA8I</a></li>
<li><a href="#Ogl_GL_RGBA8UI">GL_RGBA8UI</a></li>
<li><a href="#Ogl_GL_RGBA8_SNORM">GL_RGBA8_SNORM</a></li>
<li><a href="#Ogl_GL_RGBA_INTEGER">GL_RGBA_INTEGER</a></li>
<li><a href="#Ogl_GL_RGBA_MODE">GL_RGBA_MODE</a></li>
<li><a href="#Ogl_GL_RGBA_SNORM">GL_RGBA_SNORM</a></li>
<li><a href="#Ogl_GL_RGB_INTEGER">GL_RGB_INTEGER</a></li>
<li><a href="#Ogl_GL_RGB_SCALE">GL_RGB_SCALE</a></li>
<li><a href="#Ogl_GL_RGB_SNORM">GL_RGB_SNORM</a></li>
<li><a href="#Ogl_GL_RG_SNORM">GL_RG_SNORM</a></li>
<li><a href="#Ogl_GL_RIGHT">GL_RIGHT</a></li>
<li><a href="#Ogl_GL_S">GL_S</a></li>
<li><a href="#Ogl_GL_SAMPLER_1D">GL_SAMPLER_1D</a></li>
<li><a href="#Ogl_GL_SAMPLER_1D_ARRAY">GL_SAMPLER_1D_ARRAY</a></li>
<li><a href="#Ogl_GL_SAMPLER_1D_ARRAY_SHADOW">GL_SAMPLER_1D_ARRAY_SHADOW</a></li>
<li><a href="#Ogl_GL_SAMPLER_1D_SHADOW">GL_SAMPLER_1D_SHADOW</a></li>
<li><a href="#Ogl_GL_SAMPLER_2D">GL_SAMPLER_2D</a></li>
<li><a href="#Ogl_GL_SAMPLER_2D_ARRAY">GL_SAMPLER_2D_ARRAY</a></li>
<li><a href="#Ogl_GL_SAMPLER_2D_ARRAY_SHADOW">GL_SAMPLER_2D_ARRAY_SHADOW</a></li>
<li><a href="#Ogl_GL_SAMPLER_2D_RECT">GL_SAMPLER_2D_RECT</a></li>
<li><a href="#Ogl_GL_SAMPLER_2D_RECT_SHADOW">GL_SAMPLER_2D_RECT_SHADOW</a></li>
<li><a href="#Ogl_GL_SAMPLER_2D_SHADOW">GL_SAMPLER_2D_SHADOW</a></li>
<li><a href="#Ogl_GL_SAMPLER_3D">GL_SAMPLER_3D</a></li>
<li><a href="#Ogl_GL_SAMPLER_BUFFER">GL_SAMPLER_BUFFER</a></li>
<li><a href="#Ogl_GL_SAMPLER_CUBE">GL_SAMPLER_CUBE</a></li>
<li><a href="#Ogl_GL_SAMPLER_CUBE_MAP_ARRAY">GL_SAMPLER_CUBE_MAP_ARRAY</a></li>
<li><a href="#Ogl_GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW">GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW</a></li>
<li><a href="#Ogl_GL_SAMPLER_CUBE_SHADOW">GL_SAMPLER_CUBE_SHADOW</a></li>
<li><a href="#Ogl_GL_SAMPLES">GL_SAMPLES</a></li>
<li><a href="#Ogl_GL_SAMPLES_PASSED">GL_SAMPLES_PASSED</a></li>
<li><a href="#Ogl_GL_SAMPLE_ALPHA_TO_COVERAGE">GL_SAMPLE_ALPHA_TO_COVERAGE</a></li>
<li><a href="#Ogl_GL_SAMPLE_ALPHA_TO_ONE">GL_SAMPLE_ALPHA_TO_ONE</a></li>
<li><a href="#Ogl_GL_SAMPLE_BUFFERS">GL_SAMPLE_BUFFERS</a></li>
<li><a href="#Ogl_GL_SAMPLE_COVERAGE">GL_SAMPLE_COVERAGE</a></li>
<li><a href="#Ogl_GL_SAMPLE_COVERAGE_INVERT">GL_SAMPLE_COVERAGE_INVERT</a></li>
<li><a href="#Ogl_GL_SAMPLE_COVERAGE_VALUE">GL_SAMPLE_COVERAGE_VALUE</a></li>
<li><a href="#Ogl_GL_SAMPLE_SHADING">GL_SAMPLE_SHADING</a></li>
<li><a href="#Ogl_GL_SCISSOR_BIT">GL_SCISSOR_BIT</a></li>
<li><a href="#Ogl_GL_SCISSOR_BOX">GL_SCISSOR_BOX</a></li>
<li><a href="#Ogl_GL_SCISSOR_TEST">GL_SCISSOR_TEST</a></li>
<li><a href="#Ogl_GL_SECONDARY_COLOR_ARRAY">GL_SECONDARY_COLOR_ARRAY</a></li>
<li><a href="#Ogl_GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING">GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_SECONDARY_COLOR_ARRAY_POINTER">GL_SECONDARY_COLOR_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_SECONDARY_COLOR_ARRAY_SIZE">GL_SECONDARY_COLOR_ARRAY_SIZE</a></li>
<li><a href="#Ogl_GL_SECONDARY_COLOR_ARRAY_STRIDE">GL_SECONDARY_COLOR_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_SECONDARY_COLOR_ARRAY_TYPE">GL_SECONDARY_COLOR_ARRAY_TYPE</a></li>
<li><a href="#Ogl_GL_SELECT">GL_SELECT</a></li>
<li><a href="#Ogl_GL_SELECTION_BUFFER_POINTER">GL_SELECTION_BUFFER_POINTER</a></li>
<li><a href="#Ogl_GL_SELECTION_BUFFER_SIZE">GL_SELECTION_BUFFER_SIZE</a></li>
<li><a href="#Ogl_GL_SEPARABLE_2D">GL_SEPARABLE_2D</a></li>
<li><a href="#Ogl_GL_SEPARATE_ATTRIBS">GL_SEPARATE_ATTRIBS</a></li>
<li><a href="#Ogl_GL_SEPARATE_SPECULAR_COLOR">GL_SEPARATE_SPECULAR_COLOR</a></li>
<li><a href="#Ogl_GL_SET">GL_SET</a></li>
<li><a href="#Ogl_GL_SHADER_BINARY_FORMAT_SPIR_V">GL_SHADER_BINARY_FORMAT_SPIR_V</a></li>
<li><a href="#Ogl_GL_SHADER_INCLUDE_ARB">GL_SHADER_INCLUDE_ARB</a></li>
<li><a href="#Ogl_GL_SHADER_SOURCE_LENGTH">GL_SHADER_SOURCE_LENGTH</a></li>
<li><a href="#Ogl_GL_SHADER_TYPE">GL_SHADER_TYPE</a></li>
<li><a href="#Ogl_GL_SHADE_MODEL">GL_SHADE_MODEL</a></li>
<li><a href="#Ogl_GL_SHADING_LANGUAGE_VERSION">GL_SHADING_LANGUAGE_VERSION</a></li>
<li><a href="#Ogl_GL_SHININESS">GL_SHININESS</a></li>
<li><a href="#Ogl_GL_SHORT">GL_SHORT</a></li>
<li><a href="#Ogl_GL_SIGNED_NORMALIZED">GL_SIGNED_NORMALIZED</a></li>
<li><a href="#Ogl_GL_SINGLE_COLOR">GL_SINGLE_COLOR</a></li>
<li><a href="#Ogl_GL_SLUMINANCE">GL_SLUMINANCE</a></li>
<li><a href="#Ogl_GL_SLUMINANCE8">GL_SLUMINANCE8</a></li>
<li><a href="#Ogl_GL_SLUMINANCE8_ALPHA8">GL_SLUMINANCE8_ALPHA8</a></li>
<li><a href="#Ogl_GL_SLUMINANCE_ALPHA">GL_SLUMINANCE_ALPHA</a></li>
<li><a href="#Ogl_GL_SMOOTH">GL_SMOOTH</a></li>
<li><a href="#Ogl_GL_SMOOTH_LINE_WIDTH_GRANULARITY">GL_SMOOTH_LINE_WIDTH_GRANULARITY</a></li>
<li><a href="#Ogl_GL_SMOOTH_LINE_WIDTH_RANGE">GL_SMOOTH_LINE_WIDTH_RANGE</a></li>
<li><a href="#Ogl_GL_SMOOTH_POINT_SIZE_GRANULARITY">GL_SMOOTH_POINT_SIZE_GRANULARITY</a></li>
<li><a href="#Ogl_GL_SMOOTH_POINT_SIZE_RANGE">GL_SMOOTH_POINT_SIZE_RANGE</a></li>
<li><a href="#Ogl_GL_SOURCE0_ALPHA">GL_SOURCE0_ALPHA</a></li>
<li><a href="#Ogl_GL_SOURCE0_RGB">GL_SOURCE0_RGB</a></li>
<li><a href="#Ogl_GL_SOURCE1_ALPHA">GL_SOURCE1_ALPHA</a></li>
<li><a href="#Ogl_GL_SOURCE1_RGB">GL_SOURCE1_RGB</a></li>
<li><a href="#Ogl_GL_SOURCE2_ALPHA">GL_SOURCE2_ALPHA</a></li>
<li><a href="#Ogl_GL_SOURCE2_RGB">GL_SOURCE2_RGB</a></li>
<li><a href="#Ogl_GL_SPECULAR">GL_SPECULAR</a></li>
<li><a href="#Ogl_GL_SPHERE_MAP">GL_SPHERE_MAP</a></li>
<li><a href="#Ogl_GL_SPIR_V_BINARY">GL_SPIR_V_BINARY</a></li>
<li><a href="#Ogl_GL_SPIR_V_EXTENSIONS">GL_SPIR_V_EXTENSIONS</a></li>
<li><a href="#Ogl_GL_SPOT_CUTOFF">GL_SPOT_CUTOFF</a></li>
<li><a href="#Ogl_GL_SPOT_DIRECTION">GL_SPOT_DIRECTION</a></li>
<li><a href="#Ogl_GL_SPOT_EXPONENT">GL_SPOT_EXPONENT</a></li>
<li><a href="#Ogl_GL_SRC0_ALPHA">GL_SRC0_ALPHA</a></li>
<li><a href="#Ogl_GL_SRC0_RGB">GL_SRC0_RGB</a></li>
<li><a href="#Ogl_GL_SRC1_ALPHA">GL_SRC1_ALPHA</a></li>
<li><a href="#Ogl_GL_SRC1_RGB">GL_SRC1_RGB</a></li>
<li><a href="#Ogl_GL_SRC2_ALPHA">GL_SRC2_ALPHA</a></li>
<li><a href="#Ogl_GL_SRC2_RGB">GL_SRC2_RGB</a></li>
<li><a href="#Ogl_GL_SRC_ALPHA">GL_SRC_ALPHA</a></li>
<li><a href="#Ogl_GL_SRC_ALPHA_SATURATE">GL_SRC_ALPHA_SATURATE</a></li>
<li><a href="#Ogl_GL_SRC_COLOR">GL_SRC_COLOR</a></li>
<li><a href="#Ogl_GL_SRGB">GL_SRGB</a></li>
<li><a href="#Ogl_GL_SRGB8">GL_SRGB8</a></li>
<li><a href="#Ogl_GL_SRGB8_ALPHA8">GL_SRGB8_ALPHA8</a></li>
<li><a href="#Ogl_GL_SRGB_ALPHA">GL_SRGB_ALPHA</a></li>
<li><a href="#Ogl_GL_STACK_OVERFLOW">GL_STACK_OVERFLOW</a></li>
<li><a href="#Ogl_GL_STACK_UNDERFLOW">GL_STACK_UNDERFLOW</a></li>
<li><a href="#Ogl_GL_STATIC_COPY">GL_STATIC_COPY</a></li>
<li><a href="#Ogl_GL_STATIC_DRAW">GL_STATIC_DRAW</a></li>
<li><a href="#Ogl_GL_STATIC_READ">GL_STATIC_READ</a></li>
<li><a href="#Ogl_GL_STENCIL">GL_STENCIL</a></li>
<li><a href="#Ogl_GL_STENCIL_ATTACHMENT">GL_STENCIL_ATTACHMENT</a></li>
<li><a href="#Ogl_GL_STENCIL_BACK_FAIL">GL_STENCIL_BACK_FAIL</a></li>
<li><a href="#Ogl_GL_STENCIL_BACK_FUNC">GL_STENCIL_BACK_FUNC</a></li>
<li><a href="#Ogl_GL_STENCIL_BACK_PASS_DEPTH_FAIL">GL_STENCIL_BACK_PASS_DEPTH_FAIL</a></li>
<li><a href="#Ogl_GL_STENCIL_BACK_PASS_DEPTH_PASS">GL_STENCIL_BACK_PASS_DEPTH_PASS</a></li>
<li><a href="#Ogl_GL_STENCIL_BACK_REF">GL_STENCIL_BACK_REF</a></li>
<li><a href="#Ogl_GL_STENCIL_BACK_VALUE_MASK">GL_STENCIL_BACK_VALUE_MASK</a></li>
<li><a href="#Ogl_GL_STENCIL_BACK_WRITEMASK">GL_STENCIL_BACK_WRITEMASK</a></li>
<li><a href="#Ogl_GL_STENCIL_BITS">GL_STENCIL_BITS</a></li>
<li><a href="#Ogl_GL_STENCIL_BUFFER">GL_STENCIL_BUFFER</a></li>
<li><a href="#Ogl_GL_STENCIL_BUFFER_BIT">GL_STENCIL_BUFFER_BIT</a></li>
<li><a href="#Ogl_GL_STENCIL_CLEAR_VALUE">GL_STENCIL_CLEAR_VALUE</a></li>
<li><a href="#Ogl_GL_STENCIL_FAIL">GL_STENCIL_FAIL</a></li>
<li><a href="#Ogl_GL_STENCIL_FUNC">GL_STENCIL_FUNC</a></li>
<li><a href="#Ogl_GL_STENCIL_INDEX">GL_STENCIL_INDEX</a></li>
<li><a href="#Ogl_GL_STENCIL_INDEX1">GL_STENCIL_INDEX1</a></li>
<li><a href="#Ogl_GL_STENCIL_INDEX16">GL_STENCIL_INDEX16</a></li>
<li><a href="#Ogl_GL_STENCIL_INDEX4">GL_STENCIL_INDEX4</a></li>
<li><a href="#Ogl_GL_STENCIL_INDEX8">GL_STENCIL_INDEX8</a></li>
<li><a href="#Ogl_GL_STENCIL_PASS_DEPTH_FAIL">GL_STENCIL_PASS_DEPTH_FAIL</a></li>
<li><a href="#Ogl_GL_STENCIL_PASS_DEPTH_PASS">GL_STENCIL_PASS_DEPTH_PASS</a></li>
<li><a href="#Ogl_GL_STENCIL_REF">GL_STENCIL_REF</a></li>
<li><a href="#Ogl_GL_STENCIL_TEST">GL_STENCIL_TEST</a></li>
<li><a href="#Ogl_GL_STENCIL_VALUE_MASK">GL_STENCIL_VALUE_MASK</a></li>
<li><a href="#Ogl_GL_STENCIL_WRITEMASK">GL_STENCIL_WRITEMASK</a></li>
<li><a href="#Ogl_GL_STEREO">GL_STEREO</a></li>
<li><a href="#Ogl_GL_STREAM_COPY">GL_STREAM_COPY</a></li>
<li><a href="#Ogl_GL_STREAM_DRAW">GL_STREAM_DRAW</a></li>
<li><a href="#Ogl_GL_STREAM_READ">GL_STREAM_READ</a></li>
<li><a href="#Ogl_GL_SUBPIXEL_BITS">GL_SUBPIXEL_BITS</a></li>
<li><a href="#Ogl_GL_SUBTRACT">GL_SUBTRACT</a></li>
<li><a href="#Ogl_GL_T">GL_T</a></li>
<li><a href="#Ogl_GL_T2F_C3F_V3F">GL_T2F_C3F_V3F</a></li>
<li><a href="#Ogl_GL_T2F_C4F_N3F_V3F">GL_T2F_C4F_N3F_V3F</a></li>
<li><a href="#Ogl_GL_T2F_C4UB_V3F">GL_T2F_C4UB_V3F</a></li>
<li><a href="#Ogl_GL_T2F_N3F_V3F">GL_T2F_N3F_V3F</a></li>
<li><a href="#Ogl_GL_T2F_V3F">GL_T2F_V3F</a></li>
<li><a href="#Ogl_GL_T4F_C4F_N3F_V4F">GL_T4F_C4F_N3F_V4F</a></li>
<li><a href="#Ogl_GL_T4F_V4F">GL_T4F_V4F</a></li>
<li><a href="#Ogl_GL_TABLE_TOO_LARGE">GL_TABLE_TOO_LARGE</a></li>
<li><a href="#Ogl_GL_TESS_CONTROL_SHADER_PATCHES">GL_TESS_CONTROL_SHADER_PATCHES</a></li>
<li><a href="#Ogl_GL_TESS_EVALUATION_SHADER_INVOCATIONS">GL_TESS_EVALUATION_SHADER_INVOCATIONS</a></li>
<li><a href="#Ogl_GL_TEXTURE">GL_TEXTURE</a></li>
<li><a href="#Ogl_GL_TEXTURE0">GL_TEXTURE0</a></li>
<li><a href="#Ogl_GL_TEXTURE1">GL_TEXTURE1</a></li>
<li><a href="#Ogl_GL_TEXTURE10">GL_TEXTURE10</a></li>
<li><a href="#Ogl_GL_TEXTURE11">GL_TEXTURE11</a></li>
<li><a href="#Ogl_GL_TEXTURE12">GL_TEXTURE12</a></li>
<li><a href="#Ogl_GL_TEXTURE13">GL_TEXTURE13</a></li>
<li><a href="#Ogl_GL_TEXTURE14">GL_TEXTURE14</a></li>
<li><a href="#Ogl_GL_TEXTURE15">GL_TEXTURE15</a></li>
<li><a href="#Ogl_GL_TEXTURE16">GL_TEXTURE16</a></li>
<li><a href="#Ogl_GL_TEXTURE17">GL_TEXTURE17</a></li>
<li><a href="#Ogl_GL_TEXTURE18">GL_TEXTURE18</a></li>
<li><a href="#Ogl_GL_TEXTURE19">GL_TEXTURE19</a></li>
<li><a href="#Ogl_GL_TEXTURE2">GL_TEXTURE2</a></li>
<li><a href="#Ogl_GL_TEXTURE20">GL_TEXTURE20</a></li>
<li><a href="#Ogl_GL_TEXTURE21">GL_TEXTURE21</a></li>
<li><a href="#Ogl_GL_TEXTURE22">GL_TEXTURE22</a></li>
<li><a href="#Ogl_GL_TEXTURE23">GL_TEXTURE23</a></li>
<li><a href="#Ogl_GL_TEXTURE24">GL_TEXTURE24</a></li>
<li><a href="#Ogl_GL_TEXTURE25">GL_TEXTURE25</a></li>
<li><a href="#Ogl_GL_TEXTURE26">GL_TEXTURE26</a></li>
<li><a href="#Ogl_GL_TEXTURE27">GL_TEXTURE27</a></li>
<li><a href="#Ogl_GL_TEXTURE28">GL_TEXTURE28</a></li>
<li><a href="#Ogl_GL_TEXTURE29">GL_TEXTURE29</a></li>
<li><a href="#Ogl_GL_TEXTURE3">GL_TEXTURE3</a></li>
<li><a href="#Ogl_GL_TEXTURE30">GL_TEXTURE30</a></li>
<li><a href="#Ogl_GL_TEXTURE31">GL_TEXTURE31</a></li>
<li><a href="#Ogl_GL_TEXTURE4">GL_TEXTURE4</a></li>
<li><a href="#Ogl_GL_TEXTURE5">GL_TEXTURE5</a></li>
<li><a href="#Ogl_GL_TEXTURE6">GL_TEXTURE6</a></li>
<li><a href="#Ogl_GL_TEXTURE7">GL_TEXTURE7</a></li>
<li><a href="#Ogl_GL_TEXTURE8">GL_TEXTURE8</a></li>
<li><a href="#Ogl_GL_TEXTURE9">GL_TEXTURE9</a></li>
<li><a href="#Ogl_GL_TEXTURE_1D">GL_TEXTURE_1D</a></li>
<li><a href="#Ogl_GL_TEXTURE_1D_ARRAY">GL_TEXTURE_1D_ARRAY</a></li>
<li><a href="#Ogl_GL_TEXTURE_2D">GL_TEXTURE_2D</a></li>
<li><a href="#Ogl_GL_TEXTURE_2D_ARRAY">GL_TEXTURE_2D_ARRAY</a></li>
<li><a href="#Ogl_GL_TEXTURE_3D">GL_TEXTURE_3D</a></li>
<li><a href="#Ogl_GL_TEXTURE_ALPHA_SIZE">GL_TEXTURE_ALPHA_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_ALPHA_TYPE">GL_TEXTURE_ALPHA_TYPE</a></li>
<li><a href="#Ogl_GL_TEXTURE_BASE_LEVEL">GL_TEXTURE_BASE_LEVEL</a></li>
<li><a href="#Ogl_GL_TEXTURE_BINDING_1D">GL_TEXTURE_BINDING_1D</a></li>
<li><a href="#Ogl_GL_TEXTURE_BINDING_1D_ARRAY">GL_TEXTURE_BINDING_1D_ARRAY</a></li>
<li><a href="#Ogl_GL_TEXTURE_BINDING_2D">GL_TEXTURE_BINDING_2D</a></li>
<li><a href="#Ogl_GL_TEXTURE_BINDING_2D_ARRAY">GL_TEXTURE_BINDING_2D_ARRAY</a></li>
<li><a href="#Ogl_GL_TEXTURE_BINDING_3D">GL_TEXTURE_BINDING_3D</a></li>
<li><a href="#Ogl_GL_TEXTURE_BINDING_BUFFER">GL_TEXTURE_BINDING_BUFFER</a></li>
<li><a href="#Ogl_GL_TEXTURE_BINDING_CUBE_MAP">GL_TEXTURE_BINDING_CUBE_MAP</a></li>
<li><a href="#Ogl_GL_TEXTURE_BINDING_CUBE_MAP_ARRAY">GL_TEXTURE_BINDING_CUBE_MAP_ARRAY</a></li>
<li><a href="#Ogl_GL_TEXTURE_BINDING_RECTANGLE">GL_TEXTURE_BINDING_RECTANGLE</a></li>
<li><a href="#Ogl_GL_TEXTURE_BIT">GL_TEXTURE_BIT</a></li>
<li><a href="#Ogl_GL_TEXTURE_BLUE_SIZE">GL_TEXTURE_BLUE_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_BLUE_TYPE">GL_TEXTURE_BLUE_TYPE</a></li>
<li><a href="#Ogl_GL_TEXTURE_BORDER">GL_TEXTURE_BORDER</a></li>
<li><a href="#Ogl_GL_TEXTURE_BORDER_COLOR">GL_TEXTURE_BORDER_COLOR</a></li>
<li><a href="#Ogl_GL_TEXTURE_BUFFER">GL_TEXTURE_BUFFER</a></li>
<li><a href="#Ogl_GL_TEXTURE_BUFFER_BINDING">GL_TEXTURE_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_TEXTURE_BUFFER_DATA_STORE_BINDING">GL_TEXTURE_BUFFER_DATA_STORE_BINDING</a></li>
<li><a href="#Ogl_GL_TEXTURE_BUFFER_FORMAT">GL_TEXTURE_BUFFER_FORMAT</a></li>
<li><a href="#Ogl_GL_TEXTURE_COMPARE_FUNC">GL_TEXTURE_COMPARE_FUNC</a></li>
<li><a href="#Ogl_GL_TEXTURE_COMPARE_FUNC_ARB">GL_TEXTURE_COMPARE_FUNC_ARB</a></li>
<li><a href="#Ogl_GL_TEXTURE_COMPARE_MODE">GL_TEXTURE_COMPARE_MODE</a></li>
<li><a href="#Ogl_GL_TEXTURE_COMPARE_MODE_ARB">GL_TEXTURE_COMPARE_MODE_ARB</a></li>
<li><a href="#Ogl_GL_TEXTURE_COMPONENTS">GL_TEXTURE_COMPONENTS</a></li>
<li><a href="#Ogl_GL_TEXTURE_COMPRESSED">GL_TEXTURE_COMPRESSED</a></li>
<li><a href="#Ogl_GL_TEXTURE_COMPRESSED_IMAGE_SIZE">GL_TEXTURE_COMPRESSED_IMAGE_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_COMPRESSION_HINT">GL_TEXTURE_COMPRESSION_HINT</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY">GL_TEXTURE_COORD_ARRAY</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING">GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_COUNT_EXT">GL_TEXTURE_COORD_ARRAY_COUNT_EXT</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_EXT">GL_TEXTURE_COORD_ARRAY_EXT</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_POINTER">GL_TEXTURE_COORD_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_POINTER_EXT">GL_TEXTURE_COORD_ARRAY_POINTER_EXT</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_SIZE">GL_TEXTURE_COORD_ARRAY_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_SIZE_EXT">GL_TEXTURE_COORD_ARRAY_SIZE_EXT</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_STRIDE">GL_TEXTURE_COORD_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_STRIDE_EXT">GL_TEXTURE_COORD_ARRAY_STRIDE_EXT</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_TYPE">GL_TEXTURE_COORD_ARRAY_TYPE</a></li>
<li><a href="#Ogl_GL_TEXTURE_COORD_ARRAY_TYPE_EXT">GL_TEXTURE_COORD_ARRAY_TYPE_EXT</a></li>
<li><a href="#Ogl_GL_TEXTURE_CUBE_MAP">GL_TEXTURE_CUBE_MAP</a></li>
<li><a href="#Ogl_GL_TEXTURE_CUBE_MAP_ARRAY">GL_TEXTURE_CUBE_MAP_ARRAY</a></li>
<li><a href="#Ogl_GL_TEXTURE_CUBE_MAP_NEGATIVE_X">GL_TEXTURE_CUBE_MAP_NEGATIVE_X</a></li>
<li><a href="#Ogl_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y">GL_TEXTURE_CUBE_MAP_NEGATIVE_Y</a></li>
<li><a href="#Ogl_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z">GL_TEXTURE_CUBE_MAP_NEGATIVE_Z</a></li>
<li><a href="#Ogl_GL_TEXTURE_CUBE_MAP_POSITIVE_X">GL_TEXTURE_CUBE_MAP_POSITIVE_X</a></li>
<li><a href="#Ogl_GL_TEXTURE_CUBE_MAP_POSITIVE_Y">GL_TEXTURE_CUBE_MAP_POSITIVE_Y</a></li>
<li><a href="#Ogl_GL_TEXTURE_CUBE_MAP_POSITIVE_Z">GL_TEXTURE_CUBE_MAP_POSITIVE_Z</a></li>
<li><a href="#Ogl_GL_TEXTURE_DEPTH">GL_TEXTURE_DEPTH</a></li>
<li><a href="#Ogl_GL_TEXTURE_DEPTH_SIZE">GL_TEXTURE_DEPTH_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_DEPTH_TYPE">GL_TEXTURE_DEPTH_TYPE</a></li>
<li><a href="#Ogl_GL_TEXTURE_ENV">GL_TEXTURE_ENV</a></li>
<li><a href="#Ogl_GL_TEXTURE_ENV_COLOR">GL_TEXTURE_ENV_COLOR</a></li>
<li><a href="#Ogl_GL_TEXTURE_ENV_MODE">GL_TEXTURE_ENV_MODE</a></li>
<li><a href="#Ogl_GL_TEXTURE_FILTER_CONTROL">GL_TEXTURE_FILTER_CONTROL</a></li>
<li><a href="#Ogl_GL_TEXTURE_GEN_MODE">GL_TEXTURE_GEN_MODE</a></li>
<li><a href="#Ogl_GL_TEXTURE_GEN_Q">GL_TEXTURE_GEN_Q</a></li>
<li><a href="#Ogl_GL_TEXTURE_GEN_R">GL_TEXTURE_GEN_R</a></li>
<li><a href="#Ogl_GL_TEXTURE_GEN_S">GL_TEXTURE_GEN_S</a></li>
<li><a href="#Ogl_GL_TEXTURE_GEN_T">GL_TEXTURE_GEN_T</a></li>
<li><a href="#Ogl_GL_TEXTURE_GREEN_SIZE">GL_TEXTURE_GREEN_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_GREEN_TYPE">GL_TEXTURE_GREEN_TYPE</a></li>
<li><a href="#Ogl_GL_TEXTURE_HEIGHT">GL_TEXTURE_HEIGHT</a></li>
<li><a href="#Ogl_GL_TEXTURE_INTENSITY_SIZE">GL_TEXTURE_INTENSITY_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_INTENSITY_TYPE">GL_TEXTURE_INTENSITY_TYPE</a></li>
<li><a href="#Ogl_GL_TEXTURE_INTERNAL_FORMAT">GL_TEXTURE_INTERNAL_FORMAT</a></li>
<li><a href="#Ogl_GL_TEXTURE_LOD_BIAS">GL_TEXTURE_LOD_BIAS</a></li>
<li><a href="#Ogl_GL_TEXTURE_LUMINANCE_SIZE">GL_TEXTURE_LUMINANCE_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_LUMINANCE_TYPE">GL_TEXTURE_LUMINANCE_TYPE</a></li>
<li><a href="#Ogl_GL_TEXTURE_MAG_FILTER">GL_TEXTURE_MAG_FILTER</a></li>
<li><a href="#Ogl_GL_TEXTURE_MATRIX">GL_TEXTURE_MATRIX</a></li>
<li><a href="#Ogl_GL_TEXTURE_MAX_ANISOTROPY">GL_TEXTURE_MAX_ANISOTROPY</a></li>
<li><a href="#Ogl_GL_TEXTURE_MAX_LEVEL">GL_TEXTURE_MAX_LEVEL</a></li>
<li><a href="#Ogl_GL_TEXTURE_MAX_LOD">GL_TEXTURE_MAX_LOD</a></li>
<li><a href="#Ogl_GL_TEXTURE_MIN_FILTER">GL_TEXTURE_MIN_FILTER</a></li>
<li><a href="#Ogl_GL_TEXTURE_MIN_LOD">GL_TEXTURE_MIN_LOD</a></li>
<li><a href="#Ogl_GL_TEXTURE_PRIORITY">GL_TEXTURE_PRIORITY</a></li>
<li><a href="#Ogl_GL_TEXTURE_RECTANGLE">GL_TEXTURE_RECTANGLE</a></li>
<li><a href="#Ogl_GL_TEXTURE_RED_SIZE">GL_TEXTURE_RED_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_RED_TYPE">GL_TEXTURE_RED_TYPE</a></li>
<li><a href="#Ogl_GL_TEXTURE_RESIDENT">GL_TEXTURE_RESIDENT</a></li>
<li><a href="#Ogl_GL_TEXTURE_SHARED_SIZE">GL_TEXTURE_SHARED_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_STACK_DEPTH">GL_TEXTURE_STACK_DEPTH</a></li>
<li><a href="#Ogl_GL_TEXTURE_STENCIL_SIZE">GL_TEXTURE_STENCIL_SIZE</a></li>
<li><a href="#Ogl_GL_TEXTURE_WIDTH">GL_TEXTURE_WIDTH</a></li>
<li><a href="#Ogl_GL_TEXTURE_WRAP_R">GL_TEXTURE_WRAP_R</a></li>
<li><a href="#Ogl_GL_TEXTURE_WRAP_S">GL_TEXTURE_WRAP_S</a></li>
<li><a href="#Ogl_GL_TEXTURE_WRAP_T">GL_TEXTURE_WRAP_T</a></li>
<li><a href="#Ogl_GL_TRANSFORM_BIT">GL_TRANSFORM_BIT</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_ACTIVE">GL_TRANSFORM_FEEDBACK_ACTIVE</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_BUFFER">GL_TRANSFORM_FEEDBACK_BUFFER</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_BUFFER_BINDING">GL_TRANSFORM_FEEDBACK_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_BUFFER_MODE">GL_TRANSFORM_FEEDBACK_BUFFER_MODE</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_BUFFER_SIZE">GL_TRANSFORM_FEEDBACK_BUFFER_SIZE</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_BUFFER_START">GL_TRANSFORM_FEEDBACK_BUFFER_START</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_OVERFLOW">GL_TRANSFORM_FEEDBACK_OVERFLOW</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_PAUSED">GL_TRANSFORM_FEEDBACK_PAUSED</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN">GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW">GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_VARYINGS">GL_TRANSFORM_FEEDBACK_VARYINGS</a></li>
<li><a href="#Ogl_GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH">GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH</a></li>
<li><a href="#Ogl_GL_TRANSPOSE_COLOR_MATRIX">GL_TRANSPOSE_COLOR_MATRIX</a></li>
<li><a href="#Ogl_GL_TRANSPOSE_MODELVIEW_MATRIX">GL_TRANSPOSE_MODELVIEW_MATRIX</a></li>
<li><a href="#Ogl_GL_TRANSPOSE_PROJECTION_MATRIX">GL_TRANSPOSE_PROJECTION_MATRIX</a></li>
<li><a href="#Ogl_GL_TRANSPOSE_TEXTURE_MATRIX">GL_TRANSPOSE_TEXTURE_MATRIX</a></li>
<li><a href="#Ogl_GL_TRIANGLES">GL_TRIANGLES</a></li>
<li><a href="#Ogl_GL_TRIANGLES_ADJACENCY">GL_TRIANGLES_ADJACENCY</a></li>
<li><a href="#Ogl_GL_TRIANGLE_FAN">GL_TRIANGLE_FAN</a></li>
<li><a href="#Ogl_GL_TRIANGLE_STRIP">GL_TRIANGLE_STRIP</a></li>
<li><a href="#Ogl_GL_TRIANGLE_STRIP_ADJACENCY">GL_TRIANGLE_STRIP_ADJACENCY</a></li>
<li><a href="#Ogl_GL_TRUE">GL_TRUE</a></li>
<li><a href="#Ogl_GL_UNPACK_ALIGNMENT">GL_UNPACK_ALIGNMENT</a></li>
<li><a href="#Ogl_GL_UNPACK_IMAGE_HEIGHT">GL_UNPACK_IMAGE_HEIGHT</a></li>
<li><a href="#Ogl_GL_UNPACK_LSB_FIRST">GL_UNPACK_LSB_FIRST</a></li>
<li><a href="#Ogl_GL_UNPACK_ROW_LENGTH">GL_UNPACK_ROW_LENGTH</a></li>
<li><a href="#Ogl_GL_UNPACK_SKIP_IMAGES">GL_UNPACK_SKIP_IMAGES</a></li>
<li><a href="#Ogl_GL_UNPACK_SKIP_PIXELS">GL_UNPACK_SKIP_PIXELS</a></li>
<li><a href="#Ogl_GL_UNPACK_SKIP_ROWS">GL_UNPACK_SKIP_ROWS</a></li>
<li><a href="#Ogl_GL_UNPACK_SWAP_BYTES">GL_UNPACK_SWAP_BYTES</a></li>
<li><a href="#Ogl_GL_UNSIGNED_BYTE">GL_UNSIGNED_BYTE</a></li>
<li><a href="#Ogl_GL_UNSIGNED_BYTE_2_3_3_REV">GL_UNSIGNED_BYTE_2_3_3_REV</a></li>
<li><a href="#Ogl_GL_UNSIGNED_BYTE_3_3_2">GL_UNSIGNED_BYTE_3_3_2</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT">GL_UNSIGNED_INT</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_10F_11F_11F_REV">GL_UNSIGNED_INT_10F_11F_11F_REV</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_10_10_10_2">GL_UNSIGNED_INT_10_10_10_2</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_24_8">GL_UNSIGNED_INT_24_8</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_5_9_9_9_REV">GL_UNSIGNED_INT_5_9_9_9_REV</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_8_8_8_8">GL_UNSIGNED_INT_8_8_8_8</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_8_8_8_8_REV">GL_UNSIGNED_INT_8_8_8_8_REV</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_SAMPLER_1D">GL_UNSIGNED_INT_SAMPLER_1D</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_SAMPLER_1D_ARRAY">GL_UNSIGNED_INT_SAMPLER_1D_ARRAY</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_SAMPLER_2D">GL_UNSIGNED_INT_SAMPLER_2D</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_SAMPLER_2D_ARRAY">GL_UNSIGNED_INT_SAMPLER_2D_ARRAY</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_SAMPLER_2D_RECT">GL_UNSIGNED_INT_SAMPLER_2D_RECT</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_SAMPLER_3D">GL_UNSIGNED_INT_SAMPLER_3D</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_SAMPLER_BUFFER">GL_UNSIGNED_INT_SAMPLER_BUFFER</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_SAMPLER_CUBE">GL_UNSIGNED_INT_SAMPLER_CUBE</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY">GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_VEC2">GL_UNSIGNED_INT_VEC2</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_VEC3">GL_UNSIGNED_INT_VEC3</a></li>
<li><a href="#Ogl_GL_UNSIGNED_INT_VEC4">GL_UNSIGNED_INT_VEC4</a></li>
<li><a href="#Ogl_GL_UNSIGNED_NORMALIZED">GL_UNSIGNED_NORMALIZED</a></li>
<li><a href="#Ogl_GL_UNSIGNED_SHORT">GL_UNSIGNED_SHORT</a></li>
<li><a href="#Ogl_GL_UNSIGNED_SHORT_1_5_5_5_REV">GL_UNSIGNED_SHORT_1_5_5_5_REV</a></li>
<li><a href="#Ogl_GL_UNSIGNED_SHORT_4_4_4_4">GL_UNSIGNED_SHORT_4_4_4_4</a></li>
<li><a href="#Ogl_GL_UNSIGNED_SHORT_4_4_4_4_REV">GL_UNSIGNED_SHORT_4_4_4_4_REV</a></li>
<li><a href="#Ogl_GL_UNSIGNED_SHORT_5_5_5_1">GL_UNSIGNED_SHORT_5_5_5_1</a></li>
<li><a href="#Ogl_GL_UNSIGNED_SHORT_5_6_5">GL_UNSIGNED_SHORT_5_6_5</a></li>
<li><a href="#Ogl_GL_UNSIGNED_SHORT_5_6_5_REV">GL_UNSIGNED_SHORT_5_6_5_REV</a></li>
<li><a href="#Ogl_GL_UPPER_LEFT">GL_UPPER_LEFT</a></li>
<li><a href="#Ogl_GL_V2F">GL_V2F</a></li>
<li><a href="#Ogl_GL_V3F">GL_V3F</a></li>
<li><a href="#Ogl_GL_VALIDATE_STATUS">GL_VALIDATE_STATUS</a></li>
<li><a href="#Ogl_GL_VENDOR">GL_VENDOR</a></li>
<li><a href="#Ogl_GL_VERSION">GL_VERSION</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY">GL_VERTEX_ARRAY</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_BUFFER_BINDING">GL_VERTEX_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_COUNT_EXT">GL_VERTEX_ARRAY_COUNT_EXT</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_EXT">GL_VERTEX_ARRAY_EXT</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_POINTER">GL_VERTEX_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_POINTER_EXT">GL_VERTEX_ARRAY_POINTER_EXT</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_SIZE">GL_VERTEX_ARRAY_SIZE</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_SIZE_EXT">GL_VERTEX_ARRAY_SIZE_EXT</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_STRIDE">GL_VERTEX_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_STRIDE_EXT">GL_VERTEX_ARRAY_STRIDE_EXT</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_TYPE">GL_VERTEX_ARRAY_TYPE</a></li>
<li><a href="#Ogl_GL_VERTEX_ARRAY_TYPE_EXT">GL_VERTEX_ARRAY_TYPE_EXT</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING">GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_DIVISOR">GL_VERTEX_ATTRIB_ARRAY_DIVISOR</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_ENABLED">GL_VERTEX_ATTRIB_ARRAY_ENABLED</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_INTEGER">GL_VERTEX_ATTRIB_ARRAY_INTEGER</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_LONG">GL_VERTEX_ATTRIB_ARRAY_LONG</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_NORMALIZED">GL_VERTEX_ATTRIB_ARRAY_NORMALIZED</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_POINTER">GL_VERTEX_ATTRIB_ARRAY_POINTER</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_SIZE">GL_VERTEX_ATTRIB_ARRAY_SIZE</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_STRIDE">GL_VERTEX_ATTRIB_ARRAY_STRIDE</a></li>
<li><a href="#Ogl_GL_VERTEX_ATTRIB_ARRAY_TYPE">GL_VERTEX_ATTRIB_ARRAY_TYPE</a></li>
<li><a href="#Ogl_GL_VERTEX_PROGRAM_POINT_SIZE">GL_VERTEX_PROGRAM_POINT_SIZE</a></li>
<li><a href="#Ogl_GL_VERTEX_PROGRAM_TWO_SIDE">GL_VERTEX_PROGRAM_TWO_SIDE</a></li>
<li><a href="#Ogl_GL_VERTEX_SHADER">GL_VERTEX_SHADER</a></li>
<li><a href="#Ogl_GL_VERTEX_SHADER_INVOCATIONS">GL_VERTEX_SHADER_INVOCATIONS</a></li>
<li><a href="#Ogl_GL_VERTICES_SUBMITTED">GL_VERTICES_SUBMITTED</a></li>
<li><a href="#Ogl_GL_VIEWPORT">GL_VIEWPORT</a></li>
<li><a href="#Ogl_GL_VIEWPORT_BIT">GL_VIEWPORT_BIT</a></li>
<li><a href="#Ogl_GL_WEIGHT_ARRAY_BUFFER_BINDING">GL_WEIGHT_ARRAY_BUFFER_BINDING</a></li>
<li><a href="#Ogl_GL_WIN_draw_range_elements">GL_WIN_draw_range_elements</a></li>
<li><a href="#Ogl_GL_WIN_swap_hint">GL_WIN_swap_hint</a></li>
<li><a href="#Ogl_GL_WRAP_BORDER">GL_WRAP_BORDER</a></li>
<li><a href="#Ogl_GL_WRITE_ONLY">GL_WRITE_ONLY</a></li>
<li><a href="#Ogl_GL_XOR">GL_XOR</a></li>
<li><a href="#Ogl_GL_ZERO">GL_ZERO</a></li>
<li><a href="#Ogl_GL_ZOOM_X">GL_ZOOM_X</a></li>
<li><a href="#Ogl_GL_ZOOM_Y">GL_ZOOM_Y</a></li>
</ul>
<h3>Type Aliases</h3>
<h4></h4>
<ul>
<li><a href="#Ogl_GLbitfield">GLbitfield</a></li>
<li><a href="#Ogl_GLboolean">GLboolean</a></li>
<li><a href="#Ogl_GLbyte">GLbyte</a></li>
<li><a href="#Ogl_GLchar">GLchar</a></li>
<li><a href="#Ogl_GLclampd">GLclampd</a></li>
<li><a href="#Ogl_GLclampf">GLclampf</a></li>
<li><a href="#Ogl_GLdouble">GLdouble</a></li>
<li><a href="#Ogl_GLenum">GLenum</a></li>
<li><a href="#Ogl_GLfloat">GLfloat</a></li>
<li><a href="#Ogl_GLint">GLint</a></li>
<li><a href="#Ogl_GLint64">GLint64</a></li>
<li><a href="#Ogl_GLintptr">GLintptr</a></li>
<li><a href="#Ogl_GLshort">GLshort</a></li>
<li><a href="#Ogl_GLsizei">GLsizei</a></li>
<li><a href="#Ogl_GLsizeiptr">GLsizeiptr</a></li>
<li><a href="#Ogl_GLubyte">GLubyte</a></li>
<li><a href="#Ogl_GLuint">GLuint</a></li>
<li><a href="#Ogl_GLuint64">GLuint64</a></li>
<li><a href="#Ogl_GLushort">GLushort</a></li>
<li><a href="#Ogl_GLvoid">GLvoid</a></li>
<li><a href="#Ogl_HGLRC">HGLRC</a></li>
</ul>
<h3>Functions</h3>
<h4></h4>
<ul>
<li><a href="#Ogl_glAccum">Ogl.glAccum</a></li>
<li><a href="#Ogl_glActiveTexture">Ogl.glActiveTexture</a></li>
<li><a href="#Ogl_glAlphaFunc">Ogl.glAlphaFunc</a></li>
<li><a href="#Ogl_glAreTexturesResident">Ogl.glAreTexturesResident</a></li>
<li><a href="#Ogl_glArrayElement">Ogl.glArrayElement</a></li>
<li><a href="#Ogl_glAttachShader">Ogl.glAttachShader</a></li>
<li><a href="#Ogl_glBegin">Ogl.glBegin</a></li>
<li><a href="#Ogl_glBeginConditionalRender">Ogl.glBeginConditionalRender</a></li>
<li><a href="#Ogl_glBeginQuery">Ogl.glBeginQuery</a></li>
<li><a href="#Ogl_glBeginTransformFeedback">Ogl.glBeginTransformFeedback</a></li>
<li><a href="#Ogl_glBindAttribLocation">Ogl.glBindAttribLocation</a></li>
<li><a href="#Ogl_glBindBuffer">Ogl.glBindBuffer</a></li>
<li><a href="#Ogl_glBindFragDataLocation">Ogl.glBindFragDataLocation</a></li>
<li><a href="#Ogl_glBindFramebuffer">Ogl.glBindFramebuffer</a></li>
<li><a href="#Ogl_glBindRenderbuffer">Ogl.glBindRenderbuffer</a></li>
<li><a href="#Ogl_glBindTexture">Ogl.glBindTexture</a></li>
<li><a href="#Ogl_glBitmap">Ogl.glBitmap</a></li>
<li><a href="#Ogl_glBlendColor">Ogl.glBlendColor</a></li>
<li><a href="#Ogl_glBlendEquation">Ogl.glBlendEquation</a></li>
<li><a href="#Ogl_glBlendEquationSeparate">Ogl.glBlendEquationSeparate</a></li>
<li><a href="#Ogl_glBlendEquationSeparatei">Ogl.glBlendEquationSeparatei</a></li>
<li><a href="#Ogl_glBlendEquationi">Ogl.glBlendEquationi</a></li>
<li><a href="#Ogl_glBlendFunc">Ogl.glBlendFunc</a></li>
<li><a href="#Ogl_glBlendFuncSeparate">Ogl.glBlendFuncSeparate</a></li>
<li><a href="#Ogl_glBlendFuncSeparatei">Ogl.glBlendFuncSeparatei</a></li>
<li><a href="#Ogl_glBlendFunci">Ogl.glBlendFunci</a></li>
<li><a href="#Ogl_glBlitFramebuffer">Ogl.glBlitFramebuffer</a></li>
<li><a href="#Ogl_glBufferData">Ogl.glBufferData</a></li>
<li><a href="#Ogl_glBufferSubData">Ogl.glBufferSubData</a></li>
<li><a href="#Ogl_glCallList">Ogl.glCallList</a></li>
<li><a href="#Ogl_glCallLists">Ogl.glCallLists</a></li>
<li><a href="#Ogl_glCheckFramebufferStatus">Ogl.glCheckFramebufferStatus</a></li>
<li><a href="#Ogl_glClampColor">Ogl.glClampColor</a></li>
<li><a href="#Ogl_glClear">Ogl.glClear</a></li>
<li><a href="#Ogl_glClearAccum">Ogl.glClearAccum</a></li>
<li><a href="#Ogl_glClearBufferfi">Ogl.glClearBufferfi</a></li>
<li><a href="#Ogl_glClearBufferfv">Ogl.glClearBufferfv</a></li>
<li><a href="#Ogl_glClearBufferiv">Ogl.glClearBufferiv</a></li>
<li><a href="#Ogl_glClearBufferuiv">Ogl.glClearBufferuiv</a></li>
<li><a href="#Ogl_glClearColor">Ogl.glClearColor</a></li>
<li><a href="#Ogl_glClearDepth">Ogl.glClearDepth</a></li>
<li><a href="#Ogl_glClearIndex">Ogl.glClearIndex</a></li>
<li><a href="#Ogl_glClearStencil">Ogl.glClearStencil</a></li>
<li><a href="#Ogl_glClientActiveTexture">Ogl.glClientActiveTexture</a></li>
<li><a href="#Ogl_glClipPlane">Ogl.glClipPlane</a></li>
<li><a href="#Ogl_glColor3b">Ogl.glColor3b</a></li>
<li><a href="#Ogl_glColor3bv">Ogl.glColor3bv</a></li>
<li><a href="#Ogl_glColor3d">Ogl.glColor3d</a></li>
<li><a href="#Ogl_glColor3dv">Ogl.glColor3dv</a></li>
<li><a href="#Ogl_glColor3f">Ogl.glColor3f</a></li>
<li><a href="#Ogl_glColor3fv">Ogl.glColor3fv</a></li>
<li><a href="#Ogl_glColor3i">Ogl.glColor3i</a></li>
<li><a href="#Ogl_glColor3iv">Ogl.glColor3iv</a></li>
<li><a href="#Ogl_glColor3s">Ogl.glColor3s</a></li>
<li><a href="#Ogl_glColor3sv">Ogl.glColor3sv</a></li>
<li><a href="#Ogl_glColor3ub">Ogl.glColor3ub</a></li>
<li><a href="#Ogl_glColor3ubv">Ogl.glColor3ubv</a></li>
<li><a href="#Ogl_glColor3ui">Ogl.glColor3ui</a></li>
<li><a href="#Ogl_glColor3uiv">Ogl.glColor3uiv</a></li>
<li><a href="#Ogl_glColor3us">Ogl.glColor3us</a></li>
<li><a href="#Ogl_glColor3usv">Ogl.glColor3usv</a></li>
<li><a href="#Ogl_glColor4b">Ogl.glColor4b</a></li>
<li><a href="#Ogl_glColor4bv">Ogl.glColor4bv</a></li>
<li><a href="#Ogl_glColor4d">Ogl.glColor4d</a></li>
<li><a href="#Ogl_glColor4dv">Ogl.glColor4dv</a></li>
<li><a href="#Ogl_glColor4f">Ogl.glColor4f</a></li>
<li><a href="#Ogl_glColor4fv">Ogl.glColor4fv</a></li>
<li><a href="#Ogl_glColor4i">Ogl.glColor4i</a></li>
<li><a href="#Ogl_glColor4iv">Ogl.glColor4iv</a></li>
<li><a href="#Ogl_glColor4s">Ogl.glColor4s</a></li>
<li><a href="#Ogl_glColor4sv">Ogl.glColor4sv</a></li>
<li><a href="#Ogl_glColor4ub">Ogl.glColor4ub</a></li>
<li><a href="#Ogl_glColor4ubv">Ogl.glColor4ubv</a></li>
<li><a href="#Ogl_glColor4ui">Ogl.glColor4ui</a></li>
<li><a href="#Ogl_glColor4uiv">Ogl.glColor4uiv</a></li>
<li><a href="#Ogl_glColor4us">Ogl.glColor4us</a></li>
<li><a href="#Ogl_glColor4usv">Ogl.glColor4usv</a></li>
<li><a href="#Ogl_glColorMask">Ogl.glColorMask</a></li>
<li><a href="#Ogl_glColorMaski">Ogl.glColorMaski</a></li>
<li><a href="#Ogl_glColorMaterial">Ogl.glColorMaterial</a></li>
<li><a href="#Ogl_glColorPointer">Ogl.glColorPointer</a></li>
<li><a href="#Ogl_glCompileShader">Ogl.glCompileShader</a></li>
<li><a href="#Ogl_glCompileShaderIncludeARB">Ogl.glCompileShaderIncludeARB</a></li>
<li><a href="#Ogl_glCompressedTexImage1D">Ogl.glCompressedTexImage1D</a></li>
<li><a href="#Ogl_glCompressedTexImage2D">Ogl.glCompressedTexImage2D</a></li>
<li><a href="#Ogl_glCompressedTexImage3D">Ogl.glCompressedTexImage3D</a></li>
<li><a href="#Ogl_glCompressedTexSubImage1D">Ogl.glCompressedTexSubImage1D</a></li>
<li><a href="#Ogl_glCompressedTexSubImage2D">Ogl.glCompressedTexSubImage2D</a></li>
<li><a href="#Ogl_glCompressedTexSubImage3D">Ogl.glCompressedTexSubImage3D</a></li>
<li><a href="#Ogl_glCopyPixels">Ogl.glCopyPixels</a></li>
<li><a href="#Ogl_glCopyTexImage1D">Ogl.glCopyTexImage1D</a></li>
<li><a href="#Ogl_glCopyTexImage2D">Ogl.glCopyTexImage2D</a></li>
<li><a href="#Ogl_glCopyTexSubImage1D">Ogl.glCopyTexSubImage1D</a></li>
<li><a href="#Ogl_glCopyTexSubImage2D">Ogl.glCopyTexSubImage2D</a></li>
<li><a href="#Ogl_glCreateProgram">Ogl.glCreateProgram</a></li>
<li><a href="#Ogl_glCreateShader">Ogl.glCreateShader</a></li>
<li><a href="#Ogl_glCullFace">Ogl.glCullFace</a></li>
<li><a href="#Ogl_glDeleteBuffers">Ogl.glDeleteBuffers</a></li>
<li><a href="#Ogl_glDeleteFramebuffers">Ogl.glDeleteFramebuffers</a></li>
<li><a href="#Ogl_glDeleteLists">Ogl.glDeleteLists</a></li>
<li><a href="#Ogl_glDeleteNamedStringARB">Ogl.glDeleteNamedStringARB</a></li>
<li><a href="#Ogl_glDeleteProgram">Ogl.glDeleteProgram</a></li>
<li><a href="#Ogl_glDeleteQueries">Ogl.glDeleteQueries</a></li>
<li><a href="#Ogl_glDeleteRenderbuffers">Ogl.glDeleteRenderbuffers</a></li>
<li><a href="#Ogl_glDeleteShader">Ogl.glDeleteShader</a></li>
<li><a href="#Ogl_glDeleteTextures">Ogl.glDeleteTextures</a></li>
<li><a href="#Ogl_glDepthFunc">Ogl.glDepthFunc</a></li>
<li><a href="#Ogl_glDepthMask">Ogl.glDepthMask</a></li>
<li><a href="#Ogl_glDepthRange">Ogl.glDepthRange</a></li>
<li><a href="#Ogl_glDetachShader">Ogl.glDetachShader</a></li>
<li><a href="#Ogl_glDisable">Ogl.glDisable</a></li>
<li><a href="#Ogl_glDisableClientState">Ogl.glDisableClientState</a></li>
<li><a href="#Ogl_glDisableVertexAttribArray">Ogl.glDisableVertexAttribArray</a></li>
<li><a href="#Ogl_glDisablei">Ogl.glDisablei</a></li>
<li><a href="#Ogl_glDrawArrays">Ogl.glDrawArrays</a></li>
<li><a href="#Ogl_glDrawArraysInstanced">Ogl.glDrawArraysInstanced</a></li>
<li><a href="#Ogl_glDrawBuffer">Ogl.glDrawBuffer</a></li>
<li><a href="#Ogl_glDrawBuffers">Ogl.glDrawBuffers</a></li>
<li><a href="#Ogl_glDrawElements">Ogl.glDrawElements</a></li>
<li><a href="#Ogl_glDrawElementsInstanced">Ogl.glDrawElementsInstanced</a></li>
<li><a href="#Ogl_glDrawPixels">Ogl.glDrawPixels</a></li>
<li><a href="#Ogl_glEdgeFlag">Ogl.glEdgeFlag</a></li>
<li><a href="#Ogl_glEdgeFlagPointer">Ogl.glEdgeFlagPointer</a></li>
<li><a href="#Ogl_glEdgeFlagv">Ogl.glEdgeFlagv</a></li>
<li><a href="#Ogl_glEnable">Ogl.glEnable</a></li>
<li><a href="#Ogl_glEnableClientState">Ogl.glEnableClientState</a></li>
<li><a href="#Ogl_glEnableVertexAttribArray">Ogl.glEnableVertexAttribArray</a></li>
<li><a href="#Ogl_glEnablei">Ogl.glEnablei</a></li>
<li><a href="#Ogl_glEnd">Ogl.glEnd</a></li>
<li><a href="#Ogl_glEndConditionalRender">Ogl.glEndConditionalRender</a></li>
<li><a href="#Ogl_glEndList">Ogl.glEndList</a></li>
<li><a href="#Ogl_glEndQuery">Ogl.glEndQuery</a></li>
<li><a href="#Ogl_glEndTransformFeedback">Ogl.glEndTransformFeedback</a></li>
<li><a href="#Ogl_glEvalCoord1d">Ogl.glEvalCoord1d</a></li>
<li><a href="#Ogl_glEvalCoord1dv">Ogl.glEvalCoord1dv</a></li>
<li><a href="#Ogl_glEvalCoord1f">Ogl.glEvalCoord1f</a></li>
<li><a href="#Ogl_glEvalCoord1fv">Ogl.glEvalCoord1fv</a></li>
<li><a href="#Ogl_glEvalCoord2d">Ogl.glEvalCoord2d</a></li>
<li><a href="#Ogl_glEvalCoord2dv">Ogl.glEvalCoord2dv</a></li>
<li><a href="#Ogl_glEvalCoord2f">Ogl.glEvalCoord2f</a></li>
<li><a href="#Ogl_glEvalCoord2fv">Ogl.glEvalCoord2fv</a></li>
<li><a href="#Ogl_glEvalMesh1">Ogl.glEvalMesh1</a></li>
<li><a href="#Ogl_glEvalMesh2">Ogl.glEvalMesh2</a></li>
<li><a href="#Ogl_glEvalPoint1">Ogl.glEvalPoint1</a></li>
<li><a href="#Ogl_glEvalPoint2">Ogl.glEvalPoint2</a></li>
<li><a href="#Ogl_glFeedbackBuffer">Ogl.glFeedbackBuffer</a></li>
<li><a href="#Ogl_glFinish">Ogl.glFinish</a></li>
<li><a href="#Ogl_glFlush">Ogl.glFlush</a></li>
<li><a href="#Ogl_glFogCoordPointer">Ogl.glFogCoordPointer</a></li>
<li><a href="#Ogl_glFogCoordd">Ogl.glFogCoordd</a></li>
<li><a href="#Ogl_glFogCoorddv">Ogl.glFogCoorddv</a></li>
<li><a href="#Ogl_glFogCoordf">Ogl.glFogCoordf</a></li>
<li><a href="#Ogl_glFogCoordfv">Ogl.glFogCoordfv</a></li>
<li><a href="#Ogl_glFogf">Ogl.glFogf</a></li>
<li><a href="#Ogl_glFogfv">Ogl.glFogfv</a></li>
<li><a href="#Ogl_glFogi">Ogl.glFogi</a></li>
<li><a href="#Ogl_glFogiv">Ogl.glFogiv</a></li>
<li><a href="#Ogl_glFramebufferParameteri">Ogl.glFramebufferParameteri</a></li>
<li><a href="#Ogl_glFramebufferRenderbuffer">Ogl.glFramebufferRenderbuffer</a></li>
<li><a href="#Ogl_glFramebufferTexture">Ogl.glFramebufferTexture</a></li>
<li><a href="#Ogl_glFramebufferTexture1D">Ogl.glFramebufferTexture1D</a></li>
<li><a href="#Ogl_glFramebufferTexture2D">Ogl.glFramebufferTexture2D</a></li>
<li><a href="#Ogl_glFramebufferTexture3D">Ogl.glFramebufferTexture3D</a></li>
<li><a href="#Ogl_glFramebufferTextureLayer">Ogl.glFramebufferTextureLayer</a></li>
<li><a href="#Ogl_glFrontFace">Ogl.glFrontFace</a></li>
<li><a href="#Ogl_glFrustum">Ogl.glFrustum</a></li>
<li><a href="#Ogl_glGenBuffers">Ogl.glGenBuffers</a></li>
<li><a href="#Ogl_glGenFramebuffers">Ogl.glGenFramebuffers</a></li>
<li><a href="#Ogl_glGenLists">Ogl.glGenLists</a></li>
<li><a href="#Ogl_glGenQueries">Ogl.glGenQueries</a></li>
<li><a href="#Ogl_glGenRenderbuffers">Ogl.glGenRenderbuffers</a></li>
<li><a href="#Ogl_glGenTextures">Ogl.glGenTextures</a></li>
<li><a href="#Ogl_glGenerateMipmap">Ogl.glGenerateMipmap</a></li>
<li><a href="#Ogl_glGetActiveAttrib">Ogl.glGetActiveAttrib</a></li>
<li><a href="#Ogl_glGetActiveUniform">Ogl.glGetActiveUniform</a></li>
<li><a href="#Ogl_glGetAttachedShaders">Ogl.glGetAttachedShaders</a></li>
<li><a href="#Ogl_glGetAttribLocation">Ogl.glGetAttribLocation</a></li>
<li><a href="#Ogl_glGetBooleani_v">Ogl.glGetBooleani_v</a></li>
<li><a href="#Ogl_glGetBooleanv">Ogl.glGetBooleanv</a></li>
<li><a href="#Ogl_glGetBufferParameteri64v">Ogl.glGetBufferParameteri64v</a></li>
<li><a href="#Ogl_glGetBufferParameteriv">Ogl.glGetBufferParameteriv</a></li>
<li><a href="#Ogl_glGetBufferPointerv">Ogl.glGetBufferPointerv</a></li>
<li><a href="#Ogl_glGetBufferSubData">Ogl.glGetBufferSubData</a></li>
<li><a href="#Ogl_glGetClipPlane">Ogl.glGetClipPlane</a></li>
<li><a href="#Ogl_glGetCompressedTexImage">Ogl.glGetCompressedTexImage</a></li>
<li><a href="#Ogl_glGetDoublev">Ogl.glGetDoublev</a></li>
<li><a href="#Ogl_glGetError">Ogl.glGetError</a></li>
<li><a href="#Ogl_glGetFloatv">Ogl.glGetFloatv</a></li>
<li><a href="#Ogl_glGetFragDataLocation">Ogl.glGetFragDataLocation</a></li>
<li><a href="#Ogl_glGetFramebufferAttachmentParameteriv">Ogl.glGetFramebufferAttachmentParameteriv</a></li>
<li><a href="#Ogl_glGetFramebufferParameteriv">Ogl.glGetFramebufferParameteriv</a></li>
<li><a href="#Ogl_glGetGraphicsResetStatus">Ogl.glGetGraphicsResetStatus</a></li>
<li><a href="#Ogl_glGetInteger64i_v">Ogl.glGetInteger64i_v</a></li>
<li><a href="#Ogl_glGetIntegerv">Ogl.glGetIntegerv</a></li>
<li><a href="#Ogl_glGetLightfv">Ogl.glGetLightfv</a></li>
<li><a href="#Ogl_glGetLightiv">Ogl.glGetLightiv</a></li>
<li><a href="#Ogl_glGetMapdv">Ogl.glGetMapdv</a></li>
<li><a href="#Ogl_glGetMapfv">Ogl.glGetMapfv</a></li>
<li><a href="#Ogl_glGetMapiv">Ogl.glGetMapiv</a></li>
<li><a href="#Ogl_glGetMaterialfv">Ogl.glGetMaterialfv</a></li>
<li><a href="#Ogl_glGetMaterialiv">Ogl.glGetMaterialiv</a></li>
<li><a href="#Ogl_glGetNamedFramebufferParameterivEXT">Ogl.glGetNamedFramebufferParameterivEXT</a></li>
<li><a href="#Ogl_glGetNamedStringARB">Ogl.glGetNamedStringARB</a></li>
<li><a href="#Ogl_glGetNamedStringivARB">Ogl.glGetNamedStringivARB</a></li>
<li><a href="#Ogl_glGetPixelMapfv">Ogl.glGetPixelMapfv</a></li>
<li><a href="#Ogl_glGetPixelMapuiv">Ogl.glGetPixelMapuiv</a></li>
<li><a href="#Ogl_glGetPixelMapusv">Ogl.glGetPixelMapusv</a></li>
<li><a href="#Ogl_glGetPointerv">Ogl.glGetPointerv</a></li>
<li><a href="#Ogl_glGetPolygonStipple">Ogl.glGetPolygonStipple</a></li>
<li><a href="#Ogl_glGetProgramInfoLog">Ogl.glGetProgramInfoLog</a></li>
<li><a href="#Ogl_glGetProgramiv">Ogl.glGetProgramiv</a></li>
<li><a href="#Ogl_glGetQueryObjectiv">Ogl.glGetQueryObjectiv</a></li>
<li><a href="#Ogl_glGetQueryObjectuiv">Ogl.glGetQueryObjectuiv</a></li>
<li><a href="#Ogl_glGetQueryiv">Ogl.glGetQueryiv</a></li>
<li><a href="#Ogl_glGetRenderbufferParameteriv">Ogl.glGetRenderbufferParameteriv</a></li>
<li><a href="#Ogl_glGetShaderInfoLog">Ogl.glGetShaderInfoLog</a></li>
<li><a href="#Ogl_glGetShaderSource">Ogl.glGetShaderSource</a></li>
<li><a href="#Ogl_glGetShaderiv">Ogl.glGetShaderiv</a></li>
<li><a href="#Ogl_glGetString">Ogl.glGetString</a></li>
<li><a href="#Ogl_glGetStringi">Ogl.glGetStringi</a></li>
<li><a href="#Ogl_glGetTexEnvfv">Ogl.glGetTexEnvfv</a></li>
<li><a href="#Ogl_glGetTexEnviv">Ogl.glGetTexEnviv</a></li>
<li><a href="#Ogl_glGetTexGendv">Ogl.glGetTexGendv</a></li>
<li><a href="#Ogl_glGetTexGenfv">Ogl.glGetTexGenfv</a></li>
<li><a href="#Ogl_glGetTexGeniv">Ogl.glGetTexGeniv</a></li>
<li><a href="#Ogl_glGetTexImage">Ogl.glGetTexImage</a></li>
<li><a href="#Ogl_glGetTexLevelParameterfv">Ogl.glGetTexLevelParameterfv</a></li>
<li><a href="#Ogl_glGetTexLevelParameteriv">Ogl.glGetTexLevelParameteriv</a></li>
<li><a href="#Ogl_glGetTexParameterIiv">Ogl.glGetTexParameterIiv</a></li>
<li><a href="#Ogl_glGetTexParameterIuiv">Ogl.glGetTexParameterIuiv</a></li>
<li><a href="#Ogl_glGetTexParameterfv">Ogl.glGetTexParameterfv</a></li>
<li><a href="#Ogl_glGetTexParameteriv">Ogl.glGetTexParameteriv</a></li>
<li><a href="#Ogl_glGetTransformFeedbackVarying">Ogl.glGetTransformFeedbackVarying</a></li>
<li><a href="#Ogl_glGetUniformLocation">Ogl.glGetUniformLocation</a></li>
<li><a href="#Ogl_glGetUniformfv">Ogl.glGetUniformfv</a></li>
<li><a href="#Ogl_glGetUniformiv">Ogl.glGetUniformiv</a></li>
<li><a href="#Ogl_glGetUniformuiv">Ogl.glGetUniformuiv</a></li>
<li><a href="#Ogl_glGetVertexAttribIiv">Ogl.glGetVertexAttribIiv</a></li>
<li><a href="#Ogl_glGetVertexAttribIuiv">Ogl.glGetVertexAttribIuiv</a></li>
<li><a href="#Ogl_glGetVertexAttribPointerv">Ogl.glGetVertexAttribPointerv</a></li>
<li><a href="#Ogl_glGetVertexAttribdv">Ogl.glGetVertexAttribdv</a></li>
<li><a href="#Ogl_glGetVertexAttribfv">Ogl.glGetVertexAttribfv</a></li>
<li><a href="#Ogl_glGetVertexAttribiv">Ogl.glGetVertexAttribiv</a></li>
<li><a href="#Ogl_glGetnCompressedTexImage">Ogl.glGetnCompressedTexImage</a></li>
<li><a href="#Ogl_glGetnTexImage">Ogl.glGetnTexImage</a></li>
<li><a href="#Ogl_glGetnUniformdv">Ogl.glGetnUniformdv</a></li>
<li><a href="#Ogl_glHint">Ogl.glHint</a></li>
<li><a href="#Ogl_glIndexMask">Ogl.glIndexMask</a></li>
<li><a href="#Ogl_glIndexPointer">Ogl.glIndexPointer</a></li>
<li><a href="#Ogl_glIndexd">Ogl.glIndexd</a></li>
<li><a href="#Ogl_glIndexdv">Ogl.glIndexdv</a></li>
<li><a href="#Ogl_glIndexf">Ogl.glIndexf</a></li>
<li><a href="#Ogl_glIndexfv">Ogl.glIndexfv</a></li>
<li><a href="#Ogl_glIndexi">Ogl.glIndexi</a></li>
<li><a href="#Ogl_glIndexiv">Ogl.glIndexiv</a></li>
<li><a href="#Ogl_glIndexs">Ogl.glIndexs</a></li>
<li><a href="#Ogl_glIndexsv">Ogl.glIndexsv</a></li>
<li><a href="#Ogl_glIndexub">Ogl.glIndexub</a></li>
<li><a href="#Ogl_glIndexubv">Ogl.glIndexubv</a></li>
<li><a href="#Ogl_glInitNames">Ogl.glInitNames</a></li>
<li><a href="#Ogl_glInterleavedArrays">Ogl.glInterleavedArrays</a></li>
<li><a href="#Ogl_glIsBuffer">Ogl.glIsBuffer</a></li>
<li><a href="#Ogl_glIsEnabled">Ogl.glIsEnabled</a></li>
<li><a href="#Ogl_glIsEnabledi">Ogl.glIsEnabledi</a></li>
<li><a href="#Ogl_glIsFramebuffer">Ogl.glIsFramebuffer</a></li>
<li><a href="#Ogl_glIsList">Ogl.glIsList</a></li>
<li><a href="#Ogl_glIsNamedStringARB">Ogl.glIsNamedStringARB</a></li>
<li><a href="#Ogl_glIsProgram">Ogl.glIsProgram</a></li>
<li><a href="#Ogl_glIsQuery">Ogl.glIsQuery</a></li>
<li><a href="#Ogl_glIsRenderbuffer">Ogl.glIsRenderbuffer</a></li>
<li><a href="#Ogl_glIsShader">Ogl.glIsShader</a></li>
<li><a href="#Ogl_glIsTexture">Ogl.glIsTexture</a></li>
<li><a href="#Ogl_glLightModelf">Ogl.glLightModelf</a></li>
<li><a href="#Ogl_glLightModelfv">Ogl.glLightModelfv</a></li>
<li><a href="#Ogl_glLightModeli">Ogl.glLightModeli</a></li>
<li><a href="#Ogl_glLightModeliv">Ogl.glLightModeliv</a></li>
<li><a href="#Ogl_glLightf">Ogl.glLightf</a></li>
<li><a href="#Ogl_glLightfv">Ogl.glLightfv</a></li>
<li><a href="#Ogl_glLighti">Ogl.glLighti</a></li>
<li><a href="#Ogl_glLightiv">Ogl.glLightiv</a></li>
<li><a href="#Ogl_glLineStipple">Ogl.glLineStipple</a></li>
<li><a href="#Ogl_glLineWidth">Ogl.glLineWidth</a></li>
<li><a href="#Ogl_glLinkProgram">Ogl.glLinkProgram</a></li>
<li><a href="#Ogl_glListBase">Ogl.glListBase</a></li>
<li><a href="#Ogl_glLoadIdentity">Ogl.glLoadIdentity</a></li>
<li><a href="#Ogl_glLoadMatrixd">Ogl.glLoadMatrixd</a></li>
<li><a href="#Ogl_glLoadMatrixf">Ogl.glLoadMatrixf</a></li>
<li><a href="#Ogl_glLoadName">Ogl.glLoadName</a></li>
<li><a href="#Ogl_glLoadTransposeMatrixd">Ogl.glLoadTransposeMatrixd</a></li>
<li><a href="#Ogl_glLoadTransposeMatrixf">Ogl.glLoadTransposeMatrixf</a></li>
<li><a href="#Ogl_glLogicOp">Ogl.glLogicOp</a></li>
<li><a href="#Ogl_glMap1d">Ogl.glMap1d</a></li>
<li><a href="#Ogl_glMap1f">Ogl.glMap1f</a></li>
<li><a href="#Ogl_glMap2d">Ogl.glMap2d</a></li>
<li><a href="#Ogl_glMap2f">Ogl.glMap2f</a></li>
<li><a href="#Ogl_glMapBuffer">Ogl.glMapBuffer</a></li>
<li><a href="#Ogl_glMapGrid1d">Ogl.glMapGrid1d</a></li>
<li><a href="#Ogl_glMapGrid1f">Ogl.glMapGrid1f</a></li>
<li><a href="#Ogl_glMapGrid2d">Ogl.glMapGrid2d</a></li>
<li><a href="#Ogl_glMapGrid2f">Ogl.glMapGrid2f</a></li>
<li><a href="#Ogl_glMaterialf">Ogl.glMaterialf</a></li>
<li><a href="#Ogl_glMaterialfv">Ogl.glMaterialfv</a></li>
<li><a href="#Ogl_glMateriali">Ogl.glMateriali</a></li>
<li><a href="#Ogl_glMaterialiv">Ogl.glMaterialiv</a></li>
<li><a href="#Ogl_glMatrixMode">Ogl.glMatrixMode</a></li>
<li><a href="#Ogl_glMinSampleShading">Ogl.glMinSampleShading</a></li>
<li><a href="#Ogl_glMultMatrixd">Ogl.glMultMatrixd</a></li>
<li><a href="#Ogl_glMultMatrixf">Ogl.glMultMatrixf</a></li>
<li><a href="#Ogl_glMultTransposeMatrixd">Ogl.glMultTransposeMatrixd</a></li>
<li><a href="#Ogl_glMultTransposeMatrixf">Ogl.glMultTransposeMatrixf</a></li>
<li><a href="#Ogl_glMultiDrawArrays">Ogl.glMultiDrawArrays</a></li>
<li><a href="#Ogl_glMultiDrawArraysIndirectCount">Ogl.glMultiDrawArraysIndirectCount</a></li>
<li><a href="#Ogl_glMultiDrawElements">Ogl.glMultiDrawElements</a></li>
<li><a href="#Ogl_glMultiDrawElementsIndirectCount">Ogl.glMultiDrawElementsIndirectCount</a></li>
<li><a href="#Ogl_glMultiTexCoord1d">Ogl.glMultiTexCoord1d</a></li>
<li><a href="#Ogl_glMultiTexCoord1dv">Ogl.glMultiTexCoord1dv</a></li>
<li><a href="#Ogl_glMultiTexCoord1f">Ogl.glMultiTexCoord1f</a></li>
<li><a href="#Ogl_glMultiTexCoord1fv">Ogl.glMultiTexCoord1fv</a></li>
<li><a href="#Ogl_glMultiTexCoord1i">Ogl.glMultiTexCoord1i</a></li>
<li><a href="#Ogl_glMultiTexCoord1iv">Ogl.glMultiTexCoord1iv</a></li>
<li><a href="#Ogl_glMultiTexCoord1s">Ogl.glMultiTexCoord1s</a></li>
<li><a href="#Ogl_glMultiTexCoord1sv">Ogl.glMultiTexCoord1sv</a></li>
<li><a href="#Ogl_glMultiTexCoord2d">Ogl.glMultiTexCoord2d</a></li>
<li><a href="#Ogl_glMultiTexCoord2dv">Ogl.glMultiTexCoord2dv</a></li>
<li><a href="#Ogl_glMultiTexCoord2f">Ogl.glMultiTexCoord2f</a></li>
<li><a href="#Ogl_glMultiTexCoord2fv">Ogl.glMultiTexCoord2fv</a></li>
<li><a href="#Ogl_glMultiTexCoord2i">Ogl.glMultiTexCoord2i</a></li>
<li><a href="#Ogl_glMultiTexCoord2iv">Ogl.glMultiTexCoord2iv</a></li>
<li><a href="#Ogl_glMultiTexCoord2s">Ogl.glMultiTexCoord2s</a></li>
<li><a href="#Ogl_glMultiTexCoord2sv">Ogl.glMultiTexCoord2sv</a></li>
<li><a href="#Ogl_glMultiTexCoord3d">Ogl.glMultiTexCoord3d</a></li>
<li><a href="#Ogl_glMultiTexCoord3dv">Ogl.glMultiTexCoord3dv</a></li>
<li><a href="#Ogl_glMultiTexCoord3f">Ogl.glMultiTexCoord3f</a></li>
<li><a href="#Ogl_glMultiTexCoord3fv">Ogl.glMultiTexCoord3fv</a></li>
<li><a href="#Ogl_glMultiTexCoord3i">Ogl.glMultiTexCoord3i</a></li>
<li><a href="#Ogl_glMultiTexCoord3iv">Ogl.glMultiTexCoord3iv</a></li>
<li><a href="#Ogl_glMultiTexCoord3s">Ogl.glMultiTexCoord3s</a></li>
<li><a href="#Ogl_glMultiTexCoord3sv">Ogl.glMultiTexCoord3sv</a></li>
<li><a href="#Ogl_glMultiTexCoord4d">Ogl.glMultiTexCoord4d</a></li>
<li><a href="#Ogl_glMultiTexCoord4dv">Ogl.glMultiTexCoord4dv</a></li>
<li><a href="#Ogl_glMultiTexCoord4f">Ogl.glMultiTexCoord4f</a></li>
<li><a href="#Ogl_glMultiTexCoord4fv">Ogl.glMultiTexCoord4fv</a></li>
<li><a href="#Ogl_glMultiTexCoord4i">Ogl.glMultiTexCoord4i</a></li>
<li><a href="#Ogl_glMultiTexCoord4iv">Ogl.glMultiTexCoord4iv</a></li>
<li><a href="#Ogl_glMultiTexCoord4s">Ogl.glMultiTexCoord4s</a></li>
<li><a href="#Ogl_glMultiTexCoord4sv">Ogl.glMultiTexCoord4sv</a></li>
<li><a href="#Ogl_glNamedFramebufferParameteriEXT">Ogl.glNamedFramebufferParameteriEXT</a></li>
<li><a href="#Ogl_glNamedStringARB">Ogl.glNamedStringARB</a></li>
<li><a href="#Ogl_glNewList">Ogl.glNewList</a></li>
<li><a href="#Ogl_glNormal3b">Ogl.glNormal3b</a></li>
<li><a href="#Ogl_glNormal3bv">Ogl.glNormal3bv</a></li>
<li><a href="#Ogl_glNormal3d">Ogl.glNormal3d</a></li>
<li><a href="#Ogl_glNormal3dv">Ogl.glNormal3dv</a></li>
<li><a href="#Ogl_glNormal3f">Ogl.glNormal3f</a></li>
<li><a href="#Ogl_glNormal3fv">Ogl.glNormal3fv</a></li>
<li><a href="#Ogl_glNormal3i">Ogl.glNormal3i</a></li>
<li><a href="#Ogl_glNormal3iv">Ogl.glNormal3iv</a></li>
<li><a href="#Ogl_glNormal3s">Ogl.glNormal3s</a></li>
<li><a href="#Ogl_glNormal3sv">Ogl.glNormal3sv</a></li>
<li><a href="#Ogl_glNormalPointer">Ogl.glNormalPointer</a></li>
<li><a href="#Ogl_glOrtho">Ogl.glOrtho</a></li>
<li><a href="#Ogl_glPassThrough">Ogl.glPassThrough</a></li>
<li><a href="#Ogl_glPixelMapfv">Ogl.glPixelMapfv</a></li>
<li><a href="#Ogl_glPixelMapuiv">Ogl.glPixelMapuiv</a></li>
<li><a href="#Ogl_glPixelMapusv">Ogl.glPixelMapusv</a></li>
<li><a href="#Ogl_glPixelStoref">Ogl.glPixelStoref</a></li>
<li><a href="#Ogl_glPixelStorei">Ogl.glPixelStorei</a></li>
<li><a href="#Ogl_glPixelTransferf">Ogl.glPixelTransferf</a></li>
<li><a href="#Ogl_glPixelTransferi">Ogl.glPixelTransferi</a></li>
<li><a href="#Ogl_glPixelZoom">Ogl.glPixelZoom</a></li>
<li><a href="#Ogl_glPointParameterf">Ogl.glPointParameterf</a></li>
<li><a href="#Ogl_glPointParameterfv">Ogl.glPointParameterfv</a></li>
<li><a href="#Ogl_glPointParameteri">Ogl.glPointParameteri</a></li>
<li><a href="#Ogl_glPointParameteriv">Ogl.glPointParameteriv</a></li>
<li><a href="#Ogl_glPointSize">Ogl.glPointSize</a></li>
<li><a href="#Ogl_glPolygonMode">Ogl.glPolygonMode</a></li>
<li><a href="#Ogl_glPolygonOffset">Ogl.glPolygonOffset</a></li>
<li><a href="#Ogl_glPolygonStipple">Ogl.glPolygonStipple</a></li>
<li><a href="#Ogl_glPopAttrib">Ogl.glPopAttrib</a></li>
<li><a href="#Ogl_glPopClientAttrib">Ogl.glPopClientAttrib</a></li>
<li><a href="#Ogl_glPopMatrix">Ogl.glPopMatrix</a></li>
<li><a href="#Ogl_glPopName">Ogl.glPopName</a></li>
<li><a href="#Ogl_glPrimitiveRestartIndex">Ogl.glPrimitiveRestartIndex</a></li>
<li><a href="#Ogl_glPrioritizeTextures">Ogl.glPrioritizeTextures</a></li>
<li><a href="#Ogl_glPushAttrib">Ogl.glPushAttrib</a></li>
<li><a href="#Ogl_glPushClientAttrib">Ogl.glPushClientAttrib</a></li>
<li><a href="#Ogl_glPushMatrix">Ogl.glPushMatrix</a></li>
<li><a href="#Ogl_glPushName">Ogl.glPushName</a></li>
<li><a href="#Ogl_glRasterPos2d">Ogl.glRasterPos2d</a></li>
<li><a href="#Ogl_glRasterPos2dv">Ogl.glRasterPos2dv</a></li>
<li><a href="#Ogl_glRasterPos2f">Ogl.glRasterPos2f</a></li>
<li><a href="#Ogl_glRasterPos2fv">Ogl.glRasterPos2fv</a></li>
<li><a href="#Ogl_glRasterPos2i">Ogl.glRasterPos2i</a></li>
<li><a href="#Ogl_glRasterPos2iv">Ogl.glRasterPos2iv</a></li>
<li><a href="#Ogl_glRasterPos2s">Ogl.glRasterPos2s</a></li>
<li><a href="#Ogl_glRasterPos2sv">Ogl.glRasterPos2sv</a></li>
<li><a href="#Ogl_glRasterPos3d">Ogl.glRasterPos3d</a></li>
<li><a href="#Ogl_glRasterPos3dv">Ogl.glRasterPos3dv</a></li>
<li><a href="#Ogl_glRasterPos3f">Ogl.glRasterPos3f</a></li>
<li><a href="#Ogl_glRasterPos3fv">Ogl.glRasterPos3fv</a></li>
<li><a href="#Ogl_glRasterPos3i">Ogl.glRasterPos3i</a></li>
<li><a href="#Ogl_glRasterPos3iv">Ogl.glRasterPos3iv</a></li>
<li><a href="#Ogl_glRasterPos3s">Ogl.glRasterPos3s</a></li>
<li><a href="#Ogl_glRasterPos3sv">Ogl.glRasterPos3sv</a></li>
<li><a href="#Ogl_glRasterPos4d">Ogl.glRasterPos4d</a></li>
<li><a href="#Ogl_glRasterPos4dv">Ogl.glRasterPos4dv</a></li>
<li><a href="#Ogl_glRasterPos4f">Ogl.glRasterPos4f</a></li>
<li><a href="#Ogl_glRasterPos4fv">Ogl.glRasterPos4fv</a></li>
<li><a href="#Ogl_glRasterPos4i">Ogl.glRasterPos4i</a></li>
<li><a href="#Ogl_glRasterPos4iv">Ogl.glRasterPos4iv</a></li>
<li><a href="#Ogl_glRasterPos4s">Ogl.glRasterPos4s</a></li>
<li><a href="#Ogl_glRasterPos4sv">Ogl.glRasterPos4sv</a></li>
<li><a href="#Ogl_glReadBuffer">Ogl.glReadBuffer</a></li>
<li><a href="#Ogl_glReadPixels">Ogl.glReadPixels</a></li>
<li><a href="#Ogl_glRectd">Ogl.glRectd</a></li>
<li><a href="#Ogl_glRectdv">Ogl.glRectdv</a></li>
<li><a href="#Ogl_glRectf">Ogl.glRectf</a></li>
<li><a href="#Ogl_glRectfv">Ogl.glRectfv</a></li>
<li><a href="#Ogl_glRecti">Ogl.glRecti</a></li>
<li><a href="#Ogl_glRectiv">Ogl.glRectiv</a></li>
<li><a href="#Ogl_glRects">Ogl.glRects</a></li>
<li><a href="#Ogl_glRectsv">Ogl.glRectsv</a></li>
<li><a href="#Ogl_glRenderMode">Ogl.glRenderMode</a></li>
<li><a href="#Ogl_glRenderbufferStorage">Ogl.glRenderbufferStorage</a></li>
<li><a href="#Ogl_glRenderbufferStorageMultisample">Ogl.glRenderbufferStorageMultisample</a></li>
<li><a href="#Ogl_glRotated">Ogl.glRotated</a></li>
<li><a href="#Ogl_glRotatef">Ogl.glRotatef</a></li>
<li><a href="#Ogl_glSampleCoverage">Ogl.glSampleCoverage</a></li>
<li><a href="#Ogl_glScaled">Ogl.glScaled</a></li>
<li><a href="#Ogl_glScalef">Ogl.glScalef</a></li>
<li><a href="#Ogl_glScissor">Ogl.glScissor</a></li>
<li><a href="#Ogl_glSecondaryColor3b">Ogl.glSecondaryColor3b</a></li>
<li><a href="#Ogl_glSecondaryColor3bv">Ogl.glSecondaryColor3bv</a></li>
<li><a href="#Ogl_glSecondaryColor3d">Ogl.glSecondaryColor3d</a></li>
<li><a href="#Ogl_glSecondaryColor3dv">Ogl.glSecondaryColor3dv</a></li>
<li><a href="#Ogl_glSecondaryColor3f">Ogl.glSecondaryColor3f</a></li>
<li><a href="#Ogl_glSecondaryColor3fv">Ogl.glSecondaryColor3fv</a></li>
<li><a href="#Ogl_glSecondaryColor3i">Ogl.glSecondaryColor3i</a></li>
<li><a href="#Ogl_glSecondaryColor3iv">Ogl.glSecondaryColor3iv</a></li>
<li><a href="#Ogl_glSecondaryColor3s">Ogl.glSecondaryColor3s</a></li>
<li><a href="#Ogl_glSecondaryColor3sv">Ogl.glSecondaryColor3sv</a></li>
<li><a href="#Ogl_glSecondaryColor3ub">Ogl.glSecondaryColor3ub</a></li>
<li><a href="#Ogl_glSecondaryColor3ubv">Ogl.glSecondaryColor3ubv</a></li>
<li><a href="#Ogl_glSecondaryColor3ui">Ogl.glSecondaryColor3ui</a></li>
<li><a href="#Ogl_glSecondaryColor3uiv">Ogl.glSecondaryColor3uiv</a></li>
<li><a href="#Ogl_glSecondaryColor3us">Ogl.glSecondaryColor3us</a></li>
<li><a href="#Ogl_glSecondaryColor3usv">Ogl.glSecondaryColor3usv</a></li>
<li><a href="#Ogl_glSecondaryColorPointer">Ogl.glSecondaryColorPointer</a></li>
<li><a href="#Ogl_glSelectBuffer">Ogl.glSelectBuffer</a></li>
<li><a href="#Ogl_glShadeModel">Ogl.glShadeModel</a></li>
<li><a href="#Ogl_glShaderSource">Ogl.glShaderSource</a></li>
<li><a href="#Ogl_glSpecializeShader">Ogl.glSpecializeShader</a></li>
<li><a href="#Ogl_glStencilFunc">Ogl.glStencilFunc</a></li>
<li><a href="#Ogl_glStencilFuncSeparate">Ogl.glStencilFuncSeparate</a></li>
<li><a href="#Ogl_glStencilMask">Ogl.glStencilMask</a></li>
<li><a href="#Ogl_glStencilMaskSeparate">Ogl.glStencilMaskSeparate</a></li>
<li><a href="#Ogl_glStencilOp">Ogl.glStencilOp</a></li>
<li><a href="#Ogl_glStencilOpSeparate">Ogl.glStencilOpSeparate</a></li>
<li><a href="#Ogl_glTexBuffer">Ogl.glTexBuffer</a></li>
<li><a href="#Ogl_glTexCoord1d">Ogl.glTexCoord1d</a></li>
<li><a href="#Ogl_glTexCoord1dv">Ogl.glTexCoord1dv</a></li>
<li><a href="#Ogl_glTexCoord1f">Ogl.glTexCoord1f</a></li>
<li><a href="#Ogl_glTexCoord1fv">Ogl.glTexCoord1fv</a></li>
<li><a href="#Ogl_glTexCoord1i">Ogl.glTexCoord1i</a></li>
<li><a href="#Ogl_glTexCoord1iv">Ogl.glTexCoord1iv</a></li>
<li><a href="#Ogl_glTexCoord1s">Ogl.glTexCoord1s</a></li>
<li><a href="#Ogl_glTexCoord1sv">Ogl.glTexCoord1sv</a></li>
<li><a href="#Ogl_glTexCoord2d">Ogl.glTexCoord2d</a></li>
<li><a href="#Ogl_glTexCoord2dv">Ogl.glTexCoord2dv</a></li>
<li><a href="#Ogl_glTexCoord2f">Ogl.glTexCoord2f</a></li>
<li><a href="#Ogl_glTexCoord2fv">Ogl.glTexCoord2fv</a></li>
<li><a href="#Ogl_glTexCoord2i">Ogl.glTexCoord2i</a></li>
<li><a href="#Ogl_glTexCoord2iv">Ogl.glTexCoord2iv</a></li>
<li><a href="#Ogl_glTexCoord2s">Ogl.glTexCoord2s</a></li>
<li><a href="#Ogl_glTexCoord2sv">Ogl.glTexCoord2sv</a></li>
<li><a href="#Ogl_glTexCoord3d">Ogl.glTexCoord3d</a></li>
<li><a href="#Ogl_glTexCoord3dv">Ogl.glTexCoord3dv</a></li>
<li><a href="#Ogl_glTexCoord3f">Ogl.glTexCoord3f</a></li>
<li><a href="#Ogl_glTexCoord3fv">Ogl.glTexCoord3fv</a></li>
<li><a href="#Ogl_glTexCoord3i">Ogl.glTexCoord3i</a></li>
<li><a href="#Ogl_glTexCoord3iv">Ogl.glTexCoord3iv</a></li>
<li><a href="#Ogl_glTexCoord3s">Ogl.glTexCoord3s</a></li>
<li><a href="#Ogl_glTexCoord3sv">Ogl.glTexCoord3sv</a></li>
<li><a href="#Ogl_glTexCoord4d">Ogl.glTexCoord4d</a></li>
<li><a href="#Ogl_glTexCoord4dv">Ogl.glTexCoord4dv</a></li>
<li><a href="#Ogl_glTexCoord4f">Ogl.glTexCoord4f</a></li>
<li><a href="#Ogl_glTexCoord4fv">Ogl.glTexCoord4fv</a></li>
<li><a href="#Ogl_glTexCoord4i">Ogl.glTexCoord4i</a></li>
<li><a href="#Ogl_glTexCoord4iv">Ogl.glTexCoord4iv</a></li>
<li><a href="#Ogl_glTexCoord4s">Ogl.glTexCoord4s</a></li>
<li><a href="#Ogl_glTexCoord4sv">Ogl.glTexCoord4sv</a></li>
<li><a href="#Ogl_glTexCoordPointer">Ogl.glTexCoordPointer</a></li>
<li><a href="#Ogl_glTexEnvf">Ogl.glTexEnvf</a></li>
<li><a href="#Ogl_glTexEnvfv">Ogl.glTexEnvfv</a></li>
<li><a href="#Ogl_glTexEnvi">Ogl.glTexEnvi</a></li>
<li><a href="#Ogl_glTexEnviv">Ogl.glTexEnviv</a></li>
<li><a href="#Ogl_glTexGend">Ogl.glTexGend</a></li>
<li><a href="#Ogl_glTexGendv">Ogl.glTexGendv</a></li>
<li><a href="#Ogl_glTexGenf">Ogl.glTexGenf</a></li>
<li><a href="#Ogl_glTexGenfv">Ogl.glTexGenfv</a></li>
<li><a href="#Ogl_glTexGeni">Ogl.glTexGeni</a></li>
<li><a href="#Ogl_glTexGeniv">Ogl.glTexGeniv</a></li>
<li><a href="#Ogl_glTexImage1D">Ogl.glTexImage1D</a></li>
<li><a href="#Ogl_glTexImage2D">Ogl.glTexImage2D</a></li>
<li><a href="#Ogl_glTexParameterIiv">Ogl.glTexParameterIiv</a></li>
<li><a href="#Ogl_glTexParameterIuiv">Ogl.glTexParameterIuiv</a></li>
<li><a href="#Ogl_glTexParameterf">Ogl.glTexParameterf</a></li>
<li><a href="#Ogl_glTexParameterfv">Ogl.glTexParameterfv</a></li>
<li><a href="#Ogl_glTexParameteri">Ogl.glTexParameteri</a></li>
<li><a href="#Ogl_glTexParameteriv">Ogl.glTexParameteriv</a></li>
<li><a href="#Ogl_glTexSubImage1D">Ogl.glTexSubImage1D</a></li>
<li><a href="#Ogl_glTexSubImage2D">Ogl.glTexSubImage2D</a></li>
<li><a href="#Ogl_glTransformFeedbackVaryings">Ogl.glTransformFeedbackVaryings</a></li>
<li><a href="#Ogl_glTranslated">Ogl.glTranslated</a></li>
<li><a href="#Ogl_glTranslatef">Ogl.glTranslatef</a></li>
<li><a href="#Ogl_glUniform1f">Ogl.glUniform1f</a></li>
<li><a href="#Ogl_glUniform1fv">Ogl.glUniform1fv</a></li>
<li><a href="#Ogl_glUniform1i">Ogl.glUniform1i</a></li>
<li><a href="#Ogl_glUniform1iv">Ogl.glUniform1iv</a></li>
<li><a href="#Ogl_glUniform1ui">Ogl.glUniform1ui</a></li>
<li><a href="#Ogl_glUniform1uiv">Ogl.glUniform1uiv</a></li>
<li><a href="#Ogl_glUniform2f">Ogl.glUniform2f</a></li>
<li><a href="#Ogl_glUniform2fv">Ogl.glUniform2fv</a></li>
<li><a href="#Ogl_glUniform2i">Ogl.glUniform2i</a></li>
<li><a href="#Ogl_glUniform2iv">Ogl.glUniform2iv</a></li>
<li><a href="#Ogl_glUniform2ui">Ogl.glUniform2ui</a></li>
<li><a href="#Ogl_glUniform2uiv">Ogl.glUniform2uiv</a></li>
<li><a href="#Ogl_glUniform3f">Ogl.glUniform3f</a></li>
<li><a href="#Ogl_glUniform3fv">Ogl.glUniform3fv</a></li>
<li><a href="#Ogl_glUniform3i">Ogl.glUniform3i</a></li>
<li><a href="#Ogl_glUniform3iv">Ogl.glUniform3iv</a></li>
<li><a href="#Ogl_glUniform3ui">Ogl.glUniform3ui</a></li>
<li><a href="#Ogl_glUniform3uiv">Ogl.glUniform3uiv</a></li>
<li><a href="#Ogl_glUniform4f">Ogl.glUniform4f</a></li>
<li><a href="#Ogl_glUniform4fv">Ogl.glUniform4fv</a></li>
<li><a href="#Ogl_glUniform4i">Ogl.glUniform4i</a></li>
<li><a href="#Ogl_glUniform4iv">Ogl.glUniform4iv</a></li>
<li><a href="#Ogl_glUniform4ui">Ogl.glUniform4ui</a></li>
<li><a href="#Ogl_glUniform4uiv">Ogl.glUniform4uiv</a></li>
<li><a href="#Ogl_glUniformMatrix2fv">Ogl.glUniformMatrix2fv</a></li>
<li><a href="#Ogl_glUniformMatrix2x3fv">Ogl.glUniformMatrix2x3fv</a></li>
<li><a href="#Ogl_glUniformMatrix2x4fv">Ogl.glUniformMatrix2x4fv</a></li>
<li><a href="#Ogl_glUniformMatrix3fv">Ogl.glUniformMatrix3fv</a></li>
<li><a href="#Ogl_glUniformMatrix3x2fv">Ogl.glUniformMatrix3x2fv</a></li>
<li><a href="#Ogl_glUniformMatrix3x4fv">Ogl.glUniformMatrix3x4fv</a></li>
<li><a href="#Ogl_glUniformMatrix4fv">Ogl.glUniformMatrix4fv</a></li>
<li><a href="#Ogl_glUniformMatrix4x2fv">Ogl.glUniformMatrix4x2fv</a></li>
<li><a href="#Ogl_glUniformMatrix4x3fv">Ogl.glUniformMatrix4x3fv</a></li>
<li><a href="#Ogl_glUnmapBuffer">Ogl.glUnmapBuffer</a></li>
<li><a href="#Ogl_glUseProgram">Ogl.glUseProgram</a></li>
<li><a href="#Ogl_glValidateProgram">Ogl.glValidateProgram</a></li>
<li><a href="#Ogl_glVertex2d">Ogl.glVertex2d</a></li>
<li><a href="#Ogl_glVertex2dv">Ogl.glVertex2dv</a></li>
<li><a href="#Ogl_glVertex2f">Ogl.glVertex2f</a></li>
<li><a href="#Ogl_glVertex2fv">Ogl.glVertex2fv</a></li>
<li><a href="#Ogl_glVertex2i">Ogl.glVertex2i</a></li>
<li><a href="#Ogl_glVertex2iv">Ogl.glVertex2iv</a></li>
<li><a href="#Ogl_glVertex2s">Ogl.glVertex2s</a></li>
<li><a href="#Ogl_glVertex2sv">Ogl.glVertex2sv</a></li>
<li><a href="#Ogl_glVertex3d">Ogl.glVertex3d</a></li>
<li><a href="#Ogl_glVertex3dv">Ogl.glVertex3dv</a></li>
<li><a href="#Ogl_glVertex3f">Ogl.glVertex3f</a></li>
<li><a href="#Ogl_glVertex3fv">Ogl.glVertex3fv</a></li>
<li><a href="#Ogl_glVertex3i">Ogl.glVertex3i</a></li>
<li><a href="#Ogl_glVertex3iv">Ogl.glVertex3iv</a></li>
<li><a href="#Ogl_glVertex3s">Ogl.glVertex3s</a></li>
<li><a href="#Ogl_glVertex3sv">Ogl.glVertex3sv</a></li>
<li><a href="#Ogl_glVertex4d">Ogl.glVertex4d</a></li>
<li><a href="#Ogl_glVertex4dv">Ogl.glVertex4dv</a></li>
<li><a href="#Ogl_glVertex4f">Ogl.glVertex4f</a></li>
<li><a href="#Ogl_glVertex4fv">Ogl.glVertex4fv</a></li>
<li><a href="#Ogl_glVertex4i">Ogl.glVertex4i</a></li>
<li><a href="#Ogl_glVertex4iv">Ogl.glVertex4iv</a></li>
<li><a href="#Ogl_glVertex4s">Ogl.glVertex4s</a></li>
<li><a href="#Ogl_glVertex4sv">Ogl.glVertex4sv</a></li>
<li><a href="#Ogl_glVertexAttrib1d">Ogl.glVertexAttrib1d</a></li>
<li><a href="#Ogl_glVertexAttrib1dv">Ogl.glVertexAttrib1dv</a></li>
<li><a href="#Ogl_glVertexAttrib1f">Ogl.glVertexAttrib1f</a></li>
<li><a href="#Ogl_glVertexAttrib1fv">Ogl.glVertexAttrib1fv</a></li>
<li><a href="#Ogl_glVertexAttrib1s">Ogl.glVertexAttrib1s</a></li>
<li><a href="#Ogl_glVertexAttrib1sv">Ogl.glVertexAttrib1sv</a></li>
<li><a href="#Ogl_glVertexAttrib2d">Ogl.glVertexAttrib2d</a></li>
<li><a href="#Ogl_glVertexAttrib2dv">Ogl.glVertexAttrib2dv</a></li>
<li><a href="#Ogl_glVertexAttrib2f">Ogl.glVertexAttrib2f</a></li>
<li><a href="#Ogl_glVertexAttrib2fv">Ogl.glVertexAttrib2fv</a></li>
<li><a href="#Ogl_glVertexAttrib2s">Ogl.glVertexAttrib2s</a></li>
<li><a href="#Ogl_glVertexAttrib2sv">Ogl.glVertexAttrib2sv</a></li>
<li><a href="#Ogl_glVertexAttrib3d">Ogl.glVertexAttrib3d</a></li>
<li><a href="#Ogl_glVertexAttrib3dv">Ogl.glVertexAttrib3dv</a></li>
<li><a href="#Ogl_glVertexAttrib3f">Ogl.glVertexAttrib3f</a></li>
<li><a href="#Ogl_glVertexAttrib3fv">Ogl.glVertexAttrib3fv</a></li>
<li><a href="#Ogl_glVertexAttrib3s">Ogl.glVertexAttrib3s</a></li>
<li><a href="#Ogl_glVertexAttrib3sv">Ogl.glVertexAttrib3sv</a></li>
<li><a href="#Ogl_glVertexAttrib4Nbv">Ogl.glVertexAttrib4Nbv</a></li>
<li><a href="#Ogl_glVertexAttrib4Niv">Ogl.glVertexAttrib4Niv</a></li>
<li><a href="#Ogl_glVertexAttrib4Nsv">Ogl.glVertexAttrib4Nsv</a></li>
<li><a href="#Ogl_glVertexAttrib4Nub">Ogl.glVertexAttrib4Nub</a></li>
<li><a href="#Ogl_glVertexAttrib4Nubv">Ogl.glVertexAttrib4Nubv</a></li>
<li><a href="#Ogl_glVertexAttrib4Nuiv">Ogl.glVertexAttrib4Nuiv</a></li>
<li><a href="#Ogl_glVertexAttrib4Nusv">Ogl.glVertexAttrib4Nusv</a></li>
<li><a href="#Ogl_glVertexAttrib4bv">Ogl.glVertexAttrib4bv</a></li>
<li><a href="#Ogl_glVertexAttrib4d">Ogl.glVertexAttrib4d</a></li>
<li><a href="#Ogl_glVertexAttrib4dv">Ogl.glVertexAttrib4dv</a></li>
<li><a href="#Ogl_glVertexAttrib4f">Ogl.glVertexAttrib4f</a></li>
<li><a href="#Ogl_glVertexAttrib4fv">Ogl.glVertexAttrib4fv</a></li>
<li><a href="#Ogl_glVertexAttrib4iv">Ogl.glVertexAttrib4iv</a></li>
<li><a href="#Ogl_glVertexAttrib4s">Ogl.glVertexAttrib4s</a></li>
<li><a href="#Ogl_glVertexAttrib4sv">Ogl.glVertexAttrib4sv</a></li>
<li><a href="#Ogl_glVertexAttrib4ubv">Ogl.glVertexAttrib4ubv</a></li>
<li><a href="#Ogl_glVertexAttrib4uiv">Ogl.glVertexAttrib4uiv</a></li>
<li><a href="#Ogl_glVertexAttrib4usv">Ogl.glVertexAttrib4usv</a></li>
<li><a href="#Ogl_glVertexAttribDivisor">Ogl.glVertexAttribDivisor</a></li>
<li><a href="#Ogl_glVertexAttribI1i">Ogl.glVertexAttribI1i</a></li>
<li><a href="#Ogl_glVertexAttribI1iv">Ogl.glVertexAttribI1iv</a></li>
<li><a href="#Ogl_glVertexAttribI1ui">Ogl.glVertexAttribI1ui</a></li>
<li><a href="#Ogl_glVertexAttribI1uiv">Ogl.glVertexAttribI1uiv</a></li>
<li><a href="#Ogl_glVertexAttribI2i">Ogl.glVertexAttribI2i</a></li>
<li><a href="#Ogl_glVertexAttribI2iv">Ogl.glVertexAttribI2iv</a></li>
<li><a href="#Ogl_glVertexAttribI2ui">Ogl.glVertexAttribI2ui</a></li>
<li><a href="#Ogl_glVertexAttribI2uiv">Ogl.glVertexAttribI2uiv</a></li>
<li><a href="#Ogl_glVertexAttribI3i">Ogl.glVertexAttribI3i</a></li>
<li><a href="#Ogl_glVertexAttribI3iv">Ogl.glVertexAttribI3iv</a></li>
<li><a href="#Ogl_glVertexAttribI3ui">Ogl.glVertexAttribI3ui</a></li>
<li><a href="#Ogl_glVertexAttribI3uiv">Ogl.glVertexAttribI3uiv</a></li>
<li><a href="#Ogl_glVertexAttribI4bv">Ogl.glVertexAttribI4bv</a></li>
<li><a href="#Ogl_glVertexAttribI4i">Ogl.glVertexAttribI4i</a></li>
<li><a href="#Ogl_glVertexAttribI4iv">Ogl.glVertexAttribI4iv</a></li>
<li><a href="#Ogl_glVertexAttribI4sv">Ogl.glVertexAttribI4sv</a></li>
<li><a href="#Ogl_glVertexAttribI4ubv">Ogl.glVertexAttribI4ubv</a></li>
<li><a href="#Ogl_glVertexAttribI4ui">Ogl.glVertexAttribI4ui</a></li>
<li><a href="#Ogl_glVertexAttribI4uiv">Ogl.glVertexAttribI4uiv</a></li>
<li><a href="#Ogl_glVertexAttribI4usv">Ogl.glVertexAttribI4usv</a></li>
<li><a href="#Ogl_glVertexAttribIPointer">Ogl.glVertexAttribIPointer</a></li>
<li><a href="#Ogl_glVertexAttribPointer">Ogl.glVertexAttribPointer</a></li>
<li><a href="#Ogl_glVertexPointer">Ogl.glVertexPointer</a></li>
<li><a href="#Ogl_glViewport">Ogl.glViewport</a></li>
<li><a href="#Ogl_glWindowPos2d">Ogl.glWindowPos2d</a></li>
<li><a href="#Ogl_glWindowPos2dv">Ogl.glWindowPos2dv</a></li>
<li><a href="#Ogl_glWindowPos2f">Ogl.glWindowPos2f</a></li>
<li><a href="#Ogl_glWindowPos2fv">Ogl.glWindowPos2fv</a></li>
<li><a href="#Ogl_glWindowPos2i">Ogl.glWindowPos2i</a></li>
<li><a href="#Ogl_glWindowPos2iv">Ogl.glWindowPos2iv</a></li>
<li><a href="#Ogl_glWindowPos2s">Ogl.glWindowPos2s</a></li>
<li><a href="#Ogl_glWindowPos2sv">Ogl.glWindowPos2sv</a></li>
<li><a href="#Ogl_glWindowPos3d">Ogl.glWindowPos3d</a></li>
<li><a href="#Ogl_glWindowPos3dv">Ogl.glWindowPos3dv</a></li>
<li><a href="#Ogl_glWindowPos3f">Ogl.glWindowPos3f</a></li>
<li><a href="#Ogl_glWindowPos3fv">Ogl.glWindowPos3fv</a></li>
<li><a href="#Ogl_glWindowPos3i">Ogl.glWindowPos3i</a></li>
<li><a href="#Ogl_glWindowPos3iv">Ogl.glWindowPos3iv</a></li>
<li><a href="#Ogl_glWindowPos3s">Ogl.glWindowPos3s</a></li>
<li><a href="#Ogl_glWindowPos3sv">Ogl.glWindowPos3sv</a></li>
<li><a href="#Ogl_wglCreateContext">Ogl.wglCreateContext</a></li>
<li><a href="#Ogl_wglDeleteContext">Ogl.wglDeleteContext</a></li>
<li><a href="#Ogl_wglGetProcAddress">Ogl.wglGetProcAddress</a></li>
<li><a href="#Ogl_wglMakeCurrent">Ogl.wglMakeCurrent</a></li>
<li><a href="#Ogl_wglShareLists">Ogl.wglShareLists</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="right-page">
<h1>Module ogl</h1>
<h1>Content</h1>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_GL_2D"><span class="api-item-title-kind">const</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">Constants</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L140" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td id="Ogl_GL_2D">GL_2D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_2_BYTES">GL_2_BYTES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_3D">GL_3D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_3D_COLOR">GL_3D_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_3D_COLOR_TEXTURE">GL_3D_COLOR_TEXTURE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_3_BYTES">GL_3_BYTES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_4D_COLOR_TEXTURE">GL_4D_COLOR_TEXTURE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_4_BYTES">GL_4_BYTES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACCUM">GL_ACCUM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACCUM_ALPHA_BITS">GL_ACCUM_ALPHA_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACCUM_BLUE_BITS">GL_ACCUM_BLUE_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACCUM_BUFFER_BIT">GL_ACCUM_BUFFER_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACCUM_CLEAR_VALUE">GL_ACCUM_CLEAR_VALUE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACCUM_GREEN_BITS">GL_ACCUM_GREEN_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACCUM_RED_BITS">GL_ACCUM_RED_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACTIVE_ATTRIBUTES">GL_ACTIVE_ATTRIBUTES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACTIVE_ATTRIBUTE_MAX_LENGTH">GL_ACTIVE_ATTRIBUTE_MAX_LENGTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACTIVE_TEXTURE">GL_ACTIVE_TEXTURE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACTIVE_UNIFORMS">GL_ACTIVE_UNIFORMS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ACTIVE_UNIFORM_MAX_LENGTH">GL_ACTIVE_UNIFORM_MAX_LENGTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ADD">GL_ADD</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ADD_SIGNED">GL_ADD_SIGNED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALIASED_LINE_WIDTH_RANGE">GL_ALIASED_LINE_WIDTH_RANGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALIASED_POINT_SIZE_RANGE">GL_ALIASED_POINT_SIZE_RANGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALL_ATTRIB_BITS">GL_ALL_ATTRIB_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA">GL_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA12">GL_ALPHA12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA16">GL_ALPHA16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA4">GL_ALPHA4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA8">GL_ALPHA8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA_BIAS">GL_ALPHA_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA_BITS">GL_ALPHA_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA_INTEGER">GL_ALPHA_INTEGER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA_SCALE">GL_ALPHA_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA_TEST">GL_ALPHA_TEST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA_TEST_FUNC">GL_ALPHA_TEST_FUNC</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALPHA_TEST_REF">GL_ALPHA_TEST_REF</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ALWAYS">GL_ALWAYS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AMBIENT">GL_AMBIENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AMBIENT_AND_DIFFUSE">GL_AMBIENT_AND_DIFFUSE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AND">GL_AND</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AND_INVERTED">GL_AND_INVERTED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AND_REVERSE">GL_AND_REVERSE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ARRAY_BUFFER">GL_ARRAY_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ARRAY_BUFFER_BINDING">GL_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ATTACHED_SHADERS">GL_ATTACHED_SHADERS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ATTRIB_STACK_DEPTH">GL_ATTRIB_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AUTO_NORMAL">GL_AUTO_NORMAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AUX0">GL_AUX0</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AUX1">GL_AUX1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AUX2">GL_AUX2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AUX3">GL_AUX3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_AUX_BUFFERS">GL_AUX_BUFFERS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BACK">GL_BACK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BACK_LEFT">GL_BACK_LEFT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BACK_RIGHT">GL_BACK_RIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BGR">GL_BGR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BGRA">GL_BGRA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BGRA_EXT">GL_BGRA_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BGRA_INTEGER">GL_BGRA_INTEGER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BGR_EXT">GL_BGR_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BGR_INTEGER">GL_BGR_INTEGER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BITMAP">GL_BITMAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BITMAP_TOKEN">GL_BITMAP_TOKEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND">GL_BLEND</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_COLOR">GL_BLEND_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_DST">GL_BLEND_DST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_DST_ALPHA">GL_BLEND_DST_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_DST_RGB">GL_BLEND_DST_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_EQUATION">GL_BLEND_EQUATION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_EQUATION_ALPHA">GL_BLEND_EQUATION_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_EQUATION_RGB">GL_BLEND_EQUATION_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_SRC">GL_BLEND_SRC</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_SRC_ALPHA">GL_BLEND_SRC_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLEND_SRC_RGB">GL_BLEND_SRC_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLUE">GL_BLUE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLUE_BIAS">GL_BLUE_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLUE_BITS">GL_BLUE_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLUE_INTEGER">GL_BLUE_INTEGER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BLUE_SCALE">GL_BLUE_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BOOL">GL_BOOL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BOOL_VEC2">GL_BOOL_VEC2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BOOL_VEC3">GL_BOOL_VEC3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BOOL_VEC4">GL_BOOL_VEC4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BUFFER_ACCESS">GL_BUFFER_ACCESS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BUFFER_ACCESS_FLAGS">GL_BUFFER_ACCESS_FLAGS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BUFFER_MAPPED">GL_BUFFER_MAPPED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BUFFER_MAP_LENGTH">GL_BUFFER_MAP_LENGTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BUFFER_MAP_OFFSET">GL_BUFFER_MAP_OFFSET</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BUFFER_MAP_POINTER">GL_BUFFER_MAP_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BUFFER_SIZE">GL_BUFFER_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BUFFER_USAGE">GL_BUFFER_USAGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_BYTE">GL_BYTE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_C3F_V3F">GL_C3F_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_C4F_N3F_V3F">GL_C4F_N3F_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_C4UB_V2F">GL_C4UB_V2F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_C4UB_V3F">GL_C4UB_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CCW">GL_CCW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLAMP">GL_CLAMP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLAMP_FRAGMENT_COLOR">GL_CLAMP_FRAGMENT_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLAMP_READ_COLOR">GL_CLAMP_READ_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLAMP_TO_BORDER">GL_CLAMP_TO_BORDER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLAMP_TO_EDGE">GL_CLAMP_TO_EDGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLAMP_VERTEX_COLOR">GL_CLAMP_VERTEX_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLEAR">GL_CLEAR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIENT_ACTIVE_TEXTURE">GL_CLIENT_ACTIVE_TEXTURE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIENT_ALL_ATTRIB_BITS">GL_CLIENT_ALL_ATTRIB_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIENT_ATTRIB_STACK_DEPTH">GL_CLIENT_ATTRIB_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIENT_PIXEL_STORE_BIT">GL_CLIENT_PIXEL_STORE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIENT_VERTEX_ARRAY_BIT">GL_CLIENT_VERTEX_ARRAY_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIPPING_INPUT_PRIMITIVES">GL_CLIPPING_INPUT_PRIMITIVES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIPPING_OUTPUT_PRIMITIVES">GL_CLIPPING_OUTPUT_PRIMITIVES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_DISTANCE0">GL_CLIP_DISTANCE0</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_DISTANCE1">GL_CLIP_DISTANCE1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_DISTANCE2">GL_CLIP_DISTANCE2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_DISTANCE3">GL_CLIP_DISTANCE3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_DISTANCE4">GL_CLIP_DISTANCE4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_DISTANCE5">GL_CLIP_DISTANCE5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_PLANE0">GL_CLIP_PLANE0</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_PLANE1">GL_CLIP_PLANE1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_PLANE2">GL_CLIP_PLANE2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_PLANE3">GL_CLIP_PLANE3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_PLANE4">GL_CLIP_PLANE4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CLIP_PLANE5">GL_CLIP_PLANE5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COEFF">GL_COEFF</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR">GL_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY">GL_COLOR_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_BUFFER_BINDING">GL_COLOR_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_COUNT_EXT">GL_COLOR_ARRAY_COUNT_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_EXT">GL_COLOR_ARRAY_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_POINTER">GL_COLOR_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_POINTER_EXT">GL_COLOR_ARRAY_POINTER_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_SIZE">GL_COLOR_ARRAY_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_SIZE_EXT">GL_COLOR_ARRAY_SIZE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_STRIDE">GL_COLOR_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_STRIDE_EXT">GL_COLOR_ARRAY_STRIDE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_TYPE">GL_COLOR_ARRAY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ARRAY_TYPE_EXT">GL_COLOR_ARRAY_TYPE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT0">GL_COLOR_ATTACHMENT0</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT1">GL_COLOR_ATTACHMENT1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT10">GL_COLOR_ATTACHMENT10</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT11">GL_COLOR_ATTACHMENT11</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT12">GL_COLOR_ATTACHMENT12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT13">GL_COLOR_ATTACHMENT13</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT14">GL_COLOR_ATTACHMENT14</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT15">GL_COLOR_ATTACHMENT15</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT2">GL_COLOR_ATTACHMENT2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT3">GL_COLOR_ATTACHMENT3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT4">GL_COLOR_ATTACHMENT4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT5">GL_COLOR_ATTACHMENT5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT6">GL_COLOR_ATTACHMENT6</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT7">GL_COLOR_ATTACHMENT7</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT8">GL_COLOR_ATTACHMENT8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_ATTACHMENT9">GL_COLOR_ATTACHMENT9</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_BUFFER_BIT">GL_COLOR_BUFFER_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_CLEAR_VALUE">GL_COLOR_CLEAR_VALUE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_INDEX">GL_COLOR_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_INDEX12_EXT">GL_COLOR_INDEX12_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_INDEX16_EXT">GL_COLOR_INDEX16_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_INDEX1_EXT">GL_COLOR_INDEX1_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_INDEX2_EXT">GL_COLOR_INDEX2_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_INDEX4_EXT">GL_COLOR_INDEX4_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_INDEX8_EXT">GL_COLOR_INDEX8_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_INDEXES">GL_COLOR_INDEXES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_LOGIC_OP">GL_COLOR_LOGIC_OP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_MATERIAL">GL_COLOR_MATERIAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_MATERIAL_FACE">GL_COLOR_MATERIAL_FACE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_MATERIAL_PARAMETER">GL_COLOR_MATERIAL_PARAMETER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_MATRIX">GL_COLOR_MATRIX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_MATRIX_STACK_DEPTH">GL_COLOR_MATRIX_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_SUM">GL_COLOR_SUM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE">GL_COLOR_TABLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_ALPHA_SIZE">GL_COLOR_TABLE_ALPHA_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_ALPHA_SIZE_EXT">GL_COLOR_TABLE_ALPHA_SIZE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_BIAS">GL_COLOR_TABLE_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_BLUE_SIZE">GL_COLOR_TABLE_BLUE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_BLUE_SIZE_EXT">GL_COLOR_TABLE_BLUE_SIZE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_FORMAT">GL_COLOR_TABLE_FORMAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_FORMAT_EXT">GL_COLOR_TABLE_FORMAT_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_GREEN_SIZE">GL_COLOR_TABLE_GREEN_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_GREEN_SIZE_EXT">GL_COLOR_TABLE_GREEN_SIZE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_INTENSITY_SIZE">GL_COLOR_TABLE_INTENSITY_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_INTENSITY_SIZE_EXT">GL_COLOR_TABLE_INTENSITY_SIZE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_LUMINANCE_SIZE">GL_COLOR_TABLE_LUMINANCE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_LUMINANCE_SIZE_EXT">GL_COLOR_TABLE_LUMINANCE_SIZE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_RED_SIZE">GL_COLOR_TABLE_RED_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_RED_SIZE_EXT">GL_COLOR_TABLE_RED_SIZE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_SCALE">GL_COLOR_TABLE_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_WIDTH">GL_COLOR_TABLE_WIDTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_TABLE_WIDTH_EXT">GL_COLOR_TABLE_WIDTH_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COLOR_WRITEMASK">GL_COLOR_WRITEMASK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMBINE">GL_COMBINE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMBINE_ALPHA">GL_COMBINE_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMBINE_RGB">GL_COMBINE_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPARE_REF_TO_TEXTURE">GL_COMPARE_REF_TO_TEXTURE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPARE_R_TO_TEXTURE">GL_COMPARE_R_TO_TEXTURE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPARE_R_TO_TEXTURE_ARB">GL_COMPARE_R_TO_TEXTURE_ARB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPILE">GL_COMPILE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPILE_AND_EXECUTE">GL_COMPILE_AND_EXECUTE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPILE_STATUS">GL_COMPILE_STATUS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_ALPHA">GL_COMPRESSED_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_INTENSITY">GL_COMPRESSED_INTENSITY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_LUMINANCE">GL_COMPRESSED_LUMINANCE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_LUMINANCE_ALPHA">GL_COMPRESSED_LUMINANCE_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_RGB">GL_COMPRESSED_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_RGBA">GL_COMPRESSED_RGBA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_RGBA_BPTC_UNORM">GL_COMPRESSED_RGBA_BPTC_UNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT">GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT">GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_SLUMINANCE">GL_COMPRESSED_SLUMINANCE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_SLUMINANCE_ALPHA">GL_COMPRESSED_SLUMINANCE_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_SRGB">GL_COMPRESSED_SRGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_SRGB_ALPHA">GL_COMPRESSED_SRGB_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM">GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPRESSED_TEXTURE_FORMATS">GL_COMPRESSED_TEXTURE_FORMATS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COMPUTE_SHADER_INVOCATIONS">GL_COMPUTE_SHADER_INVOCATIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONSTANT">GL_CONSTANT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONSTANT_ALPHA">GL_CONSTANT_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONSTANT_ATTENUATION">GL_CONSTANT_ATTENUATION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONSTANT_BORDER">GL_CONSTANT_BORDER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONSTANT_COLOR">GL_CONSTANT_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONTEXT_COMPATIBILITY_PROFILE_BIT">GL_CONTEXT_COMPATIBILITY_PROFILE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONTEXT_CORE_PROFILE_BIT">GL_CONTEXT_CORE_PROFILE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONTEXT_FLAGS">GL_CONTEXT_FLAGS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT">GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONTEXT_FLAG_NO_ERROR_BIT">GL_CONTEXT_FLAG_NO_ERROR_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT">GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONTEXT_PROFILE_MASK">GL_CONTEXT_PROFILE_MASK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONVOLUTION_1D">GL_CONVOLUTION_1D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONVOLUTION_2D">GL_CONVOLUTION_2D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONVOLUTION_BORDER_COLOR">GL_CONVOLUTION_BORDER_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONVOLUTION_BORDER_MODE">GL_CONVOLUTION_BORDER_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONVOLUTION_FILTER_BIAS">GL_CONVOLUTION_FILTER_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONVOLUTION_FILTER_SCALE">GL_CONVOLUTION_FILTER_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONVOLUTION_FORMAT">GL_CONVOLUTION_FORMAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONVOLUTION_HEIGHT">GL_CONVOLUTION_HEIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CONVOLUTION_WIDTH">GL_CONVOLUTION_WIDTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COORD_REPLACE">GL_COORD_REPLACE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COPY">GL_COPY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COPY_INVERTED">GL_COPY_INVERTED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COPY_PIXEL_TOKEN">GL_COPY_PIXEL_TOKEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COPY_READ_BUFFER_BINDING">GL_COPY_READ_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_COPY_WRITE_BUFFER_BINDING">GL_COPY_WRITE_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CULL_FACE">GL_CULL_FACE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CULL_FACE_MODE">GL_CULL_FACE_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_BIT">GL_CURRENT_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_COLOR">GL_CURRENT_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_FOG_COORD">GL_CURRENT_FOG_COORD</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_FOG_COORDINATE">GL_CURRENT_FOG_COORDINATE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_INDEX">GL_CURRENT_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_NORMAL">GL_CURRENT_NORMAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_PROGRAM">GL_CURRENT_PROGRAM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_QUERY">GL_CURRENT_QUERY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_RASTER_COLOR">GL_CURRENT_RASTER_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_RASTER_DISTANCE">GL_CURRENT_RASTER_DISTANCE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_RASTER_INDEX">GL_CURRENT_RASTER_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_RASTER_POSITION">GL_CURRENT_RASTER_POSITION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_RASTER_POSITION_VALID">GL_CURRENT_RASTER_POSITION_VALID</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_RASTER_SECONDARY_COLOR">GL_CURRENT_RASTER_SECONDARY_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_RASTER_TEXTURE_COORDS">GL_CURRENT_RASTER_TEXTURE_COORDS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_SECONDARY_COLOR">GL_CURRENT_SECONDARY_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_TEXTURE_COORDS">GL_CURRENT_TEXTURE_COORDS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CURRENT_VERTEX_ATTRIB">GL_CURRENT_VERTEX_ATTRIB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_CW">GL_CW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DECAL">GL_DECAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DECR">GL_DECR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DECR_WRAP">GL_DECR_WRAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DELETE_STATUS">GL_DELETE_STATUS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH">GL_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH24_STENCIL8">GL_DEPTH24_STENCIL8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_ATTACHMENT">GL_DEPTH_ATTACHMENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_BIAS">GL_DEPTH_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_BITS">GL_DEPTH_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_BUFFER">GL_DEPTH_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_BUFFER_BIT">GL_DEPTH_BUFFER_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_CLEAR_VALUE">GL_DEPTH_CLEAR_VALUE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_COMPONENT">GL_DEPTH_COMPONENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_COMPONENT16">GL_DEPTH_COMPONENT16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_COMPONENT24">GL_DEPTH_COMPONENT24</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_COMPONENT32">GL_DEPTH_COMPONENT32</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_FUNC">GL_DEPTH_FUNC</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_RANGE">GL_DEPTH_RANGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_SCALE">GL_DEPTH_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_STENCIL">GL_DEPTH_STENCIL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_STENCIL_ATTACHMENT">GL_DEPTH_STENCIL_ATTACHMENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_TEST">GL_DEPTH_TEST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_TEXTURE_MODE">GL_DEPTH_TEXTURE_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DEPTH_WRITEMASK">GL_DEPTH_WRITEMASK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DIFFUSE">GL_DIFFUSE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DITHER">GL_DITHER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DOMAIN">GL_DOMAIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DONT_CARE">GL_DONT_CARE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DOT3_RGB">GL_DOT3_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DOT3_RGBA">GL_DOT3_RGBA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DOUBLE">GL_DOUBLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DOUBLEBUFFER">GL_DOUBLEBUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DOUBLE_EXT">GL_DOUBLE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER">GL_DRAW_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER0">GL_DRAW_BUFFER0</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER1">GL_DRAW_BUFFER1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER10">GL_DRAW_BUFFER10</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER11">GL_DRAW_BUFFER11</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER12">GL_DRAW_BUFFER12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER13">GL_DRAW_BUFFER13</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER14">GL_DRAW_BUFFER14</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER15">GL_DRAW_BUFFER15</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER2">GL_DRAW_BUFFER2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER3">GL_DRAW_BUFFER3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER4">GL_DRAW_BUFFER4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER5">GL_DRAW_BUFFER5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER6">GL_DRAW_BUFFER6</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER7">GL_DRAW_BUFFER7</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER8">GL_DRAW_BUFFER8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_BUFFER9">GL_DRAW_BUFFER9</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_FRAMEBUFFER">GL_DRAW_FRAMEBUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_FRAMEBUFFER_BINDING">GL_DRAW_FRAMEBUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DRAW_PIXEL_TOKEN">GL_DRAW_PIXEL_TOKEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DST_ALPHA">GL_DST_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DST_COLOR">GL_DST_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DYNAMIC_COPY">GL_DYNAMIC_COPY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DYNAMIC_DRAW">GL_DYNAMIC_DRAW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_DYNAMIC_READ">GL_DYNAMIC_READ</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EDGE_FLAG">GL_EDGE_FLAG</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EDGE_FLAG_ARRAY">GL_EDGE_FLAG_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EDGE_FLAG_ARRAY_BUFFER_BINDING">GL_EDGE_FLAG_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EDGE_FLAG_ARRAY_COUNT_EXT">GL_EDGE_FLAG_ARRAY_COUNT_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EDGE_FLAG_ARRAY_EXT">GL_EDGE_FLAG_ARRAY_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EDGE_FLAG_ARRAY_POINTER">GL_EDGE_FLAG_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EDGE_FLAG_ARRAY_POINTER_EXT">GL_EDGE_FLAG_ARRAY_POINTER_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EDGE_FLAG_ARRAY_STRIDE">GL_EDGE_FLAG_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EDGE_FLAG_ARRAY_STRIDE_EXT">GL_EDGE_FLAG_ARRAY_STRIDE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ELEMENT_ARRAY_BUFFER">GL_ELEMENT_ARRAY_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ELEMENT_ARRAY_BUFFER_BINDING">GL_ELEMENT_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EMISSION">GL_EMISSION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ENABLE_BIT">GL_ENABLE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EQUAL">GL_EQUAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EQUIV">GL_EQUIV</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EVAL_BIT">GL_EVAL_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EXP">GL_EXP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EXP2">GL_EXP2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EXTENSIONS">GL_EXTENSIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EXT_bgra">GL_EXT_bgra</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EXT_paletted_texture">GL_EXT_paletted_texture</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EXT_vertex_array">GL_EXT_vertex_array</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EYE_LINEAR">GL_EYE_LINEAR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_EYE_PLANE">GL_EYE_PLANE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FALSE">GL_FALSE</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FASTEST">GL_FASTEST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FEEDBACK">GL_FEEDBACK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FEEDBACK_BUFFER_POINTER">GL_FEEDBACK_BUFFER_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FEEDBACK_BUFFER_SIZE">GL_FEEDBACK_BUFFER_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FEEDBACK_BUFFER_TYPE">GL_FEEDBACK_BUFFER_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FILL">GL_FILL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FIXED_ONLY">GL_FIXED_ONLY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLAT">GL_FLAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT">GL_FLOAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_MAT2">GL_FLOAT_MAT2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_MAT2x3">GL_FLOAT_MAT2x3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_MAT2x4">GL_FLOAT_MAT2x4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_MAT3">GL_FLOAT_MAT3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_MAT3x2">GL_FLOAT_MAT3x2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_MAT3x4">GL_FLOAT_MAT3x4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_MAT4">GL_FLOAT_MAT4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_MAT4x2">GL_FLOAT_MAT4x2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_MAT4x3">GL_FLOAT_MAT4x3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_VEC2">GL_FLOAT_VEC2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_VEC3">GL_FLOAT_VEC3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FLOAT_VEC4">GL_FLOAT_VEC4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG">GL_FOG</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_BIT">GL_FOG_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COLOR">GL_FOG_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORD">GL_FOG_COORD</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORDINATE">GL_FOG_COORDINATE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORDINATE_ARRAY">GL_FOG_COORDINATE_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING">GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORDINATE_ARRAY_POINTER">GL_FOG_COORDINATE_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORDINATE_ARRAY_STRIDE">GL_FOG_COORDINATE_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORDINATE_ARRAY_TYPE">GL_FOG_COORDINATE_ARRAY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORDINATE_SOURCE">GL_FOG_COORDINATE_SOURCE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORD_ARRAY">GL_FOG_COORD_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORD_ARRAY_BUFFER_BINDING">GL_FOG_COORD_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORD_ARRAY_POINTER">GL_FOG_COORD_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORD_ARRAY_STRIDE">GL_FOG_COORD_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORD_ARRAY_TYPE">GL_FOG_COORD_ARRAY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_COORD_SRC">GL_FOG_COORD_SRC</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_DENSITY">GL_FOG_DENSITY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_END">GL_FOG_END</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_HINT">GL_FOG_HINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_INDEX">GL_FOG_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_MODE">GL_FOG_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_SPECULAR_TEXTURE_WIN">GL_FOG_SPECULAR_TEXTURE_WIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FOG_START">GL_FOG_START</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAGMENT_DEPTH">GL_FRAGMENT_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAGMENT_SHADER">GL_FRAGMENT_SHADER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAGMENT_SHADER_DERIVATIVE_HINT">GL_FRAGMENT_SHADER_DERIVATIVE_HINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAGMENT_SHADER_INVOCATIONS">GL_FRAGMENT_SHADER_INVOCATIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER">GL_FRAMEBUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE">GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE">GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING">GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE">GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE">GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE">GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_LAYERED">GL_FRAMEBUFFER_ATTACHMENT_LAYERED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME">GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE">GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE">GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE">GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE">GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER">GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL">GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_BINDING">GL_FRAMEBUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_COMPLETE">GL_FRAMEBUFFER_COMPLETE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_DEFAULT">GL_FRAMEBUFFER_DEFAULT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS">GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_DEFAULT_HEIGHT">GL_FRAMEBUFFER_DEFAULT_HEIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_DEFAULT_LAYERS">GL_FRAMEBUFFER_DEFAULT_LAYERS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_DEFAULT_SAMPLES">GL_FRAMEBUFFER_DEFAULT_SAMPLES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_DEFAULT_WIDTH">GL_FRAMEBUFFER_DEFAULT_WIDTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT">GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER">GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS">GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT">GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE">GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER">GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_SRGB">GL_FRAMEBUFFER_SRGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_UNDEFINED">GL_FRAMEBUFFER_UNDEFINED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRAMEBUFFER_UNSUPPORTED">GL_FRAMEBUFFER_UNSUPPORTED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRONT">GL_FRONT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRONT_AND_BACK">GL_FRONT_AND_BACK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRONT_FACE">GL_FRONT_FACE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRONT_LEFT">GL_FRONT_LEFT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FRONT_RIGHT">GL_FRONT_RIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FUNC_ADD">GL_FUNC_ADD</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FUNC_REVERSE_SUBTRACT">GL_FUNC_REVERSE_SUBTRACT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_FUNC_SUBTRACT">GL_FUNC_SUBTRACT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GENERATE_MIPMAP">GL_GENERATE_MIPMAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GENERATE_MIPMAP_HINT">GL_GENERATE_MIPMAP_HINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GEOMETRY_INPUT_TYPE">GL_GEOMETRY_INPUT_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GEOMETRY_OUTPUT_TYPE">GL_GEOMETRY_OUTPUT_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GEOMETRY_SHADER">GL_GEOMETRY_SHADER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED">GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GEOMETRY_VERTICES_OUT">GL_GEOMETRY_VERTICES_OUT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GEQUAL">GL_GEQUAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GREATER">GL_GREATER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GREEN">GL_GREEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GREEN_BIAS">GL_GREEN_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GREEN_BITS">GL_GREEN_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GREEN_INTEGER">GL_GREEN_INTEGER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_GREEN_SCALE">GL_GREEN_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HINT_BIT">GL_HINT_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HISTOGRAM">GL_HISTOGRAM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HISTOGRAM_ALPHA_SIZE">GL_HISTOGRAM_ALPHA_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HISTOGRAM_BLUE_SIZE">GL_HISTOGRAM_BLUE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HISTOGRAM_FORMAT">GL_HISTOGRAM_FORMAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HISTOGRAM_GREEN_SIZE">GL_HISTOGRAM_GREEN_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HISTOGRAM_LUMINANCE_SIZE">GL_HISTOGRAM_LUMINANCE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HISTOGRAM_RED_SIZE">GL_HISTOGRAM_RED_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HISTOGRAM_SINK">GL_HISTOGRAM_SINK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_HISTOGRAM_WIDTH">GL_HISTOGRAM_WIDTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_IGNORE_BORDER">GL_IGNORE_BORDER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INCR">GL_INCR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INCR_WRAP">GL_INCR_WRAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX">GL_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY">GL_INDEX_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY_BUFFER_BINDING">GL_INDEX_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY_COUNT_EXT">GL_INDEX_ARRAY_COUNT_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY_EXT">GL_INDEX_ARRAY_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY_POINTER">GL_INDEX_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY_POINTER_EXT">GL_INDEX_ARRAY_POINTER_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY_STRIDE">GL_INDEX_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY_STRIDE_EXT">GL_INDEX_ARRAY_STRIDE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY_TYPE">GL_INDEX_ARRAY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_ARRAY_TYPE_EXT">GL_INDEX_ARRAY_TYPE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_BITS">GL_INDEX_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_CLEAR_VALUE">GL_INDEX_CLEAR_VALUE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_LOGIC_OP">GL_INDEX_LOGIC_OP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_MODE">GL_INDEX_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_OFFSET">GL_INDEX_OFFSET</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_SHIFT">GL_INDEX_SHIFT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INDEX_WRITEMASK">GL_INDEX_WRITEMASK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INFO_LOG_LENGTH">GL_INFO_LOG_LENGTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT">GL_INT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INTENSITY">GL_INTENSITY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INTENSITY12">GL_INTENSITY12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INTENSITY16">GL_INTENSITY16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INTENSITY4">GL_INTENSITY4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INTENSITY8">GL_INTENSITY8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INTERLEAVED_ATTRIBS">GL_INTERLEAVED_ATTRIBS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INTERPOLATE">GL_INTERPOLATE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_SAMPLER_1D">GL_INT_SAMPLER_1D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_SAMPLER_1D_ARRAY">GL_INT_SAMPLER_1D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_SAMPLER_2D">GL_INT_SAMPLER_2D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_SAMPLER_2D_ARRAY">GL_INT_SAMPLER_2D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_SAMPLER_2D_RECT">GL_INT_SAMPLER_2D_RECT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_SAMPLER_3D">GL_INT_SAMPLER_3D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_SAMPLER_BUFFER">GL_INT_SAMPLER_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_SAMPLER_CUBE">GL_INT_SAMPLER_CUBE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_SAMPLER_CUBE_MAP_ARRAY">GL_INT_SAMPLER_CUBE_MAP_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_VEC2">GL_INT_VEC2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_VEC3">GL_INT_VEC3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INT_VEC4">GL_INT_VEC4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INVALID_ENUM">GL_INVALID_ENUM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INVALID_FRAMEBUFFER_OPERATION">GL_INVALID_FRAMEBUFFER_OPERATION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INVALID_OPERATION">GL_INVALID_OPERATION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INVALID_VALUE">GL_INVALID_VALUE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_INVERT">GL_INVERT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_KEEP">GL_KEEP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LEFT">GL_LEFT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LEQUAL">GL_LEQUAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LESS">GL_LESS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT0">GL_LIGHT0</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT1">GL_LIGHT1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT2">GL_LIGHT2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT3">GL_LIGHT3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT4">GL_LIGHT4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT5">GL_LIGHT5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT6">GL_LIGHT6</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT7">GL_LIGHT7</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHTING">GL_LIGHTING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHTING_BIT">GL_LIGHTING_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT_MODEL_AMBIENT">GL_LIGHT_MODEL_AMBIENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT_MODEL_COLOR_CONTROL">GL_LIGHT_MODEL_COLOR_CONTROL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT_MODEL_LOCAL_VIEWER">GL_LIGHT_MODEL_LOCAL_VIEWER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIGHT_MODEL_TWO_SIDE">GL_LIGHT_MODEL_TWO_SIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE">GL_LINE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINEAR">GL_LINEAR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINEAR_ATTENUATION">GL_LINEAR_ATTENUATION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINEAR_MIPMAP_LINEAR">GL_LINEAR_MIPMAP_LINEAR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINEAR_MIPMAP_NEAREST">GL_LINEAR_MIPMAP_NEAREST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINES">GL_LINES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINES_ADJACENCY">GL_LINES_ADJACENCY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_BIT">GL_LINE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_LOOP">GL_LINE_LOOP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_RESET_TOKEN">GL_LINE_RESET_TOKEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_SMOOTH">GL_LINE_SMOOTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_SMOOTH_HINT">GL_LINE_SMOOTH_HINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_STIPPLE">GL_LINE_STIPPLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_STIPPLE_PATTERN">GL_LINE_STIPPLE_PATTERN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_STIPPLE_REPEAT">GL_LINE_STIPPLE_REPEAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_STRIP">GL_LINE_STRIP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_STRIP_ADJACENCY">GL_LINE_STRIP_ADJACENCY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_TOKEN">GL_LINE_TOKEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_WIDTH">GL_LINE_WIDTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_WIDTH_GRANULARITY">GL_LINE_WIDTH_GRANULARITY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINE_WIDTH_RANGE">GL_LINE_WIDTH_RANGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LINK_STATUS">GL_LINK_STATUS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIST_BASE">GL_LIST_BASE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIST_BIT">GL_LIST_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIST_INDEX">GL_LIST_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LIST_MODE">GL_LIST_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LOAD">GL_LOAD</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LOGIC_OP">GL_LOGIC_OP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LOGIC_OP_MODE">GL_LOGIC_OP_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LOWER_LEFT">GL_LOWER_LEFT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE">GL_LUMINANCE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE12">GL_LUMINANCE12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE12_ALPHA12">GL_LUMINANCE12_ALPHA12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE12_ALPHA4">GL_LUMINANCE12_ALPHA4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE16">GL_LUMINANCE16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE16_ALPHA16">GL_LUMINANCE16_ALPHA16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE4">GL_LUMINANCE4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE4_ALPHA4">GL_LUMINANCE4_ALPHA4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE6_ALPHA2">GL_LUMINANCE6_ALPHA2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE8">GL_LUMINANCE8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE8_ALPHA8">GL_LUMINANCE8_ALPHA8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_LUMINANCE_ALPHA">GL_LUMINANCE_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAJOR_VERSION">GL_MAJOR_VERSION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_COLOR_4">GL_MAP1_COLOR_4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_GRID_DOMAIN">GL_MAP1_GRID_DOMAIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_GRID_SEGMENTS">GL_MAP1_GRID_SEGMENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_INDEX">GL_MAP1_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_NORMAL">GL_MAP1_NORMAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_TEXTURE_COORD_1">GL_MAP1_TEXTURE_COORD_1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_TEXTURE_COORD_2">GL_MAP1_TEXTURE_COORD_2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_TEXTURE_COORD_3">GL_MAP1_TEXTURE_COORD_3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_TEXTURE_COORD_4">GL_MAP1_TEXTURE_COORD_4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_VERTEX_3">GL_MAP1_VERTEX_3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP1_VERTEX_4">GL_MAP1_VERTEX_4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_COLOR_4">GL_MAP2_COLOR_4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_GRID_DOMAIN">GL_MAP2_GRID_DOMAIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_GRID_SEGMENTS">GL_MAP2_GRID_SEGMENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_INDEX">GL_MAP2_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_NORMAL">GL_MAP2_NORMAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_TEXTURE_COORD_1">GL_MAP2_TEXTURE_COORD_1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_TEXTURE_COORD_2">GL_MAP2_TEXTURE_COORD_2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_TEXTURE_COORD_3">GL_MAP2_TEXTURE_COORD_3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_TEXTURE_COORD_4">GL_MAP2_TEXTURE_COORD_4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_VERTEX_3">GL_MAP2_VERTEX_3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP2_VERTEX_4">GL_MAP2_VERTEX_4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP_COLOR">GL_MAP_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAP_STENCIL">GL_MAP_STENCIL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MATRIX_MODE">GL_MATRIX_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX">GL_MAX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_3D_TEXTURE_SIZE">GL_MAX_3D_TEXTURE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_ARRAY_TEXTURE_LAYERS">GL_MAX_ARRAY_TEXTURE_LAYERS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_ATTRIB_STACK_DEPTH">GL_MAX_ATTRIB_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_CLIENT_ATTRIB_STACK_DEPTH">GL_MAX_CLIENT_ATTRIB_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_CLIP_DISTANCES">GL_MAX_CLIP_DISTANCES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_CLIP_PLANES">GL_MAX_CLIP_PLANES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_COLOR_ATTACHMENTS">GL_MAX_COLOR_ATTACHMENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_COLOR_MATRIX_STACK_DEPTH">GL_MAX_COLOR_MATRIX_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS">GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_CONVOLUTION_HEIGHT">GL_MAX_CONVOLUTION_HEIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_CONVOLUTION_WIDTH">GL_MAX_CONVOLUTION_WIDTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_CUBE_MAP_TEXTURE_SIZE">GL_MAX_CUBE_MAP_TEXTURE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_DRAW_BUFFERS">GL_MAX_DRAW_BUFFERS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_ELEMENTS_INDICES">GL_MAX_ELEMENTS_INDICES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_ELEMENTS_INDICES_WIN">GL_MAX_ELEMENTS_INDICES_WIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_ELEMENTS_VERTICES">GL_MAX_ELEMENTS_VERTICES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_ELEMENTS_VERTICES_WIN">GL_MAX_ELEMENTS_VERTICES_WIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_EVAL_ORDER">GL_MAX_EVAL_ORDER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_FRAGMENT_INPUT_COMPONENTS">GL_MAX_FRAGMENT_INPUT_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_FRAGMENT_UNIFORM_COMPONENTS">GL_MAX_FRAGMENT_UNIFORM_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_FRAMEBUFFER_HEIGHT">GL_MAX_FRAMEBUFFER_HEIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_FRAMEBUFFER_LAYERS">GL_MAX_FRAMEBUFFER_LAYERS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_FRAMEBUFFER_SAMPLES">GL_MAX_FRAMEBUFFER_SAMPLES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_FRAMEBUFFER_WIDTH">GL_MAX_FRAMEBUFFER_WIDTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_GEOMETRY_INPUT_COMPONENTS">GL_MAX_GEOMETRY_INPUT_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_GEOMETRY_OUTPUT_COMPONENTS">GL_MAX_GEOMETRY_OUTPUT_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_GEOMETRY_OUTPUT_VERTICES">GL_MAX_GEOMETRY_OUTPUT_VERTICES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS">GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS">GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_GEOMETRY_UNIFORM_COMPONENTS">GL_MAX_GEOMETRY_UNIFORM_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_LIGHTS">GL_MAX_LIGHTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_LIST_NESTING">GL_MAX_LIST_NESTING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_MODELVIEW_STACK_DEPTH">GL_MAX_MODELVIEW_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_NAME_STACK_DEPTH">GL_MAX_NAME_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_PIXEL_MAP_TABLE">GL_MAX_PIXEL_MAP_TABLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_PROGRAM_TEXEL_OFFSET">GL_MAX_PROGRAM_TEXEL_OFFSET</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS">GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET">GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_PROJECTION_STACK_DEPTH">GL_MAX_PROJECTION_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_RECTANGLE_TEXTURE_SIZE">GL_MAX_RECTANGLE_TEXTURE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_RENDERBUFFER_SIZE">GL_MAX_RENDERBUFFER_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_SAMPLES">GL_MAX_SAMPLES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TEXTURE_BUFFER_SIZE">GL_MAX_TEXTURE_BUFFER_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TEXTURE_COORDS">GL_MAX_TEXTURE_COORDS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TEXTURE_IMAGE_UNITS">GL_MAX_TEXTURE_IMAGE_UNITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TEXTURE_LOD_BIAS">GL_MAX_TEXTURE_LOD_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TEXTURE_MAX_ANISOTROPY">GL_MAX_TEXTURE_MAX_ANISOTROPY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TEXTURE_SIZE">GL_MAX_TEXTURE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TEXTURE_STACK_DEPTH">GL_MAX_TEXTURE_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TEXTURE_UNITS">GL_MAX_TEXTURE_UNITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS">GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS">GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS">GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_VARYING_COMPONENTS">GL_MAX_VARYING_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_VARYING_FLOATS">GL_MAX_VARYING_FLOATS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_VERTEX_ATTRIBS">GL_MAX_VERTEX_ATTRIBS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_VERTEX_ATTRIB_STRIDE">GL_MAX_VERTEX_ATTRIB_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_VERTEX_OUTPUT_COMPONENTS">GL_MAX_VERTEX_OUTPUT_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS">GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_VERTEX_UNIFORM_COMPONENTS">GL_MAX_VERTEX_UNIFORM_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MAX_VIEWPORT_DIMS">GL_MAX_VIEWPORT_DIMS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MIN">GL_MIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MINMAX">GL_MINMAX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MINMAX_FORMAT">GL_MINMAX_FORMAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MINMAX_SINK">GL_MINMAX_SINK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MINOR_VERSION">GL_MINOR_VERSION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MIN_PROGRAM_TEXEL_OFFSET">GL_MIN_PROGRAM_TEXEL_OFFSET</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET">GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MIN_SAMPLE_SHADING_VALUE">GL_MIN_SAMPLE_SHADING_VALUE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MIRRORED_REPEAT">GL_MIRRORED_REPEAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MODELVIEW">GL_MODELVIEW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MODELVIEW_MATRIX">GL_MODELVIEW_MATRIX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MODELVIEW_STACK_DEPTH">GL_MODELVIEW_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MODULATE">GL_MODULATE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MULT">GL_MULT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MULTISAMPLE">GL_MULTISAMPLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_MULTISAMPLE_BIT">GL_MULTISAMPLE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_N3F_V3F">GL_N3F_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NAMED_STRING_LENGTH_ARB">GL_NAMED_STRING_LENGTH_ARB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NAMED_STRING_TYPE_ARB">GL_NAMED_STRING_TYPE_ARB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NAME_STACK_DEPTH">GL_NAME_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NAND">GL_NAND</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NEAREST">GL_NEAREST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NEAREST_MIPMAP_LINEAR">GL_NEAREST_MIPMAP_LINEAR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NEAREST_MIPMAP_NEAREST">GL_NEAREST_MIPMAP_NEAREST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NEVER">GL_NEVER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NICEST">GL_NICEST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NONE">GL_NONE</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NOOP">GL_NOOP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NOR">GL_NOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMALIZE">GL_NORMALIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY">GL_NORMAL_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY_BUFFER_BINDING">GL_NORMAL_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY_COUNT_EXT">GL_NORMAL_ARRAY_COUNT_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY_EXT">GL_NORMAL_ARRAY_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY_POINTER">GL_NORMAL_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY_POINTER_EXT">GL_NORMAL_ARRAY_POINTER_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY_STRIDE">GL_NORMAL_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY_STRIDE_EXT">GL_NORMAL_ARRAY_STRIDE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY_TYPE">GL_NORMAL_ARRAY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_ARRAY_TYPE_EXT">GL_NORMAL_ARRAY_TYPE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NORMAL_MAP">GL_NORMAL_MAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NOTEQUAL">GL_NOTEQUAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NO_ERROR">GL_NO_ERROR</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NUM_COMPRESSED_TEXTURE_FORMATS">GL_NUM_COMPRESSED_TEXTURE_FORMATS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NUM_EXTENSIONS">GL_NUM_EXTENSIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NUM_SHADING_LANGUAGE_VERSIONS">GL_NUM_SHADING_LANGUAGE_VERSIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_NUM_SPIR_V_EXTENSIONS">GL_NUM_SPIR_V_EXTENSIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OBJECT_LINEAR">GL_OBJECT_LINEAR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OBJECT_PLANE">GL_OBJECT_PLANE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ONE">GL_ONE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ONE_MINUS_CONSTANT_ALPHA">GL_ONE_MINUS_CONSTANT_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ONE_MINUS_CONSTANT_COLOR">GL_ONE_MINUS_CONSTANT_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ONE_MINUS_DST_ALPHA">GL_ONE_MINUS_DST_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ONE_MINUS_DST_COLOR">GL_ONE_MINUS_DST_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ONE_MINUS_SRC_ALPHA">GL_ONE_MINUS_SRC_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ONE_MINUS_SRC_COLOR">GL_ONE_MINUS_SRC_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OPERAND0_ALPHA">GL_OPERAND0_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OPERAND0_RGB">GL_OPERAND0_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OPERAND1_ALPHA">GL_OPERAND1_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OPERAND1_RGB">GL_OPERAND1_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OPERAND2_ALPHA">GL_OPERAND2_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OPERAND2_RGB">GL_OPERAND2_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OR">GL_OR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ORDER">GL_ORDER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OR_INVERTED">GL_OR_INVERTED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OR_REVERSE">GL_OR_REVERSE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_OUT_OF_MEMORY">GL_OUT_OF_MEMORY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PACK_ALIGNMENT">GL_PACK_ALIGNMENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PACK_IMAGE_HEIGHT">GL_PACK_IMAGE_HEIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PACK_LSB_FIRST">GL_PACK_LSB_FIRST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PACK_ROW_LENGTH">GL_PACK_ROW_LENGTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PACK_SKIP_IMAGES">GL_PACK_SKIP_IMAGES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PACK_SKIP_PIXELS">GL_PACK_SKIP_PIXELS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PACK_SKIP_ROWS">GL_PACK_SKIP_ROWS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PACK_SWAP_BYTES">GL_PACK_SWAP_BYTES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PARAMETER_BUFFER">GL_PARAMETER_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PARAMETER_BUFFER_BINDING">GL_PARAMETER_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PASS_THROUGH_TOKEN">GL_PASS_THROUGH_TOKEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PERSPECTIVE_CORRECTION_HINT">GL_PERSPECTIVE_CORRECTION_HINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PHONG_HINT_WIN">GL_PHONG_HINT_WIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PHONG_WIN">GL_PHONG_WIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_A_TO_A">GL_PIXEL_MAP_A_TO_A</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_A_TO_A_SIZE">GL_PIXEL_MAP_A_TO_A_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_B_TO_B">GL_PIXEL_MAP_B_TO_B</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_B_TO_B_SIZE">GL_PIXEL_MAP_B_TO_B_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_G_TO_G">GL_PIXEL_MAP_G_TO_G</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_G_TO_G_SIZE">GL_PIXEL_MAP_G_TO_G_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_A">GL_PIXEL_MAP_I_TO_A</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_A_SIZE">GL_PIXEL_MAP_I_TO_A_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_B">GL_PIXEL_MAP_I_TO_B</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_B_SIZE">GL_PIXEL_MAP_I_TO_B_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_G">GL_PIXEL_MAP_I_TO_G</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_G_SIZE">GL_PIXEL_MAP_I_TO_G_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_I">GL_PIXEL_MAP_I_TO_I</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_I_SIZE">GL_PIXEL_MAP_I_TO_I_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_R">GL_PIXEL_MAP_I_TO_R</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_I_TO_R_SIZE">GL_PIXEL_MAP_I_TO_R_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_R_TO_R">GL_PIXEL_MAP_R_TO_R</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_R_TO_R_SIZE">GL_PIXEL_MAP_R_TO_R_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_S_TO_S">GL_PIXEL_MAP_S_TO_S</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MAP_S_TO_S_SIZE">GL_PIXEL_MAP_S_TO_S_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_MODE_BIT">GL_PIXEL_MODE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_PACK_BUFFER">GL_PIXEL_PACK_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_PACK_BUFFER_BINDING">GL_PIXEL_PACK_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_UNPACK_BUFFER">GL_PIXEL_UNPACK_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PIXEL_UNPACK_BUFFER_BINDING">GL_PIXEL_UNPACK_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT">GL_POINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINTS">GL_POINTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_BIT">GL_POINT_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_DISTANCE_ATTENUATION">GL_POINT_DISTANCE_ATTENUATION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_FADE_THRESHOLD_SIZE">GL_POINT_FADE_THRESHOLD_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_SIZE">GL_POINT_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_SIZE_GRANULARITY">GL_POINT_SIZE_GRANULARITY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_SIZE_MAX">GL_POINT_SIZE_MAX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_SIZE_MIN">GL_POINT_SIZE_MIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_SIZE_RANGE">GL_POINT_SIZE_RANGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_SMOOTH">GL_POINT_SMOOTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_SMOOTH_HINT">GL_POINT_SMOOTH_HINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_SPRITE">GL_POINT_SPRITE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_SPRITE_COORD_ORIGIN">GL_POINT_SPRITE_COORD_ORIGIN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POINT_TOKEN">GL_POINT_TOKEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON">GL_POLYGON</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_BIT">GL_POLYGON_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_MODE">GL_POLYGON_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_OFFSET_CLAMP">GL_POLYGON_OFFSET_CLAMP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_OFFSET_FACTOR">GL_POLYGON_OFFSET_FACTOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_OFFSET_FILL">GL_POLYGON_OFFSET_FILL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_OFFSET_LINE">GL_POLYGON_OFFSET_LINE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_OFFSET_POINT">GL_POLYGON_OFFSET_POINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_OFFSET_UNITS">GL_POLYGON_OFFSET_UNITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_SMOOTH">GL_POLYGON_SMOOTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_SMOOTH_HINT">GL_POLYGON_SMOOTH_HINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_STIPPLE">GL_POLYGON_STIPPLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_STIPPLE_BIT">GL_POLYGON_STIPPLE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POLYGON_TOKEN">GL_POLYGON_TOKEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POSITION">GL_POSITION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_COLOR_MATRIX_ALPHA_BIAS">GL_POST_COLOR_MATRIX_ALPHA_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_COLOR_MATRIX_ALPHA_SCALE">GL_POST_COLOR_MATRIX_ALPHA_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_COLOR_MATRIX_BLUE_BIAS">GL_POST_COLOR_MATRIX_BLUE_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_COLOR_MATRIX_BLUE_SCALE">GL_POST_COLOR_MATRIX_BLUE_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_COLOR_MATRIX_COLOR_TABLE">GL_POST_COLOR_MATRIX_COLOR_TABLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_COLOR_MATRIX_GREEN_BIAS">GL_POST_COLOR_MATRIX_GREEN_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_COLOR_MATRIX_GREEN_SCALE">GL_POST_COLOR_MATRIX_GREEN_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_COLOR_MATRIX_RED_BIAS">GL_POST_COLOR_MATRIX_RED_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_COLOR_MATRIX_RED_SCALE">GL_POST_COLOR_MATRIX_RED_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_CONVOLUTION_ALPHA_BIAS">GL_POST_CONVOLUTION_ALPHA_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_CONVOLUTION_ALPHA_SCALE">GL_POST_CONVOLUTION_ALPHA_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_CONVOLUTION_BLUE_BIAS">GL_POST_CONVOLUTION_BLUE_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_CONVOLUTION_BLUE_SCALE">GL_POST_CONVOLUTION_BLUE_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_CONVOLUTION_COLOR_TABLE">GL_POST_CONVOLUTION_COLOR_TABLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_CONVOLUTION_GREEN_BIAS">GL_POST_CONVOLUTION_GREEN_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_CONVOLUTION_GREEN_SCALE">GL_POST_CONVOLUTION_GREEN_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_CONVOLUTION_RED_BIAS">GL_POST_CONVOLUTION_RED_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_POST_CONVOLUTION_RED_SCALE">GL_POST_CONVOLUTION_RED_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PREVIOUS">GL_PREVIOUS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PRIMARY_COLOR">GL_PRIMARY_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PRIMITIVES_GENERATED">GL_PRIMITIVES_GENERATED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PRIMITIVES_SUBMITTED">GL_PRIMITIVES_SUBMITTED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PRIMITIVE_RESTART">GL_PRIMITIVE_RESTART</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED">GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PRIMITIVE_RESTART_INDEX">GL_PRIMITIVE_RESTART_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROGRAM_POINT_SIZE">GL_PROGRAM_POINT_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROJECTION">GL_PROJECTION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROJECTION_MATRIX">GL_PROJECTION_MATRIX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROJECTION_STACK_DEPTH">GL_PROJECTION_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_COLOR_TABLE">GL_PROXY_COLOR_TABLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_HISTOGRAM">GL_PROXY_HISTOGRAM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE">GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_POST_CONVOLUTION_COLOR_TABLE">GL_PROXY_POST_CONVOLUTION_COLOR_TABLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_TEXTURE_1D">GL_PROXY_TEXTURE_1D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_TEXTURE_1D_ARRAY">GL_PROXY_TEXTURE_1D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_TEXTURE_2D">GL_PROXY_TEXTURE_2D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_TEXTURE_2D_ARRAY">GL_PROXY_TEXTURE_2D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_TEXTURE_3D">GL_PROXY_TEXTURE_3D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_TEXTURE_CUBE_MAP">GL_PROXY_TEXTURE_CUBE_MAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_TEXTURE_CUBE_MAP_ARRAY">GL_PROXY_TEXTURE_CUBE_MAP_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_PROXY_TEXTURE_RECTANGLE">GL_PROXY_TEXTURE_RECTANGLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_Q">GL_Q</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUADRATIC_ATTENUATION">GL_QUADRATIC_ATTENUATION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUADS">GL_QUADS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUAD_STRIP">GL_QUAD_STRIP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUERY_BY_REGION_NO_WAIT">GL_QUERY_BY_REGION_NO_WAIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUERY_BY_REGION_WAIT">GL_QUERY_BY_REGION_WAIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUERY_COUNTER_BITS">GL_QUERY_COUNTER_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUERY_NO_WAIT">GL_QUERY_NO_WAIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUERY_RESULT">GL_QUERY_RESULT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUERY_RESULT_AVAILABLE">GL_QUERY_RESULT_AVAILABLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_QUERY_WAIT">GL_QUERY_WAIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_R">GL_R</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_R11F_G11F_B10F">GL_R11F_G11F_B10F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_R16_SNORM">GL_R16_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_R3_G3_B2">GL_R3_G3_B2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_R8_SNORM">GL_R8_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RASTERIZER_DISCARD">GL_RASTERIZER_DISCARD</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_READ_BUFFER">GL_READ_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_READ_FRAMEBUFFER">GL_READ_FRAMEBUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_READ_FRAMEBUFFER_BINDING">GL_READ_FRAMEBUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_READ_ONLY">GL_READ_ONLY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_READ_WRITE">GL_READ_WRITE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RED">GL_RED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_REDUCE">GL_REDUCE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RED_BIAS">GL_RED_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RED_BITS">GL_RED_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RED_INTEGER">GL_RED_INTEGER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RED_SCALE">GL_RED_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RED_SNORM">GL_RED_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_REFLECTION_MAP">GL_REFLECTION_MAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDER">GL_RENDER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER">GL_RENDERBUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_ALPHA_SIZE">GL_RENDERBUFFER_ALPHA_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_BINDING">GL_RENDERBUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_BLUE_SIZE">GL_RENDERBUFFER_BLUE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_DEPTH_SIZE">GL_RENDERBUFFER_DEPTH_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_GREEN_SIZE">GL_RENDERBUFFER_GREEN_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_HEIGHT">GL_RENDERBUFFER_HEIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_INTERNAL_FORMAT">GL_RENDERBUFFER_INTERNAL_FORMAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_RED_SIZE">GL_RENDERBUFFER_RED_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_SAMPLES">GL_RENDERBUFFER_SAMPLES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_STENCIL_SIZE">GL_RENDERBUFFER_STENCIL_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERBUFFER_WIDTH">GL_RENDERBUFFER_WIDTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDERER">GL_RENDERER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RENDER_MODE">GL_RENDER_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_REPEAT">GL_REPEAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_REPLACE">GL_REPLACE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_REPLICATE_BORDER">GL_REPLICATE_BORDER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RESCALE_NORMAL">GL_RESCALE_NORMAL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RETURN">GL_RETURN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RG16_SNORM">GL_RG16_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RG8_SNORM">GL_RG8_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB">GL_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB10">GL_RGB10</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB10_A2">GL_RGB10_A2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB10_A2UI">GL_RGB10_A2UI</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB12">GL_RGB12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB16">GL_RGB16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB16F">GL_RGB16F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB16I">GL_RGB16I</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB16UI">GL_RGB16UI</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB16_SNORM">GL_RGB16_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB32F">GL_RGB32F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB32I">GL_RGB32I</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB32UI">GL_RGB32UI</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB4">GL_RGB4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB5">GL_RGB5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB5_A1">GL_RGB5_A1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB8">GL_RGB8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB8I">GL_RGB8I</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB8UI">GL_RGB8UI</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB8_SNORM">GL_RGB8_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB9_E5">GL_RGB9_E5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA">GL_RGBA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA12">GL_RGBA12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA16">GL_RGBA16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA16F">GL_RGBA16F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA16I">GL_RGBA16I</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA16UI">GL_RGBA16UI</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA16_SNORM">GL_RGBA16_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA2">GL_RGBA2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA32F">GL_RGBA32F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA32I">GL_RGBA32I</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA32UI">GL_RGBA32UI</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA4">GL_RGBA4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA8">GL_RGBA8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA8I">GL_RGBA8I</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA8UI">GL_RGBA8UI</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA8_SNORM">GL_RGBA8_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA_INTEGER">GL_RGBA_INTEGER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA_MODE">GL_RGBA_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGBA_SNORM">GL_RGBA_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB_INTEGER">GL_RGB_INTEGER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB_SCALE">GL_RGB_SCALE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RGB_SNORM">GL_RGB_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RG_SNORM">GL_RG_SNORM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_RIGHT">GL_RIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_S">GL_S</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_1D">GL_SAMPLER_1D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_1D_ARRAY">GL_SAMPLER_1D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_1D_ARRAY_SHADOW">GL_SAMPLER_1D_ARRAY_SHADOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_1D_SHADOW">GL_SAMPLER_1D_SHADOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_2D">GL_SAMPLER_2D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_2D_ARRAY">GL_SAMPLER_2D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_2D_ARRAY_SHADOW">GL_SAMPLER_2D_ARRAY_SHADOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_2D_RECT">GL_SAMPLER_2D_RECT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_2D_RECT_SHADOW">GL_SAMPLER_2D_RECT_SHADOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_2D_SHADOW">GL_SAMPLER_2D_SHADOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_3D">GL_SAMPLER_3D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_BUFFER">GL_SAMPLER_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_CUBE">GL_SAMPLER_CUBE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_CUBE_MAP_ARRAY">GL_SAMPLER_CUBE_MAP_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW">GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLER_CUBE_SHADOW">GL_SAMPLER_CUBE_SHADOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLES">GL_SAMPLES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLES_PASSED">GL_SAMPLES_PASSED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLE_ALPHA_TO_COVERAGE">GL_SAMPLE_ALPHA_TO_COVERAGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLE_ALPHA_TO_ONE">GL_SAMPLE_ALPHA_TO_ONE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLE_BUFFERS">GL_SAMPLE_BUFFERS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLE_COVERAGE">GL_SAMPLE_COVERAGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLE_COVERAGE_INVERT">GL_SAMPLE_COVERAGE_INVERT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLE_COVERAGE_VALUE">GL_SAMPLE_COVERAGE_VALUE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SAMPLE_SHADING">GL_SAMPLE_SHADING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SCISSOR_BIT">GL_SCISSOR_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SCISSOR_BOX">GL_SCISSOR_BOX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SCISSOR_TEST">GL_SCISSOR_TEST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SECONDARY_COLOR_ARRAY">GL_SECONDARY_COLOR_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING">GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SECONDARY_COLOR_ARRAY_POINTER">GL_SECONDARY_COLOR_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SECONDARY_COLOR_ARRAY_SIZE">GL_SECONDARY_COLOR_ARRAY_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SECONDARY_COLOR_ARRAY_STRIDE">GL_SECONDARY_COLOR_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SECONDARY_COLOR_ARRAY_TYPE">GL_SECONDARY_COLOR_ARRAY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SELECT">GL_SELECT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SELECTION_BUFFER_POINTER">GL_SELECTION_BUFFER_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SELECTION_BUFFER_SIZE">GL_SELECTION_BUFFER_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SEPARABLE_2D">GL_SEPARABLE_2D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SEPARATE_ATTRIBS">GL_SEPARATE_ATTRIBS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SEPARATE_SPECULAR_COLOR">GL_SEPARATE_SPECULAR_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SET">GL_SET</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SHADER_BINARY_FORMAT_SPIR_V">GL_SHADER_BINARY_FORMAT_SPIR_V</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SHADER_INCLUDE_ARB">GL_SHADER_INCLUDE_ARB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SHADER_SOURCE_LENGTH">GL_SHADER_SOURCE_LENGTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SHADER_TYPE">GL_SHADER_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SHADE_MODEL">GL_SHADE_MODEL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SHADING_LANGUAGE_VERSION">GL_SHADING_LANGUAGE_VERSION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SHININESS">GL_SHININESS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SHORT">GL_SHORT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SIGNED_NORMALIZED">GL_SIGNED_NORMALIZED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SINGLE_COLOR">GL_SINGLE_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SLUMINANCE">GL_SLUMINANCE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SLUMINANCE8">GL_SLUMINANCE8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SLUMINANCE8_ALPHA8">GL_SLUMINANCE8_ALPHA8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SLUMINANCE_ALPHA">GL_SLUMINANCE_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SMOOTH">GL_SMOOTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SMOOTH_LINE_WIDTH_GRANULARITY">GL_SMOOTH_LINE_WIDTH_GRANULARITY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SMOOTH_LINE_WIDTH_RANGE">GL_SMOOTH_LINE_WIDTH_RANGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SMOOTH_POINT_SIZE_GRANULARITY">GL_SMOOTH_POINT_SIZE_GRANULARITY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SMOOTH_POINT_SIZE_RANGE">GL_SMOOTH_POINT_SIZE_RANGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SOURCE0_ALPHA">GL_SOURCE0_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SOURCE0_RGB">GL_SOURCE0_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SOURCE1_ALPHA">GL_SOURCE1_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SOURCE1_RGB">GL_SOURCE1_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SOURCE2_ALPHA">GL_SOURCE2_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SOURCE2_RGB">GL_SOURCE2_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SPECULAR">GL_SPECULAR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SPHERE_MAP">GL_SPHERE_MAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SPIR_V_BINARY">GL_SPIR_V_BINARY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SPIR_V_EXTENSIONS">GL_SPIR_V_EXTENSIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SPOT_CUTOFF">GL_SPOT_CUTOFF</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SPOT_DIRECTION">GL_SPOT_DIRECTION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SPOT_EXPONENT">GL_SPOT_EXPONENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRC0_ALPHA">GL_SRC0_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRC0_RGB">GL_SRC0_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRC1_ALPHA">GL_SRC1_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRC1_RGB">GL_SRC1_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRC2_ALPHA">GL_SRC2_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRC2_RGB">GL_SRC2_RGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRC_ALPHA">GL_SRC_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRC_ALPHA_SATURATE">GL_SRC_ALPHA_SATURATE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRC_COLOR">GL_SRC_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRGB">GL_SRGB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRGB8">GL_SRGB8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRGB8_ALPHA8">GL_SRGB8_ALPHA8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SRGB_ALPHA">GL_SRGB_ALPHA</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STACK_OVERFLOW">GL_STACK_OVERFLOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STACK_UNDERFLOW">GL_STACK_UNDERFLOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STATIC_COPY">GL_STATIC_COPY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STATIC_DRAW">GL_STATIC_DRAW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STATIC_READ">GL_STATIC_READ</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL">GL_STENCIL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_ATTACHMENT">GL_STENCIL_ATTACHMENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BACK_FAIL">GL_STENCIL_BACK_FAIL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BACK_FUNC">GL_STENCIL_BACK_FUNC</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BACK_PASS_DEPTH_FAIL">GL_STENCIL_BACK_PASS_DEPTH_FAIL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BACK_PASS_DEPTH_PASS">GL_STENCIL_BACK_PASS_DEPTH_PASS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BACK_REF">GL_STENCIL_BACK_REF</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BACK_VALUE_MASK">GL_STENCIL_BACK_VALUE_MASK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BACK_WRITEMASK">GL_STENCIL_BACK_WRITEMASK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BITS">GL_STENCIL_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BUFFER">GL_STENCIL_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_BUFFER_BIT">GL_STENCIL_BUFFER_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_CLEAR_VALUE">GL_STENCIL_CLEAR_VALUE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_FAIL">GL_STENCIL_FAIL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_FUNC">GL_STENCIL_FUNC</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_INDEX">GL_STENCIL_INDEX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_INDEX1">GL_STENCIL_INDEX1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_INDEX16">GL_STENCIL_INDEX16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_INDEX4">GL_STENCIL_INDEX4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_INDEX8">GL_STENCIL_INDEX8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_PASS_DEPTH_FAIL">GL_STENCIL_PASS_DEPTH_FAIL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_PASS_DEPTH_PASS">GL_STENCIL_PASS_DEPTH_PASS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_REF">GL_STENCIL_REF</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_TEST">GL_STENCIL_TEST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_VALUE_MASK">GL_STENCIL_VALUE_MASK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STENCIL_WRITEMASK">GL_STENCIL_WRITEMASK</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STEREO">GL_STEREO</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STREAM_COPY">GL_STREAM_COPY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STREAM_DRAW">GL_STREAM_DRAW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_STREAM_READ">GL_STREAM_READ</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SUBPIXEL_BITS">GL_SUBPIXEL_BITS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_SUBTRACT">GL_SUBTRACT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_T">GL_T</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_T2F_C3F_V3F">GL_T2F_C3F_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_T2F_C4F_N3F_V3F">GL_T2F_C4F_N3F_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_T2F_C4UB_V3F">GL_T2F_C4UB_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_T2F_N3F_V3F">GL_T2F_N3F_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_T2F_V3F">GL_T2F_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_T4F_C4F_N3F_V4F">GL_T4F_C4F_N3F_V4F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_T4F_V4F">GL_T4F_V4F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TABLE_TOO_LARGE">GL_TABLE_TOO_LARGE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TESS_CONTROL_SHADER_PATCHES">GL_TESS_CONTROL_SHADER_PATCHES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TESS_EVALUATION_SHADER_INVOCATIONS">GL_TESS_EVALUATION_SHADER_INVOCATIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE">GL_TEXTURE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE0">GL_TEXTURE0</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE1">GL_TEXTURE1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE10">GL_TEXTURE10</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE11">GL_TEXTURE11</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE12">GL_TEXTURE12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE13">GL_TEXTURE13</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE14">GL_TEXTURE14</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE15">GL_TEXTURE15</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE16">GL_TEXTURE16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE17">GL_TEXTURE17</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE18">GL_TEXTURE18</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE19">GL_TEXTURE19</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE2">GL_TEXTURE2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE20">GL_TEXTURE20</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE21">GL_TEXTURE21</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE22">GL_TEXTURE22</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE23">GL_TEXTURE23</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE24">GL_TEXTURE24</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE25">GL_TEXTURE25</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE26">GL_TEXTURE26</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE27">GL_TEXTURE27</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE28">GL_TEXTURE28</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE29">GL_TEXTURE29</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE3">GL_TEXTURE3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE30">GL_TEXTURE30</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE31">GL_TEXTURE31</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE4">GL_TEXTURE4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE5">GL_TEXTURE5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE6">GL_TEXTURE6</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE7">GL_TEXTURE7</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE8">GL_TEXTURE8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE9">GL_TEXTURE9</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_1D">GL_TEXTURE_1D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_1D_ARRAY">GL_TEXTURE_1D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_2D">GL_TEXTURE_2D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_2D_ARRAY">GL_TEXTURE_2D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_3D">GL_TEXTURE_3D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_ALPHA_SIZE">GL_TEXTURE_ALPHA_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_ALPHA_TYPE">GL_TEXTURE_ALPHA_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BASE_LEVEL">GL_TEXTURE_BASE_LEVEL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BINDING_1D">GL_TEXTURE_BINDING_1D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BINDING_1D_ARRAY">GL_TEXTURE_BINDING_1D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BINDING_2D">GL_TEXTURE_BINDING_2D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BINDING_2D_ARRAY">GL_TEXTURE_BINDING_2D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BINDING_3D">GL_TEXTURE_BINDING_3D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BINDING_BUFFER">GL_TEXTURE_BINDING_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BINDING_CUBE_MAP">GL_TEXTURE_BINDING_CUBE_MAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BINDING_CUBE_MAP_ARRAY">GL_TEXTURE_BINDING_CUBE_MAP_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BINDING_RECTANGLE">GL_TEXTURE_BINDING_RECTANGLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BIT">GL_TEXTURE_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BLUE_SIZE">GL_TEXTURE_BLUE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BLUE_TYPE">GL_TEXTURE_BLUE_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BORDER">GL_TEXTURE_BORDER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BORDER_COLOR">GL_TEXTURE_BORDER_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BUFFER">GL_TEXTURE_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BUFFER_BINDING">GL_TEXTURE_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BUFFER_DATA_STORE_BINDING">GL_TEXTURE_BUFFER_DATA_STORE_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_BUFFER_FORMAT">GL_TEXTURE_BUFFER_FORMAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COMPARE_FUNC">GL_TEXTURE_COMPARE_FUNC</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COMPARE_FUNC_ARB">GL_TEXTURE_COMPARE_FUNC_ARB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COMPARE_MODE">GL_TEXTURE_COMPARE_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COMPARE_MODE_ARB">GL_TEXTURE_COMPARE_MODE_ARB</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COMPONENTS">GL_TEXTURE_COMPONENTS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COMPRESSED">GL_TEXTURE_COMPRESSED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COMPRESSED_IMAGE_SIZE">GL_TEXTURE_COMPRESSED_IMAGE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COMPRESSION_HINT">GL_TEXTURE_COMPRESSION_HINT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY">GL_TEXTURE_COORD_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING">GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_COUNT_EXT">GL_TEXTURE_COORD_ARRAY_COUNT_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_EXT">GL_TEXTURE_COORD_ARRAY_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_POINTER">GL_TEXTURE_COORD_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_POINTER_EXT">GL_TEXTURE_COORD_ARRAY_POINTER_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_SIZE">GL_TEXTURE_COORD_ARRAY_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_SIZE_EXT">GL_TEXTURE_COORD_ARRAY_SIZE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_STRIDE">GL_TEXTURE_COORD_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_STRIDE_EXT">GL_TEXTURE_COORD_ARRAY_STRIDE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_TYPE">GL_TEXTURE_COORD_ARRAY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_COORD_ARRAY_TYPE_EXT">GL_TEXTURE_COORD_ARRAY_TYPE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_CUBE_MAP">GL_TEXTURE_CUBE_MAP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_CUBE_MAP_ARRAY">GL_TEXTURE_CUBE_MAP_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_CUBE_MAP_NEGATIVE_X">GL_TEXTURE_CUBE_MAP_NEGATIVE_X</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y">GL_TEXTURE_CUBE_MAP_NEGATIVE_Y</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z">GL_TEXTURE_CUBE_MAP_NEGATIVE_Z</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_CUBE_MAP_POSITIVE_X">GL_TEXTURE_CUBE_MAP_POSITIVE_X</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_CUBE_MAP_POSITIVE_Y">GL_TEXTURE_CUBE_MAP_POSITIVE_Y</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_CUBE_MAP_POSITIVE_Z">GL_TEXTURE_CUBE_MAP_POSITIVE_Z</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_DEPTH">GL_TEXTURE_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_DEPTH_SIZE">GL_TEXTURE_DEPTH_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_DEPTH_TYPE">GL_TEXTURE_DEPTH_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_ENV">GL_TEXTURE_ENV</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_ENV_COLOR">GL_TEXTURE_ENV_COLOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_ENV_MODE">GL_TEXTURE_ENV_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_FILTER_CONTROL">GL_TEXTURE_FILTER_CONTROL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_GEN_MODE">GL_TEXTURE_GEN_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_GEN_Q">GL_TEXTURE_GEN_Q</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_GEN_R">GL_TEXTURE_GEN_R</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_GEN_S">GL_TEXTURE_GEN_S</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_GEN_T">GL_TEXTURE_GEN_T</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_GREEN_SIZE">GL_TEXTURE_GREEN_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_GREEN_TYPE">GL_TEXTURE_GREEN_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_HEIGHT">GL_TEXTURE_HEIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_INTENSITY_SIZE">GL_TEXTURE_INTENSITY_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_INTENSITY_TYPE">GL_TEXTURE_INTENSITY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_INTERNAL_FORMAT">GL_TEXTURE_INTERNAL_FORMAT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_LOD_BIAS">GL_TEXTURE_LOD_BIAS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_LUMINANCE_SIZE">GL_TEXTURE_LUMINANCE_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_LUMINANCE_TYPE">GL_TEXTURE_LUMINANCE_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_MAG_FILTER">GL_TEXTURE_MAG_FILTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_MATRIX">GL_TEXTURE_MATRIX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_MAX_ANISOTROPY">GL_TEXTURE_MAX_ANISOTROPY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_MAX_LEVEL">GL_TEXTURE_MAX_LEVEL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_MAX_LOD">GL_TEXTURE_MAX_LOD</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_MIN_FILTER">GL_TEXTURE_MIN_FILTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_MIN_LOD">GL_TEXTURE_MIN_LOD</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_PRIORITY">GL_TEXTURE_PRIORITY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_RECTANGLE">GL_TEXTURE_RECTANGLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_RED_SIZE">GL_TEXTURE_RED_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_RED_TYPE">GL_TEXTURE_RED_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_RESIDENT">GL_TEXTURE_RESIDENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_SHARED_SIZE">GL_TEXTURE_SHARED_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_STACK_DEPTH">GL_TEXTURE_STACK_DEPTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_STENCIL_SIZE">GL_TEXTURE_STENCIL_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_WIDTH">GL_TEXTURE_WIDTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_WRAP_R">GL_TEXTURE_WRAP_R</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_WRAP_S">GL_TEXTURE_WRAP_S</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TEXTURE_WRAP_T">GL_TEXTURE_WRAP_T</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_BIT">GL_TRANSFORM_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_ACTIVE">GL_TRANSFORM_FEEDBACK_ACTIVE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_BUFFER">GL_TRANSFORM_FEEDBACK_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_BUFFER_BINDING">GL_TRANSFORM_FEEDBACK_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_BUFFER_MODE">GL_TRANSFORM_FEEDBACK_BUFFER_MODE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_BUFFER_SIZE">GL_TRANSFORM_FEEDBACK_BUFFER_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_BUFFER_START">GL_TRANSFORM_FEEDBACK_BUFFER_START</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_OVERFLOW">GL_TRANSFORM_FEEDBACK_OVERFLOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_PAUSED">GL_TRANSFORM_FEEDBACK_PAUSED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN">GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW">GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_VARYINGS">GL_TRANSFORM_FEEDBACK_VARYINGS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH">GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSPOSE_COLOR_MATRIX">GL_TRANSPOSE_COLOR_MATRIX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSPOSE_MODELVIEW_MATRIX">GL_TRANSPOSE_MODELVIEW_MATRIX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSPOSE_PROJECTION_MATRIX">GL_TRANSPOSE_PROJECTION_MATRIX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRANSPOSE_TEXTURE_MATRIX">GL_TRANSPOSE_TEXTURE_MATRIX</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRIANGLES">GL_TRIANGLES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRIANGLES_ADJACENCY">GL_TRIANGLES_ADJACENCY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRIANGLE_FAN">GL_TRIANGLE_FAN</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRIANGLE_STRIP">GL_TRIANGLE_STRIP</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRIANGLE_STRIP_ADJACENCY">GL_TRIANGLE_STRIP_ADJACENCY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_TRUE">GL_TRUE</td>
<td class="code-type"><span class="STpe">u8</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNPACK_ALIGNMENT">GL_UNPACK_ALIGNMENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNPACK_IMAGE_HEIGHT">GL_UNPACK_IMAGE_HEIGHT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNPACK_LSB_FIRST">GL_UNPACK_LSB_FIRST</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNPACK_ROW_LENGTH">GL_UNPACK_ROW_LENGTH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNPACK_SKIP_IMAGES">GL_UNPACK_SKIP_IMAGES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNPACK_SKIP_PIXELS">GL_UNPACK_SKIP_PIXELS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNPACK_SKIP_ROWS">GL_UNPACK_SKIP_ROWS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNPACK_SWAP_BYTES">GL_UNPACK_SWAP_BYTES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_BYTE">GL_UNSIGNED_BYTE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_BYTE_2_3_3_REV">GL_UNSIGNED_BYTE_2_3_3_REV</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_BYTE_3_3_2">GL_UNSIGNED_BYTE_3_3_2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT">GL_UNSIGNED_INT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_10F_11F_11F_REV">GL_UNSIGNED_INT_10F_11F_11F_REV</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_10_10_10_2">GL_UNSIGNED_INT_10_10_10_2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_24_8">GL_UNSIGNED_INT_24_8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_5_9_9_9_REV">GL_UNSIGNED_INT_5_9_9_9_REV</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_8_8_8_8">GL_UNSIGNED_INT_8_8_8_8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_8_8_8_8_REV">GL_UNSIGNED_INT_8_8_8_8_REV</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_SAMPLER_1D">GL_UNSIGNED_INT_SAMPLER_1D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_SAMPLER_1D_ARRAY">GL_UNSIGNED_INT_SAMPLER_1D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_SAMPLER_2D">GL_UNSIGNED_INT_SAMPLER_2D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_SAMPLER_2D_ARRAY">GL_UNSIGNED_INT_SAMPLER_2D_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_SAMPLER_2D_RECT">GL_UNSIGNED_INT_SAMPLER_2D_RECT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_SAMPLER_3D">GL_UNSIGNED_INT_SAMPLER_3D</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_SAMPLER_BUFFER">GL_UNSIGNED_INT_SAMPLER_BUFFER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_SAMPLER_CUBE">GL_UNSIGNED_INT_SAMPLER_CUBE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY">GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_VEC2">GL_UNSIGNED_INT_VEC2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_VEC3">GL_UNSIGNED_INT_VEC3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_INT_VEC4">GL_UNSIGNED_INT_VEC4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_NORMALIZED">GL_UNSIGNED_NORMALIZED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_SHORT">GL_UNSIGNED_SHORT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_SHORT_1_5_5_5_REV">GL_UNSIGNED_SHORT_1_5_5_5_REV</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_SHORT_4_4_4_4">GL_UNSIGNED_SHORT_4_4_4_4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_SHORT_4_4_4_4_REV">GL_UNSIGNED_SHORT_4_4_4_4_REV</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_SHORT_5_5_5_1">GL_UNSIGNED_SHORT_5_5_5_1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_SHORT_5_6_5">GL_UNSIGNED_SHORT_5_6_5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UNSIGNED_SHORT_5_6_5_REV">GL_UNSIGNED_SHORT_5_6_5_REV</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_UPPER_LEFT">GL_UPPER_LEFT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_V2F">GL_V2F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_V3F">GL_V3F</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VALIDATE_STATUS">GL_VALIDATE_STATUS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VENDOR">GL_VENDOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERSION">GL_VERSION</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY">GL_VERTEX_ARRAY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_BUFFER_BINDING">GL_VERTEX_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_COUNT_EXT">GL_VERTEX_ARRAY_COUNT_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_EXT">GL_VERTEX_ARRAY_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_POINTER">GL_VERTEX_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_POINTER_EXT">GL_VERTEX_ARRAY_POINTER_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_SIZE">GL_VERTEX_ARRAY_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_SIZE_EXT">GL_VERTEX_ARRAY_SIZE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_STRIDE">GL_VERTEX_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_STRIDE_EXT">GL_VERTEX_ARRAY_STRIDE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_TYPE">GL_VERTEX_ARRAY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ARRAY_TYPE_EXT">GL_VERTEX_ARRAY_TYPE_EXT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING">GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_DIVISOR">GL_VERTEX_ATTRIB_ARRAY_DIVISOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_ENABLED">GL_VERTEX_ATTRIB_ARRAY_ENABLED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_INTEGER">GL_VERTEX_ATTRIB_ARRAY_INTEGER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_LONG">GL_VERTEX_ATTRIB_ARRAY_LONG</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_NORMALIZED">GL_VERTEX_ATTRIB_ARRAY_NORMALIZED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_POINTER">GL_VERTEX_ATTRIB_ARRAY_POINTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_SIZE">GL_VERTEX_ATTRIB_ARRAY_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_STRIDE">GL_VERTEX_ATTRIB_ARRAY_STRIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_ATTRIB_ARRAY_TYPE">GL_VERTEX_ATTRIB_ARRAY_TYPE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_PROGRAM_POINT_SIZE">GL_VERTEX_PROGRAM_POINT_SIZE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_PROGRAM_TWO_SIDE">GL_VERTEX_PROGRAM_TWO_SIDE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_SHADER">GL_VERTEX_SHADER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTEX_SHADER_INVOCATIONS">GL_VERTEX_SHADER_INVOCATIONS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VERTICES_SUBMITTED">GL_VERTICES_SUBMITTED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VIEWPORT">GL_VIEWPORT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_VIEWPORT_BIT">GL_VIEWPORT_BIT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_WEIGHT_ARRAY_BUFFER_BINDING">GL_WEIGHT_ARRAY_BUFFER_BINDING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_WIN_draw_range_elements">GL_WIN_draw_range_elements</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_WIN_swap_hint">GL_WIN_swap_hint</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_WRAP_BORDER">GL_WRAP_BORDER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_WRITE_ONLY">GL_WRITE_ONLY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_XOR">GL_XOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ZERO">GL_ZERO</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ZOOM_X">GL_ZOOM_X</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GL_ZOOM_Y">GL_ZOOM_Y</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_GLbitfield"><span class="api-item-title-kind">type alias</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">Type Aliases</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td id="Ogl_GLbitfield">GLbitfield</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLbitfield">GLbitfield</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLboolean">GLboolean</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLbyte">GLbyte</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLchar">GLchar</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLclampd">GLclampd</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLclampd">GLclampd</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLclampf">GLclampf</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLdouble">GLdouble</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLenum">GLenum</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLfloat">GLfloat</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLint">GLint</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLint">GLint</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLint64">GLint64</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLint64">GLint64</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLintptr">GLintptr</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLintptr">GLintptr</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLshort">GLshort</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLsizei">GLsizei</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLsizeiptr">GLsizeiptr</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLsizeiptr">GLsizeiptr</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLubyte">GLubyte</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLuint">GLuint</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLuint64">GLuint64</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLuint64">GLuint64</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLushort">GLushort</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_GLvoid">GLvoid</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Ogl_HGLRC">HGLRC</td>
<td class="code-type"><span class="SCde"><span class="SCst"></span><span class="SCst"><a href="#Ogl_HGLRC">HGLRC</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glAccum"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glAccum</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L723" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glAccum</span>(op: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, value: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glActiveTexture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glActiveTexture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glActiveTexture</span>(texture: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glAlphaFunc"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glAlphaFunc</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L724" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glAlphaFunc</span>(fc: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, refr: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glAreTexturesResident"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glAreTexturesResident</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L725" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glAreTexturesResident</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, textures: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, residences: *<span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glArrayElement"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glArrayElement</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L726" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glArrayElement</span>(i: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glAttachShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glAttachShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glAttachShader</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBegin"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBegin</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L727" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBegin</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBeginConditionalRender"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBeginConditionalRender</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBeginConditionalRender</span>(id: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBeginQuery"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBeginQuery</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBeginQuery</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, id: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBeginTransformFeedback"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBeginTransformFeedback</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBeginTransformFeedback</span>(primitiveMode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBindAttribLocation"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBindAttribLocation</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBindAttribLocation</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBindBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBindBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBindBuffer</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, buffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBindFragDataLocation"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBindFragDataLocation</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBindFragDataLocation</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, colorNumber: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBindFramebuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBindFramebuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L188" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBindFramebuffer</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, framebuffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBindRenderbuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBindRenderbuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L189" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBindRenderbuffer</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, renderbuffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBindTexture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBindTexture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L728" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBindTexture</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, texture: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBitmap"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBitmap</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L729" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBitmap</span>(width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, xorig: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, yorig: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, xmove: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, ymove: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, bitmap: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlendColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlendColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlendColor</span>(red: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>, green: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>, blue: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlendEquation"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlendEquation</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlendEquation</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlendEquationSeparate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlendEquationSeparate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlendEquationSeparate</span>(modeRGB: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, modeAlpha: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlendEquationSeparatei"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlendEquationSeparatei</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_0.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlendEquationSeparatei</span>(buf: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, modeRGB: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, modeAlpha: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlendEquationi"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlendEquationi</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_0.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlendEquationi</span>(buf: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlendFunc"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlendFunc</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L730" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlendFunc</span>(sfactor: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, dfactor: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlendFuncSeparate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlendFuncSeparate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlendFuncSeparate</span>(sfactorRGB: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, dfactorRGB: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, sfactorAlpha: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, dfactorAlpha: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlendFuncSeparatei"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlendFuncSeparatei</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_0.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlendFuncSeparatei</span>(buf: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, srcRGB: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, dstRGB: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, srcAlpha: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, dstAlpha: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlendFunci"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlendFunci</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_0.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlendFunci</span>(buf: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, src: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, dst: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBlitFramebuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBlitFramebuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L190" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBlitFramebuffer</span>(srcX0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, srcY0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, srcX1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, srcY1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, dstX0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, dstY0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, dstX1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, dstY1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, mask: <span class="SCst"><a href="#Ogl_GLbitfield">GLbitfield</a></span>, filter: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBufferData"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBufferData</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBufferData</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, size: <span class="SCst"><a href="#Ogl_GLsizeiptr">GLsizeiptr</a></span>, data: <span class="SKwd">const</span> *<span class="STpe">void</span>, usage: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glBufferSubData"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glBufferSubData</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glBufferSubData</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, offset: <span class="SCst"><a href="#Ogl_GLintptr">GLintptr</a></span>, size: <span class="SCst"><a href="#Ogl_GLsizeiptr">GLsizeiptr</a></span>, data: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCallList"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCallList</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L731" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCallList</span>(list: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCallLists"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCallLists</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L732" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCallLists</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, lists: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCheckFramebufferStatus"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCheckFramebufferStatus</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L191" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCheckFramebufferStatus</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClampColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClampColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClampColor</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, clamp: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L733" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClear</span>(mask: <span class="SCst"><a href="#Ogl_GLbitfield">GLbitfield</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClearAccum"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClearAccum</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L734" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClearAccum</span>(red: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, green: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, blue: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClearBufferfi"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClearBufferfi</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClearBufferfi</span>(buffer: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, drawBuffer: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, depth: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, stencil: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClearBufferfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClearBufferfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClearBufferfv</span>(buffer: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, drawBuffer: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClearBufferiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClearBufferiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClearBufferiv</span>(buffer: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, drawBuffer: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClearBufferuiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClearBufferuiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClearBufferuiv</span>(buffer: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, drawBuffer: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClearColor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClearColor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L735" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClearColor</span>(red: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>, green: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>, blue: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClearDepth"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClearDepth</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L736" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClearDepth</span>(depth: <span class="SCst"><a href="#Ogl_GLclampd">GLclampd</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClearIndex"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClearIndex</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L737" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClearIndex</span>(c: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClearStencil"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClearStencil</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L738" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClearStencil</span>(s: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClientActiveTexture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClientActiveTexture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClientActiveTexture</span>(texture: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glClipPlane"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glClipPlane</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L739" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glClipPlane</span>(plane: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, equation: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3b"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3b</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L740" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3b</span>(red: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>, green: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>, blue: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3bv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3bv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L741" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3bv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L742" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3d</span>(red: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, green: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, blue: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L743" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L744" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3f</span>(red: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, green: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, blue: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L745" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L746" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3i</span>(red: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, green: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, blue: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L747" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L748" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3s</span>(red: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, green: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, blue: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L749" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3ub"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3ub</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L750" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3ub</span>(red: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, green: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, blue: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3ubv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3ubv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L751" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3ubv</span>(v: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L752" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3ui</span>(red: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, green: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, blue: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L753" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3uiv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3us"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3us</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L754" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3us</span>(red: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>, green: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>, blue: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor3usv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor3usv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L755" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor3usv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4b"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4b</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L756" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4b</span>(red: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>, green: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>, blue: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4bv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4bv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L757" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4bv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L758" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4d</span>(red: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, green: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, blue: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L759" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L760" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4f</span>(red: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, green: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, blue: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L761" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L762" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4i</span>(red: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, green: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, blue: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L763" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L764" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4s</span>(red: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, green: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, blue: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L765" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4ub"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4ub</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L766" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4ub</span>(red: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, green: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, blue: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4ubv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4ubv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L767" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4ubv</span>(v: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L768" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4ui</span>(red: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, green: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, blue: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L769" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4uiv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4us"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4us</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L770" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4us</span>(red: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>, green: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>, blue: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColor4usv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColor4usv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L771" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColor4usv</span>(v: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColorMask"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColorMask</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L772" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColorMask</span>(red: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, green: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, blue: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColorMaski"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColorMaski</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColorMaski</span>(buf: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, red: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, green: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, blue: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, alpha: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColorMaterial"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColorMaterial</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L773" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColorMaterial</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glColorPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glColorPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L774" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glColorPointer</span>(size: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCompileShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCompileShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCompileShader</span>(shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCompileShaderIncludeARB"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCompileShaderIncludeARB</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L220" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCompileShaderIncludeARB</span>(shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, path: <span class="SKwd">const</span> **<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>, length: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCompressedTexImage1D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCompressedTexImage1D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCompressedTexImage1D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, internalformat: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, border: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, imageSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, data: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCompressedTexImage2D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCompressedTexImage2D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCompressedTexImage2D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, internalformat: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, border: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, imageSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, data: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCompressedTexImage3D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCompressedTexImage3D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCompressedTexImage3D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, internalformat: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, depth: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, border: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, imageSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, data: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCompressedTexSubImage1D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCompressedTexSubImage1D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCompressedTexSubImage1D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, xoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, imageSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, data: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCompressedTexSubImage2D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCompressedTexSubImage2D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCompressedTexSubImage2D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, xoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, yoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, imageSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, data: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCompressedTexSubImage3D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCompressedTexSubImage3D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCompressedTexSubImage3D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, xoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, yoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, zoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, depth: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, imageSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, data: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCopyPixels"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCopyPixels</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L775" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCopyPixels</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCopyTexImage1D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCopyTexImage1D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L776" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCopyTexImage1D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, internalFormat: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, border: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCopyTexImage2D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCopyTexImage2D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L777" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCopyTexImage2D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, internalFormat: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, border: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCopyTexSubImage1D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCopyTexSubImage1D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L778" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCopyTexSubImage1D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, xoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCopyTexSubImage2D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCopyTexSubImage2D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L779" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCopyTexSubImage2D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, xoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, yoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCreateProgram"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCreateProgram</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCreateProgram</span>()-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCreateShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCreateShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCreateShader</span>(type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glCullFace"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glCullFace</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L780" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glCullFace</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDeleteBuffers"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDeleteBuffers</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDeleteBuffers</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, buffers: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDeleteFramebuffers"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDeleteFramebuffers</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L192" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDeleteFramebuffers</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, framebuffers: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDeleteLists"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDeleteLists</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L781" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDeleteLists</span>(list: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, range: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDeleteNamedStringARB"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDeleteNamedStringARB</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L221" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDeleteNamedStringARB</span>(namelen: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDeleteProgram"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDeleteProgram</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDeleteProgram</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDeleteQueries"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDeleteQueries</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDeleteQueries</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, ids: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDeleteRenderbuffers"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDeleteRenderbuffers</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L193" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDeleteRenderbuffers</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, renderbuffers: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDeleteShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDeleteShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDeleteShader</span>(shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDeleteTextures"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDeleteTextures</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L782" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDeleteTextures</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, textures: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDepthFunc"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDepthFunc</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L783" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDepthFunc</span>(fc: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDepthMask"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDepthMask</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L784" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDepthMask</span>(flag: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDepthRange"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDepthRange</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L785" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDepthRange</span>(zNear: <span class="SCst"><a href="#Ogl_GLclampd">GLclampd</a></span>, zFar: <span class="SCst"><a href="#Ogl_GLclampd">GLclampd</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDetachShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDetachShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDetachShader</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDisable"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDisable</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L786" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDisable</span>(cap: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDisableClientState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDisableClientState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L787" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDisableClientState</span>(array: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDisableVertexAttribArray"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDisableVertexAttribArray</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDisableVertexAttribArray</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDisablei"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDisablei</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDisablei</span>(cap: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDrawArrays"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDrawArrays</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L788" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDrawArrays</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, first: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDrawArraysInstanced"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDrawArraysInstanced</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_1.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDrawArraysInstanced</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, first: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, primcount: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDrawBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDrawBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L789" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDrawBuffer</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDrawBuffers"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDrawBuffers</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDrawBuffers</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, bufs: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDrawElements"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDrawElements</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L790" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDrawElements</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, indices: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDrawElementsInstanced"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDrawElementsInstanced</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_1.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDrawElementsInstanced</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, indices: <span class="SKwd">const</span> *<span class="STpe">void</span>, primcount: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glDrawPixels"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glDrawPixels</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L791" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glDrawPixels</span>(width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pixels: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEdgeFlag"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEdgeFlag</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L792" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEdgeFlag</span>(flag: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEdgeFlagPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEdgeFlagPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L793" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEdgeFlagPointer</span>(stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEdgeFlagv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEdgeFlagv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L794" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEdgeFlagv</span>(flag: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEnable"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEnable</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L795" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEnable</span>(cap: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEnableClientState"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEnableClientState</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L796" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEnableClientState</span>(array: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEnableVertexAttribArray"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEnableVertexAttribArray</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEnableVertexAttribArray</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEnablei"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEnablei</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEnablei</span>(cap: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEnd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEnd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L797" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEnd</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEndConditionalRender"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEndConditionalRender</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEndConditionalRender</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEndList"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEndList</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L798" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEndList</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEndQuery"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEndQuery</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEndQuery</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEndTransformFeedback"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEndTransformFeedback</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEndTransformFeedback</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalCoord1d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalCoord1d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L799" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalCoord1d</span>(u: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalCoord1dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalCoord1dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L800" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalCoord1dv</span>(u: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalCoord1f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalCoord1f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L801" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalCoord1f</span>(u: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalCoord1fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalCoord1fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L802" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalCoord1fv</span>(u: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalCoord2d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalCoord2d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L803" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalCoord2d</span>(u: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, v: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalCoord2dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalCoord2dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L804" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalCoord2dv</span>(u: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalCoord2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalCoord2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L805" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalCoord2f</span>(u: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalCoord2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalCoord2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L806" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalCoord2fv</span>(u: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalMesh1"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalMesh1</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L807" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalMesh1</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, i1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, i2: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalMesh2"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalMesh2</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L808" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalMesh2</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, i1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, i2: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, j1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, j2: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalPoint1"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalPoint1</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L809" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalPoint1</span>(i: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glEvalPoint2"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glEvalPoint2</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L810" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glEvalPoint2</span>(i: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, j: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFeedbackBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFeedbackBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L811" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFeedbackBuffer</span>(size: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, buffer: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFinish"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFinish</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L812" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFinish</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFlush"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFlush</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L813" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFlush</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFogCoordPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFogCoordPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFogCoordPointer</span>(type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFogCoordd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFogCoordd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFogCoordd</span>(coord: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFogCoorddv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFogCoorddv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFogCoorddv</span>(coord: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFogCoordf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFogCoordf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFogCoordf</span>(coord: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFogCoordfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFogCoordfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFogCoordfv</span>(coord: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFogf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFogf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L814" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFogf</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFogfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFogfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L815" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFogfv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFogi"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFogi</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L816" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFogi</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFogiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFogiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L817" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFogiv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFramebufferParameteri"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFramebufferParameteri</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L104" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFramebufferParameteri</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFramebufferRenderbuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFramebufferRenderbuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L194" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFramebufferRenderbuffer</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, attachment: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, renderbuffertarget: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, renderbuffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFramebufferTexture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFramebufferTexture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_2.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFramebufferTexture</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, attachment: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, texture: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFramebufferTexture1D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFramebufferTexture1D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L195" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFramebufferTexture1D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, attachment: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, textarget: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, texture: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFramebufferTexture2D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFramebufferTexture2D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L196" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFramebufferTexture2D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, attachment: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, textarget: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, texture: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFramebufferTexture3D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFramebufferTexture3D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L197" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFramebufferTexture3D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, attachment: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, textarget: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, texture: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, layer: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFramebufferTextureLayer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFramebufferTextureLayer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L198" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFramebufferTextureLayer</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, attachment: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, texture: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, layer: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFrontFace"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFrontFace</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L818" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFrontFace</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glFrustum"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glFrustum</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L819" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glFrustum</span>(left: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, right: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, bottom: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, top: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, zNear: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, zFar: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGenBuffers"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGenBuffers</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGenBuffers</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, buffers: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGenFramebuffers"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGenFramebuffers</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L199" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGenFramebuffers</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, framebuffers: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGenLists"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGenLists</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L820" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGenLists</span>(range: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGenQueries"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGenQueries</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGenQueries</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, ids: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGenRenderbuffers"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGenRenderbuffers</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L200" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGenRenderbuffers</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, renderbuffers: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGenTextures"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGenTextures</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L821" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGenTextures</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, textures: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGenerateMipmap"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGenerateMipmap</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L201" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGenerateMipmap</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetActiveAttrib"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetActiveAttrib</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetActiveAttrib</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, maxLength: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, length: *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, size: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, type: *<span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, name: *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetActiveUniform"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetActiveUniform</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetActiveUniform</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, maxLength: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, length: *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, size: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, type: *<span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, name: *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetAttachedShaders"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetAttachedShaders</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetAttachedShaders</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, maxCount: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, count: *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, shaders: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetAttribLocation"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetAttribLocation</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetAttribLocation</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLint">GLint</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetBooleani_v"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetBooleani_v</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetBooleani_v</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, data: *<span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetBooleanv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetBooleanv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L822" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetBooleanv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetBufferParameteri64v"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetBufferParameteri64v</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_2.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetBufferParameteri64v</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, value: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, data: *<span class="SCst"><a href="#Ogl_GLint64">GLint64</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetBufferParameteriv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetBufferParameteriv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetBufferParameteriv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetBufferPointerv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetBufferPointerv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetBufferPointerv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: **<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetBufferSubData"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetBufferSubData</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetBufferSubData</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, offset: <span class="SCst"><a href="#Ogl_GLintptr">GLintptr</a></span>, size: <span class="SCst"><a href="#Ogl_GLsizeiptr">GLsizeiptr</a></span>, data: *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetClipPlane"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetClipPlane</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L823" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetClipPlane</span>(plane: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, equation: *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetCompressedTexImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetCompressedTexImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetCompressedTexImage</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, lod: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, img: *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetDoublev"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetDoublev</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L824" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetDoublev</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetError"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetError</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L825" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetError</span>()-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetFloatv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetFloatv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L826" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetFloatv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetFragDataLocation"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetFragDataLocation</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetFragDataLocation</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLint">GLint</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetFramebufferAttachmentParameteriv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetFramebufferAttachmentParameteriv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L202" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetFramebufferAttachmentParameteriv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, attachment: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetFramebufferParameteriv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetFramebufferParameteriv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetFramebufferParameteriv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetGraphicsResetStatus"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetGraphicsResetStatus</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_5.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetGraphicsResetStatus</span>()-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetInteger64i_v"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetInteger64i_v</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_2.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetInteger64i_v</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, data: *<span class="SCst"><a href="#Ogl_GLint64">GLint64</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetIntegerv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetIntegerv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L827" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetIntegerv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetLightfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetLightfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L828" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetLightfv</span>(light: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetLightiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetLightiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L829" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetLightiv</span>(light: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetMapdv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetMapdv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L830" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetMapdv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, query: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetMapfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetMapfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L831" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetMapfv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, query: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetMapiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetMapiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L832" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetMapiv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, query: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetMaterialfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetMaterialfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L833" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetMaterialfv</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetMaterialiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetMaterialiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L834" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetMaterialiv</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetNamedFramebufferParameterivEXT"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetNamedFramebufferParameterivEXT</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L106" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetNamedFramebufferParameterivEXT</span>(framebuffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetNamedStringARB"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetNamedStringARB</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L222" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetNamedStringARB</span>(namelen: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>, bufSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, stringlen: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, str: *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetNamedStringivARB"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetNamedStringivARB</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L223" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetNamedStringivARB</span>(namelen: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetPixelMapfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetPixelMapfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L835" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetPixelMapfv</span>(map: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, values: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetPixelMapuiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetPixelMapuiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L836" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetPixelMapuiv</span>(map: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, values: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetPixelMapusv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetPixelMapusv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L837" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetPixelMapusv</span>(map: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, values: *<span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetPointerv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetPointerv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L838" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetPointerv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: **<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetPolygonStipple"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetPolygonStipple</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L839" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetPolygonStipple</span>(mask: *<span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetProgramInfoLog"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetProgramInfoLog</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetProgramInfoLog</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, bufSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, length: *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, infoLog: *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetProgramiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetProgramiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetProgramiv</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetQueryObjectiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetQueryObjectiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetQueryObjectiv</span>(id: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetQueryObjectuiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetQueryObjectuiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetQueryObjectuiv</span>(id: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetQueryiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetQueryiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetQueryiv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetRenderbufferParameteriv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetRenderbufferParameteriv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L203" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetRenderbufferParameteriv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetShaderInfoLog"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetShaderInfoLog</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetShaderInfoLog</span>(shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, bufSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, length: *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, infoLog: *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetShaderSource"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetShaderSource</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetShaderSource</span>(obj: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, maxLength: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, length: *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, source: *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetShaderiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetShaderiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetShaderiv</span>(shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetString"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetString</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L840" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetString</span>(name: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)-&gt;<span class="SKwd">const</span> *<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetStringi"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetStringi</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetStringi</span>(name: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SKwd">const</span> *<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexEnvfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexEnvfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L841" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexEnvfv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexEnviv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexEnviv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L842" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexEnviv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexGendv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexGendv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L843" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexGendv</span>(coord: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexGenfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexGenfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L844" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexGenfv</span>(coord: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexGeniv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexGeniv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L845" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexGeniv</span>(coord: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L846" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexImage</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pixels: *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexLevelParameterfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexLevelParameterfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L847" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexLevelParameterfv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexLevelParameteriv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexLevelParameteriv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L848" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexLevelParameteriv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexParameterIiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexParameterIiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexParameterIiv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexParameterIuiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexParameterIuiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexParameterIuiv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexParameterfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexParameterfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L849" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexParameterfv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTexParameteriv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTexParameteriv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L850" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTexParameteriv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetTransformFeedbackVarying"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetTransformFeedbackVarying</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetTransformFeedbackVarying</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, bufSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, length: *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, size: *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, type: *<span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, name: *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetUniformLocation"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetUniformLocation</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetUniformLocation</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLint">GLint</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetUniformfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetUniformfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetUniformfv</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, params: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetUniformiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetUniformiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetUniformiv</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetUniformuiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetUniformuiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetUniformuiv</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, params: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetVertexAttribIiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetVertexAttribIiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetVertexAttribIiv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetVertexAttribIuiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetVertexAttribIuiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetVertexAttribIuiv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetVertexAttribPointerv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetVertexAttribPointerv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetVertexAttribPointerv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pointer: **<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetVertexAttribdv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetVertexAttribdv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetVertexAttribdv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetVertexAttribfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetVertexAttribfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetVertexAttribfv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetVertexAttribiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetVertexAttribiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetVertexAttribiv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetnCompressedTexImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetnCompressedTexImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_5.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetnCompressedTexImage</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, lod: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, bufSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pixels: *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetnTexImage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetnTexImage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_5.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetnTexImage</span>(tex: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, bufSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pixels: *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glGetnUniformdv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glGetnUniformdv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_5.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glGetnUniformdv</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, bufSize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, params: *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glHint"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glHint</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L851" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glHint</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexMask"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexMask</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L852" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexMask</span>(mask: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L853" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexPointer</span>(type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L854" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexd</span>(c: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexdv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexdv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L855" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexdv</span>(c: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L856" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexf</span>(c: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L857" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexfv</span>(c: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexi"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexi</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L858" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexi</span>(c: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L859" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexiv</span>(c: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexs"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexs</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L860" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexs</span>(c: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexsv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexsv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L861" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexsv</span>(c: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexub"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexub</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L862" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexub</span>(c: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIndexubv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIndexubv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L863" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIndexubv</span>(c: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glInitNames"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glInitNames</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L864" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glInitNames</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glInterleavedArrays"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glInterleavedArrays</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L865" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glInterleavedArrays</span>(format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsBuffer</span>(buffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsEnabled"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsEnabled</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L866" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsEnabled</span>(cap: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsEnabledi"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsEnabledi</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsEnabledi</span>(cap: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsFramebuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsFramebuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L204" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsFramebuffer</span>(framebuffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsList"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsList</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L867" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsList</span>(list: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsNamedStringARB"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsNamedStringARB</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L224" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsNamedStringARB</span>(namelen: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsProgram"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsProgram</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsProgram</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsQuery"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsQuery</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsQuery</span>(id: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsRenderbuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsRenderbuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L205" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsRenderbuffer</span>(renderbuffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsShader</span>(shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glIsTexture"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glIsTexture</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L868" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glIsTexture</span>(texture: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLightModelf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLightModelf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L869" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLightModelf</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLightModelfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLightModelfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L870" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLightModelfv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLightModeli"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLightModeli</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L871" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLightModeli</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLightModeliv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLightModeliv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L872" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLightModeliv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLightf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLightf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L873" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLightf</span>(light: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLightfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLightfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L874" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLightfv</span>(light: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLighti"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLighti</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L875" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLighti</span>(light: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLightiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLightiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L876" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLightiv</span>(light: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLineStipple"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLineStipple</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L877" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLineStipple</span>(factor: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, pattern: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLineWidth"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLineWidth</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L878" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLineWidth</span>(width: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLinkProgram"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLinkProgram</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLinkProgram</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glListBase"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glListBase</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L879" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glListBase</span>(base: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLoadIdentity"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLoadIdentity</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L880" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLoadIdentity</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLoadMatrixd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLoadMatrixd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L881" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLoadMatrixd</span>(m: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLoadMatrixf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLoadMatrixf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L882" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLoadMatrixf</span>(m: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLoadName"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLoadName</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L883" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLoadName</span>(name: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLoadTransposeMatrixd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLoadTransposeMatrixd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLoadTransposeMatrixd</span>(m: <span class="SKwd">const</span> [<span class="SNum">16</span>] <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLoadTransposeMatrixf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLoadTransposeMatrixf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLoadTransposeMatrixf</span>(m: <span class="SKwd">const</span> [<span class="SNum">16</span>] <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glLogicOp"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glLogicOp</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L884" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glLogicOp</span>(opcode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMap1d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMap1d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L885" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMap1d</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, u1: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, u2: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, stride: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, order: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, points: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMap1f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMap1f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L886" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMap1f</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, u1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, u2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, stride: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, order: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, points: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMap2d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMap2d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L887" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMap2d</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, u1: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, u2: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, ustride: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, uorder: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, v2: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, vstride: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, vorder: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, points: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMap2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMap2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L888" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMap2f</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, u1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, u2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, ustride: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, uorder: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, vstride: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, vorder: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, points: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMapBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMapBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMapBuffer</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, access: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)-&gt;*<span class="STpe">void</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMapGrid1d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMapGrid1d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L889" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMapGrid1d</span>(un: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, u1: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, u2: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMapGrid1f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMapGrid1f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L890" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMapGrid1f</span>(un: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, u1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, u2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMapGrid2d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMapGrid2d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L891" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMapGrid2d</span>(un: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, u1: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, u2: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, vn: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, v2: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMapGrid2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMapGrid2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L892" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMapGrid2f</span>(un: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, u1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, u2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, vn: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMaterialf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMaterialf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L893" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMaterialf</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMaterialfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMaterialfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L894" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMaterialfv</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMateriali"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMateriali</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L895" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMateriali</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMaterialiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMaterialiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L896" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMaterialiv</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMatrixMode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMatrixMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L897" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMatrixMode</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMinSampleShading"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMinSampleShading</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_0.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMinSampleShading</span>(value: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultMatrixd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultMatrixd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L898" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultMatrixd</span>(m: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultMatrixf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultMatrixf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L899" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultMatrixf</span>(m: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultTransposeMatrixd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultTransposeMatrixd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultTransposeMatrixd</span>(m: <span class="SKwd">const</span> [<span class="SNum">16</span>] <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultTransposeMatrixf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultTransposeMatrixf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultTransposeMatrixf</span>(m: <span class="SKwd">const</span> [<span class="SNum">16</span>] <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiDrawArrays"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiDrawArrays</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiDrawArrays</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, first: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, drawcount: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiDrawArraysIndirectCount"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiDrawArraysIndirectCount</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_6.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiDrawArraysIndirectCount</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, indirect: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>, drawcount: <span class="SCst"><a href="#Ogl_GLintptr">GLintptr</a></span>, maxdrawcount: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiDrawElements"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiDrawElements</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiDrawElements</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, count: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, indices: <span class="SKwd">const</span> **<span class="STpe">void</span>, drawcount: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiDrawElementsIndirectCount"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiDrawElementsIndirectCount</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_6.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiDrawElementsIndirectCount</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, indirect: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>, drawcount: <span class="SCst"><a href="#Ogl_GLintptr">GLintptr</a></span>, maxdrawcount: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord1d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord1d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord1d</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord1dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord1dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord1dv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord1f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord1f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord1f</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord1fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord1fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord1fv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord1i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord1i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord1i</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord1iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord1iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord1iv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord1s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord1s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord1s</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord1sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord1sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord1sv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord2d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord2d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord2d</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, t: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord2dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord2dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord2dv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord2f</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, t: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord2fv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord2i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord2i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord2i</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, t: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord2iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord2iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord2iv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord2s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord2s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord2s</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, t: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord2sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord2sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord2sv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord3d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord3d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord3d</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, t: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, r: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord3dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord3dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord3dv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord3f</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, t: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, r: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord3fv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord3i</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, t: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, r: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord3iv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord3s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord3s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord3s</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, t: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, r: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord3sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord3sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord3sv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord4d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord4d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord4d</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, t: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, r: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, q: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord4dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord4dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord4dv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord4f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord4f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord4f</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, t: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, r: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, q: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord4fv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord4i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord4i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord4i</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, t: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, r: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, q: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord4iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord4iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord4iv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord4s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord4s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord4s</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, s: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, t: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, r: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, q: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glMultiTexCoord4sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glMultiTexCoord4sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glMultiTexCoord4sv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNamedFramebufferParameteriEXT"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNamedFramebufferParameteriEXT</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L107" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNamedFramebufferParameteriEXT</span>(framebuffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNamedStringARB"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNamedStringARB</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L225" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNamedStringARB</span>(type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, namelen: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, name: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>, stringlen: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, str: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNewList"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNewList</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L900" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNewList</span>(list: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3b"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3b</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L901" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3b</span>(nc: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>, ny: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>, nz: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3bv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3bv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L902" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3bv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L903" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3d</span>(nx: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, ny: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, nz: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L904" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L905" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3f</span>(nx: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, ny: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, nz: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L906" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L907" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3i</span>(nx: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, ny: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, nz: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L908" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L909" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3s</span>(nx: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, ny: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, nz: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormal3sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormal3sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L910" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormal3sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glNormalPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glNormalPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L911" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glNormalPointer</span>(type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glOrtho"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glOrtho</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L912" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glOrtho</span>(left: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, right: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, bottom: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, top: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, zNear: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, zFar: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPassThrough"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPassThrough</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L913" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPassThrough</span>(token: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPixelMapfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPixelMapfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L914" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPixelMapfv</span>(map: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, mapsize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, values: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPixelMapuiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPixelMapuiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L915" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPixelMapuiv</span>(map: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, mapsize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, values: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPixelMapusv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPixelMapusv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L916" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPixelMapusv</span>(map: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, mapsize: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, values: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPixelStoref"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPixelStoref</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L917" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPixelStoref</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPixelStorei"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPixelStorei</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L918" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPixelStorei</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPixelTransferf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPixelTransferf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L919" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPixelTransferf</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPixelTransferi"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPixelTransferi</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L920" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPixelTransferi</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPixelZoom"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPixelZoom</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L921" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPixelZoom</span>(xfactor: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, yfactor: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPointParameterf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPointParameterf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPointParameterf</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPointParameterfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPointParameterfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPointParameterfv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPointParameteri"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPointParameteri</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPointParameteri</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPointParameteriv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPointParameteriv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPointParameteriv</span>(pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPointSize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPointSize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L922" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPointSize</span>(size: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPolygonMode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPolygonMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L923" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPolygonMode</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPolygonOffset"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPolygonOffset</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L924" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPolygonOffset</span>(factor: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, units: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPolygonStipple"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPolygonStipple</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L925" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPolygonStipple</span>(mask: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPopAttrib"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPopAttrib</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L926" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPopAttrib</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPopClientAttrib"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPopClientAttrib</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L927" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPopClientAttrib</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPopMatrix"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPopMatrix</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L928" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPopMatrix</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPopName"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPopName</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L929" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPopName</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPrimitiveRestartIndex"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPrimitiveRestartIndex</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_1.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPrimitiveRestartIndex</span>(buffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPrioritizeTextures"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPrioritizeTextures</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L930" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPrioritizeTextures</span>(n: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, textures: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, priorities: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPushAttrib"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPushAttrib</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L931" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPushAttrib</span>(mask: <span class="SCst"><a href="#Ogl_GLbitfield">GLbitfield</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPushClientAttrib"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPushClientAttrib</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L932" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPushClientAttrib</span>(mask: <span class="SCst"><a href="#Ogl_GLbitfield">GLbitfield</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPushMatrix"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPushMatrix</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L933" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPushMatrix</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glPushName"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glPushName</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L934" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glPushName</span>(name: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos2d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos2d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L935" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos2d</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos2dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos2dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L936" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos2dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L937" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos2f</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L938" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos2fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos2i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos2i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L939" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos2i</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos2iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos2iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L940" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos2iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos2s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos2s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L941" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos2s</span>(x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos2sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos2sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L942" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos2sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos3d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos3d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L943" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos3d</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos3dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos3dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L944" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos3dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L945" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos3f</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L946" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos3fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L947" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos3i</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, z: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L948" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos3iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos3s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos3s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L949" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos3s</span>(x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, z: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos3sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos3sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L950" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos3sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos4d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos4d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L951" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos4d</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, w: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos4dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos4dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L952" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos4dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos4f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos4f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L953" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos4f</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, w: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L954" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos4fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos4i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos4i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L955" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos4i</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, z: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, w: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos4iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos4iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L956" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos4iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos4s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos4s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L957" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos4s</span>(x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, z: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, w: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRasterPos4sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRasterPos4sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L958" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRasterPos4sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glReadBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glReadBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L959" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glReadBuffer</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glReadPixels"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glReadPixels</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L960" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glReadPixels</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pixels: *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRectd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRectd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L961" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRectd</span>(x1: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y1: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, x2: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y2: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRectdv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRectdv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L962" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRectdv</span>(v1: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, v2: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRectf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRectf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L963" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRectf</span>(x1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, x2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRectfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRectfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L964" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRectfv</span>(v1: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v2: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRecti"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRecti</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L965" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRecti</span>(x1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, x2: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y2: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRectiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRectiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L966" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRectiv</span>(v1: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v2: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRects"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRects</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L967" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRects</span>(x1: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y1: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, x2: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y2: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRectsv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRectsv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L968" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRectsv</span>(v1: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, v2: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRenderMode"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRenderMode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L969" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRenderMode</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLint">GLint</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRenderbufferStorage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRenderbufferStorage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L206" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRenderbufferStorage</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, internalformat: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRenderbufferStorageMultisample"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRenderbufferStorageMultisample</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_arb.swg#L207" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRenderbufferStorageMultisample</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, samples: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, internalformat: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRotated"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRotated</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L970" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRotated</span>(angle: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glRotatef"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glRotatef</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L971" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glRotatef</span>(angle: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSampleCoverage"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSampleCoverage</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_3.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSampleCoverage</span>(value: <span class="SCst"><a href="#Ogl_GLclampf">GLclampf</a></span>, invert: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glScaled"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glScaled</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L972" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glScaled</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glScalef"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glScalef</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L973" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glScalef</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glScissor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glScissor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L974" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glScissor</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3b"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3b</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3b</span>(red: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>, green: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>, blue: <span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3bv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3bv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3bv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3d</span>(red: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, green: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, blue: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3f</span>(red: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, green: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, blue: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3i</span>(red: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, green: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, blue: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3s</span>(red: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, green: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, blue: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3ub"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3ub</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3ub</span>(red: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, green: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, blue: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3ubv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3ubv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3ubv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3ui</span>(red: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, green: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, blue: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3uiv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3us"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3us</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3us</span>(red: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>, green: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>, blue: <span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColor3usv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColor3usv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColor3usv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSecondaryColorPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSecondaryColorPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSecondaryColorPointer</span>(size: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSelectBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSelectBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L975" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSelectBuffer</span>(size: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, buffer: *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glShadeModel"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glShadeModel</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L976" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glShadeModel</span>(mode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glShaderSource"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glShaderSource</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glShaderSource</span>(shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, str: *<span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>, length: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glSpecializeShader"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glSpecializeShader</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_4_6.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glSpecializeShader</span>(shader: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pEntryPoint: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>, numSpecializationConstants: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pConstantIndex: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, pConstantValue: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glStencilFunc"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glStencilFunc</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L977" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glStencilFunc</span>(fc: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, refr: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, mask: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glStencilFuncSeparate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glStencilFuncSeparate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glStencilFuncSeparate</span>(frontfunc: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, backfunc: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, refr: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, mask: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glStencilMask"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glStencilMask</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L978" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glStencilMask</span>(mask: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glStencilMaskSeparate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glStencilMaskSeparate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glStencilMaskSeparate</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, mask: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glStencilOp"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glStencilOp</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L979" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glStencilOp</span>(fail: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, zfail: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, zpass: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glStencilOpSeparate"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glStencilOpSeparate</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glStencilOpSeparate</span>(face: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, sfail: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, dpfail: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, dppass: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_1.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexBuffer</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, internalFormat: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, buffer: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord1d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord1d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L980" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord1d</span>(s: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord1dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord1dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L981" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord1dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord1f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord1f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L982" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord1f</span>(s: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord1fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord1fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L983" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord1fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord1i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord1i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L984" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord1i</span>(s: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord1iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord1iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L985" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord1iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord1s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord1s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L986" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord1s</span>(s: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord1sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord1sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L987" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord1sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord2d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord2d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L988" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord2d</span>(s: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, t: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord2dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord2dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L989" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord2dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L990" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord2f</span>(s: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, t: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L991" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord2fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord2i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord2i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L992" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord2i</span>(s: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, t: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord2iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord2iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L993" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord2iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord2s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord2s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L994" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord2s</span>(s: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, t: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord2sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord2sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L995" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord2sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord3d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord3d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L996" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord3d</span>(s: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, t: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, r: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord3dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord3dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L997" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord3dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L998" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord3f</span>(s: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, t: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, r: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L999" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord3fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1000" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord3i</span>(s: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, t: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, r: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1001" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord3iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord3s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord3s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1002" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord3s</span>(s: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, t: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, r: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord3sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord3sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1003" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord3sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord4d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord4d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1004" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord4d</span>(s: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, t: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, r: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, q: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord4dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord4dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1005" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord4dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord4f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord4f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1006" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord4f</span>(s: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, t: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, r: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, q: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1007" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord4fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord4i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord4i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1008" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord4i</span>(s: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, t: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, r: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, q: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord4iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord4iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1009" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord4iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord4s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord4s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1010" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord4s</span>(s: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, t: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, r: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, q: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoord4sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoord4sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1011" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoord4sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexCoordPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexCoordPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1012" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexCoordPointer</span>(size: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexEnvf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexEnvf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1013" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexEnvf</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexEnvfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexEnvfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1014" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexEnvfv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexEnvi"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexEnvi</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1015" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexEnvi</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexEnviv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexEnviv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1016" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexEnviv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexGend"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexGend</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1017" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexGend</span>(coord: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexGendv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexGendv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1018" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexGendv</span>(coord: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexGenf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexGenf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1019" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexGenf</span>(coord: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexGenfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexGenfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1020" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexGenfv</span>(coord: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexGeni"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexGeni</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1021" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexGeni</span>(coord: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexGeniv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexGeniv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1022" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexGeniv</span>(coord: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexImage1D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexImage1D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1023" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexImage1D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, internalformat: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, border: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pixels: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexImage2D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexImage2D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1024" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexImage2D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, internalformat: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, border: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pixels: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexParameterIiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexParameterIiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexParameterIiv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexParameterIuiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexParameterIuiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexParameterIuiv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexParameterf"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexParameterf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1025" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexParameterf</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexParameterfv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexParameterfv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1026" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexParameterfv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexParameteri"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexParameteri</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1027" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexParameteri</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, param: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexParameteriv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexParameteriv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1028" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexParameteriv</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pname: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, params: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexSubImage1D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexSubImage1D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1029" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexSubImage1D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, xoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pixels: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTexSubImage2D"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTexSubImage2D</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1030" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTexSubImage2D</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, level: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, xoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, yoffset: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, format: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, pixels: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTransformFeedbackVaryings"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTransformFeedbackVaryings</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTransformFeedbackVaryings</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, varyings: *<span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLchar">GLchar</a></span>, bufferMode: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTranslated"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTranslated</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1031" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTranslated</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glTranslatef"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glTranslatef</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1032" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glTranslatef</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform1f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform1f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform1f</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform1fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform1fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform1fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform1i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform1i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform1i</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform1iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform1iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform1iv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform1ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform1ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform1ui</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform1uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform1uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform1uiv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform2f</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform2fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform2i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform2i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform2i</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform2iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform2iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform2iv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform2ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform2ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform2ui</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform2uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform2uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform2uiv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform3f</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform3fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform3i</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v2: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L51" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform3iv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform3ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform3ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform3ui</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v2: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform3uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform3uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L36" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform3uiv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform4f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform4f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform4f</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v1: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v2: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, v3: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform4fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform4i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform4i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L54" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform4i</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v2: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v3: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform4iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform4iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L55" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform4iv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform4ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform4ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform4ui</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v2: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v3: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniform4uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniform4uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniform4uiv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniformMatrix2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniformMatrix2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniformMatrix2fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, transpose: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniformMatrix2x3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniformMatrix2x3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_1.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniformMatrix2x3fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, transpose: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniformMatrix2x4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniformMatrix2x4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_1.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniformMatrix2x4fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, transpose: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniformMatrix3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniformMatrix3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L57" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniformMatrix3fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, transpose: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniformMatrix3x2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniformMatrix3x2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_1.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniformMatrix3x2fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, transpose: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniformMatrix3x4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniformMatrix3x4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_1.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniformMatrix3x4fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, transpose: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniformMatrix4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniformMatrix4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L58" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniformMatrix4fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, transpose: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniformMatrix4x2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniformMatrix4x2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_1.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniformMatrix4x2fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, transpose: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUniformMatrix4x3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUniformMatrix4x3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_1.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUniformMatrix4x3fv</span>(location: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, count: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, transpose: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, value: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUnmapBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUnmapBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_5.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUnmapBuffer</span>(target: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glUseProgram"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glUseProgram</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glUseProgram</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glValidateProgram"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glValidateProgram</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L60" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glValidateProgram</span>(program: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex2d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex2d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1033" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex2d</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex2dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex2dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1034" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex2dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1035" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex2f</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1036" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex2fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex2i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex2i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1037" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex2i</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex2iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex2iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1038" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex2iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex2s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex2s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1039" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex2s</span>(x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex2sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex2sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1040" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex2sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex3d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex3d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1041" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex3d</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex3dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex3dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1042" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex3dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1043" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex3f</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1044" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex3fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1045" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex3i</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, z: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1046" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex3iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex3s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex3s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1047" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex3s</span>(x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, z: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex3sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex3sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1048" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex3sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex4d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex4d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1049" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex4d</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, w: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex4dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex4dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1050" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex4dv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex4f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex4f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1051" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex4f</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, w: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1052" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex4fv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex4i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex4i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1053" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex4i</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, z: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, w: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex4iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex4iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1054" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex4iv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex4s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex4s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1055" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex4s</span>(x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, z: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, w: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertex4sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertex4sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1056" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertex4sv</span>(v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib1d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib1d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L61" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib1d</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib1dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib1dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L62" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib1dv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib1f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib1f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib1f</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib1fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib1fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L64" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib1fv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib1s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib1s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib1s</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib1sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib1sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib1sv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib2d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib2d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L67" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib2d</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib2dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib2dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L68" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib2dv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib2f</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L70" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib2fv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib2s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib2s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib2s</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib2sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib2sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib2sv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib3d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib3d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L73" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib3d</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib3dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib3dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib3dv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L75" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib3f</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L76" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib3fv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib3s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib3s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L77" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib3s</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, z: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib3sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib3sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib3sv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4Nbv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4Nbv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L79" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4Nbv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4Niv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4Niv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L80" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4Niv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4Nsv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4Nsv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L81" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4Nsv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4Nub"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4Nub</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4Nub</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, y: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, z: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>, w: <span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4Nubv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4Nubv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4Nubv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4Nuiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4Nuiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4Nuiv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4Nusv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4Nusv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L85" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4Nusv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4bv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4bv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L86" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4bv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L87" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4d</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, w: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L88" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4dv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L89" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4f</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, w: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L90" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4fv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L91" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4iv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4s</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, z: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, w: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4sv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4ubv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4ubv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L94" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4ubv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L95" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4uiv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttrib4usv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttrib4usv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttrib4usv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribDivisor"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribDivisor</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_3.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribDivisor</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, divisor: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI1i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI1i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI1i</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI1iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI1iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI1iv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI1ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI1ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI1ui</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI1uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI1uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI1uiv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI2i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI2i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI2i</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI2iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI2iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI2iv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI2ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI2ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI2ui</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI2uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI2uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI2uiv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI3i</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v2: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI3iv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI3ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI3ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI3ui</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v2: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI3uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI3uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI3uiv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI4bv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI4bv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L51" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI4bv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLbyte">GLbyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI4i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI4i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI4i</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v2: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, v3: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI4iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI4iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI4iv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI4sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI4sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L54" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI4sv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI4ubv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI4ubv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L55" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI4ubv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLubyte">GLubyte</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI4ui"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI4ui</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI4ui</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v1: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v2: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v3: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI4uiv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI4uiv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L57" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI4uiv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribI4usv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribI4usv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L58" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribI4usv</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, v0: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLushort">GLushort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribIPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribIPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_3_0.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribIPointer</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, size: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexAttribPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexAttribPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_2_0.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexAttribPointer</span>(index: <span class="SCst"><a href="#Ogl_GLuint">GLuint</a></span>, size: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, normalized: <span class="SCst"><a href="#Ogl_GLboolean">GLboolean</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="STpe">void</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glVertexPointer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glVertexPointer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1057" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glVertexPointer</span>(size: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, type: <span class="SCst"><a href="#Ogl_GLenum">GLenum</a></span>, stride: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, pointer: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLvoid">GLvoid</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glViewport"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glViewport</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_1.swg#L1058" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glViewport</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, width: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>, height: <span class="SCst"><a href="#Ogl_GLsizei">GLsizei</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos2d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos2d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos2d</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos2dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos2dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos2dv</span>(p: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos2f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos2f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos2f</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos2fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos2fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos2fv</span>(p: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos2i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos2i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos2i</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos2iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos2iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos2iv</span>(p: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos2s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos2s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos2s</span>(x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos2sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos2sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos2sv</span>(p: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos3d"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos3d</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L45" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos3d</span>(x: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, y: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>, z: <span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos3dv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos3dv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos3dv</span>(p: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLdouble">GLdouble</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos3f"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos3f</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos3f</span>(x: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, y: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>, z: <span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos3fv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos3fv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos3fv</span>(p: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLfloat">GLfloat</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos3i"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos3i</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos3i</span>(x: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, y: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>, z: <span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos3iv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos3iv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L50" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos3iv</span>(p: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLint">GLint</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos3s"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos3s</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L51" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos3s</span>(x: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, y: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>, z: <span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_glWindowPos3sv"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">glWindowPos3sv</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl_1_4.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">glWindowPos3sv</span>(p: <span class="SKwd">const</span> *<span class="SCst"><a href="#Ogl_GLshort">GLshort</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_wglCreateContext"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">wglCreateContext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl.win32.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">wglCreateContext</span>(hDc: <span class="SCst">HDC</span>)-&gt;<span class="SCst"></span><span class="SCst"><a href="#Ogl_HGLRC">HGLRC</a></span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_wglDeleteContext"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">wglDeleteContext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl.win32.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">wglDeleteContext</span>(hRc: <span class="SCst"><a href="#Ogl_HGLRC">HGLRC</a></span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_wglGetProcAddress"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">wglGetProcAddress</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl.win32.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">wglGetProcAddress</span>(arg1: <span class="SCst">LPCSTR</span>)-&gt;<span class="SKwd">const</span> *<span class="STpe">void</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_wglMakeCurrent"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">wglMakeCurrent</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl.win32.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">wglMakeCurrent</span>(hDc: <span class="SCst">HDC</span>, hRc: <span class="SCst"><a href="#Ogl_HGLRC">HGLRC</a></span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Ogl_wglShareLists"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Ogl.</span><span class="api-item-title-strong">wglShareLists</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/ogl\src\ogl.win32.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">wglShareLists</span>(hRc0, hRc1: <span class="SCst"><a href="#Ogl_HGLRC">HGLRC</a></span>) <span class="SKwd">throw</span></span></div>
<div class="swag-watermark">
Generated on 19-03-2024 with <a href="https://swag-lang.org/index.php">swag</a> 0.31.0</div>
</div>
</div>
</div>

    <script> 
		function getOffsetTop(element, parent) {
			let offsetTop = 0;
			while (element && element != parent) {
				offsetTop += element.offsetTop;
				element = element.offsetParent;
			}
			return offsetTop;
		}	
		document.addEventListener("DOMContentLoaded", function() {
			let hash = window.location.hash;
			if (hash)
			{
				let parentScrollable = document.querySelector('.right');
				if (parentScrollable)
				{
					let targetElement = parentScrollable.querySelector(hash);
					if (targetElement)
					{
						parentScrollable.scrollTop = getOffsetTop(targetElement, parentScrollable);
					}
				}
			}
        });
    </script>\n</body>
</html>
