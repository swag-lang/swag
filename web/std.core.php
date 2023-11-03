<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Swag Std.Core</title>
<link rel="icon" type="image/x-icon" href="favicon.ico">
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
<?php include('common/end-head.php'); ?></head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="left">
<div class="left-page">
<h2>Table of Contents</h2>
<h3>Namespaces</h3>
<h4></h4>
<ul>
<li><a href="#Core_CommandLine">CommandLine</a></li>
<li><a href="#Core_Slice">Slice</a></li>
</ul>
<h3>Structs</h3>
<h4>collections</h4>
<ul>
<li><a href="#Core_Array">Array</a></li>
<li><a href="#Core_ArrayPtr">ArrayPtr</a></li>
<li><a href="#Core_HashSet">HashSet</a></li>
<li><a href="#Core_HashSetEntry">HashSetEntry</a></li>
<li><a href="#Core_HashTable">HashTable</a></li>
<li><a href="#Core_HashTableEntry">HashTableEntry</a></li>
<li><a href="#Core_List">List</a></li>
<li><a href="#Core_ListNode">ListNode</a></li>
<li><a href="#Core_StaticArray">StaticArray</a></li>
</ul>
<h4>math</h4>
<ul>
<li><a href="#Core_Math_NumericArray">NumericArray</a></li>
<li><a href="#Core_Math_Variant">Variant</a></li>
</ul>
<h4>serialization</h4>
<ul>
<li><a href="#Core_Serialization_Serializer">Serializer</a></li>
</ul>
<h4>serialization/read</h4>
<ul>
<li><a href="#Core_Serialization_Decoder">Decoder</a></li>
</ul>
<h4>serialization/write</h4>
<ul>
<li><a href="#Core_Serialization_Encoder">Encoder</a></li>
</ul>
<h4>system</h4>
<ul>
<li><a href="#Core_CommandLine_IsSet">IsSet</a></li>
<li><a href="#Core_CommandLine_ParseOptions">ParseOptions</a></li>
<li><a href="#Core_CommandLine_Result">Result</a></li>
</ul>
<h3>Interfaces</h3>
<h4>serialization</h4>
<ul>
<li><a href="#Core_Serialization_ISerialize">ISerialize</a></li>
</ul>
<h4>serialization/read</h4>
<ul>
<li><a href="#Core_Serialization_IDecoder">IDecoder</a></li>
</ul>
<h4>serialization/write</h4>
<ul>
<li><a href="#Core_Serialization_IEncoder">IEncoder</a></li>
</ul>
<h3>Enums</h3>
<h4>serialization/read</h4>
<ul>
<li><a href="#Core_Serialization_DecoderFlags">DecoderFlags</a></li>
</ul>
<h4>serialization/write</h4>
<ul>
<li><a href="#Core_Serialization_SectionKind">SectionKind</a></li>
</ul>
<h3>Attributes</h3>
<h4>serialization</h4>
<ul>
<li><a href="#Core_Serialization_Alias">Alias</a></li>
<li><a href="#Core_Serialization_Final">Final</a></li>
<li><a href="#Core_Serialization_NoSerialize">NoSerialize</a></li>
<li><a href="#Core_Serialization_PodFinal">PodFinal</a></li>
<li><a href="#Core_Serialization_Version">Version</a></li>
</ul>
<h4>system</h4>
<ul>
<li><a href="#Core_CommandLine_ArgParams">ArgParams</a></li>
</ul>
<h3>Functions</h3>
<h4>algorithms</h4>
<ul>
<li><a href="#Core_swap">Core.swap</a></li>
</ul>
<h4>collections</h4>
<ul>
<li><a href="#Core_Array_add">Array.add</a></li>
<li><a href="#Core_Array_addOnce">Array.addOnce</a></li>
<li><a href="#Core_Array_back">Array.back</a></li>
<li><a href="#Core_Array_backPtr">Array.backPtr</a></li>
<li><a href="#Core_Array_clear">Array.clear</a></li>
<li><a href="#Core_Array_contains">Array.contains</a></li>
<li><a href="#Core_Array_createBuffer">Array.createBuffer</a></li>
<li><a href="#Core_Array_createTemp">Array.createTemp</a></li>
<li><a href="#Core_Array_emplaceAddress">Array.emplaceAddress</a></li>
<li><a href="#Core_Array_emplaceAt">Array.emplaceAt</a></li>
<li><a href="#Core_Array_emplaceInitAddress">Array.emplaceInitAddress</a></li>
<li><a href="#Core_Array_free">Array.free</a></li>
<li><a href="#Core_Array_front">Array.front</a></li>
<li><a href="#Core_Array_frontPtr">Array.frontPtr</a></li>
<li><a href="#Core_Array_grow">Array.grow</a></li>
<li><a href="#Core_Array_insertAt">Array.insertAt</a></li>
<li><a href="#Core_Array_isEmpty">Array.isEmpty</a></li>
<li><a href="#Core_Array_opAffect">Array.opAffect</a></li>
<li><a href="#Core_Array_opCast">Array.opCast</a></li>
<li><a href="#Core_Array_opCount">Array.opCount</a></li>
<li><a href="#Core_Array_opData">Array.opData</a></li>
<li><a href="#Core_Array_opDrop">Array.opDrop</a></li>
<li><a href="#Core_Array_opIndex">Array.opIndex</a></li>
<li><a href="#Core_Array_opIndexAffect">Array.opIndexAffect</a></li>
<li><a href="#Core_Array_opIndexAssign">Array.opIndexAssign</a></li>
<li><a href="#Core_Array_opPostCopy">Array.opPostCopy</a></li>
<li><a href="#Core_Array_opSlice">Array.opSlice</a></li>
<li><a href="#Core_Array_opVisit">Array.opVisit</a></li>
<li><a href="#Core_Array_popBack">Array.popBack</a></li>
<li><a href="#Core_Array_realloc">Array.realloc</a></li>
<li><a href="#Core_Array_remove">Array.remove</a></li>
<li><a href="#Core_Array_removeAt">Array.removeAt</a></li>
<li><a href="#Core_Array_removeAtOrdered">Array.removeAtOrdered</a></li>
<li><a href="#Core_Array_removeBack">Array.removeBack</a></li>
<li><a href="#Core_Array_removeOrdered">Array.removeOrdered</a></li>
<li><a href="#Core_Array_reserve">Array.reserve</a></li>
<li><a href="#Core_Array_resize">Array.resize</a></li>
<li><a href="#Core_Array_sort">Array.sort</a></li>
<li><a href="#Core_Array_sortReverse">Array.sortReverse</a></li>
<li><a href="#Core_Array_swap">Array.swap</a></li>
<li><a href="#Core_Array_toSlice">Array.toSlice</a></li>
<li><a href="#Core_ArrayPtr_addNewPtr">ArrayPtr.addNewPtr</a></li>
<li><a href="#Core_ArrayPtr_clear">ArrayPtr.clear</a></li>
<li><a href="#Core_ArrayPtr_deletePtr">ArrayPtr.deletePtr</a></li>
<li><a href="#Core_ArrayPtr_newPtr">ArrayPtr.newPtr</a></li>
<li><a href="#Core_ArrayPtr_opDrop">ArrayPtr.opDrop</a></li>
<li><a href="#Core_HashSet_add">HashSet.add</a></li>
<li><a href="#Core_HashSet_clear">HashSet.clear</a></li>
<li><a href="#Core_HashSet_contains">HashSet.contains</a></li>
<li><a href="#Core_HashSet_emplaceInternal">HashSet.emplaceInternal</a></li>
<li><a href="#Core_HashSet_free">HashSet.free</a></li>
<li><a href="#Core_HashSet_grow">HashSet.grow</a></li>
<li><a href="#Core_HashSet_hashKey">HashSet.hashKey</a></li>
<li><a href="#Core_HashSet_opCount">HashSet.opCount</a></li>
<li><a href="#Core_HashSet_opDrop">HashSet.opDrop</a></li>
<li><a href="#Core_HashSet_opPostCopy">HashSet.opPostCopy</a></li>
<li><a href="#Core_HashSet_opVisit">HashSet.opVisit</a></li>
<li><a href="#Core_HashSet_remove">HashSet.remove</a></li>
<li><a href="#Core_HashSet_reserve">HashSet.reserve</a></li>
<li><a href="#Core_HashTable_add">HashTable.add</a></li>
<li><a href="#Core_HashTable_clear">HashTable.clear</a></li>
<li><a href="#Core_HashTable_contains">HashTable.contains</a></li>
<li><a href="#Core_HashTable_emplaceInternal">HashTable.emplaceInternal</a></li>
<li><a href="#Core_HashTable_find">HashTable.find</a></li>
<li><a href="#Core_HashTable_free">HashTable.free</a></li>
<li><a href="#Core_HashTable_grow">HashTable.grow</a></li>
<li><a href="#Core_HashTable_hashKey">HashTable.hashKey</a></li>
<li><a href="#Core_HashTable_opCount">HashTable.opCount</a></li>
<li><a href="#Core_HashTable_opDrop">HashTable.opDrop</a></li>
<li><a href="#Core_HashTable_opPostCopy">HashTable.opPostCopy</a></li>
<li><a href="#Core_HashTable_opVisit">HashTable.opVisit</a></li>
<li><a href="#Core_HashTable_remove">HashTable.remove</a></li>
<li><a href="#Core_HashTable_reserve">HashTable.reserve</a></li>
<li><a href="#Core_List_addBack">List.addBack</a></li>
<li><a href="#Core_List_addFront">List.addFront</a></li>
<li><a href="#Core_List_clear">List.clear</a></li>
<li><a href="#Core_List_insertAfter">List.insertAfter</a></li>
<li><a href="#Core_List_insertBefore">List.insertBefore</a></li>
<li><a href="#Core_List_opDrop">List.opDrop</a></li>
<li><a href="#Core_List_opVisit">List.opVisit</a></li>
<li><a href="#Core_StaticArray_add">StaticArray.add</a></li>
<li><a href="#Core_StaticArray_addOnce">StaticArray.addOnce</a></li>
<li><a href="#Core_StaticArray_back">StaticArray.back</a></li>
<li><a href="#Core_StaticArray_backPtr">StaticArray.backPtr</a></li>
<li><a href="#Core_StaticArray_clear">StaticArray.clear</a></li>
<li><a href="#Core_StaticArray_contains">StaticArray.contains</a></li>
<li><a href="#Core_StaticArray_emplaceAddress">StaticArray.emplaceAddress</a></li>
<li><a href="#Core_StaticArray_emplaceAt">StaticArray.emplaceAt</a></li>
<li><a href="#Core_StaticArray_emplaceInitAddress">StaticArray.emplaceInitAddress</a></li>
<li><a href="#Core_StaticArray_free">StaticArray.free</a></li>
<li><a href="#Core_StaticArray_front">StaticArray.front</a></li>
<li><a href="#Core_StaticArray_frontPtr">StaticArray.frontPtr</a></li>
<li><a href="#Core_StaticArray_insertAt">StaticArray.insertAt</a></li>
<li><a href="#Core_StaticArray_isEmpty">StaticArray.isEmpty</a></li>
<li><a href="#Core_StaticArray_opAffect">StaticArray.opAffect</a></li>
<li><a href="#Core_StaticArray_opCast">StaticArray.opCast</a></li>
<li><a href="#Core_StaticArray_opCount">StaticArray.opCount</a></li>
<li><a href="#Core_StaticArray_opData">StaticArray.opData</a></li>
<li><a href="#Core_StaticArray_opDrop">StaticArray.opDrop</a></li>
<li><a href="#Core_StaticArray_opIndex">StaticArray.opIndex</a></li>
<li><a href="#Core_StaticArray_opIndexAffect">StaticArray.opIndexAffect</a></li>
<li><a href="#Core_StaticArray_opIndexAssign">StaticArray.opIndexAssign</a></li>
<li><a href="#Core_StaticArray_opSlice">StaticArray.opSlice</a></li>
<li><a href="#Core_StaticArray_opVisit">StaticArray.opVisit</a></li>
<li><a href="#Core_StaticArray_popBack">StaticArray.popBack</a></li>
<li><a href="#Core_StaticArray_remove">StaticArray.remove</a></li>
<li><a href="#Core_StaticArray_removeAt">StaticArray.removeAt</a></li>
<li><a href="#Core_StaticArray_removeAtOrdered">StaticArray.removeAtOrdered</a></li>
<li><a href="#Core_StaticArray_removeBack">StaticArray.removeBack</a></li>
<li><a href="#Core_StaticArray_removeOrdered">StaticArray.removeOrdered</a></li>
<li><a href="#Core_StaticArray_resize">StaticArray.resize</a></li>
<li><a href="#Core_StaticArray_sort">StaticArray.sort</a></li>
<li><a href="#Core_StaticArray_sortReverse">StaticArray.sortReverse</a></li>
<li><a href="#Core_StaticArray_toSlice">StaticArray.toSlice</a></li>
</ul>
<h4>math</h4>
<ul>
<li><a href="#Core_Math_NumericArray_from">NumericArray.from</a></li>
<li><a href="#Core_Math_NumericArray_mulAdd">NumericArray.mulAdd</a></li>
<li><a href="#Core_Math_NumericArray_opAffect">NumericArray.opAffect</a></li>
<li><a href="#Core_Math_NumericArray_opAssign">NumericArray.opAssign</a></li>
<li><a href="#Core_Math_NumericArray_opBinary">NumericArray.opBinary</a></li>
<li><a href="#Core_Math_NumericArray_opEquals">NumericArray.opEquals</a></li>
<li><a href="#Core_Math_NumericArray_opIndex">NumericArray.opIndex</a></li>
<li><a href="#Core_Math_NumericArray_set">NumericArray.set</a></li>
<li><a href="#Core_Math_Variant_checkValidType">Variant.checkValidType</a></li>
<li><a href="#Core_Math_Variant_drop">Variant.drop</a></li>
<li><a href="#Core_Math_Variant_get">Variant.get</a></li>
<li><a href="#Core_Math_Variant_opAffect">Variant.opAffect</a></li>
<li><a href="#Core_Math_Variant_opCmp">Variant.opCmp</a></li>
<li><a href="#Core_Math_Variant_opDrop">Variant.opDrop</a></li>
<li><a href="#Core_Math_Variant_opEquals">Variant.opEquals</a></li>
<li><a href="#Core_Math_Variant_opPostCopy">Variant.opPostCopy</a></li>
<li><a href="#Core_Math_Variant_opPostMove">Variant.opPostMove</a></li>
<li><a href="#Core_Math_Variant_set">Variant.set</a></li>
</ul>
<h4>serialization</h4>
<ul>
<li><a href="#Core_Serialization_ISerialize_postSerialize">ISerialize.postSerialize</a></li>
<li><a href="#Core_Serialization_ISerialize_read">ISerialize.read</a></li>
<li><a href="#Core_Serialization_ISerialize_readElement">ISerialize.readElement</a></li>
<li><a href="#Core_Serialization_ISerialize_write">ISerialize.write</a></li>
<li><a href="#Core_Serialization_ISerialize_writeElement">ISerialize.writeElement</a></li>
<li><a href="#Core_Serialization_Serializer_beginSection">Serializer.beginSection</a></li>
<li><a href="#Core_Serialization_Serializer_end">Serializer.end</a></li>
<li><a href="#Core_Serialization_Serializer_endSection">Serializer.endSection</a></li>
<li><a href="#Core_Serialization_Serializer_isReading">Serializer.isReading</a></li>
<li><a href="#Core_Serialization_Serializer_isWriting">Serializer.isWriting</a></li>
<li><a href="#Core_Serialization_Serializer_serialize">Serializer.serialize</a></li>
<li><a href="#Core_Serialization_Serializer_startRead">Serializer.startRead</a></li>
<li><a href="#Core_Serialization_Serializer_startWrite">Serializer.startWrite</a></li>
</ul>
<h4>serialization/read</h4>
<ul>
<li><a href="#Core_Serialization_Decoder_IDecoder_beginField">Decoder.IDecoder.beginField</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_beginSection">Decoder.IDecoder.beginSection</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_end">Decoder.IDecoder.end</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_endField">Decoder.IDecoder.endField</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_endSection">Decoder.IDecoder.endSection</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_getVersion">Decoder.IDecoder.getVersion</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_read">Decoder.IDecoder.read</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readBool">Decoder.IDecoder.readBool</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readBufferU8">Decoder.IDecoder.readBufferU8</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readF32">Decoder.IDecoder.readF32</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readF64">Decoder.IDecoder.readF64</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readS16">Decoder.IDecoder.readS16</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readS32">Decoder.IDecoder.readS32</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readS64">Decoder.IDecoder.readS64</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readS8">Decoder.IDecoder.readS8</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readU16">Decoder.IDecoder.readU16</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readU32">Decoder.IDecoder.readU32</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readU64">Decoder.IDecoder.readU64</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_readU8">Decoder.IDecoder.readU8</a></li>
<li><a href="#Core_Serialization_Decoder_IDecoder_start">Decoder.IDecoder.start</a></li>
<li><a href="#Core_Serialization_Decoder_end">Decoder.end</a></li>
<li><a href="#Core_Serialization_Decoder_readAll">Decoder.readAll</a></li>
<li><a href="#Core_Serialization_Decoder_readTypeValue">Decoder.readTypeValue</a></li>
<li><a href="#Core_Serialization_Decoder_readValue">Decoder.readValue</a></li>
<li><a href="#Core_Serialization_Decoder_start">Decoder.start</a></li>
<li><a href="#Core_Serialization_IDecoder_beginField">Serialization.IDecoder.beginField</a></li>
<li><a href="#Core_Serialization_IDecoder_beginSection">Serialization.IDecoder.beginSection</a></li>
<li><a href="#Core_Serialization_IDecoder_end">Serialization.IDecoder.end</a></li>
<li><a href="#Core_Serialization_IDecoder_endField">Serialization.IDecoder.endField</a></li>
<li><a href="#Core_Serialization_IDecoder_endSection">Serialization.IDecoder.endSection</a></li>
<li><a href="#Core_Serialization_IDecoder_getVersion">Serialization.IDecoder.getVersion</a></li>
<li><a href="#Core_Serialization_IDecoder_read">Serialization.IDecoder.read</a></li>
<li><a href="#Core_Serialization_IDecoder_readBool">Serialization.IDecoder.readBool</a></li>
<li><a href="#Core_Serialization_IDecoder_readBufferU8">Serialization.IDecoder.readBufferU8</a></li>
<li><a href="#Core_Serialization_IDecoder_readF32">Serialization.IDecoder.readF32</a></li>
<li><a href="#Core_Serialization_IDecoder_readF64">Serialization.IDecoder.readF64</a></li>
<li><a href="#Core_Serialization_IDecoder_readS16">Serialization.IDecoder.readS16</a></li>
<li><a href="#Core_Serialization_IDecoder_readS32">Serialization.IDecoder.readS32</a></li>
<li><a href="#Core_Serialization_IDecoder_readS64">Serialization.IDecoder.readS64</a></li>
<li><a href="#Core_Serialization_IDecoder_readS8">Serialization.IDecoder.readS8</a></li>
<li><a href="#Core_Serialization_IDecoder_readU16">Serialization.IDecoder.readU16</a></li>
<li><a href="#Core_Serialization_IDecoder_readU32">Serialization.IDecoder.readU32</a></li>
<li><a href="#Core_Serialization_IDecoder_readU64">Serialization.IDecoder.readU64</a></li>
<li><a href="#Core_Serialization_IDecoder_readU8">Serialization.IDecoder.readU8</a></li>
<li><a href="#Core_Serialization_IDecoder_start">Serialization.IDecoder.start</a></li>
</ul>
<h4>serialization/write</h4>
<ul>
<li><a href="#Core_Serialization_Encoder_IEncoder_beginField">Encoder.IEncoder.beginField</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_beginSection">Encoder.IEncoder.beginSection</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_end">Encoder.IEncoder.end</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_endField">Encoder.IEncoder.endField</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_endSection">Encoder.IEncoder.endSection</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_start">Encoder.IEncoder.start</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_write">Encoder.IEncoder.write</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeBool">Encoder.IEncoder.writeBool</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeBufferU8">Encoder.IEncoder.writeBufferU8</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeF32">Encoder.IEncoder.writeF32</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeF64">Encoder.IEncoder.writeF64</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeS16">Encoder.IEncoder.writeS16</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeS32">Encoder.IEncoder.writeS32</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeS64">Encoder.IEncoder.writeS64</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeS8">Encoder.IEncoder.writeS8</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeU16">Encoder.IEncoder.writeU16</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeU32">Encoder.IEncoder.writeU32</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeU64">Encoder.IEncoder.writeU64</a></li>
<li><a href="#Core_Serialization_Encoder_IEncoder_writeU8">Encoder.IEncoder.writeU8</a></li>
<li><a href="#Core_Serialization_Encoder_end">Encoder.end</a></li>
<li><a href="#Core_Serialization_Encoder_start">Encoder.start</a></li>
<li><a href="#Core_Serialization_Encoder_writeAll">Encoder.writeAll</a></li>
<li><a href="#Core_Serialization_Encoder_writeTypeValue">Encoder.writeTypeValue</a></li>
<li><a href="#Core_Serialization_Encoder_writeValue">Encoder.writeValue</a></li>
<li><a href="#Core_Serialization_IEncoder_beginField">Serialization.IEncoder.beginField</a></li>
<li><a href="#Core_Serialization_IEncoder_beginSection">Serialization.IEncoder.beginSection</a></li>
<li><a href="#Core_Serialization_IEncoder_end">Serialization.IEncoder.end</a></li>
<li><a href="#Core_Serialization_IEncoder_endField">Serialization.IEncoder.endField</a></li>
<li><a href="#Core_Serialization_IEncoder_endSection">Serialization.IEncoder.endSection</a></li>
<li><a href="#Core_Serialization_IEncoder_start">Serialization.IEncoder.start</a></li>
<li><a href="#Core_Serialization_IEncoder_write">Serialization.IEncoder.write</a></li>
<li><a href="#Core_Serialization_IEncoder_writeBool">Serialization.IEncoder.writeBool</a></li>
<li><a href="#Core_Serialization_IEncoder_writeBufferU8">Serialization.IEncoder.writeBufferU8</a></li>
<li><a href="#Core_Serialization_IEncoder_writeF32">Serialization.IEncoder.writeF32</a></li>
<li><a href="#Core_Serialization_IEncoder_writeF64">Serialization.IEncoder.writeF64</a></li>
<li><a href="#Core_Serialization_IEncoder_writeS16">Serialization.IEncoder.writeS16</a></li>
<li><a href="#Core_Serialization_IEncoder_writeS32">Serialization.IEncoder.writeS32</a></li>
<li><a href="#Core_Serialization_IEncoder_writeS64">Serialization.IEncoder.writeS64</a></li>
<li><a href="#Core_Serialization_IEncoder_writeS8">Serialization.IEncoder.writeS8</a></li>
<li><a href="#Core_Serialization_IEncoder_writeU16">Serialization.IEncoder.writeU16</a></li>
<li><a href="#Core_Serialization_IEncoder_writeU32">Serialization.IEncoder.writeU32</a></li>
<li><a href="#Core_Serialization_IEncoder_writeU64">Serialization.IEncoder.writeU64</a></li>
<li><a href="#Core_Serialization_IEncoder_writeU8">Serialization.IEncoder.writeU8</a></li>
</ul>
<h4>slice</h4>
<ul>
<li><a href="#Core_Slice_contains">Slice.contains</a></li>
<li><a href="#Core_Slice_equals">Slice.equals</a></li>
<li><a href="#Core_Slice_findLinear">Slice.findLinear</a></li>
<li><a href="#Core_Slice_findSorted">Slice.findSorted</a></li>
<li><a href="#Core_Slice_insertionSort">Slice.insertionSort</a></li>
<li><a href="#Core_Slice_isSorted">Slice.isSorted</a></li>
<li><a href="#Core_Slice_modifyInPlace">Slice.modifyInPlace</a></li>
<li><a href="#Core_Slice_quickSort">Slice.quickSort</a></li>
<li><a href="#Core_Slice_reverse">Slice.reverse</a></li>
<li><a href="#Core_Slice_sort">Slice.sort</a></li>
</ul>
<h4>system</h4>
<ul>
<li><a href="#Core_CommandLine_getField">CommandLine.getField</a></li>
<li><a href="#Core_CommandLine_isOption">CommandLine.isOption</a></li>
<li><a href="#Core_CommandLine_parse">CommandLine.parse</a></li>
<li><a href="#Core_CommandLine_splitArguments">CommandLine.splitArguments</a></li>
<li><a href="#Core_CommandLine_parse_parse_checkNext">parse.checkNext</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="right-page">
<h1>Swag Std.Core</h1>
<p> The <span class="code-inline">Std.core</span> module contains basic types and algorithms.  It is the base of all other modules of the standard <span class="code-inline">Std</span> workspace. </p>
<p> You will find collections like <a href="#Core_Array">Array</a> or <a href="#Core_HashTable">HashTable</a>, a dynamic [[String]] and so on. </p>
<h1>Content</h1>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">Array</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>This is a generic dynamic array. </p>
<table class="table-enumeration">
<tr>
<td>allocator</td>
<td class="code-type"><span class="SCst">IAllocator</span></td>
<td>Associated allocator. </td>
</tr>
<tr>
<td>buffer</td>
<td class="code-type"><span class="SCde">^<span class="SCst">T</span></span></td>
<td>Memory block of all datas. </td>
</tr>
<tr>
<td>count</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td>Number of valid datas. </td>
</tr>
<tr>
<td>capacity</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td>Number of elements that can be stored in the buffer. </td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_add">add</a></span><span class="SCde">(*<span class="SCst">Array</span>, <span class="SCst">T</span>)</span></td>
<td>Add a copy of one element at the end of the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_add">add</a></span><span class="SCde">(*<span class="SCst">Array</span>, <span class="SCst">T</span>)</span></td>
<td>Move one element at the end of the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_add">add</a></span><span class="SCde">(*<span class="SCst">Array</span>, <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></td>
<td>Append a slice to the end of this instance. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_addOnce">addOnce</a></span></td>
<td>Add a copy of one element at the end of the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_back">back</a></span></td>
<td>Returns a copy of the last element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_backPtr">backPtr</a></span></td>
<td>Returns the address of the last element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_clear">clear</a></span></td>
<td>Set the number of elements to 0. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_contains">contains</a></span></td>
<td>Returns true if the given <span class="code-inline">value</span> is in the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_createBuffer">createBuffer</a></span></td>
<td>Create a working buffer. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_createTemp">createTemp</a></span></td>
<td>Create a temporary array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_emplaceAddress">emplaceAddress</a></span></td>
<td>Reserve room at the end of the array for <span class="code-inline">num</span> elements, but does not  initialize them. Returns the address of the first element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_emplaceAt">emplaceAt</a></span></td>
<td>Move some values at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the values are moved at the end of the array.  Order is preserved. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_emplaceInitAddress">emplaceInitAddress</a></span></td>
<td>Reserve room at the end of the array for <span class="code-inline">num</span> elements. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_free">free</a></span></td>
<td>Free the array content. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_front">front</a></span></td>
<td>Returns a copy of the first element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_frontPtr">frontPtr</a></span></td>
<td>Returns the address of the first element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_grow">grow</a></span></td>
<td>Ensure the Array is big enough to store at least <span class="code-inline">newCount</span> elements  Number of valid elements does not change. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_insertAt">insertAt</a></span><span class="SCde">(*<span class="SCst">Array</span>, <span class="STpe">u64</span>, <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></td>
<td>Insert some values at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the values are added at the end of the array.  Order is preserved. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_insertAt">insertAt</a></span><span class="SCde">(*<span class="SCst">Array</span>, <span class="STpe">u64</span>, <span class="SCst">T</span>)</span></td>
<td>Move a value at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the value is added at the end of the array.  Order is preserved. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_insertAt">insertAt</a></span><span class="SCde">(*<span class="SCst">Array</span>, <span class="STpe">u64</span>, <span class="SCst">T</span>)</span></td>
<td>Insert a value at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the value is added at the end of the array.  Order is preserved. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_isEmpty">isEmpty</a></span></td>
<td>Returns true if the array is empty. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_popBack">popBack</a></span></td>
<td>Returns a copy of the last element, and remove it from the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_realloc">realloc</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_remove">remove</a></span><span class="SCde">(*<span class="SCst">Array</span>, <span class="STpe">u64</span>, <span class="STpe">u64</span>)</span></td>
<td>Remove <span class="code-inline">num</span> elements starting at <span class="code-inline">index</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_remove">remove</a></span><span class="SCde">(*<span class="SCst">Array</span>, <span class="SCst">V</span>)</span></td>
<td>Remove the given <span class="code-inline">value</span>  If not found, does nothing. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_removeAt">removeAt</a></span></td>
<td>Remove an element at <span class="code-inline">index</span> by replacing it with the last element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_removeAtOrdered">removeAtOrdered</a></span></td>
<td>Remove <span class="code-inline">numValues</span> elements at <span class="code-inline">index</span> by shifting all others. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_removeBack">removeBack</a></span></td>
<td>Remove the last element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_removeOrdered">removeOrdered</a></span></td>
<td>Remove the given <span class="code-inline">value</span>  If not found, does nothing. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_reserve">reserve</a></span></td>
<td>Reserve room for <span class="code-inline">newCapacity</span> elements without changing the array count  Number of valid elements does not change. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_resize">resize</a></span></td>
<td>Change the number of valid elements in the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_sort">sort</a></span><span class="SCde">(*<span class="SCst">Array</span>, <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-><span class="STpe">s32</span>)</span></td>
<td>Sort array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_sort">sort</a></span><span class="SCde">(*<span class="SCst">Array</span>)</span></td>
<td>Sort array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_sortReverse">sortReverse</a></span></td>
<td>Sort array in reverse order (from biggest to lowest value). </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_swap">swap</a></span></td>
<td>Swap two elements. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_toSlice">toSlice</a></span></td>
<td>Returns a slice. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opAffect">opAffect</a></span></td>
<td>Initializes an Array that contains values copied from the specified array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opCast">opCast</a></span><span class="SCde">(*<span class="SCst">Array</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opCast">opCast</a></span><span class="SCde">(*<span class="SCst">Array</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opCount">opCount</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opData">opData</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opDrop">opDrop</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opIndex">opIndex</a></span><span class="SCde">(<span class="SKwd">const</span> *<span class="SCst">Array</span>, <span class="STpe">u64</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opIndex">opIndex</a></span><span class="SCde">(<span class="SKwd">const</span> *<span class="SCst">Array</span>, <span class="STpe">u64</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opIndexAffect">opIndexAffect</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opIndexAssign">opIndexAssign</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opPostCopy">opPostCopy</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opSlice">opSlice</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Array_opVisit">opVisit</a></span></td>
<td>Visit every elements of the array. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_add"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">add</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L307" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a copy of one element at the end of the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>Move one element at the end of the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>Append a slice to the end of this instance. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, values: <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_addOnce"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">addOnce</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L296" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a copy of one element at the end of the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addOnce</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_back"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">back</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L636" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a copy of the last element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">back</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_backPtr"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">backPtr</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L644" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the address of the last element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">backPtr</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L284" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the number of elements to 0. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_contains"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">contains</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L595" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the given <span class="code-inline">value</span> is in the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">V</span>) <span class="SFct">contains</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">V</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_createBuffer"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">createBuffer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L219" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a working buffer. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createBuffer</span>(size: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_createTemp"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">createTemp</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L210" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Create a temporary array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">TA</span>) <span class="SFct">createTemp</span>()</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_emplaceAddress"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">emplaceAddress</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L325" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reserve room at the end of the array for <span class="code-inline">num</span> elements, but does not  initialize them. Returns the address of the first element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">emplaceAddress</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, num: <span class="STpe">u32</span> = <span class="SNum">1</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_emplaceAt"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">emplaceAt</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L432" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Move some values at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the values are moved at the end of the array.  Order is preserved. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">emplaceAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, values: [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_emplaceInitAddress"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">emplaceInitAddress</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L334" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reserve room at the end of the array for <span class="code-inline">num</span> elements. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">emplaceInitAddress</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, num: <span class="STpe">u32</span> = <span class="SNum">1</span>)</span></div>
<p> Returns the address of the first element </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_free"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">free</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L192" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Free the array content. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">free</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_front"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">front</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L621" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a copy of the first element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">front</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_frontPtr"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">frontPtr</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L629" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the address of the first element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">frontPtr</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_grow"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">grow</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L239" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Ensure the Array is big enough to store at least <span class="code-inline">newCount</span> elements  Number of valid elements does not change. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">grow</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newCount: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_insertAt"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">insertAt</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L346" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert a value at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the value is added at the end of the array.  Order is preserved. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, value: <span class="SCst">T</span>)</span></div>
<p>Move a value at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the value is added at the end of the array.  Order is preserved. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, value: <span class="SCst">T</span>)</span></div>
<p>Insert some values at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the values are added at the end of the array.  Order is preserved. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, values: <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_isEmpty"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">isEmpty</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L228" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the array is empty. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isEmpty</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opAffect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opAffect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initializes an Array that contains values copied from the specified array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, arr: <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opCast"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opCast</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opCast</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)
<span class="SKwd">func</span> <span class="SFct">opCast</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opCount"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opCount</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opCount</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opData"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opData</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opData</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opDrop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opDrop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opDrop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opIndex"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opIndex</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opIndex</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>)
<span class="SKwd">func</span> <span class="SFct">opIndex</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opIndexAffect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opIndexAffect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L90" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opIndexAffect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opIndexAssign"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opIndexAssign</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opIndexAssign</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opPostCopy"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opPostCopy</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opPostCopy</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opSlice"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opSlice</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opSlice</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, lower: <span class="STpe">u64</span>, upper: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_opVisit"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">opVisit</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L130" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Visit every elements of the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, stmt: <span class="STpe">code</span>)</span></div>
<p> Visiting by address and in reverse order is supported. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_popBack"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">popBack</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L651" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a copy of the last element, and remove it from the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">popBack</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_realloc"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">realloc</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L162" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">realloc</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newCapacity: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_remove"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">remove</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L508" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove <span class="code-inline">num</span> elements starting at <span class="code-inline">index</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">remove</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, num: <span class="STpe">u64</span>)</span></div>
<p>Remove the given <span class="code-inline">value</span>  If not found, does nothing. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">V</span>) <span class="SFct">remove</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">V</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_removeAt"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">removeAt</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L531" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove an element at <span class="code-inline">index</span> by replacing it with the last element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">removeAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>)</span></div>
<p> Order is <b>not preserved</b> </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_removeAtOrdered"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">removeAtOrdered</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L543" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove <span class="code-inline">numValues</span> elements at <span class="code-inline">index</span> by shifting all others. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">removeAtOrdered</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, numValues: <span class="STpe">u64</span> = <span class="SNum">1</span>)</span></div>
<p> Order is preserved </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_removeBack"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">removeBack</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L661" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove the last element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">removeBack</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_removeOrdered"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">removeOrdered</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L586" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove the given <span class="code-inline">value</span>  If not found, does nothing. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">V</span>) <span class="SFct">removeOrdered</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">V</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_reserve"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">reserve</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L249" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reserve room for <span class="code-inline">newCapacity</span> elements without changing the array count  Number of valid elements does not change. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">reserve</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newCapacity: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_resize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">resize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L266" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the number of valid elements in the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newCount: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_sort"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">sort</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L602" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Sort array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sort</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>Sort array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sort</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cb: <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-&gt;<span class="STpe">s32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_sortReverse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">sortReverse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L608" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Sort array in reverse order (from biggest to lowest value). </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sortReverse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_swap"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">swap</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L498" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Swap two elements. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">swap</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, idx0: <span class="STpe">u64</span>, idx1: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Array_toSlice"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Array.</span><span class="api-item-title-strong">toSlice</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\array.swg#L232" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a slice. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toSlice</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_ArrayPtr"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">ArrayPtr</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\arrayptr.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>using base</td>
<td class="code-type"><span class="SCde"><span class="SCst"><a href="#Core_Array">Array</a></span>'(*<span class="SCst">T</span>)</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_ArrayPtr_addNewPtr">addNewPtr</a></span></td>
<td>Allocate a new pointer, and add it to the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_ArrayPtr_clear">clear</a></span></td>
<td>Release content. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_ArrayPtr_deletePtr">deletePtr</a></span></td>
<td>Delete one pointer allocated here. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_ArrayPtr_newPtr">newPtr</a></span></td>
<td>Allocate a new pointer by using the contextual allocator. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_ArrayPtr_opDrop">opDrop</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_ArrayPtr_addNewPtr"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ArrayPtr.</span><span class="api-item-title-strong">addNewPtr</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\arrayptr.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Allocate a new pointer, and add it to the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addNewPtr</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_ArrayPtr_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ArrayPtr.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\arrayptr.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release content. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_ArrayPtr_deletePtr"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ArrayPtr.</span><span class="api-item-title-strong">deletePtr</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\arrayptr.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Delete one pointer allocated here. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">deletePtr</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, item: *<span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_ArrayPtr_newPtr"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ArrayPtr.</span><span class="api-item-title-strong">newPtr</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\arrayptr.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Allocate a new pointer by using the contextual allocator. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">newPtr</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_ArrayPtr_opDrop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ArrayPtr.</span><span class="api-item-title-strong">opDrop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\arrayptr.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opDrop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine"><span class="api-item-title-kind">namespace</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">CommandLine</span></span>
</td>
</tr>
</table>
</p>
<h3>Structs</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><a href="#Core_CommandLine_IsSet"><span class="SCst">IsSet</span></a></td>
<td>The <span class="code-inline">IsSet</span> generic struct is a mirror of the user input struct. </td>
</tr>
<tr>
<td class="code-type"><a href="#Core_CommandLine_ParseOptions"><span class="SCst">ParseOptions</span></a></td>
<td></td>
</tr>
<tr>
<td class="code-type"><a href="#Core_CommandLine_Result"><span class="SCst">Result</span></a></td>
<td>Result of the [parse] function. </td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_CommandLine_getField">getField</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_CommandLine_isOption">isOption</a></span></td>
<td>Returns true if <span class="code-inline">oneArg</span> is a valid option (starting with a delimiter). </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_CommandLine_parse">parse</a></span></td>
<td>Parse all the arguments and fill the result. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_CommandLine_splitArguments">splitArguments</a></span></td>
<td>Clean and split a list of arguments  -option:value or -option=value =&gt; -option value. </td>
</tr>
</table>
<h3>Attributes</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><a href="#Core_CommandLine_ArgParams"><span class="SCst">ArgParams</span></a></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine_ArgParams"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">CommandLine.</span><span class="api-item-title-strong">ArgParams</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\system\commandline.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Core_CommandLine_ArgParams">ArgParams</a></span>(nameAlias: <span class="STpe">string</span> = <span class="SKwd">null</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine_IsSet"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">CommandLine.</span><span class="api-item-title-strong">IsSet</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\system\commandline.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The <span class="code-inline">IsSet</span> generic struct is a mirror of the user input struct. </p>
<p> Each field has the same name as the original one, but with a <span class="code-inline">bool</span> type. </p>
<p> It will indicate, after the command line parsing, that the corresponding argument has been  specified or not by the user. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine_ParseOptions"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">CommandLine.</span><span class="api-item-title-strong">ParseOptions</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\system\commandline.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine_Result"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">CommandLine.</span><span class="api-item-title-strong">Result</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\system\commandline.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Result of the [parse] function. </p>
<table class="table-enumeration">
<tr>
<td>isSet</td>
<td class="code-type"><span class="SCst"><a href="#Core_CommandLine_IsSet">IsSet</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine_getField"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">CommandLine.</span><span class="api-item-title-strong">getField</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\system\commandline.swg#L78" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getField</span>(typeStruct: <span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfoStruct</span>, name: <span class="STpe">string</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine_isOption"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">CommandLine.</span><span class="api-item-title-strong">isOption</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\system\commandline.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if <span class="code-inline">oneArg</span> is a valid option (starting with a delimiter). </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isOption</span>(oneArg: <span class="STpe">string</span>, _options: <span class="SCst"><a href="#Core_CommandLine_ParseOptions">ParseOptions</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine_parse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">CommandLine.</span><span class="api-item-title-strong">parse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\system\commandline.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Parse all the arguments and fill the result. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">parse</span>(st: *<span class="SCst">T</span>, args: <span class="SKwd">const</span> [..] <span class="STpe">string</span>, options = <span class="SCst"><a href="#Core_CommandLine">CommandLine</a></span>.<span class="SCst"><a href="#Core_CommandLine_ParseOptions">ParseOptions</a></span>{}) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine_parse_parse_checkNext"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">parse.</span><span class="api-item-title-strong">checkNext</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\system\commandline.swg#L99" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">checkNext</span>() <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_CommandLine_splitArguments"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">CommandLine.</span><span class="api-item-title-strong">splitArguments</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\system\commandline.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clean and split a list of arguments  -option:value or -option=value =&gt; -option value. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">splitArguments</span>(args: <span class="SKwd">const</span> [..] <span class="STpe">string</span>, options: <span class="SCst"><a href="#Core_CommandLine_ParseOptions">ParseOptions</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">HashSet</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>HASH_FREE</td>
<td class="code-type"></td>
<td></td>
</tr>
<tr>
<td>HASH_DELETED</td>
<td class="code-type"></td>
<td></td>
</tr>
<tr>
<td>HASH_FIRST</td>
<td class="code-type"></td>
<td></td>
</tr>
<tr>
<td>HASH_MASK</td>
<td class="code-type"></td>
<td></td>
</tr>
<tr>
<td>allocator</td>
<td class="code-type"><span class="SCst">IAllocator</span></td>
<td></td>
</tr>
<tr>
<td>table</td>
<td class="code-type"><span class="SCde">^<span class="SCst"><a href="#Core_HashSetEntry">HashSetEntry</a></span></span></td>
<td></td>
</tr>
<tr>
<td>count</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td>Number of valid entries. </td>
</tr>
<tr>
<td>capacity</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td>Number of total entries the table can hold. </td>
</tr>
<tr>
<td>deleted</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td>Number of deleted entries. </td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_add">add</a></span><span class="SCde">(*<span class="SCst">HashSet</span>, <span class="SCst">K</span>)</span></td>
<td>Add a new key. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_add">add</a></span><span class="SCde">(*<span class="SCst">HashSet</span>, <span class="SCst">K</span>)</span></td>
<td>Add a new key. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_clear">clear</a></span></td>
<td>Remove all elements. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_contains">contains</a></span></td>
<td>Returns true if the table contains the given key. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_emplaceInternal">emplaceInternal</a></span></td>
<td>Add a new key. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_free">free</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_grow">grow</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_hashKey">hashKey</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_remove">remove</a></span></td>
<td>Remove the given key if it exists. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_reserve">reserve</a></span></td>
<td>Reserve <span class="code-inline">newCapacity</span> elements in the table. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_opCount">opCount</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_opDrop">opDrop</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_opPostCopy">opPostCopy</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashSet_opVisit">opVisit</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_add"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">add</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L171" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new key. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">K</span>)</span></div>
<p>Add a new key. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">K</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L309" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove all elements. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_contains"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">contains</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L290" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the table contains the given key. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">contains</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">K</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_emplaceInternal"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">emplaceInternal</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L236" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new key. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">emplaceInternal</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, hashVal: <span class="STpe">u32</span>, key: <span class="STpe">u8</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_free"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">free</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L103" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">free</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_grow"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">grow</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">grow</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_hashKey"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">hashKey</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hashKey</span>(key: <span class="SCst">K</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_opCount"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">opCount</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opCount</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_opDrop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">opDrop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opDrop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_opPostCopy"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">opPostCopy</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opPostCopy</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_opVisit"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">opVisit</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L60" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, stmt: <span class="STpe">code</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_remove"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">remove</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L267" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove the given key if it exists. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">remove</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">K</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSet_reserve"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashSet.</span><span class="api-item-title-strong">reserve</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L132" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reserve <span class="code-inline">newCapacity</span> elements in the table. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">reserve</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newCapacity: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashSetEntry"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">HashSetEntry</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashset.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>hash</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>key</td>
<td class="code-type"><span class="SCst">K</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">HashTable</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>HASH_FREE</td>
<td class="code-type"></td>
<td></td>
</tr>
<tr>
<td>HASH_DELETED</td>
<td class="code-type"></td>
<td></td>
</tr>
<tr>
<td>HASH_FIRST</td>
<td class="code-type"></td>
<td></td>
</tr>
<tr>
<td>HASH_MASK</td>
<td class="code-type"></td>
<td></td>
</tr>
<tr>
<td>allocator</td>
<td class="code-type"><span class="SCst">IAllocator</span></td>
<td></td>
</tr>
<tr>
<td>table</td>
<td class="code-type"><span class="SCde">^<span class="SCst"><a href="#Core_HashTableEntry">HashTableEntry</a></span></span></td>
<td></td>
</tr>
<tr>
<td>count</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td>Number of valid entries. </td>
</tr>
<tr>
<td>capacity</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td>Number of total entries the table can hold. </td>
</tr>
<tr>
<td>deleted</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td>Number of deleted entries. </td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_add">add</a></span><span class="SCde">(*<span class="SCst">HashTable</span>, <span class="SCst">K</span>, <span class="SCst">V</span>)</span></td>
<td>Add a new key value pair. If the key already exists, then the value will be replaced. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_add">add</a></span><span class="SCde">(*<span class="SCst">HashTable</span>, <span class="SCst">K</span>, <span class="SCst">V</span>)</span></td>
<td>Add a new key value pair. If the key already exists, then the value will be replaced. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_clear">clear</a></span></td>
<td>Remove all elements. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_contains">contains</a></span></td>
<td>Returns true if the table contains the given key. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_emplaceInternal">emplaceInternal</a></span></td>
<td>Use with care !. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_find">find</a></span></td>
<td>Find the given key, and returns the corresponding entry. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_free">free</a></span></td>
<td>Free the hashtable content. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_grow">grow</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_hashKey">hashKey</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_remove">remove</a></span></td>
<td>Remove the given key if it exists. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_reserve">reserve</a></span></td>
<td>Reserve <span class="code-inline">newCapacity</span> elements in the table. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_opCount">opCount</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_opDrop">opDrop</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_opPostCopy">opPostCopy</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_HashTable_opVisit">opVisit</a></span></td>
<td>alias0 is the key, and alias1 is the value. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_add"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">add</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L184" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new key value pair. If the key already exists, then the value will be replaced. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">K</span>, value: <span class="SCst">V</span>)</span></div>
<p>Add a new key value pair. If the key already exists, then the value will be replaced. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">K</span>, value: <span class="SCst">V</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L361" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove all elements. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_contains"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">contains</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L322" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the table contains the given key. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">contains</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">K</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_emplaceInternal"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">emplaceInternal</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L262" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Use with care !. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">emplaceInternal</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, hashVal: <span class="STpe">u32</span>, key: <span class="STpe">u8</span>, value: <span class="STpe">u8</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_find"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">find</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L342" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Find the given key, and returns the corresponding entry. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">find</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">K</span>)</span></div>
<p> Returns null if the key is not there. </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_free"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">free</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L120" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Free the hashtable content. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">free</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_grow"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">grow</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L109" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">grow</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_hashKey"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">hashKey</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L104" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">hashKey</span>(key: <span class="SCst">K</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_opCount"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">opCount</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opCount</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_opDrop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">opDrop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opDrop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_opPostCopy"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">opPostCopy</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opPostCopy</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_opVisit"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">opVisit</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>alias0 is the key, and alias1 is the value. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, stmt: <span class="STpe">code</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_remove"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">remove</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L299" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove the given key if it exists. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">remove</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, key: <span class="SCst">K</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTable_reserve"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">HashTable.</span><span class="api-item-title-strong">reserve</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L144" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reserve <span class="code-inline">newCapacity</span> elements in the table. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">reserve</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newCapacity: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_HashTableEntry"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">HashTableEntry</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\hashtable.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>hash</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>key</td>
<td class="code-type"><span class="SCst">K</span></td>
<td></td>
</tr>
<tr>
<td>value</td>
<td class="code-type"><span class="SCst">V</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_List"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">List</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\list.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>head</td>
<td class="code-type"><span class="SCde">*<span class="SCst"><a href="#Core_ListNode">ListNode</a></span></span></td>
<td></td>
</tr>
<tr>
<td>tail</td>
<td class="code-type"><span class="SCde">*<span class="SCst"><a href="#Core_ListNode">ListNode</a></span></span></td>
<td></td>
</tr>
<tr>
<td>count</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_addBack">addBack</a></span><span class="SCde">(*<span class="SCst">List</span>, <span class="SCst">T</span>)</span></td>
<td>Add a new element on front. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_addBack">addBack</a></span><span class="SCde">(*<span class="SCst">List</span>, <span class="SCst">T</span>)</span></td>
<td>Add a new element on front. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_addFront">addFront</a></span><span class="SCde">(*<span class="SCst">List</span>, <span class="SCst">T</span>)</span></td>
<td>Add a new element on front. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_addFront">addFront</a></span><span class="SCde">(*<span class="SCst">List</span>, <span class="SCst">T</span>)</span></td>
<td>Add a new element on front. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_clear">clear</a></span></td>
<td>Clear all elements. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_insertAfter">insertAfter</a></span><span class="SCde">(*<span class="SCst">List</span>, *<span class="SCst">ListNode</span>, <span class="SCst">T</span>)</span></td>
<td>Insert a node before the reference. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_insertAfter">insertAfter</a></span><span class="SCde">(*<span class="SCst">List</span>, *<span class="SCst">ListNode</span>, <span class="SCst">T</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_insertBefore">insertBefore</a></span><span class="SCde">(*<span class="SCst">List</span>, *<span class="SCst">ListNode</span>, <span class="SCst">T</span>)</span></td>
<td>Insert a node before the reference. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_insertBefore">insertBefore</a></span><span class="SCde">(*<span class="SCst">List</span>, *<span class="SCst">ListNode</span>, <span class="SCst">T</span>)</span></td>
<td></td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_opDrop">opDrop</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_List_opVisit">opVisit</a></span></td>
<td>Visit every elements of the list. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_List_addBack"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">List.</span><span class="api-item-title-strong">addBack</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\list.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new element on front. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addBack</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>Add a new element on front. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addBack</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_List_addFront"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">List.</span><span class="api-item-title-strong">addFront</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\list.swg#L79" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a new element on front. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addFront</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>Add a new element on front. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addFront</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_List_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">List.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\list.swg#L63" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Clear all elements. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_List_insertAfter"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">List.</span><span class="api-item-title-strong">insertAfter</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\list.swg#L202" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert a node before the reference. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertAfter</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, node: *<span class="SCst"><a href="#Core_ListNode">ListNode</a></span>, value: <span class="SCst">T</span>)</span></div>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertAfter</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, node: *<span class="SCst"><a href="#Core_ListNode">ListNode</a></span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_List_insertBefore"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">List.</span><span class="api-item-title-strong">insertBefore</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\list.swg#L163" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert a node before the reference. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertBefore</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, node: *<span class="SCst"><a href="#Core_ListNode">ListNode</a></span>, value: <span class="SCst">T</span>)</span></div>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertBefore</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, node: *<span class="SCst"><a href="#Core_ListNode">ListNode</a></span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_List_opDrop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">List.</span><span class="api-item-title-strong">opDrop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\list.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opDrop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_List_opVisit"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">List.</span><span class="api-item-title-strong">opVisit</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\list.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Visit every elements of the list. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, stmt: <span class="STpe">code</span>)</span></div>
<ul>
<li><span class="code-inline">@alias0</span> is the value</li>
<li><span class="code-inline">@alias1</span> is the node</li>
<li><span class="code-inline">@alias2</span> is the index</li>
</ul>
<p> Visiting by pointer and in reverse order is supported </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_ListNode"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">ListNode</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\list.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>prev</td>
<td class="code-type"><span class="SCde">*<span class="SCst"><a href="#Core_ListNode">ListNode</a></span></span></td>
<td></td>
</tr>
<tr>
<td>next</td>
<td class="code-type"><span class="SCde">*<span class="SCst"><a href="#Core_ListNode">ListNode</a></span></span></td>
<td></td>
</tr>
<tr>
<td>value</td>
<td class="code-type"><span class="SCst">T</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_NumericArray"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Math.</span><span class="api-item-title-strong">NumericArray</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\numericarray.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>buf</td>
<td class="code-type"><span class="SCde">[<span class="SCst">N</span>] <span class="SCst">N</span></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_from">from</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_mulAdd">mulAdd</a></span><span class="SCde">(*<span class="SCst">NumericArray</span>, <span class="SCst">T</span>, <span class="SCst">T</span>)</span></td>
<td>Multiply & add. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_mulAdd">mulAdd</a></span><span class="SCde">(*<span class="SCst">NumericArray</span>, <span class="SCst">NumericArray</span>, <span class="SCst">NumericArray</span>)</span></td>
<td>Multiply & Add. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_set">set</a></span></td>
<td>Set all values. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_opAffect">opAffect</a></span><span class="SCde">(*<span class="SCst">NumericArray</span>, <span class="SCst">T</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_opAffect">opAffect</a></span><span class="SCde">(*<span class="SCst">NumericArray</span>, <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_opAssign">opAssign</a></span><span class="SCde">(*<span class="SCst">NumericArray</span>, <span class="SCst">T</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_opAssign">opAssign</a></span><span class="SCde">(*<span class="SCst">NumericArray</span>, <span class="SCst">NumericArray</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_opBinary">opBinary</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_opEquals">opEquals</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_NumericArray_opIndex">opIndex</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_NumericArray_from"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NumericArray.</span><span class="api-item-title-strong">from</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\numericarray.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">from</span>(values: ...)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_NumericArray_mulAdd"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NumericArray.</span><span class="api-item-title-strong">mulAdd</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\numericarray.swg#L118" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Multiply & add. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">mulAdd</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value1: <span class="SCst">T</span>, value2: <span class="SCst">T</span>)</span></div>
<p>Multiply & Add. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">mulAdd</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value1: <span class="SCst"><a href="#Core_Math_NumericArray">NumericArray</a></span>, value2: <span class="SCst"><a href="#Core_Math_NumericArray">NumericArray</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_NumericArray_opAffect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NumericArray.</span><span class="api-item-title-strong">opAffect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\numericarray.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, what: <span class="SCst">T</span>)
<span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, what: <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_NumericArray_opAssign"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NumericArray.</span><span class="api-item-title-strong">opAssign</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\numericarray.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opAssign</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)
<span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opAssign</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst"><a href="#Core_Math_NumericArray">NumericArray</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_NumericArray_opBinary"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NumericArray.</span><span class="api-item-title-strong">opBinary</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\numericarray.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opBinary</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, other: <span class="SCst"><a href="#Core_Math_NumericArray">NumericArray</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_NumericArray_opEquals"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NumericArray.</span><span class="api-item-title-strong">opEquals</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\numericarray.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opEquals</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, other: <span class="SCst"><a href="#Core_Math_NumericArray">NumericArray</a></span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_NumericArray_opIndex"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NumericArray.</span><span class="api-item-title-strong">opIndex</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\numericarray.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opIndex</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, idx: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_NumericArray_set"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">NumericArray.</span><span class="api-item-title-strong">set</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\numericarray.swg#L109" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set all values. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">set</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, values: ...)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Math.</span><span class="api-item-title-strong">Variant</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>type</td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="SCst">TypeInfo</span></span></td>
<td></td>
</tr>
<tr>
<td>buffer</td>
<td class="code-type"><span class="SCde">[<span class="SCst">Reflection</span>.<span class="SFct">maxSizeOf</span>(<span class="SCst">T</span>)] <span class="SCst">Reflection</span>.<span class="SFct">maxSizeOf</span>(<span class="SCst">T</span>)</span></td>
<td></td>
</tr>
<tr>
<td>AllFlags</td>
<td class="code-type"></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_checkValidType">checkValidType</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_drop">drop</a></span></td>
<td>Drop the variant content (if necessary). </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_get">get</a></span></td>
<td>Get the variant value of the given type  Will assert if the current type does not match. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_set">set</a></span></td>
<td>Set the variant value. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_opAffect">opAffect</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_opCmp">opCmp</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_opDrop">opDrop</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_opEquals">opEquals</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_opPostCopy">opPostCopy</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Math_Variant_opPostMove">opPostMove</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_checkValidType"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">checkValidType</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">checkValidType</span>(t: <span class="SKwd">const</span> [..] <span class="SKwd">const</span> *<span class="SCst">TypeInfo</span>, tn: <span class="SKwd">const</span> *<span class="SCst">TypeInfo</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_drop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">drop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Drop the variant content (if necessary). </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">drop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_get"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">get</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L98" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the variant value of the given type  Will assert if the current type does not match. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">TN</span>) <span class="SFct">get</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_opAffect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">opAffect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">TN</span>) <span class="SFct">opAffect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, other: <span class="SCst">TN</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_opCmp"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">opCmp</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L40" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">TN</span>) <span class="SFct">opCmp</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, other: <span class="SCst">TN</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_opDrop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">opDrop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opDrop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_opEquals"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">opEquals</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">TN</span>) <span class="SFct">opEquals</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, other: <span class="SCst">TN</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_opPostCopy"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">opPostCopy</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opPostCopy</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_opPostMove"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">opPostMove</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opPostMove</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Math_Variant_set"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Variant.</span><span class="api-item-title-strong">set</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\math\variant.swg#L86" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the variant value. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">TN</span>) <span class="SFct">set</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">TN</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Alias"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">Alias</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Accept this other name for the field. </p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Core_Serialization_Alias">Alias</a></span>(name: <span class="STpe">string</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">Decoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>serializer</td>
<td class="code-type"><span class="SCst">T</span></td>
<td></td>
</tr>
<tr>
<td>flags</td>
<td class="code-type"><span class="SCst"><a href="#Core_Serialization_DecoderFlags">DecoderFlags</a></span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Decoder_end">end</a></span></td>
<td>Finish reading. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Decoder_readAll">readAll</a></span></td>
<td>Read a full struct. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Decoder_readTypeValue">readTypeValue</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Decoder_readValue">readValue</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Decoder_start">start</a></span></td>
<td>Start reading. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_beginField"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">beginField</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L271" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">beginField</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, val: <span class="SCst">Swag</span>.<span class="SCst">TypeValue</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_beginSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">beginSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L285" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">beginSection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, kind: <span class="SCst"><a href="#Core_Serialization_SectionKind">SectionKind</a></span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_end"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">end</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L266" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">end</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_endField"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">endField</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L279" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endField</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_endSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">endSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L290" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endSection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_getVersion"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">getVersion</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L295" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getVersion</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_read"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">read</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L300" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">read</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, data: *<span class="STpe">void</span>, dataType: <span class="SKwd">const</span> *<span class="SCst">TypeInfo</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readBool"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readBool</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L328" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readBool</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readBufferU8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readBufferU8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L305" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readBufferU8</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, count: <span class="STpe">u64</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readF32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readF32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L337" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readF32</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readF64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readF64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L338" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readF64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readS16"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readS16</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L330" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readS16</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readS32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readS32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L331" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readS32</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readS64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readS64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L332" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readS64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readS8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readS8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L329" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readS8</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readU16"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readU16</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L334" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readU16</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readU32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readU32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L335" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readU32</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readU64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readU64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L336" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readU64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_readU8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readU8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L333" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readU8</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_IDecoder_start"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">start</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L260" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">start</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, input: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_end"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">end</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L236" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Finish reading. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">end</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_readAll"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">readAll</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L242" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Read a full struct. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">TR</span>) <span class="SFct">readAll</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, input: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_readTypeValue"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">readTypeValue</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readTypeValue</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, typeVal: <span class="SCst">TypeValue</span>, data: ^<span class="STpe">void</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_readValue"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">readValue</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L54" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readValue</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, data: ^<span class="STpe">void</span>, dataType: <span class="SKwd">const</span> *<span class="SCst">TypeInfo</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Decoder_start"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Decoder.</span><span class="api-item-title-strong">start</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L229" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Start reading. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">start</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, input: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_DecoderFlags"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">DecoderFlags</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>IgnoreStructFieldError</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">Encoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>serializer</td>
<td class="code-type"><span class="SCst">T</span></td>
<td></td>
</tr>
<tr>
<td>errorIfUnknown</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td>If true, raise an error if a type cannot be saved, and just ignore if false. </td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Encoder_end">end</a></span></td>
<td>End serialization. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Encoder_start">start</a></span></td>
<td>Start serialization. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Encoder_writeAll">writeAll</a></span></td>
<td>Write a full struct. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Encoder_writeTypeValue">writeTypeValue</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Encoder_writeValue">writeValue</a></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_beginField"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">beginField</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L262" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">beginField</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, val: <span class="SCst">Swag</span>.<span class="SCst">TypeValue</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_beginSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">beginSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L274" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">beginSection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, kind: <span class="SCst"><a href="#Core_Serialization_SectionKind">SectionKind</a></span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_end"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">end</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L257" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>End serialization. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">end</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_endField"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">endField</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L268" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endField</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_endSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">endSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L279" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endSection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_start"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">start</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L250" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Start serialization. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">start</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, result: *<span class="SCst">ConcatBuffer</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_write"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">write</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L284" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">write</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, data: <span class="SKwd">const</span> ^<span class="STpe">void</span>, type: <span class="SKwd">const</span> *<span class="SCst">TypeInfo</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeBool"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeBool</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L307" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeBool</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">bool</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeBufferU8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeBufferU8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L289" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeBufferU8</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, data: <span class="SKwd">const</span> ^<span class="STpe">u8</span>, count: <span class="STpe">u64</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeF32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeF32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L316" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeF32</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeF64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeF64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L317" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeF64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">f64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeS16"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeS16</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L309" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeS16</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">s16</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeS32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeS32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L310" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeS32</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">s32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeS64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeS64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L311" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeS64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">s64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeS8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeS8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L308" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeS8</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">s8</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeU16"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeU16</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L313" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeU16</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">u16</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeU32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeU32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L314" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeU32</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">u32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeU64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeU64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L315" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeU64</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_IEncoder_writeU8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeU8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L312" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeU8</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="STpe">u8</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_end"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Encoder.</span><span class="api-item-title-strong">end</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L54" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>End serialization. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">end</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_start"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Encoder.</span><span class="api-item-title-strong">start</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Start serialization. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">start</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, result: *<span class="SCst">ConcatBuffer</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_writeAll"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Encoder.</span><span class="api-item-title-strong">writeAll</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L238" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Write a full struct. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeAll</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, result: *<span class="SCst">ConcatBuffer</span>, value: <span class="STpe">any</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_writeTypeValue"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Encoder.</span><span class="api-item-title-strong">writeTypeValue</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeTypeValue</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, typeVal: <span class="SCst">TypeValue</span>, data: <span class="SKwd">const</span> ^<span class="STpe">void</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Encoder_writeValue"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Encoder.</span><span class="api-item-title-strong">writeValue</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L66" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeValue</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, data: <span class="SKwd">const</span> ^<span class="STpe">void</span>, dataType: <span class="SKwd">const</span> *<span class="SCst">TypeInfo</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Final"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">Final</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The struct does not need versionning. </p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Core_Serialization_Final">Final</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder"><span class="api-item-title-kind">interface</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">IDecoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_beginField"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">beginField</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L10" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">beginField</span>(<span class="SKwd">self</span>, val: <span class="SCst">Swag</span>.<span class="SCst">TypeValue</span>)<span class="STpe">bool</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_beginSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">beginSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">beginSection</span>(<span class="SKwd">self</span>, kind: <span class="SCst"><a href="#Core_Serialization_SectionKind">SectionKind</a></span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_end"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">end</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">end</span>(<span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_endField"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">endField</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L11" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endField</span>(<span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_endSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">endSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endSection</span>(<span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_getVersion"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">getVersion</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getVersion</span>(<span class="SKwd">self</span>)<span class="STpe">u32</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_read"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">read</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L16" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">read</span>(<span class="SKwd">self</span>, data: *<span class="STpe">void</span>, dataType: <span class="SKwd">const</span> *<span class="SCst">TypeInfo</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readBool"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readBool</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readBool</span>(<span class="SKwd">self</span>)<span class="STpe">bool</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readBufferU8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readBufferU8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readBufferU8</span>(<span class="SKwd">self</span>, count: <span class="STpe">u64</span>)*<span class="STpe">u8</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readF32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readF32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readF32</span>(<span class="SKwd">self</span>)<span class="STpe">f32</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readF64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readF64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readF64</span>(<span class="SKwd">self</span>)<span class="STpe">f64</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readS16"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readS16</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readS16</span>(<span class="SKwd">self</span>)<span class="STpe">s16</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readS32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readS32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readS32</span>(<span class="SKwd">self</span>)<span class="STpe">s32</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readS64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readS64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readS64</span>(<span class="SKwd">self</span>)<span class="STpe">s64</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readS8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readS8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readS8</span>(<span class="SKwd">self</span>)<span class="STpe">s8</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readU16"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readU16</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readU16</span>(<span class="SKwd">self</span>)<span class="STpe">u16</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readU32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readU32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readU32</span>(<span class="SKwd">self</span>)<span class="STpe">u32</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readU64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readU64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readU64</span>(<span class="SKwd">self</span>)<span class="STpe">u64</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_readU8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">readU8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readU8</span>(<span class="SKwd">self</span>)<span class="STpe">u8</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IDecoder_start"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IDecoder.</span><span class="api-item-title-strong">start</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\read\decoder.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">start</span>(<span class="SKwd">self</span>, input: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder"><span class="api-item-title-kind">interface</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">IEncoder</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_beginField"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">beginField</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L17" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">beginField</span>(<span class="SKwd">self</span>, val: <span class="SCst">Swag</span>.<span class="SCst">TypeValue</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_beginSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">beginSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">beginSection</span>(<span class="SKwd">self</span>, kind: <span class="SCst"><a href="#Core_Serialization_SectionKind">SectionKind</a></span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_end"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">end</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">end</span>(<span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_endField"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">endField</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endField</span>(<span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_endSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">endSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endSection</span>(<span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_start"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">start</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">start</span>(<span class="SKwd">self</span>, result: *<span class="SCst">ConcatBuffer</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_write"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">write</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">write</span>(<span class="SKwd">self</span>, data: <span class="SKwd">const</span> ^<span class="STpe">void</span>, type: <span class="SKwd">const</span> *<span class="SCst">TypeInfo</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeBool"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeBool</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeBool</span>(<span class="SKwd">self</span>, value: <span class="STpe">bool</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeBufferU8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeBufferU8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeBufferU8</span>(<span class="SKwd">self</span>, data: <span class="SKwd">const</span> ^<span class="STpe">u8</span>, count: <span class="STpe">u64</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeF32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeF32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeF32</span>(<span class="SKwd">self</span>, value: <span class="STpe">f32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeF64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeF64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeF64</span>(<span class="SKwd">self</span>, value: <span class="STpe">f64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeS16"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeS16</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeS16</span>(<span class="SKwd">self</span>, value: <span class="STpe">s16</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeS32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeS32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeS32</span>(<span class="SKwd">self</span>, value: <span class="STpe">s32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeS64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeS64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeS64</span>(<span class="SKwd">self</span>, value: <span class="STpe">s64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeS8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeS8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeS8</span>(<span class="SKwd">self</span>, value: <span class="STpe">s8</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeU16"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeU16</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeU16</span>(<span class="SKwd">self</span>, value: <span class="STpe">u16</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeU32"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeU32</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeU32</span>(<span class="SKwd">self</span>, value: <span class="STpe">u32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeU64"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeU64</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L33" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeU64</span>(<span class="SKwd">self</span>, value: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_IEncoder_writeU8"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">IEncoder.</span><span class="api-item-title-strong">writeU8</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeU8</span>(<span class="SKwd">self</span>, value: <span class="STpe">u8</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_ISerialize"><span class="api-item-title-kind">interface</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">ISerialize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_ISerialize_postSerialize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ISerialize.</span><span class="api-item-title-strong">postSerialize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">postSerialize</span>(<span class="SKwd">self</span>, data: *<span class="STpe">void</span>, decoder: <span class="SCst"><a href="#Core_Serialization_IDecoder">IDecoder</a></span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_ISerialize_read"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ISerialize.</span><span class="api-item-title-strong">read</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">read</span>(<span class="SKwd">self</span>, decoder: <span class="SCst"><a href="#Core_Serialization_IDecoder">IDecoder</a></span>)<span class="STpe">bool</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_ISerialize_readElement"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ISerialize.</span><span class="api-item-title-strong">readElement</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L29" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">readElement</span>(<span class="SKwd">self</span>, type: <span class="SCst">TypeValue</span>, data: *<span class="STpe">void</span>, decoder: <span class="SCst"><a href="#Core_Serialization_IDecoder">IDecoder</a></span>)<span class="STpe">bool</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_ISerialize_write"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ISerialize.</span><span class="api-item-title-strong">write</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L28" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">write</span>(<span class="SKwd">self</span>, decoder: <span class="SCst"><a href="#Core_Serialization_IEncoder">IEncoder</a></span>)<span class="STpe">bool</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_ISerialize_writeElement"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">ISerialize.</span><span class="api-item-title-strong">writeElement</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">writeElement</span>(<span class="SKwd">self</span>, type: <span class="SCst">TypeValue</span>, data: <span class="SKwd">const</span> *<span class="STpe">void</span>, decoder: <span class="SCst"><a href="#Core_Serialization_IEncoder">IEncoder</a></span>)<span class="STpe">bool</span> <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_NoSerialize"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">NoSerialize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Do not serialize a struct or a field. </p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Core_Serialization_NoSerialize">NoSerialize</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_PodFinal"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">PodFinal</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The struct does not need versionning an can be serialized in place. </p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Core_Serialization_PodFinal">PodFinal</a></span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_SectionKind"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">SectionKind</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\write\encoder.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>Raw</td>
<td></td>
</tr>
<tr>
<td>Unordered</td>
<td></td>
</tr>
<tr>
<td>Ordered</td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Serializer"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">Serializer</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>encode</td>
<td class="code-type"><span class="SCst"><a href="#Core_Serialization_IEncoder">IEncoder</a></span></td>
<td></td>
</tr>
<tr>
<td>decode</td>
<td class="code-type"><span class="SCst"><a href="#Core_Serialization_IDecoder">IDecoder</a></span></td>
<td></td>
</tr>
<tr>
<td>isWrite</td>
<td class="code-type"><span class="STpe">bool</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Serializer_beginSection">beginSection</a></span></td>
<td>Start a new section. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Serializer_end">end</a></span></td>
<td>End encoding/decoding. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Serializer_endSection">endSection</a></span></td>
<td>End the previous section. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Serializer_isReading">isReading</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Serializer_isWriting">isWriting</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Serializer_serialize">serialize</a></span><span class="SCde">(*<span class="SCst">Serializer</span>, <span class="STpe">string</span>, <span class="STpe">u32</span>, *<span class="SCst">MT</span>)</span></td>
<td>Serialize one value. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Serializer_serialize">serialize</a></span><span class="SCde">(*<span class="SCst">Serializer</span>, <span class="STpe">string</span>, *<span class="SCst">MT</span>)</span></td>
<td>Serialize one value. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Serializer_startRead">startRead</a></span></td>
<td>Start decoding. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Serialization_Serializer_startWrite">startWrite</a></span></td>
<td>Start encoding. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Serializer_beginSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Serializer.</span><span class="api-item-title-strong">beginSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L74" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Start a new section. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">beginSection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, kind = <span class="SCst"><a href="#Core_Serialization_SectionKind">SectionKind</a></span>.<span class="SCst">Unordered</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Serializer_end"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Serializer.</span><span class="api-item-title-strong">end</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>End encoding/decoding. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">end</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Serializer_endSection"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Serializer.</span><span class="api-item-title-strong">endSection</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>End the previous section. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">endSection</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Serializer_isReading"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Serializer.</span><span class="api-item-title-strong">isReading</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L46" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isReading</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Serializer_isWriting"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Serializer.</span><span class="api-item-title-strong">isWriting</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L44" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isWriting</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Serializer_serialize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Serializer.</span><span class="api-item-title-strong">serialize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Serialize one value. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">MT</span>) <span class="SFct">serialize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, crc32: <span class="STpe">u32</span>, res: *<span class="SCst">MT</span>) <span class="SKwd">throw</span></span></div>
<p>Serialize one value. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">MT</span>) <span class="SFct">serialize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, name: <span class="STpe">string</span>, res: *<span class="SCst">MT</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Serializer_startRead"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Serializer.</span><span class="api-item-title-strong">startRead</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L57" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Start decoding. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">startRead</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, decoder: <span class="SCst"><a href="#Core_Serialization_IDecoder">IDecoder</a></span>, input: <span class="SKwd">const</span> [..] <span class="STpe">u8</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Serializer_startWrite"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Serializer.</span><span class="api-item-title-strong">startWrite</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L49" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Start encoding. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">startWrite</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, encoder: <span class="SCst"><a href="#Core_Serialization_IEncoder">IEncoder</a></span>, output: *<span class="SCst">ConcatBuffer</span>) <span class="SKwd">throw</span></span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Serialization_Version"><span class="api-item-title-kind">attr</span> <span class="api-item-title-light">Serialization.</span><span class="api-item-title-strong">Version</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\serialization\serialize.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the struct version number. </p>
<div class="api-additional-infos"><b>Usage</b>: all 
</div>
<div class="code-block"><span class="SCde"><span class="SKwd">attr</span> <span class="SCst"><a href="#Core_Serialization_Version">Version</a></span>(v: <span class="STpe">u32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice"><span class="api-item-title-kind">namespace</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">Slice</span></span>
</td>
</tr>
</table>
</p>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_contains">contains</a></span></td>
<td>Returns true if the given slice contains the <span class="code-inline">value</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_equals">equals</a></span></td>
<td>Returns true if two slices are equal. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_findLinear">findLinear</a></span></td>
<td>Find <span class="code-inline">value</span> in a slice by performing a linear search. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_findSorted">findSorted</a></span></td>
<td>Find <span class="code-inline">value</span> in a slice by performing a binary search. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_insertionSort">insertionSort</a></span></td>
<td>Insertion sort algorithm (slow). </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_isSorted">isSorted</a></span><span class="SCde">(<span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></td>
<td>Returns true if the slice is sorted. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_isSorted">isSorted</a></span><span class="SCde">(<span class="SKwd">const</span> [..] <span class="SCst">T</span>, <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-><span class="STpe">s32</span>)</span></td>
<td>Returns true if the slice is sorted. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_modifyInPlace">modifyInPlace</a></span></td>
<td>Transform the content of a slice with a given lambda. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_quickSort">quickSort</a></span></td>
<td>Quick sort algorithm. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_reverse">reverse</a></span></td>
<td>Reverse the content of a slice. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_sort">sort</a></span><span class="SCde">([..] <span class="SCst">T</span>)</span></td>
<td>Sort the slice, by picking the right algorithm depending on the type and the  number of elements. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_Slice_sort">sort</a></span><span class="SCde">([..] <span class="SCst">T</span>, <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-><span class="STpe">s32</span>)</span></td>
<td>Sort the slice, by picking the right algorithm depending on the type and the  number of elements. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_contains"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">contains</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\find.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the given slice contains the <span class="code-inline">value</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>, <span class="SCst">V</span>) <span class="SFct">contains</span>(values: <span class="SKwd">const</span> [..] <span class="SCst">T</span>, value: <span class="SCst">V</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_equals"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">equals</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\find.swg#L69" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if two slices are equal. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">equals</span>(dst: <span class="SKwd">const</span> [..] <span class="SCst">T</span>, src: <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_findLinear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">findLinear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\find.swg#L38" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Find <span class="code-inline">value</span> in a slice by performing a linear search. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>, <span class="SCst">V</span>) <span class="SFct">findLinear</span>(values: <span class="SKwd">const</span> [..] <span class="SCst">T</span>, value: <span class="SCst">V</span>)</span></div>
<p> O^n, but slice does not have to be sorted </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_findSorted"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">findSorted</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\find.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Find <span class="code-inline">value</span> in a slice by performing a binary search. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>, <span class="SCst">V</span>) <span class="SFct">findSorted</span>(values: <span class="SKwd">const</span> [..] <span class="SCst">T</span>, value: <span class="SCst">V</span>)</span></div>
<p> The slice must be sorted in ascending order </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_insertionSort"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">insertionSort</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\sort.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insertion sort algorithm (slow). </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">insertionSort</span>(values: [..] <span class="SCst">T</span>, cb: <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-&gt;<span class="STpe">s32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_isSorted"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">isSorted</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\sort.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the slice is sorted. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">isSorted</span>(values: <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></div>
<p>Returns true if the slice is sorted. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">isSorted</span>(values: <span class="SKwd">const</span> [..] <span class="SCst">T</span>, cb: <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-&gt;<span class="STpe">s32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_modifyInPlace"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">modifyInPlace</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\treat.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Transform the content of a slice with a given lambda. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">modifyInPlace</span>(values: [..] <span class="SCst">T</span>, cb: <span class="SKwd">func</span>(*<span class="SCst">T</span>))</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_quickSort"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">quickSort</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\sort.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Quick sort algorithm. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">quickSort</span>(values: [..] <span class="SCst">T</span>, cb: <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-&gt;<span class="STpe">s32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_reverse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">reverse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\treat.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reverse the content of a slice. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">reverse</span>(values: [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_Slice_sort"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Slice.</span><span class="api-item-title-strong">sort</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\slice\sort.swg#L88" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Sort the slice, by picking the right algorithm depending on the type and the  number of elements. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">sort</span>(values: [..] <span class="SCst">T</span>)</span></div>
<p>Sort the slice, by picking the right algorithm depending on the type and the  number of elements. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">sort</span>(values: [..] <span class="SCst">T</span>, cb: <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-&gt;<span class="STpe">s32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">StaticArray</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>buffer</td>
<td class="code-type"><span class="SCde">[<span class="SCst">N</span>] <span class="SCst">N</span></span></td>
<td></td>
</tr>
<tr>
<td>count</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_add">add</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>, <span class="SCst">T</span>)</span></td>
<td>Add a copy of one element at the end of the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_add">add</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>, <span class="SCst">T</span>)</span></td>
<td>Move one element at the end of the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_add">add</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>, <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></td>
<td>Append a slice to the end of this instance. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_addOnce">addOnce</a></span></td>
<td>Add a copy of one element at the end of the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_back">back</a></span></td>
<td>Returns a copy of the last element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_backPtr">backPtr</a></span></td>
<td>Returns the address of the last element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_clear">clear</a></span></td>
<td>Set the number of elements to 0. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_contains">contains</a></span></td>
<td>Returns true if the given <span class="code-inline">value</span> is in the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_emplaceAddress">emplaceAddress</a></span></td>
<td>Reserve room at the end of the array for <span class="code-inline">num</span> elements, but does not  initialize them. Returns the address of the first element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_emplaceAt">emplaceAt</a></span></td>
<td>Move some values at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the values are moved at the end of the array.  Order is preserved. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_emplaceInitAddress">emplaceInitAddress</a></span></td>
<td>Reserve room at the end of the array for <span class="code-inline">num</span> elements. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_free">free</a></span></td>
<td>Free the array content. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_front">front</a></span></td>
<td>Returns a copy of the first element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_frontPtr">frontPtr</a></span></td>
<td>Returns the address of the first element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_insertAt">insertAt</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>, <span class="STpe">u64</span>, <span class="SCst">T</span>)</span></td>
<td>Insert a value at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the value is added at the end of the array.  Order is preserved. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_insertAt">insertAt</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>, <span class="STpe">u64</span>, <span class="SCst">T</span>)</span></td>
<td>Move a value at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the value is added at the end of the array.  Order is preserved. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_insertAt">insertAt</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>, <span class="STpe">u64</span>, <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></td>
<td>Insert some values at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the values are added at the end of the array.  Order is preserved. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_isEmpty">isEmpty</a></span></td>
<td>Returns true if the array is empty. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_popBack">popBack</a></span></td>
<td>Returns a copy of the last element, and remove it from the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_remove">remove</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>, <span class="STpe">u64</span>, <span class="STpe">u64</span>)</span></td>
<td>Remove <span class="code-inline">num</span> elements starting at <span class="code-inline">index</span>. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_remove">remove</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>, <span class="SCst">V</span>)</span></td>
<td>Remove the given <span class="code-inline">value</span>  If not found, does nothing. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_removeAt">removeAt</a></span></td>
<td>Remove an element at <span class="code-inline">index</span> by replacing it with the last element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_removeAtOrdered">removeAtOrdered</a></span></td>
<td>Remove <span class="code-inline">numValues</span> elements at <span class="code-inline">index</span> by shifting all others. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_removeBack">removeBack</a></span></td>
<td>Remove the last element. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_removeOrdered">removeOrdered</a></span></td>
<td>Remove the given <span class="code-inline">value</span>  If not found, does nothing. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_resize">resize</a></span></td>
<td>Change the number of valid elements in the array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_sort">sort</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>)</span></td>
<td>Sort array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_sort">sort</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>, <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-><span class="STpe">s32</span>)</span></td>
<td>Sort array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_sortReverse">sortReverse</a></span></td>
<td>Sort array in reverse order (from biggest to lowest value). </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_toSlice">toSlice</a></span></td>
<td>Returns a slice. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="table-enumeration">
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opAffect">opAffect</a></span></td>
<td>Initializes an Array that contains values copied from the specified array. </td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opCast">opCast</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opCast">opCast</a></span><span class="SCde">(*<span class="SCst">StaticArray</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opCount">opCount</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opData">opData</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opDrop">opDrop</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opIndex">opIndex</a></span><span class="SCde">(<span class="SKwd">const</span> *<span class="SCst">StaticArray</span>, <span class="STpe">u64</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opIndex">opIndex</a></span><span class="SCde">(<span class="SKwd">const</span> *<span class="SCst">StaticArray</span>, <span class="STpe">u64</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opIndexAffect">opIndexAffect</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opIndexAssign">opIndexAssign</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opSlice">opSlice</a></span></td>
<td></td>
</tr>
<tr>
<td class="code-type"><span class="SFct"><a href="#Core_StaticArray_opVisit">opVisit</a></span></td>
<td>Visit every elements of the array. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_add"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">add</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L207" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a copy of one element at the end of the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>Move one element at the end of the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>Append a slice to the end of this instance. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">add</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, values: <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_addOnce"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">addOnce</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L196" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Add a copy of one element at the end of the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">addOnce</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_back"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">back</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L518" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a copy of the last element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">back</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_backPtr"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">backPtr</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L526" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the address of the last element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">backPtr</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_clear"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">clear</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L184" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the number of elements to 0. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">clear</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_contains"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">contains</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L477" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the given <span class="code-inline">value</span> is in the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">V</span>) <span class="SFct">contains</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">V</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_emplaceAddress"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">emplaceAddress</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L225" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reserve room at the end of the array for <span class="code-inline">num</span> elements, but does not  initialize them. Returns the address of the first element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">emplaceAddress</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, num: <span class="STpe">u32</span> = <span class="SNum">1</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_emplaceAt"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">emplaceAt</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L332" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Move some values at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the values are moved at the end of the array.  Order is preserved. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">emplaceAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, values: [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_emplaceInitAddress"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">emplaceInitAddress</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L234" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reserve room at the end of the array for <span class="code-inline">num</span> elements. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">emplaceInitAddress</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, num: <span class="STpe">u32</span> = <span class="SNum">1</span>)</span></div>
<p> Returns the address of the first element </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_free"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">free</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L140" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Free the array content. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">free</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_front"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">front</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L503" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a copy of the first element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">front</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_frontPtr"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">frontPtr</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L511" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the address of the first element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">frontPtr</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_insertAt"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">insertAt</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L246" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Insert a value at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the value is added at the end of the array.  Order is preserved. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, value: <span class="SCst">T</span>)</span></div>
<p>Move a value at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the value is added at the end of the array.  Order is preserved. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, value: <span class="SCst">T</span>)</span></div>
<p>Insert some values at the given index. If <span class="code-inline">index</span> is equal to <span class="code-inline">count</span>, then  the values are added at the end of the array.  Order is preserved. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">insertAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, values: <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_isEmpty"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">isEmpty</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L156" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the array is empty. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isEmpty</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opAffect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opAffect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L82" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Initializes an Array that contains values copied from the specified array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opAffect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, arr: <span class="SKwd">const</span> [..] <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opCast"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opCast</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opCast</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)
<span class="SKwd">func</span> <span class="SFct">opCast</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opCount"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opCount</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opCount</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opData"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opData</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L19" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opData</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opDrop"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opDrop</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opDrop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opIndex"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opIndex</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L51" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opIndex</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>)
<span class="SKwd">func</span> <span class="SFct">opIndex</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opIndexAffect"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opIndexAffect</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L67" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opIndexAffect</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opIndexAssign"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opIndexAssign</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L75" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(op: <span class="STpe">string</span>) <span class="SFct">opIndexAssign</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, value: <span class="SCst">T</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opSlice"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opSlice</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L42" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">opSlice</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, lower: <span class="STpe">u64</span>, upper: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_opVisit"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">opVisit</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L108" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Visit every elements of the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(ptr: <span class="STpe">bool</span>, back: <span class="STpe">bool</span>) <span class="SFct">opVisit</span>(<span class="SKwd">using</span> <span class="SKwd">const</span> <span class="SKwd">self</span>, stmt: <span class="STpe">code</span>)</span></div>
<p> Visiting by pointer and in reverse order is supported </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_popBack"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">popBack</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L533" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a copy of the last element, and remove it from the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">popBack</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_remove"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">remove</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L393" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove <span class="code-inline">num</span> elements starting at <span class="code-inline">index</span>. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">remove</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, num: <span class="STpe">u64</span>)</span></div>
<p>Remove the given <span class="code-inline">value</span>  If not found, does nothing. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">V</span>) <span class="SFct">remove</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">V</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_removeAt"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">removeAt</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L415" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove an element at <span class="code-inline">index</span> by replacing it with the last element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">removeAt</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>)</span></div>
<p> Order is not preserved </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_removeAtOrdered"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">removeAtOrdered</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L427" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove <span class="code-inline">numValues</span> elements at <span class="code-inline">index</span> by shifting all others. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">removeAtOrdered</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, index: <span class="STpe">u64</span>, numValues: <span class="STpe">u64</span> = <span class="SNum">1</span>)</span></div>
<p> Order is preserved </p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_removeBack"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">removeBack</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L543" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove the last element. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">removeBack</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_removeOrdered"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">removeOrdered</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L469" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Remove the given <span class="code-inline">value</span>  If not found, does nothing. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">V</span>) <span class="SFct">removeOrdered</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, value: <span class="SCst">V</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_resize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">resize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L166" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the number of valid elements in the array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">resize</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, newCount: <span class="STpe">u64</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_sort"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">sort</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L484" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Sort array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sort</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>Sort array. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sort</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, cb: <span class="SKwd">closure</span>(<span class="SCst">T</span>, <span class="SCst">T</span>)-&gt;<span class="STpe">s32</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_sortReverse"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">sortReverse</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L490" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Sort array in reverse order (from biggest to lowest value). </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">sortReverse</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_StaticArray_toSlice"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">StaticArray.</span><span class="api-item-title-strong">toSlice</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\collections\staticarray.swg#L160" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a slice. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span> <span class="SFct">toSlice</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="Core_swap"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">Core.</span><span class="api-item-title-strong">swap</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/core\src\algorithms\generic.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Swap two values. </p>
<div class="code-block"><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">swap</span>(x: *<span class="SCst">T</span>, y: *<span class="SCst">T</span>)</span></div>
<div class="swag-watermark">
Generated on 03-11-2023 with <a href="https://swag-lang.org/index.php">swag</a> 0.26.0</div>
</div>
</div>
</div>
</body>
</html>
