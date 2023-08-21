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
            .container {
                height:     100vh;
            }
            .left {
                display:    block;
                overflow-y: scroll;
                width:      600px;
            }
            .leftpage {
                margin:     10px;
            }
            .right {
                overflow-y: scroll;
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
        table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .item td:first-child {
            width: 33%;
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
</head>
<body>
<div class="container">
<div class="left">
<div class="leftpage">
<h1>Swag Runtime</h1>
<h2>Namespaces</h2>
<h3></h3>
<ul>
<li><a href="#Swag_F32">F32</a></li>
<li><a href="#Swag_F64">F64</a></li>
<li><a href="#Swag_S16">S16</a></li>
<li><a href="#Swag_S32">S32</a></li>
<li><a href="#Swag_S64">S64</a></li>
<li><a href="#Swag_S8">S8</a></li>
<li><a href="#Swag">Swag</a></li>
<li><a href="#Swag_U16">U16</a></li>
<li><a href="#Swag_U32">U32</a></li>
<li><a href="#Swag_U64">U64</a></li>
<li><a href="#Swag_U8">U8</a></li>
</ul>
<h2>Structs</h2>
<h3></h3>
<ul>
<li><a href="#Swag_AllocatorRequest">AllocatorRequest</a></li>
<li><a href="#Swag_Attribute">Attribute</a></li>
<li><a href="#Swag_AttributeParam">AttributeParam</a></li>
<li><a href="#Swag_BuildCfg">BuildCfg</a></li>
<li><a href="#Swag_BuildCfgBackendLLVM">BuildCfgBackendLLVM</a></li>
<li><a href="#Swag_BuildCfgBackendX64">BuildCfgBackendX64</a></li>
<li><a href="#Swag_CVaList">CVaList</a></li>
<li><a href="#Swag_CompilerMessage">CompilerMessage</a></li>
<li><a href="#Swag_Context">Context</a></li>
<li><a href="#Swag_DebugAllocator">DebugAllocator</a></li>
<li><a href="#Swag_DebugAllocatorFooter">DebugAllocatorFooter</a></li>
<li><a href="#Swag_DebugAllocatorHeader">DebugAllocatorHeader</a></li>
<li><a href="#Swag_Error">Error</a></li>
<li><a href="#Swag_Gvtd">Gvtd</a></li>
<li><a href="#Swag_Interface">Interface</a></li>
<li><a href="#Swag_ScratchAllocator_LeakHeader">LeakHeader</a></li>
<li><a href="#Swag_Module">Module</a></li>
<li><a href="#Swag_ProcessInfos">ProcessInfos</a></li>
<li><a href="#Swag_ScratchAllocator">ScratchAllocator</a></li>
<li><a href="#Swag_SourceCodeLocation">SourceCodeLocation</a></li>
<li><a href="#Swag_SystemAllocator">SystemAllocator</a></li>
<li><a href="#Swag_TypeInfo">TypeInfo</a></li>
<li><a href="#Swag_TypeInfoAlias">TypeInfoAlias</a></li>
<li><a href="#Swag_TypeInfoArray">TypeInfoArray</a></li>
<li><a href="#Swag_TypeInfoEnum">TypeInfoEnum</a></li>
<li><a href="#Swag_TypeInfoFunc">TypeInfoFunc</a></li>
<li><a href="#Swag_TypeInfoGeneric">TypeInfoGeneric</a></li>
<li><a href="#Swag_TypeInfoNamespace">TypeInfoNamespace</a></li>
<li><a href="#Swag_TypeInfoNative">TypeInfoNative</a></li>
<li><a href="#Swag_TypeInfoPointer">TypeInfoPointer</a></li>
<li><a href="#Swag_TypeInfoSlice">TypeInfoSlice</a></li>
<li><a href="#Swag_TypeInfoStruct">TypeInfoStruct</a></li>
<li><a href="#Swag_TypeInfoVariadic">TypeInfoVariadic</a></li>
<li><a href="#Swag_TypeValue">TypeValue</a></li>
</ul>
<h2>Interfaces</h2>
<h3></h3>
<ul>
<li><a href="#Swag_IAllocator">IAllocator</a></li>
<li><a href="#Swag_ICompiler">ICompiler</a></li>
</ul>
<h2>Enums</h2>
<h3></h3>
<ul>
<li><a href="#Swag_AllocatorMode">AllocatorMode</a></li>
<li><a href="#Swag_AttributeUsage">AttributeUsage</a></li>
<li><a href="#Swag_Backend">Backend</a></li>
<li><a href="#Swag_BuildCfgBackendKind">BuildCfgBackendKind</a></li>
<li><a href="#Swag_BuildCfgBackendOptim">BuildCfgBackendOptim</a></li>
<li><a href="#Swag_BuildCfgBackendSubKind">BuildCfgBackendSubKind</a></li>
<li><a href="#Swag_CompilerMsgKind">CompilerMsgKind</a></li>
<li><a href="#Swag_CompilerMsgMask">CompilerMsgMask</a></li>
<li><a href="#Swag_ContextFlags">ContextFlags</a></li>
<li><a href="#Swag_DocKind">DocKind</a></li>
<li><a href="#Swag_ExceptionKind">ExceptionKind</a></li>
<li><a href="#Swag_RuntimeFlags">RuntimeFlags</a></li>
<li><a href="#Swag_TargetArch">TargetArch</a></li>
<li><a href="#Swag_TargetOs">TargetOs</a></li>
<li><a href="#Swag_TypeCmpFlags">TypeCmpFlags</a></li>
<li><a href="#Swag_TypeInfoFlags">TypeInfoFlags</a></li>
<li><a href="#Swag_TypeInfoKind">TypeInfoKind</a></li>
<li><a href="#Swag_TypeInfoNativeKind">TypeInfoNativeKind</a></li>
<li><a href="#Swag_WarnLevel">WarnLevel</a></li>
</ul>
<h2>Constants</h2>
<h3></h3>
<ul>
<li><a href="#Swag_F32_Bias">F32.Bias</a></li>
<li><a href="#Swag_F32_ExpBits">F32.ExpBits</a></li>
<li><a href="#Swag_F32_Inf">F32.Inf</a></li>
<li><a href="#Swag_F32_MantBits">F32.MantBits</a></li>
<li><a href="#Swag_F32_Max">F32.Max</a></li>
<li><a href="#Swag_F32_Min">F32.Min</a></li>
<li><a href="#Swag_F32_Nan">F32.Nan</a></li>
<li><a href="#Swag_F32_NegInf">F32.NegInf</a></li>
<li><a href="#Swag_F64_Bias">F64.Bias</a></li>
<li><a href="#Swag_F64_ExpBits">F64.ExpBits</a></li>
<li><a href="#Swag_F64_Inf">F64.Inf</a></li>
<li><a href="#Swag_F64_MantBits">F64.MantBits</a></li>
<li><a href="#Swag_F64_Max">F64.Max</a></li>
<li><a href="#Swag_F64_Min">F64.Min</a></li>
<li><a href="#Swag_F64_Nan">F64.Nan</a></li>
<li><a href="#Swag_F64_NegInf">F64.NegInf</a></li>
<li><a href="#Swag_MaxErrors">MaxErrors</a></li>
<li><a href="#Swag_MaxLenErrorMsg">MaxLenErrorMsg</a></li>
<li><a href="#Swag_MaxTraces">MaxTraces</a></li>
<li><a href="#Swag_S16_Max">S16.Max</a></li>
<li><a href="#Swag_S16_Min">S16.Min</a></li>
<li><a href="#Swag_S32_Max">S32.Max</a></li>
<li><a href="#Swag_S32_Min">S32.Min</a></li>
<li><a href="#Swag_S64_Max">S64.Max</a></li>
<li><a href="#Swag_S64_Min">S64.Min</a></li>
<li><a href="#Swag_S8_Max">S8.Max</a></li>
<li><a href="#Swag_S8_Min">S8.Min</a></li>
<li><a href="#Swag_SafetyAll">SafetyAll</a></li>
<li><a href="#Swag_SafetyAny">SafetyAny</a></li>
<li><a href="#Swag_SafetyBool">SafetyBool</a></li>
<li><a href="#Swag_SafetyBoundCheck">SafetyBoundCheck</a></li>
<li><a href="#Swag_SafetyMath">SafetyMath</a></li>
<li><a href="#Swag_SafetyNaN">SafetyNaN</a></li>
<li><a href="#Swag_SafetyNullCheck">SafetyNullCheck</a></li>
<li><a href="#Swag_SafetyOverflow">SafetyOverflow</a></li>
<li><a href="#Swag_SafetySanity">SafetySanity</a></li>
<li><a href="#Swag_SafetySwitch">SafetySwitch</a></li>
<li><a href="#Swag_SafetyUnreachable">SafetyUnreachable</a></li>
<li><a href="#Swag_U16_Max">U16.Max</a></li>
<li><a href="#Swag_U16_Min">U16.Min</a></li>
<li><a href="#Swag_U32_Max">U32.Max</a></li>
<li><a href="#Swag_U32_Min">U32.Min</a></li>
<li><a href="#Swag_U64_Max">U64.Max</a></li>
<li><a href="#Swag_U64_Min">U64.Min</a></li>
<li><a href="#Swag_U8_Max">U8.Max</a></li>
<li><a href="#Swag_U8_Min">U8.Min</a></li>
</ul>
<h2>Attributes</h2>
<h3></h3>
<ul>
<li><a href="#Swag_Align">Align</a></li>
<li><a href="#Swag_AttrMulti">AttrMulti</a></li>
<li><a href="#Swag_AttrUsage">AttrUsage</a></li>
<li><a href="#Swag_CalleeReturn">CalleeReturn</a></li>
<li><a href="#Swag_Compiler">Compiler</a></li>
<li><a href="#Swag_Complete">Complete</a></li>
<li><a href="#Swag_ConstExpr">ConstExpr</a></li>
<li><a href="#Swag_Deprecated">Deprecated</a></li>
<li><a href="#Swag_Discardable">Discardable</a></li>
<li><a href="#Swag_EnumFlags">EnumFlags</a></li>
<li><a href="#Swag_EnumIndex">EnumIndex</a></li>
<li><a href="#Swag_ExportType">ExportType</a></li>
<li><a href="#Swag_Foreign">Foreign</a></li>
<li><a href="#Swag_Global">Global</a></li>
<li><a href="#Swag_Implicit">Implicit</a></li>
<li><a href="#Swag_Incomplete">Incomplete</a></li>
<li><a href="#Swag_Inline">Inline</a></li>
<li><a href="#Swag_Macro">Macro</a></li>
<li><a href="#Swag_Match">Match</a></li>
<li><a href="#Swag_Mixin">Mixin</a></li>
<li><a href="#Swag_NoCopy">NoCopy</a></li>
<li><a href="#Swag_NoDoc">NoDoc</a></li>
<li><a href="#Swag_NoInline">NoInline</a></li>
<li><a href="#Swag_NotGeneric">NotGeneric</a></li>
<li><a href="#Swag_Offset">Offset</a></li>
<li><a href="#Swag_Opaque">Opaque</a></li>
<li><a href="#Swag_Optim">Optim</a></li>
<li><a href="#Swag_Overflow">Overflow</a></li>
<li><a href="#Swag_Pack">Pack</a></li>
<li><a href="#Swag_PrintBc">PrintBc</a></li>
<li><a href="#Swag_PrintGenBc">PrintGenBc</a></li>
<li><a href="#Swag_Safety">Safety</a></li>
<li><a href="#Swag_Strict">Strict</a></li>
<li><a href="#Swag_Tls">Tls</a></li>
<li><a href="#Swag_Using">Using</a></li>
<li><a href="#Swag_Warn">Warn</a></li>
</ul>
<h2>Functions</h2>
<h3></h3>
<ul>
<li><a href="#@abs">@abs</a></li>
<li><a href="#@acos">@acos</a></li>
<li><a href="#@alloc">@alloc</a></li>
<li><a href="#@args">@args</a></li>
<li><a href="#@asin">@asin</a></li>
<li><a href="#@assert">@assert</a></li>
<li><a href="#@atan">@atan</a></li>
<li><a href="#@atan2">@atan2</a></li>
<li><a href="#@atomadd">@atomadd</a></li>
<li><a href="#@atomand">@atomand</a></li>
<li><a href="#@atomcmpxchg">@atomcmpxchg</a></li>
<li><a href="#@atomor">@atomor</a></li>
<li><a href="#@atomxchg">@atomxchg</a></li>
<li><a href="#@atomxor">@atomxor</a></li>
<li><a href="#@bitcountlz">@bitcountlz</a></li>
<li><a href="#@bitcountnz">@bitcountnz</a></li>
<li><a href="#@bitcounttz">@bitcounttz</a></li>
<li><a href="#@breakpoint">@breakpoint</a></li>
<li><a href="#@byteswap">@byteswap</a></li>
<li><a href="#@ceil">@ceil</a></li>
<li><a href="#@compiler">@compiler</a></li>
<li><a href="#@compilererror">@compilererror</a></li>
<li><a href="#@compilerwarning">@compilerwarning</a></li>
<li><a href="#@cos">@cos</a></li>
<li><a href="#@cosh">@cosh</a></li>
<li><a href="#@dbgalloc">@dbgalloc</a></li>
<li><a href="#@err">@err</a></li>
<li><a href="#@exp">@exp</a></li>
<li><a href="#@exp2">@exp2</a></li>
<li><a href="#@floor">@floor</a></li>
<li><a href="#@free">@free</a></li>
<li><a href="#@getcontext">@getcontext</a></li>
<li><a href="#@getpinfos">@getpinfos</a></li>
<li><a href="#@gvtd">@gvtd</a></li>
<li><a href="#@isbytecode">@isbytecode</a></li>
<li><a href="#@itftableof">@itftableof</a></li>
<li><a href="#@log">@log</a></li>
<li><a href="#@log10">@log10</a></li>
<li><a href="#@log2">@log2</a></li>
<li><a href="#@max">@max</a></li>
<li><a href="#@memcmp">@memcmp</a></li>
<li><a href="#@memcpy">@memcpy</a></li>
<li><a href="#@memmove">@memmove</a></li>
<li><a href="#@memset">@memset</a></li>
<li><a href="#@min">@min</a></li>
<li><a href="#@modules">@modules</a></li>
<li><a href="#@muladd">@muladd</a></li>
<li><a href="#@panic">@panic</a></li>
<li><a href="#@pow">@pow</a></li>
<li><a href="#@print">@print</a></li>
<li><a href="#@realloc">@realloc</a></li>
<li><a href="#@round">@round</a></li>
<li><a href="#@rtflags">@rtflags</a></li>
<li><a href="#@setcontext">@setcontext</a></li>
<li><a href="#@sin">@sin</a></li>
<li><a href="#@sinh">@sinh</a></li>
<li><a href="#@sqrt">@sqrt</a></li>
<li><a href="#@strcmp">@strcmp</a></li>
<li><a href="#@stringcmp">@stringcmp</a></li>
<li><a href="#@strlen">@strlen</a></li>
<li><a href="#@sysalloc">@sysalloc</a></li>
<li><a href="#@tan">@tan</a></li>
<li><a href="#@tanh">@tanh</a></li>
<li><a href="#@trunc">@trunc</a></li>
<li><a href="#@typecmp">@typecmp</a></li>
<li><a href="#Swag_DebugAllocator_IAllocator_alloc">DebugAllocator.IAllocator.alloc</a></li>
<li><a href="#Swag_DebugAllocator_IAllocator_free">DebugAllocator.IAllocator.free</a></li>
<li><a href="#Swag_DebugAllocator_IAllocator_req">DebugAllocator.IAllocator.req</a></li>
<li><a href="#Swag_DebugAllocator_assertIsAllocated">DebugAllocator.assertIsAllocated</a></li>
<li><a href="#Swag_DebugAllocator_checkAllMemory">DebugAllocator.checkAllMemory</a></li>
<li><a href="#Swag_DebugAllocator_checkIsAllocated">DebugAllocator.checkIsAllocated</a></li>
<li><a href="#Swag_DebugAllocator_checkIsFreed">DebugAllocator.checkIsFreed</a></li>
<li><a href="#Swag_DebugAllocator_memAlign">DebugAllocator.memAlign</a></li>
<li><a href="#Swag_DebugAllocator_opDrop">DebugAllocator.opDrop</a></li>
<li><a href="#Swag_DebugAllocator_printDisplaySize">DebugAllocator.printDisplaySize</a></li>
<li><a href="#Swag_DebugAllocator_printLeaks">DebugAllocator.printLeaks</a></li>
<li><a href="#Swag_DebugAllocator_setup">DebugAllocator.setup</a></li>
<li><a href="#Swag_DebugAllocator_IAllocator_releaseLast">IAllocator.releaseLast</a></li>
<li><a href="#Swag_ICompiler_compileString">ICompiler.compileString</a></li>
<li><a href="#Swag_ICompiler_getBuildCfg">ICompiler.getBuildCfg</a></li>
<li><a href="#Swag_ICompiler_getMessage">ICompiler.getMessage</a></li>
<li><a href="#Swag_ScratchAllocator_IAllocator_alloc">ScratchAllocator.IAllocator.alloc</a></li>
<li><a href="#Swag_ScratchAllocator_IAllocator_free">ScratchAllocator.IAllocator.free</a></li>
<li><a href="#Swag_ScratchAllocator_IAllocator_req">ScratchAllocator.IAllocator.req</a></li>
<li><a href="#Swag_ScratchAllocator_align">ScratchAllocator.align</a></li>
<li><a href="#Swag_ScratchAllocator_alloc">ScratchAllocator.alloc</a></li>
<li><a href="#Swag_ScratchAllocator_free">ScratchAllocator.free</a></li>
<li><a href="#Swag_ScratchAllocator_freeAll">ScratchAllocator.freeAll</a></li>
<li><a href="#Swag_ScratchAllocator_release">ScratchAllocator.release</a></li>
<li><a href="#Swag_ScratchAllocator_setCapacity">ScratchAllocator.setCapacity</a></li>
<li><a href="#Swag_IAllocator_req">Swag.IAllocator.req</a></li>
<li><a href="#Swag_SystemAllocator_IAllocator_free">SystemAllocator.IAllocator.free</a></li>
<li><a href="#Swag_SystemAllocator_IAllocator_req">SystemAllocator.IAllocator.req</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Overview</h1>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_F32_Bias"><span class="titletype">const</span> <span class="titlelight">F32.</span><span class="titlestrong">Constants</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L253" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Swag_F32_Bias">Bias</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F32_ExpBits">ExpBits</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F32_Inf">Inf</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F32_MantBits">MantBits</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F32_Max">Max</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F32_Min">Min</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F32_Nan">Nan</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F32_NegInf">NegInf</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F64_Bias">Bias</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F64_ExpBits">ExpBits</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F64_Inf">Inf</td>
<td>f64</td>
<td></td>
</tr>
<tr>
<td id="Swag_F64_MantBits">MantBits</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Swag_F64_Max">Max</td>
<td>f64</td>
<td></td>
</tr>
<tr>
<td id="Swag_F64_Min">Min</td>
<td>f64</td>
<td></td>
</tr>
<tr>
<td id="Swag_F64_Nan">Nan</td>
<td>f64</td>
<td></td>
</tr>
<tr>
<td id="Swag_F64_NegInf">NegInf</td>
<td>f64</td>
<td></td>
</tr>
<tr>
<td id="Swag_MaxErrors">MaxErrors</td>
<td>s32</td>
<td>Maximum nested errors. </td>
</tr>
<tr>
<td id="Swag_MaxLenErrorMsg">MaxLenErrorMsg</td>
<td>s32</td>
<td>Maximum length of an error message. </td>
</tr>
<tr>
<td id="Swag_MaxTraces">MaxTraces</td>
<td>s32</td>
<td>Maximum error traces. </td>
</tr>
<tr>
<td id="Swag_S16_Max">Max</td>
<td>s16</td>
<td></td>
</tr>
<tr>
<td id="Swag_S16_Min">Min</td>
<td>s16</td>
<td></td>
</tr>
<tr>
<td id="Swag_S32_Max">Max</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Swag_S32_Min">Min</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Swag_S64_Max">Max</td>
<td>s64</td>
<td></td>
</tr>
<tr>
<td id="Swag_S64_Min">Min</td>
<td>s64</td>
<td></td>
</tr>
<tr>
<td id="Swag_S8_Max">Max</td>
<td>s8</td>
<td></td>
</tr>
<tr>
<td id="Swag_S8_Min">Min</td>
<td>s8</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetyAll">SafetyAll</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetyAny">SafetyAny</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetyBool">SafetyBool</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetyBoundCheck">SafetyBoundCheck</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetyMath">SafetyMath</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetyNaN">SafetyNaN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetyNullCheck">SafetyNullCheck</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetyOverflow">SafetyOverflow</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetySanity">SafetySanity</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetySwitch">SafetySwitch</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_SafetyUnreachable">SafetyUnreachable</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_U16_Max">Max</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td id="Swag_U16_Min">Min</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td id="Swag_U32_Max">Max</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_U32_Min">Min</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Swag_U64_Max">Max</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td id="Swag_U64_Min">Min</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td id="Swag_U8_Max">Max</td>
<td>u8</td>
<td>Max <code class="incode">u8</code> value. </td>
</tr>
<tr>
<td id="Swag_U8_Min">Min</td>
<td>u8</td>
<td>Min <code class="incode">u8</code> value. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@abs"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@abs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1000" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@abs</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@acos"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@acos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L975" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@acos</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@acos</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@alloc"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@alloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L863" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns a system allocated memory block of <code class="incode">size</code> bytes. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@alloc</span><span class="SyntaxCode">(size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></code>
</pre>
<p> Use <a href="#@free">@free</a> to release the allocated memory. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@args"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@args</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime.swg#L109" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@args</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxType">string</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@asin"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@asin</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L973" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@asin</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@asin</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@assert"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@assert</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L842" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Panic if the expression is false. Typically used in tests. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@assert</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@atan"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@atan</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L977" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atan</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atan</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@atan2"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@atan2</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1009" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atan2</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atan2</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@atomadd"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@atomadd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L894" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Atomic <code class="incode">add</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode"></code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomadd</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@atomand"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@atomand</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L904" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Atomic <code class="incode">and</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode"></code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomand</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@atomcmpxchg"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@atomcmpxchg</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L944" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Atomic 'compare and exchange'. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, compareTo: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, compareTo: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, compareTo: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, compareTo: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode"></code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, compareTo: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, compareTo: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, compareTo: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomcmpxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, compareTo: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@atomor"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@atomor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L914" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Atomic <code class="incode">or</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode"></code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@atomxchg"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@atomxchg</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L934" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Atomic <code class="incode">exchange</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode"></code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxchg</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, exchangeWith: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@atomxor"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@atomxor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L924" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Atomic <code class="incode">xor</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode"></code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@atomxor</span><span class="SyntaxCode">(addr: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@bitcountlz"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@bitcountlz</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1048" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the index+1 of the first set bit, starting from the left (msb). 0 if all zero. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountlz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountlz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountlz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountlz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@bitcountnz"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@bitcountnz</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1036" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Count the number of bits set to 1. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountnz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountnz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountnz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcountnz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@bitcounttz"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@bitcounttz</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1042" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the index+1 of the first set bit, starting from right (lsb). 0 if all zero. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcounttz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcounttz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcounttz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@bitcounttz</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@breakpoint"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@breakpoint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L844" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Stop a bytecode execution, and launch the bytecode debugger. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@breakpoint</span><span class="SyntaxCode">()</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@byteswap"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@byteswap</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1053" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Reverses the order of bytes in an integer. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@byteswap</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode"></code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@byteswap</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@byteswap</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@ceil"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@ceil</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L989" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@ceil</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@ceil</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@compiler"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@compiler</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L858" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns an <a href="#Swag_ICompiler">ICompiler</a> interface to communicate with the compiler. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@compiler</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_ICompiler">ICompiler</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@compilererror"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@compilererror</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime_err.swg#L71" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Raise a compiler error at the given source location. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@compilererror</span><span class="SyntaxCode">(message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, loc: </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_SourceCodeLocation">SourceCodeLocation</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@compilerwarning"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@compilerwarning</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime_err.swg#L83" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Raise a compiler warning at the given source location. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@compilerwarning</span><span class="SyntaxCode">(message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, loc: </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_SourceCodeLocation">SourceCodeLocation</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@cos"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@cos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L961" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@cos</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@cos</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@cosh"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@cosh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L968" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@cosh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@cosh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@dbgalloc"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@dbgalloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime.swg#L130" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@dbgalloc</span><span class="SyntaxCode">()</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@err"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@err</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime_err.swg#L24" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the current raised <a href="#Swag_Error">Error</a> or null if none. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@err</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_Error">Error</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@exp"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@exp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1003" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@exp</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@exp</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@exp2"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@exp2</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1005" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@exp2</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@exp2</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@floor"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@floor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L987" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@floor</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@floor</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@free"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@free</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L869" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Free a system memory block allocated with <a href="#@alloc">@alloc</a>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@free</span><span class="SyntaxCode">(ptr: ^</span><span class="SyntaxType">void</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@getcontext"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@getcontext</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L846" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the current thread context. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@getcontext</span><span class="SyntaxCode">()-&gt;*</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_Context">Context</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@getpinfos"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@getpinfos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L850" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get informations about the current process. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@getpinfos</span><span class="SyntaxCode">()-&gt;*</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_ProcessInfos">ProcessInfos</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@gvtd"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@gvtd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L856" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the list of all global variables. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@gvtd</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_Gvtd">Gvtd</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@isbytecode"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@isbytecode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L852" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns <code class="incode">true</code> is the current execution is bytecode. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@isbytecode</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@itftableof"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@itftableof</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime.swg#L60" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@itftableof</span><span class="SyntaxCode">(structType: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_TypeInfoStruct">TypeInfoStruct</a></span><span class="SyntaxCode">, itfType: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_TypeInfoStruct">TypeInfoStruct</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@log"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@log</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L980" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@log10"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@log10</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L984" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log10</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log10</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@log2"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@log2</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L982" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log2</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@log2</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@max"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@max</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1031" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@max</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@memcmp"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@memcmp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L881" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Compare two memory blocks, and returns -1, 0 if equal, or 1. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@memcmp</span><span class="SyntaxCode">(dst: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode">, src: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@memcpy"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@memcpy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L875" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Copy one memory block to another address. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@memcpy</span><span class="SyntaxCode">(dst: ^</span><span class="SyntaxType">void</span><span class="SyntaxCode">, src: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@memmove"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@memmove</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L878" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Move one memory block to another address. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@memmove</span><span class="SyntaxCode">(dst: ^</span><span class="SyntaxType">void</span><span class="SyntaxCode">, src: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@memset"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@memset</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L872" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set all bytes of a given memory block to <code class="incode">value</code>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@memset</span><span class="SyntaxCode">(dst: ^</span><span class="SyntaxType">void</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@min"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@min</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1020" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">s16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">s64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">s8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s8</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">u16</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@min</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@modules"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@modules</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L854" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the list of all loaded modules. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@modules</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_Module">Module</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@muladd"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@muladd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1058" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns '(val1 * val2) + val3'. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@muladd</span><span class="SyntaxCode">(val1: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, val2: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, val3: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@muladd</span><span class="SyntaxCode">(val1: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">, val2: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">, val3: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@panic"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@panic</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime_err.swg#L96" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Stop the execution and panic. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@panic</span><span class="SyntaxCode">(message: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, loc: </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_SourceCodeLocation">SourceCodeLocation</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@pow"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@pow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L1007" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@pow</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@pow</span><span class="SyntaxCode">(value1: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">, value2: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@print"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@print</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime_str.swg#L165" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Basic print to console function. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@print</span><span class="SyntaxCode">(params: ...)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@realloc"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@realloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L866" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Realloc a system memory block allocated with <a href="#@alloc">@alloc</a>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@realloc</span><span class="SyntaxCode">(ptr: ^</span><span class="SyntaxType">void</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@round"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@round</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L993" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@round</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@round</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@rtflags"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@rtflags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime.swg#L129" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@rtflags</span><span class="SyntaxCode">()</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@setcontext"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@setcontext</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L848" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Replace the current thread context. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@setcontext</span><span class="SyntaxCode">(context: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_Context">Context</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@sin"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@sin</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L959" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sin</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sin</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@sinh"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@sinh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L966" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sinh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sinh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@sqrt"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@sqrt</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L956" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sqrt</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sqrt</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@strcmp"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@strcmp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L887" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Compare two zero terminated strings, and returns -1, 0 if equal, or 1. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@strcmp</span><span class="SyntaxCode">(str1: </span><span class="SyntaxType">cstring</span><span class="SyntaxCode">, str2: </span><span class="SyntaxType">cstring</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@stringcmp"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@stringcmp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime_str.swg#L228" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Compares two strings, and returns <code class="incode">true</code> if they are equal. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@stringcmp</span><span class="SyntaxCode">(str1: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, str2: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@strlen"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@strlen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L884" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the length of the given 0 terminated string. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@strlen</span><span class="SyntaxCode">(str: </span><span class="SyntaxType">cstring</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@sysalloc"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@sysalloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime.swg#L131" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@sysalloc</span><span class="SyntaxCode">()</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@tan"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@tan</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L963" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@tan</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@tan</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@tanh"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@tanh</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L970" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@tanh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@tanh</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@trunc"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@trunc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L991" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@trunc</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode">
</span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@trunc</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">f64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="@typecmp"><span class="titletype">func</span> <span class="titlelight"></span><span class="titlestrong">@typecmp</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxIntrinsic">@typecmp</span><span class="SyntaxCode">(typeA: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_TypeInfo">TypeInfo</a></span><span class="SyntaxCode">, typeB: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_TypeInfo">TypeInfo</a></span><span class="SyntaxCode">, cmpFlags: </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_TypeCmpFlags">TypeCmpFlags</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag"><span class="titletype">namespace</span> <span class="titlelight"></span><span class="titlestrong">Swag</span></span>
</td>
</tr>
</table>
</p>
<h3>Structs</h3>
<table class="enumeration">
<tr>
<td><a href="#Swag_AllocatorRequest">AllocatorRequest</a></td>
<td>Represents the request for a given allocator. </td>
</tr>
<tr>
<td><a href="#Swag_Attribute">Attribute</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_AttributeParam">AttributeParam</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_BuildCfg">BuildCfg</a></td>
<td>The current module build configuration. </td>
</tr>
<tr>
<td><a href="#Swag_BuildCfgBackendLLVM">BuildCfgBackendLLVM</a></td>
<td><code class="incode">LLVM</code> backend specific options. </td>
</tr>
<tr>
<td><a href="#Swag_BuildCfgBackendX64">BuildCfgBackendX64</a></td>
<td><code class="incode">x86_64</code> backend specific options. </td>
</tr>
<tr>
<td><a href="#Swag_CVaList">CVaList</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_CompilerMessage">CompilerMessage</a></td>
<td>A message received in a <code class="incode">#message</code> function. </td>
</tr>
<tr>
<td><a href="#Swag_Context">Context</a></td>
<td>Thread context as returned by [@context]. </td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocator">DebugAllocator</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocatorFooter">DebugAllocatorFooter</a></td>
<td>Will be put just after the returned address. </td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocatorHeader">DebugAllocatorHeader</a></td>
<td>Will be put just before the returned address. </td>
</tr>
<tr>
<td><a href="#Swag_Error">Error</a></td>
<td>Store a <code class="incode">throw</code> error informations. </td>
</tr>
<tr>
<td><a href="#Swag_Gvtd">Gvtd</a></td>
<td>Defined a global variable. </td>
</tr>
<tr>
<td><a href="#Swag_Interface">Interface</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_Module">Module</a></td>
<td>Defined some informations about a loaded module. </td>
</tr>
<tr>
<td><a href="#Swag_ProcessInfos">ProcessInfos</a></td>
<td>Defined some informations about the current process. </td>
</tr>
<tr>
<td><a href="#Swag_ScratchAllocator">ScratchAllocator</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_SourceCodeLocation">SourceCodeLocation</a></td>
<td>Represent a part of a source code file. </td>
</tr>
<tr>
<td><a href="#Swag_SystemAllocator">SystemAllocator</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfo">TypeInfo</a></td>
<td>Will be available for all types. </td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoAlias">TypeInfoAlias</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoArray">TypeInfoArray</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoEnum">TypeInfoEnum</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoFunc">TypeInfoFunc</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoGeneric">TypeInfoGeneric</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoNamespace">TypeInfoNamespace</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoNative">TypeInfoNative</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoPointer">TypeInfoPointer</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoSlice">TypeInfoSlice</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoStruct">TypeInfoStruct</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoVariadic">TypeInfoVariadic</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeValue">TypeValue</a></td>
<td>Represents a value, like a function parameter or an enum value. </td>
</tr>
</table>
<h3>Enums</h3>
<table class="enumeration">
<tr>
<td><a href="#Swag_AllocatorMode">AllocatorMode</a></td>
<td>The <code class="incode">mode</code> of an <a href="#Swag_AllocatorRequest">AllocatorRequest</a>. </td>
</tr>
<tr>
<td><a href="#Swag_AttributeUsage">AttributeUsage</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_Backend">Backend</a></td>
<td>The type of backend to use. </td>
</tr>
<tr>
<td><a href="#Swag_BuildCfgBackendKind">BuildCfgBackendKind</a></td>
<td>The kind of native backend to generate. </td>
</tr>
<tr>
<td><a href="#Swag_BuildCfgBackendOptim">BuildCfgBackendOptim</a></td>
<td>These are the optimization levels for the backend. </td>
</tr>
<tr>
<td><a href="#Swag_BuildCfgBackendSubKind">BuildCfgBackendSubKind</a></td>
<td>The native backend sub category. </td>
</tr>
<tr>
<td><a href="#Swag_CompilerMsgKind">CompilerMsgKind</a></td>
<td>The kind of message in <a href="#Swag_CompilerMessage">CompilerMessage</a>. </td>
</tr>
<tr>
<td><a href="#Swag_CompilerMsgMask">CompilerMsgMask</a></td>
<td>The type of message you want to retreive in a <code class="incode">#message</code> function. </td>
</tr>
<tr>
<td><a href="#Swag_ContextFlags">ContextFlags</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_DocKind">DocKind</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_ExceptionKind">ExceptionKind</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_RuntimeFlags">RuntimeFlags</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TargetArch">TargetArch</a></td>
<td>Target processor. </td>
</tr>
<tr>
<td><a href="#Swag_TargetOs">TargetOs</a></td>
<td>The <code class="incode">OS</code> to target when generating native code. </td>
</tr>
<tr>
<td><a href="#Swag_TypeCmpFlags">TypeCmpFlags</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoFlags">TypeInfoFlags</a></td>
<td>Flags in each <a href="#Swag_TypeInfo">TypeInfo</a>. </td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoKind">TypeInfoKind</a></td>
<td>The kind of the typeinfo. </td>
</tr>
<tr>
<td><a href="#Swag_TypeInfoNativeKind">TypeInfoNativeKind</a></td>
<td>The native type if the type is... native. </td>
</tr>
<tr>
<td><a href="#Swag_WarnLevel">WarnLevel</a></td>
<td>Warning behavior for <a href="#Swag_Warn">Warn</a> attribute. </td>
</tr>
</table>
<h3>Attributes</h3>
<table class="enumeration">
<tr>
<td><a href="#Swag_Align">Align</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_AttrMulti">AttrMulti</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_AttrUsage">AttrUsage</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_CalleeReturn">CalleeReturn</a></td>
<td>A <code class="incode">return</code> in the following inlined function must be done in the callee context. </td>
</tr>
<tr>
<td><a href="#Swag_Compiler">Compiler</a></td>
<td>The following function or variable is only defined at compile time. </td>
</tr>
<tr>
<td><a href="#Swag_Complete">Complete</a></td>
<td>The following switch must be complete. </td>
</tr>
<tr>
<td><a href="#Swag_ConstExpr">ConstExpr</a></td>
<td>Can be executed at compile time. </td>
</tr>
<tr>
<td><a href="#Swag_Deprecated">Deprecated</a></td>
<td>The following definition is deprecated and should not be used. </td>
</tr>
<tr>
<td><a href="#Swag_Discardable">Discardable</a></td>
<td>The following function accepts that the called does not use its return value. </td>
</tr>
<tr>
<td><a href="#Swag_EnumFlags">EnumFlags</a></td>
<td>The following enum is a set of flags. </td>
</tr>
<tr>
<td><a href="#Swag_EnumIndex">EnumIndex</a></td>
<td>The following enum can be used to index arrays without casting. </td>
</tr>
<tr>
<td><a href="#Swag_ExportType">ExportType</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_Foreign">Foreign</a></td>
<td>The following function is foreign (imported). </td>
</tr>
<tr>
<td><a href="#Swag_Global">Global</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_Implicit">Implicit</a></td>
<td>Can force an <code class="incode">opCast</code> special function to work as implicit. </td>
</tr>
<tr>
<td><a href="#Swag_Incomplete">Incomplete</a></td>
<td>The following switch is incomplete. </td>
</tr>
<tr>
<td><a href="#Swag_Inline">Inline</a></td>
<td>Force a function to be inlined. </td>
</tr>
<tr>
<td><a href="#Swag_Macro">Macro</a></td>
<td>The following function is a <code class="incode">macro</code>. </td>
</tr>
<tr>
<td><a href="#Swag_Match">Match</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_Mixin">Mixin</a></td>
<td>The following function is a <code class="incode">mixin</code>. </td>
</tr>
<tr>
<td><a href="#Swag_NoCopy">NoCopy</a></td>
<td>The following struct should never be copied. </td>
</tr>
<tr>
<td><a href="#Swag_NoDoc">NoDoc</a></td>
<td>Do not generate documentation. </td>
</tr>
<tr>
<td><a href="#Swag_NoInline">NoInline</a></td>
<td>Never inline the following function. </td>
</tr>
<tr>
<td><a href="#Swag_NotGeneric">NotGeneric</a></td>
<td>The following function is forced to not be generic, even if defined inside a generic <code class="incode">struct</code>. </td>
</tr>
<tr>
<td><a href="#Swag_Offset">Offset</a></td>
<td>Struct field member relocation. </td>
</tr>
<tr>
<td><a href="#Swag_Opaque">Opaque</a></td>
<td>When exporting the following struct, do not export its content. </td>
</tr>
<tr>
<td><a href="#Swag_Optim">Optim</a></td>
<td>Enable/Disable a given function optimization. </td>
</tr>
<tr>
<td><a href="#Swag_Overflow">Overflow</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_Pack">Pack</a></td>
<td><code class="incode">struct</code> packing information. </td>
</tr>
<tr>
<td><a href="#Swag_PrintBc">PrintBc</a></td>
<td>On a function or a struct, this will print the associated generated bytecode (after bytecode optimizations). </td>
</tr>
<tr>
<td><a href="#Swag_PrintGenBc">PrintGenBc</a></td>
<td>On a function or a struct, this will print the associated generated bytecode (right after generation, without bytecode optimizations). </td>
</tr>
<tr>
<td><a href="#Swag_Safety">Safety</a></td>
<td>Enable/Disable one or more safety checks. </td>
</tr>
<tr>
<td><a href="#Swag_Strict">Strict</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_Tls">Tls</a></td>
<td>Put the following global variable in the <code class="incode">tls</code> segment. </td>
</tr>
<tr>
<td><a href="#Swag_Using">Using</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_Warn">Warn</a></td>
<td>Change the behavior of a given warning or list of warnings. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Align"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Align</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L190" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Align">Align</a></span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_AllocatorMode"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">AllocatorMode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L298" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The <code class="incode">mode</code> of an <a href="#Swag_AllocatorRequest">AllocatorRequest</a>. </p>
<table class="enumeration">
<tr>
<td>Alloc</td>
<td>Allocate a block of memory. </td>
</tr>
<tr>
<td>Free</td>
<td>Free a block of memory. </td>
</tr>
<tr>
<td>Realloc</td>
<td>Reallocate a block of memory. </td>
</tr>
<tr>
<td>FreeAll</td>
<td>Free all memory allocated with the underlying allocator (ff possible). </td>
</tr>
<tr>
<td>AssertIsAllocated</td>
<td>The allocator must assert if the address is not currently allocated (if possible). </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_AllocatorRequest"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">AllocatorRequest</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L326" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Represents the request for a given allocator. </p>
<table class="enumeration">
<tr>
<td>callerLoc</td>
<td><a href="#Swag_SourceCodeLocation">Swag.SourceCodeLocation</a></td>
<td>The caller code, to help tracking leaks. Optional. </td>
</tr>
<tr>
<td>hint</td>
<td>string</td>
<td>A hint message, to help tracking leaks. Optional. </td>
</tr>
<tr>
<td>address</td>
<td>*void</td>
<td>The returned address or the address to deal with. </td>
</tr>
<tr>
<td>size</td>
<td>u64</td>
<td>The requested size or the size to deal with. </td>
</tr>
<tr>
<td>oldSize</td>
<td>u64</td>
<td>When reallocating. </td>
</tr>
<tr>
<td>mode</td>
<td><a href="#Swag_AllocatorMode">Swag.AllocatorMode</a></td>
<td>Alloc, free, reallocate... </td>
</tr>
<tr>
<td>alignement</td>
<td>u32</td>
<td>Alignement constraint. </td>
</tr>
</table>
<p> To allocate: </p>
<ul>
<li><code class="incode">mode</code> must be <b>AllocatorMode.Alloc</b></li>
<li><code class="incode">size</code> must be the size in bytes to allocate</li>
<li><code class="incode">alignement</code> must be the alignement constraint in bytes (or 0)</li>
<li><code class="incode">address</code> will be the returned allocated memory address</li>
</ul>
<p> To free: </p>
<ul>
<li><code class="incode">mode</code> must be <b>AllocatorMode.Free</b></li>
<li><code class="incode">size</code> must be the original allocated size in bytes of <code class="incode">address</code></li>
<li><code class="incode">address</code> must be the memory address to release</li>
</ul>
<p> To reallocate: </p>
<ul>
<li><code class="incode">mode</code> must be <b>AllocatorMode.Realloc</b></li>
<li><code class="incode">size</code> must be the original allocated size in bytes of <code class="incode">address</code></li>
<li><code class="incode">address</code> must be the memory address to reallocate</li>
</ul>
<p> See <a href="#Swag_IAllocator">IAllocator</a> </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_AttrMulti"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">AttrMulti</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_AttrMulti">AttrMulti</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_AttrUsage"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">AttrUsage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L26" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_AttrUsage">AttrUsage</a></span><span class="SyntaxCode">(usage: </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AttributeUsage">AttributeUsage</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Attribute"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">Attribute</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L498" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>type</td>
<td>const *Swag.TypeInfo</td>
<td></td>
</tr>
<tr>
<td>params</td>
<td>const [..] Swag.AttributeParam</td>
<td>Attribute parameters. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_AttributeParam"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">AttributeParam</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L492" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>name</td>
<td>string</td>
<td>Name of the attribute parameter. </td>
</tr>
<tr>
<td>value</td>
<td>any</td>
<td>Optional default value. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_AttributeUsage"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">AttributeUsage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Enum</td>
<td>Attribute can be used on an enum. </td>
</tr>
<tr>
<td>EnumValue</td>
<td>Attribute can be used on an enum value. </td>
</tr>
<tr>
<td>StructVariable</td>
<td>Attribute can be used on an struct member. </td>
</tr>
<tr>
<td>GlobalVariable</td>
<td>Attribute can be used on a global variable. </td>
</tr>
<tr>
<td>Variable</td>
<td>Attribute can be used on any variable. </td>
</tr>
<tr>
<td>Struct</td>
<td>Attribute can be used on a struct. </td>
</tr>
<tr>
<td>Function</td>
<td>Attribute can be used on a function. </td>
</tr>
<tr>
<td>FunctionParameter</td>
<td>Attribute can be used on a struct parameter. </td>
</tr>
<tr>
<td>File</td>
<td>Attribute can be used with <code class="incode">#global</code>. </td>
</tr>
<tr>
<td>Constant</td>
<td>Attribute can be used on a constant. </td>
</tr>
<tr>
<td>Multi</td>
<td>Attribute can be used more than once. </td>
</tr>
<tr>
<td>Gen</td>
<td></td>
</tr>
<tr>
<td>All</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Backend"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">Backend</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L636" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The type of backend to use. </p>
<table class="enumeration">
<tr>
<td>ByteCode</td>
<td>Bytecode. </td>
</tr>
<tr>
<td>X64</td>
<td>X86_64 backend. </td>
</tr>
<tr>
<td>LLVM</td>
<td>LLVM backend. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_BuildCfg"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">BuildCfg</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L713" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The current module build configuration. </p>
<table class="enumeration">
<tr>
<td>moduleVersion</td>
<td>u32</td>
<td>The module version. </td>
</tr>
<tr>
<td>moduleRevision</td>
<td>u32</td>
<td>The module revision. </td>
</tr>
<tr>
<td>moduleBuildNum</td>
<td>u32</td>
<td>The module build value. </td>
</tr>
<tr>
<td>moduleNamespace</td>
<td>string</td>
<td>The namespace name of the module. </td>
</tr>
<tr>
<td>embbedImports</td>
<td>bool</td>
<td>Module should embbed all its dependencies. </td>
</tr>
<tr>
<td>scratchAllocatorCapacity</td>
<td>s32</td>
<td>Default capacity of the <code class="incode">temp</code> allocator (in bytes). </td>
</tr>
<tr>
<td>safetyGuards</td>
<td>u16</td>
<td>Activate specific safety checks. </td>
</tr>
<tr>
<td>debugAllocator</td>
<td>bool</td>
<td>Use Swag.DebugAllocator by default. </td>
</tr>
<tr>
<td>debugAllocatorCaptureStack</td>
<td>bool</td>
<td>Capture the call stack for each allocation. </td>
</tr>
<tr>
<td>debugAllocatorLeaks</td>
<td>bool</td>
<td>Detect memory leaks. </td>
</tr>
<tr>
<td>errorStackTrace</td>
<td>bool</td>
<td>Add stack trace in case a <code class="incode">throw</code> is raised. </td>
</tr>
<tr>
<td>warnAsErrors</td>
<td>string</td>
<td>Force some warnings to be treated as errors. Format is "WrnXXXX|WrnXXXX...". </td>
</tr>
<tr>
<td>warnAsWarning</td>
<td>string</td>
<td>Override <code class="incode">warnAsErrors</code>, restoring warnings as warnings. </td>
</tr>
<tr>
<td>warnAsDisabled</td>
<td>string</td>
<td>Disable some specific warnings. </td>
</tr>
<tr>
<td>warnDefaultDisabled</td>
<td>bool</td>
<td>All warnings are disabled, except those specified above. </td>
</tr>
<tr>
<td>warnDefaultErrors</td>
<td>bool</td>
<td>All warnings are treated as errors, except those specified above. </td>
</tr>
<tr>
<td>byteCodeOptimizeLevel</td>
<td>s32</td>
<td>Optimization level of bytecode [0, 1 or 2]. </td>
</tr>
<tr>
<td>byteCodeDebugInline</td>
<td>bool</td>
<td>Trace into inlined code when generating debug infos. </td>
</tr>
<tr>
<td>byteCodeEmitAssume</td>
<td>bool</td>
<td><code class="incode">assume</code> will panic if an error is raised. </td>
</tr>
<tr>
<td>byteCodeInline</td>
<td>bool</td>
<td>Enable bytecode inlining with #<a href="#Swag_Inline">Swag.Inline</a>. </td>
</tr>
<tr>
<td>byteCodeAutoInline</td>
<td>bool</td>
<td>Automatic inlining of some other functions. </td>
</tr>
<tr>
<td>backendKind</td>
<td><a href="#Swag_BuildCfgBackendKind">Swag.BuildCfgBackendKind</a></td>
<td>Backend type (executable, dynamic lib...). </td>
</tr>
<tr>
<td>backendSubKind</td>
<td><a href="#Swag_BuildCfgBackendSubKind">Swag.BuildCfgBackendSubKind</a></td>
<td>Backend sub kind. </td>
</tr>
<tr>
<td>backendDebugInformations</td>
<td>bool</td>
<td>Output debug informations if <code class="incode">true</code>. </td>
</tr>
<tr>
<td>backendOptimize</td>
<td><a href="#Swag_BuildCfgBackendOptim">Swag.BuildCfgBackendOptim</a></td>
<td>Backend optimization level. </td>
</tr>
<tr>
<td>backendNumCU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>linkerArgs</td>
<td>string</td>
<td>Additional linker arguments. </td>
</tr>
<tr>
<td>backendLLVM</td>
<td><a href="#Swag_BuildCfgBackendLLVM">Swag.BuildCfgBackendLLVM</a></td>
<td>Specific LLVM options. </td>
</tr>
<tr>
<td>backendX64</td>
<td><a href="#Swag_BuildCfgBackendX64">Swag.BuildCfgBackendX64</a></td>
<td>Specific X86_64 options. </td>
</tr>
<tr>
<td>repoPath</td>
<td>string</td>
<td>Repository access path to the module. </td>
</tr>
<tr>
<td>docKind</td>
<td><a href="#Swag_DocKind">Swag.DocKind</a></td>
<td></td>
</tr>
<tr>
<td>docOutputName</td>
<td>string</td>
<td>The name of the output file (without extension). </td>
</tr>
<tr>
<td>docOutputExtension</td>
<td>string</td>
<td>The output extension. </td>
</tr>
<tr>
<td>docTitleToc</td>
<td>string</td>
<td>Title for the table of content. </td>
</tr>
<tr>
<td>docTitleContent</td>
<td>string</td>
<td>Title for the main document content. </td>
</tr>
<tr>
<td>docCss</td>
<td>string</td>
<td>The <code class="incode">css</code> file to include in generated documentations. </td>
</tr>
<tr>
<td>docStartHead</td>
<td>string</td>
<td>To insert at the start of the &lt;head&gt; section. </td>
</tr>
<tr>
<td>docEndHead</td>
<td>string</td>
<td>To insert at the end of the &lt;head&gt; section. </td>
</tr>
<tr>
<td>docStartBody</td>
<td>string</td>
<td>To insert add at the start of the &lt;body&gt; section. </td>
</tr>
<tr>
<td>docEndBody</td>
<td>string</td>
<td>To insert add at the end of the &lt;body&gt; section. </td>
</tr>
<tr>
<td>docStyleSection</td>
<td>bool</td>
<td>Generate a default &lt;style&gt; section. </td>
</tr>
<tr>
<td>docSyntaxColorLum</td>
<td>f32</td>
<td>Code syntax color luminosity between 0 and 1. </td>
</tr>
</table>
<p> See <a href="#Swag_ICompiler">ICompiler</a> </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_BuildCfgBackendKind"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">BuildCfgBackendKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L674" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The kind of native backend to generate. </p>
<table class="enumeration">
<tr>
<td>None</td>
<td>Nothing. </td>
</tr>
<tr>
<td>Export</td>
<td>The module is an <code class="incode">export</code> module which does not have its own code. </td>
</tr>
<tr>
<td>Executable</td>
<td>The module needs to be compiled to an executable application. </td>
</tr>
<tr>
<td>DynamicLib</td>
<td>The module needs to be compiled to an dynamic library. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_BuildCfgBackendLLVM"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">BuildCfgBackendLLVM</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L656" class="src">[src]</a></td>
</tr>
</table>
</p>
<p><code class="incode">LLVM</code> backend specific options. </p>
<table class="enumeration">
<tr>
<td>outputIR</td>
<td>bool</td>
<td>Output IR in a file. </td>
</tr>
<tr>
<td>fpMathFma</td>
<td>bool</td>
<td>true in <code class="incode">release</code>. </td>
</tr>
<tr>
<td>fpMathNoNaN</td>
<td>bool</td>
<td>true in <code class="incode">release</code>. </td>
</tr>
<tr>
<td>fpMathNoInf</td>
<td>bool</td>
<td>true in <code class="incode">release</code>. </td>
</tr>
<tr>
<td>fpMathNoSignedZero</td>
<td>bool</td>
<td>true in <code class="incode">release</code>. </td>
</tr>
<tr>
<td>fpMathUnsafe</td>
<td>bool</td>
<td></td>
</tr>
<tr>
<td>fpMathApproxFunc</td>
<td>bool</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_BuildCfgBackendOptim"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">BuildCfgBackendOptim</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L693" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>These are the optimization levels for the backend. </p>
<table class="enumeration">
<tr>
<td>O0</td>
<td>No optimization. </td>
</tr>
<tr>
<td>O1</td>
<td>Optimizations level 1. </td>
</tr>
<tr>
<td>O2</td>
<td>Optimizations level 2. </td>
</tr>
<tr>
<td>O3</td>
<td>Optimizations level 3. </td>
</tr>
<tr>
<td>Os</td>
<td>Optim for size level 1. </td>
</tr>
<tr>
<td>Oz</td>
<td>Optim for size level 2. </td>
</tr>
</table>
<p> Only <code class="incode">LLVM</code> backend uses this, as the <code class="incode">X86_64</code> backend does not have an optimization pass. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_BuildCfgBackendSubKind"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">BuildCfgBackendSubKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L685" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The native backend sub category. </p>
<table class="enumeration">
<tr>
<td>Default</td>
<td></td>
</tr>
<tr>
<td>Console</td>
<td></td>
</tr>
</table>
<p> Under windows, by default, the application will be compiled to make a <code class="incode">windowed</code> application. But you can  change it and force the application to behave like a <code class="incode">console</code> one. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_BuildCfgBackendX64"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">BuildCfgBackendX64</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L669" class="src">[src]</a></td>
</tr>
</table>
</p>
<p><code class="incode">x86_64</code> backend specific options. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_CVaList"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">CVaList</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L835" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>buf</td>
<td>[2048] u8</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_CalleeReturn"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">CalleeReturn</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L72" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>A <code class="incode">return</code> in the following inlined function must be done in the callee context. </p>
<div class="addinfos"><b>Usage</b>: function 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_CalleeReturn">CalleeReturn</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Compiler"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Compiler</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following function or variable is only defined at compile time. </p>
<div class="addinfos"><b>Usage</b>: function global-var const 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Compiler">Compiler</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_CompilerMessage"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">CompilerMessage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L805" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>A message received in a <code class="incode">#message</code> function. </p>
<table class="enumeration">
<tr>
<td>moduleName</td>
<td>string</td>
<td>Name of the module for which the message is sent. </td>
</tr>
<tr>
<td>name</td>
<td>string</td>
<td>Depends on <code class="incode">kind</code>. </td>
</tr>
<tr>
<td>type</td>
<td>const *Swag.TypeInfo</td>
<td>Depends on <code class="incode">kind</code>. </td>
</tr>
<tr>
<td>kind</td>
<td><a href="#Swag_CompilerMsgKind">Swag.CompilerMsgKind</a></td>
<td>Kind of the message. </td>
</tr>
</table>
<p> In a <code class="incode">#message</code> function, you can retreive the associated message by calling <code class="incode">getMessage</code> of the  interface returned by <a href="#@compiler">@compiler</a> </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_CompilerMsgKind"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">CompilerMsgKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L772" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The kind of message in <a href="#Swag_CompilerMessage">CompilerMessage</a>. </p>
<table class="enumeration">
<tr>
<td>PassAfterSemantic</td>
<td>Called once the program semantic has been done. </td>
</tr>
<tr>
<td>PassBeforeRunByteCode</td>
<td>Called just before running bytecode. </td>
</tr>
<tr>
<td>PassBeforeOutput</td>
<td>Called just before generating the native code. </td>
</tr>
<tr>
<td>PassAllDone</td>
<td>Called when everything has be done. </td>
</tr>
<tr>
<td>SemFunctions</td>
<td>Called for each function in the module. </td>
</tr>
<tr>
<td>SemTypes</td>
<td>Called for each type in the module. </td>
</tr>
<tr>
<td>SemGlobals</td>
<td>Called for each global variable in the module. </td>
</tr>
<tr>
<td>AttributeGen</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_CompilerMsgMask"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">CompilerMsgMask</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L788" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The type of message you want to retreive in a <code class="incode">#message</code> function. </p>
<table class="enumeration">
<tr>
<td>PassAfterSemantic</td>
<td></td>
</tr>
<tr>
<td>PassBeforeRun</td>
<td></td>
</tr>
<tr>
<td>PassBeforeOutput</td>
<td></td>
</tr>
<tr>
<td>PassAllDone</td>
<td></td>
</tr>
<tr>
<td>SemFunctions</td>
<td></td>
</tr>
<tr>
<td>SemTypes</td>
<td></td>
</tr>
<tr>
<td>SemGlobals</td>
<td></td>
</tr>
<tr>
<td>AttributeGen</td>
<td></td>
</tr>
<tr>
<td>All</td>
<td></td>
</tr>
</table>
<p> The function could be called for more than one reason, as this is a mask. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Complete"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Complete</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L68" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following switch must be complete. </p>
<div class="addinfos"><b>Usage</b>: function 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Complete">Complete</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ConstExpr"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">ConstExpr</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Can be executed at compile time. </p>
<div class="addinfos"><b>Usage</b>: function struct 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_ConstExpr">ConstExpr</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Context"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">Context</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L383" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Thread context as returned by [@context]. </p>
<table class="enumeration">
<tr>
<td>allocator</td>
<td><a href="#Swag_IAllocator">Swag.IAllocator</a></td>
<td>The current allocator interface. </td>
</tr>
<tr>
<td>flags</td>
<td><a href="#Swag_ContextFlags">Swag.ContextFlags</a></td>
<td>Context flags. </td>
</tr>
<tr>
<td>tempAllocator</td>
<td><a href="#Swag_ScratchAllocator">Swag.ScratchAllocator</a></td>
<td>Other fields, can be in any order A temporary allocator. </td>
</tr>
<tr>
<td>traces</td>
<td>[32] const *Swag.SourceCodeLocation</td>
<td>Stack trace, in case of errors. </td>
</tr>
<tr>
<td>errors</td>
<td>[32] Swag.Error</td>
<td>All errors. </td>
</tr>
<tr>
<td>exceptionLoc</td>
<td><a href="#Swag_SourceCodeLocation">Swag.SourceCodeLocation</a></td>
<td>When an exception is raised, this is the code location. </td>
</tr>
<tr>
<td>exceptionParams</td>
<td>[4] const *void</td>
<td>When an exception is raised, this are the parameters. </td>
</tr>
<tr>
<td>panic</td>
<td>func(string, Swag.SourceCodeLocation)</td>
<td>A function to call if there's a panic. </td>
</tr>
<tr>
<td>errorIndex</td>
<td>u32</td>
<td>Number of errors in the array. </td>
</tr>
<tr>
<td>traceIndex</td>
<td>u32</td>
<td>The current trace index. </td>
</tr>
<tr>
<td>hasError</td>
<td>u32</td>
<td>!= 0 if an error is raised. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ContextFlags"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">ContextFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L355" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td></td>
</tr>
<tr>
<td>Test</td>
<td></td>
</tr>
<tr>
<td>ByteCode</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">DebugAllocator</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L25" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>allocator</td>
<td><a href="#Swag_IAllocator">Swag.IAllocator</a></td>
<td></td>
</tr>
<tr>
<td>mutex</td>
<td>MutexRW</td>
<td></td>
</tr>
<tr>
<td>firstAlloc</td>
<td>*Swag.DebugAllocatorHeader</td>
<td>First allocated block. </td>
</tr>
<tr>
<td>firstFree</td>
<td>*Swag.DebugAllocatorHeader</td>
<td>First freed block. </td>
</tr>
<tr>
<td>lastFree</td>
<td>*Swag.DebugAllocatorHeader</td>
<td>Last freed block. </td>
</tr>
<tr>
<td>sizeAlloc</td>
<td>u64</td>
<td>Total allocated memory, in bytes. </td>
</tr>
<tr>
<td>sizeFree</td>
<td>u64</td>
<td>Total of free blocks in quarantine. </td>
</tr>
<tr>
<td>countAlloc</td>
<td>u32</td>
<td>Number of allocated blocks. </td>
</tr>
<tr>
<td>nextId</td>
<td>u32</td>
<td>The next allocated id to assign. </td>
</tr>
<tr>
<td>maxFreeSize</td>
<td>u64</td>
<td>Debug/behaviour parameters Maximum total size (in kbs) of free blocks in quarantine. </td>
</tr>
<tr>
<td>breakOnAllocId</td>
<td>u32</td>
<td>Will @assert when the current allocation id reaches that value. </td>
</tr>
<tr>
<td>showMaxLeaks</td>
<td>u32</td>
<td>Maximum number of memory leaks to show. </td>
</tr>
<tr>
<td>captureAllocStack</td>
<td>bool</td>
<td>For each allocation, capture stack. </td>
</tr>
<tr>
<td>detectLeaks</td>
<td>bool</td>
<td>Detect memory leaks if true. </td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Swag_DebugAllocator_assertIsAllocated">assertIsAllocated(self, *void, const ref SourceCodeLocation)</a></td>
<td>This function will assert if the given user address is not conform  to an allocated block. </td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocator_checkAllMemory">checkAllMemory(self)</a></td>
<td>Check all allocated blocks. </td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocator_checkIsAllocated">checkIsAllocated(self, ^DebugAllocatorHeader, const ref SourceCodeLocation)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocator_checkIsFreed">checkIsFreed(self, ^DebugAllocatorHeader)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocator_memAlign">memAlign(u64, u32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocator_printDisplaySize">printDisplaySize(u64)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocator_printLeaks">printLeaks(self)</a></td>
<td>Output to the console the list of all allocated blocks (leaks). </td>
</tr>
<tr>
<td><a href="#Swag_DebugAllocator_setup">setup(self, IAllocator)</a></td>
<td>Setup the allocator. </td>
</tr>
</table>
<h3>Special Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Swag_DebugAllocator_opDrop">opDrop(self)</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_IAllocator_alloc"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">alloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L350" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">alloc</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, request: *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AllocatorRequest">AllocatorRequest</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_IAllocator_free"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">free</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L253" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">free</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, request: *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AllocatorRequest">AllocatorRequest</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_IAllocator_releaseLast"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">releaseLast</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L308" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">releaseLast</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_IAllocator_req"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">req</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L445" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">req</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, request: *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AllocatorRequest">AllocatorRequest</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_assertIsAllocated"><span class="titletype">func</span> <span class="titlelight">DebugAllocator.</span><span class="titlestrong">assertIsAllocated</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L224" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>This function will assert if the given user address is not conform  to an allocated block. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">assertIsAllocated</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, addr: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, callerLoc = #callerlocation)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_checkAllMemory"><span class="titletype">func</span> <span class="titlelight">DebugAllocator.</span><span class="titlestrong">checkAllMemory</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L233" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Check all allocated blocks. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">checkAllMemory</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_checkIsAllocated"><span class="titletype">func</span> <span class="titlelight">DebugAllocator.</span><span class="titlestrong">checkIsAllocated</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L115" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">checkIsAllocated</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, ptrHeader: ^</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_DebugAllocatorHeader">DebugAllocatorHeader</a></span><span class="SyntaxCode">, callerLoc = #callerlocation)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_checkIsFreed"><span class="titletype">func</span> <span class="titlelight">DebugAllocator.</span><span class="titlestrong">checkIsFreed</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L92" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">checkIsFreed</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, ptrHeader: ^</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_DebugAllocatorHeader">DebugAllocatorHeader</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_memAlign"><span class="titletype">func</span> <span class="titlelight">DebugAllocator.</span><span class="titlestrong">memAlign</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L86" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">memAlign</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, alignement: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_opDrop"><span class="titletype">func</span> <span class="titlelight">DebugAllocator.</span><span class="titlestrong">opDrop</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L150" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">opDrop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_printDisplaySize"><span class="titletype">func</span> <span class="titlelight">DebugAllocator.</span><span class="titlestrong">printDisplaySize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">printDisplaySize</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_printLeaks"><span class="titletype">func</span> <span class="titlelight">DebugAllocator.</span><span class="titlestrong">printLeaks</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L165" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Output to the console the list of all allocated blocks (leaks). </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">printLeaks</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocator_setup"><span class="titletype">func</span> <span class="titlelight">DebugAllocator.</span><span class="titlestrong">setup</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Setup the allocator. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setup</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, allocator: </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_IAllocator">IAllocator</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocatorFooter"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">DebugAllocatorFooter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Will be put just after the returned address. </p>
<table class="enumeration">
<tr>
<td>magic</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DebugAllocatorHeader"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">DebugAllocatorHeader</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\debugallocator.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Will be put just before the returned address. </p>
<table class="enumeration">
<tr>
<td>loc</td>
<td><a href="#Swag_SourceCodeLocation">Swag.SourceCodeLocation</a></td>
<td></td>
</tr>
<tr>
<td>allocAddr</td>
<td>^void</td>
<td></td>
</tr>
<tr>
<td>allocSize</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>userSize</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>hint</td>
<td>string</td>
<td></td>
</tr>
<tr>
<td>prev</td>
<td>*Swag.DebugAllocatorHeader</td>
<td></td>
</tr>
<tr>
<td>next</td>
<td>*Swag.DebugAllocatorHeader</td>
<td></td>
</tr>
<tr>
<td>stack</td>
<td>[16] *void</td>
<td></td>
</tr>
<tr>
<td>stackCount</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>magic</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>allocId</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Deprecated"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Deprecated</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following definition is deprecated and should not be used. </p>
<div class="addinfos"><b>Usage</b>: function enum enum-value struct 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Deprecated">Deprecated</a></span><span class="SyntaxCode">(msg: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Discardable"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Discardable</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L80" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following function accepts that the called does not use its return value. </p>
<div class="addinfos"><b>Usage</b>: function var 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Discardable">Discardable</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_DocKind"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">DocKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L703" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>None</td>
<td>Do not generate documentation for that module. </td>
</tr>
<tr>
<td>Api</td>
<td>Generate 'api like' documentation. </td>
</tr>
<tr>
<td>Examples</td>
<td>Generate 'examples like' documentation. </td>
</tr>
<tr>
<td>Pages</td>
<td>Generate one page per file. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_EnumFlags"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">EnumFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L114" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following enum is a set of flags. </p>
<div class="addinfos"><b>Usage</b>: enum 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_EnumFlags">EnumFlags</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_EnumIndex"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">EnumIndex</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L118" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following enum can be used to index arrays without casting. </p>
<div class="addinfos"><b>Usage</b>: enum 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_EnumIndex">EnumIndex</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Error"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">Error</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L374" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Store a <code class="incode">throw</code> error informations. </p>
<table class="enumeration">
<tr>
<td>msgBuf</td>
<td>[128] u8</td>
<td>Buffer to store the error message. </td>
</tr>
<tr>
<td>msg</td>
<td>string</td>
<td>Error message. </td>
</tr>
<tr>
<td>pushHasError</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>pushTraceIndex</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ExceptionKind"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">ExceptionKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L362" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Panic</td>
<td></td>
</tr>
<tr>
<td>Error</td>
<td></td>
</tr>
<tr>
<td>Warning</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ExportType"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">ExportType</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L125" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: struct 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_ExportType">ExportType</a></span><span class="SyntaxCode">(what: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_F32"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">F32</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_F64"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">F64</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Foreign"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Foreign</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L76" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following function is foreign (imported). </p>
<div class="addinfos"><b>Usage</b>: function 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Foreign">Foreign</a></span><span class="SyntaxCode">(module: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, function: </span><span class="SyntaxType">string</span><span class="SyntaxCode"> = </span><span class="SyntaxString">""</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Global"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Global</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L189" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Global">Global</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Gvtd"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">Gvtd</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L287" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Defined a global variable. </p>
<table class="enumeration">
<tr>
<td>ptr</td>
<td>*void</td>
<td>Pointer to the global variable memory. </td>
</tr>
<tr>
<td>opDrop</td>
<td>func(*void)</td>
<td>The <code class="incode">opDrop</code> function to call when the process must exit. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_IAllocator"><span class="titletype">interface</span> <span class="titlelight">Swag.</span><span class="titlestrong">IAllocator</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L337" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>req</td>
<td>func(*Swag.IAllocator, *Swag.AllocatorRequest)</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_IAllocator_req"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">req</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L339" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">req</span><span class="SyntaxCode">(self, request: *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AllocatorRequest">AllocatorRequest</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ICompiler"><span class="titletype">interface</span> <span class="titlelight">Swag.</span><span class="titlestrong">ICompiler</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L816" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>This is the interface to communicate with the compiler. </p>
<table class="enumeration">
<tr>
<td>getMessage</td>
<td>func(*Swag.ICompiler)->const *Swag.CompilerMessage</td>
<td>Returns the compiler message in a <code class="incode">#message</code> function. </td>
</tr>
<tr>
<td>getBuildCfg</td>
<td>func(*Swag.ICompiler)->*Swag.BuildCfg</td>
<td>Returns the build configuration of the current module. </td>
</tr>
<tr>
<td>compileString</td>
<td>func(*Swag.ICompiler, string)</td>
<td>Compile a global string. </td>
</tr>
</table>
<p> The intrinsic <code class="incode">@compiler</code> will return that interface at compile-time, and <code class="incode">null</code> at runtime. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ICompiler_compileString"><span class="titletype">func</span> <span class="titlelight">ICompiler.</span><span class="titlestrong">compileString</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L820" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">compileString</span><span class="SyntaxCode">(self, str: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ICompiler_getBuildCfg"><span class="titletype">func</span> <span class="titlelight">ICompiler.</span><span class="titlestrong">getBuildCfg</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L819" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getBuildCfg</span><span class="SyntaxCode">(self)*</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_BuildCfg">BuildCfg</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ICompiler_getMessage"><span class="titletype">func</span> <span class="titlelight">ICompiler.</span><span class="titlestrong">getMessage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L818" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getMessage</span><span class="SyntaxCode">(self)</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_CompilerMessage">CompilerMessage</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Implicit"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Implicit</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L64" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Can force an <code class="incode">opCast</code> special function to work as implicit. </p>
<div class="addinfos"><b>Usage</b>: function 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Implicit">Implicit</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Incomplete"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Incomplete</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L122" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following switch is incomplete. </p>
<div class="addinfos"><b>Usage</b>: enum 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Incomplete">Incomplete</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Inline"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Inline</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L47" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Force a function to be inlined. </p>
<div class="addinfos"><b>Usage</b>: function 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Inline">Inline</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Interface"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">Interface</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L486" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>obj</td>
<td>*void</td>
<td>Pointer to the associated struct instance. </td>
</tr>
<tr>
<td>itable</td>
<td>const ^^void</td>
<td>Pointer to the virtual table. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Macro"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Macro</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L56" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following function is a <code class="incode">macro</code>. </p>
<div class="addinfos"><b>Usage</b>: function 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Macro">Macro</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Match"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Match</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L186" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Match">Match</a></span><span class="SyntaxCode">(what: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Mixin"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Mixin</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L60" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following function is a <code class="incode">mixin</code>. </p>
<div class="addinfos"><b>Usage</b>: function 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Mixin">Mixin</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Module"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">Module</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L273" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Defined some informations about a loaded module. </p>
<table class="enumeration">
<tr>
<td>name</td>
<td>string</td>
<td>Name of the module. </td>
</tr>
<tr>
<td>types</td>
<td>const [..] const *Swag.TypeInfo</td>
<td>All exported types. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_NoCopy"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">NoCopy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L101" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following struct should never be copied. </p>
<div class="addinfos"><b>Usage</b>: struct 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_NoCopy">NoCopy</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_NoDoc"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">NoDoc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L129" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Do not generate documentation. </p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_NoDoc">NoDoc</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_NoInline"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">NoInline</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L52" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Never inline the following function. </p>
<div class="addinfos"><b>Usage</b>: function 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_NoInline">NoInline</a></span><span class="SyntaxCode"></code>
</pre>
<p> This is a hint for the <code class="incode">llvm</code> backend. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_NotGeneric"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">NotGeneric</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L88" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The following function is forced to not be generic, even if defined inside a generic <code class="incode">struct</code>. </p>
<div class="addinfos"><b>Usage</b>: function 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_NotGeneric">NotGeneric</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Offset"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Offset</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L110" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Struct field member relocation. </p>
<div class="addinfos"><b>Usage</b>: struct-var 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Offset">Offset</a></span><span class="SyntaxCode">(name: </span><span class="SyntaxType">string</span><span class="SyntaxCode">)</code>
</pre>
<p> The field offset in the struct should be the same as the variable <code class="incode">name</code> </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Opaque"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Opaque</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>When exporting the following struct, do not export its content. </p>
<div class="addinfos"><b>Usage</b>: struct 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Opaque">Opaque</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Optim"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Optim</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L160" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enable/Disable a given function optimization. </p>
<div class="addinfos"><b>Usage</b>: function multi 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Optim">Optim</a></span><span class="SyntaxCode">(what: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p> Options are: </p>
<table class="enumeration">
<tr><td> <code class="incode">bytecode</code>   </td><td> Enable/Disable bytecode optimization for the function</td></tr>
<tr><td> <code class="incode">backend</code>    </td><td> Enable/Disable backend machine code optimization for the function (llvm only)</td></tr>
</table>
<p> If <code class="incode">what</code> is null or empty, every options will be affected. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Overflow"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Overflow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L163" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Overflow">Overflow</a></span><span class="SyntaxCode">(value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Pack"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Pack</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L97" class="src">[src]</a></td>
</tr>
</table>
</p>
<p><code class="incode">struct</code> packing information. </p>
<div class="addinfos"><b>Usage</b>: struct 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Pack">Pack</a></span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u8</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_PrintBc"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">PrintBc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L39" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>On a function or a struct, this will print the associated generated bytecode (after bytecode optimizations). </p>
<div class="addinfos"><b>Usage</b>: function struct 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_PrintBc">PrintBc</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_PrintGenBc"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">PrintGenBc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L35" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>On a function or a struct, this will print the associated generated bytecode (right after generation, without bytecode optimizations). </p>
<div class="addinfos"><b>Usage</b>: function struct 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_PrintGenBc">PrintGenBc</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ProcessInfos"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">ProcessInfos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L280" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Defined some informations about the current process. </p>
<table class="enumeration">
<tr>
<td>modules</td>
<td>const [..] Swag.Module</td>
<td>The list of all modules. </td>
</tr>
<tr>
<td>args</td>
<td>string</td>
<td>The application arguments, as returned by <a href="#@args">@args</a>. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_RuntimeFlags"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">RuntimeFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L615" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>FromCompiler</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_S16"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">S16</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_S32"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">S32</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_S64"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">S64</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_S8"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">S8</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Safety"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Safety</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L151" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Enable/Disable one or more safety checks. </p>
<div class="addinfos"><b>Usage</b>: all multi 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Safety">Safety</a></span><span class="SyntaxCode">(what: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, value: </span><span class="SyntaxType">bool</span><span class="SyntaxCode">)</code>
</pre>
<p> For example: </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Safety("", false)]</span><span class="SyntaxCode">                    </span><span class="SyntaxComment">// Disable all</span><span class="SyntaxCode">
 </span><span class="SyntaxAttribute">#[Swag.Safety("boundcheck|nan", false)]</span><span class="SyntaxCode">      </span><span class="SyntaxComment">// Disable 'boundcheck' and 'nan' checks</span><span class="SyntaxCode"></code>
</pre>
<p> Safety checks are: </p>
<table class="enumeration">
<tr><td> <code class="incode">boundcheck</code>   </td><td> Check out of bound access</td></tr>
<tr><td> <code class="incode">overflow</code>     </td><td> Check type conversion lost of bits or precision</td></tr>
<tr><td> <code class="incode">math</code>         </td><td> Various math checks (like a negative <code class="incode">@sqrt</code>)</td></tr>
<tr><td> <code class="incode">switch</code>       </td><td> Check an invalid case in a <code class="incode">#[Swag.Complete]</code> switch</td></tr>
<tr><td> <code class="incode">unreachable</code>  </td><td> Panic if an <code class="incode">@unreachable</code> instruction is executed</td></tr>
<tr><td> <code class="incode">any</code>          </td><td> Panic if a cast from a <code class="incode">any</code> variable does not match the real underlying type</td></tr>
<tr><td> <code class="incode">bool</code>         </td><td> Panic if a <code class="incode">bool</code> does not have a valid value (<code class="incode">true</code> or <code class="incode">false</code>)</td></tr>
<tr><td> <code class="incode">nan</code>          </td><td> Panic if a <code class="incode">nan</code> is used in a float arithmetic operation</td></tr>
<tr><td> <code class="incode">sanity</code>       </td><td> Do a <code class="incode">sanity</code> check (per function)</td></tr>
<tr><td> <code class="incode">null</code>         </td><td> Panic on derefencing some null pointers</td></tr>
</table>
<p> If <code class="incode">what</code> is null or empty, every options are will be affected. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">ScratchAllocator</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L342" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>allocator</td>
<td><a href="#Swag_IAllocator">Swag.IAllocator</a></td>
<td></td>
</tr>
<tr>
<td>block</td>
<td>^u8</td>
<td></td>
</tr>
<tr>
<td>capacity</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>used</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>maxUsed</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>firstLeak</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>totalLeak</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>maxLeak</td>
<td>u64</td>
<td></td>
</tr>
</table>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Swag_ScratchAllocator_align">align(u64, u32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_ScratchAllocator_alloc">alloc(self, u64, u32)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_ScratchAllocator_free">free(self, *void, u64)</a></td>
<td></td>
</tr>
<tr>
<td><a href="#Swag_ScratchAllocator_freeAll">freeAll(self)</a></td>
<td>Release all memory. </td>
</tr>
<tr>
<td><a href="#Swag_ScratchAllocator_release">release(self)</a></td>
<td>Release the allocated block. </td>
</tr>
<tr>
<td><a href="#Swag_ScratchAllocator_setCapacity">setCapacity(self, u64, IAllocator)</a></td>
<td>This will call <code class="incode">release</code> prior to changing the capacity, so this must  be called at an early stage. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_IAllocator_alloc"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">alloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L121" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">alloc</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, request: *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AllocatorRequest">AllocatorRequest</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_IAllocator_free"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">free</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L84" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">free</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, request: *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AllocatorRequest">AllocatorRequest</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_IAllocator_req"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">req</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L161" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">req</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, request: *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AllocatorRequest">AllocatorRequest</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_LeakHeader"><span class="titletype">struct</span> <span class="titlelight">ScratchAllocator.</span><span class="titlestrong">LeakHeader</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>prev</td>
<td>*Swag.ScratchAllocator.LeakHeader</td>
<td></td>
</tr>
<tr>
<td>next</td>
<td>*Swag.ScratchAllocator.LeakHeader</td>
<td></td>
</tr>
<tr>
<td>size</td>
<td>u64</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_align"><span class="titletype">func</span> <span class="titlelight">ScratchAllocator.</span><span class="titlestrong">align</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">align</span><span class="SyntaxCode">(value: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, alignement: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u64</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_alloc"><span class="titletype">func</span> <span class="titlelight">ScratchAllocator.</span><span class="titlestrong">alloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L30" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">alloc</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, alignement: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_free"><span class="titletype">func</span> <span class="titlelight">ScratchAllocator.</span><span class="titlestrong">free</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">free</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, addr: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, size: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_freeAll"><span class="titletype">func</span> <span class="titlelight">ScratchAllocator.</span><span class="titlestrong">freeAll</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L53" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release all memory. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">freeAll</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_release"><span class="titletype">func</span> <span class="titlelight">ScratchAllocator.</span><span class="titlestrong">release</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L41" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Release the allocated block. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">release</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_ScratchAllocator_setCapacity"><span class="titletype">func</span> <span class="titlelight">ScratchAllocator.</span><span class="titlestrong">setCapacity</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\scratchallocator.swg#L67" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>This will call <code class="incode">release</code> prior to changing the capacity, so this must  be called at an early stage. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setCapacity</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, newCapacity: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">, allocator: </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_IAllocator">IAllocator</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_SourceCodeLocation"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">SourceCodeLocation</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L826" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Represent a part of a source code file. </p>
<table class="enumeration">
<tr>
<td>fileName</td>
<td>string</td>
<td>Full path name of the source file. </td>
</tr>
<tr>
<td>lineStart</td>
<td>u32</td>
<td>Start line (starts at 0). </td>
</tr>
<tr>
<td>colStart</td>
<td>u32</td>
<td>Start column. </td>
</tr>
<tr>
<td>lineEnd</td>
<td>u32</td>
<td>End line. </td>
</tr>
<tr>
<td>colEnd</td>
<td>u32</td>
<td>End column. </td>
</tr>
</table>
<p> This is typically what will be returned by <code class="incode">#location</code> or <code class="incode">#callerlocation</code>. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Strict"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Strict</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L188" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Strict">Strict</a></span><span class="SyntaxCode"></code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_SystemAllocator"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">SystemAllocator</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\systemallocator.swg#L3" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_SystemAllocator_IAllocator_free"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">free</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\systemallocator.swg#L9" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">free</span><span class="SyntaxCode">(request: *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AllocatorRequest">AllocatorRequest</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_SystemAllocator_IAllocator_req"><span class="titletype">func</span> <span class="titlelight">IAllocator.</span><span class="titlestrong">req</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\systemallocator.swg#L18" class="src">[src]</a></td>
</tr>
</table>
</p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">req</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, request: *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_AllocatorRequest">AllocatorRequest</a></span><span class="SyntaxCode">)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TargetArch"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">TargetArch</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L622" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Target processor. </p>
<table class="enumeration">
<tr>
<td>X86_64</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TargetOs"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">TargetOs</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L628" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The <code class="incode">OS</code> to target when generating native code. </p>
<table class="enumeration">
<tr>
<td>Windows</td>
<td>Windows 10/11. </td>
</tr>
<tr>
<td>Linux</td>
<td>Linux (<b>unsupported</b>). </td>
</tr>
<tr>
<td>MaxOSX</td>
<td>MacOS (<b>unsupported</b>). </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Tls"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Tls</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L93" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Put the following global variable in the <code class="incode">tls</code> segment. </p>
<div class="addinfos"><b>Usage</b>: global-var 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Tls">Tls</a></span><span class="SyntaxCode"></code>
</pre>
<p> A copy of the variable will be available for each thread. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeCmpFlags"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeCmpFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L409" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>Strict</td>
<td></td>
</tr>
<tr>
<td>CastAny</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfo"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L516" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Will be available for all types. </p>
<table class="enumeration">
<tr>
<td>fullname</td>
<td>string</td>
<td>The full scoped name of the type. </td>
</tr>
<tr>
<td>name</td>
<td>string</td>
<td>The unscoped name. </td>
</tr>
<tr>
<td>sizeof</td>
<td>u32</td>
<td>Size in bytes. </td>
</tr>
<tr>
<td>crc32</td>
<td>u32</td>
<td>A unique CRC to identify the type. </td>
</tr>
<tr>
<td>flags</td>
<td><a href="#Swag_TypeInfoFlags">Swag.TypeInfoFlags</a></td>
<td>Some additional flags. </td>
</tr>
<tr>
<td>kind</td>
<td><a href="#Swag_TypeInfoKind">Swag.TypeInfoKind</a></td>
<td>The kind of the type. </td>
</tr>
<tr>
<td>padding</td>
<td>[3] u8</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoAlias"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoAlias</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L539" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>rawType</td>
<td>const *Swag.TypeInfo</td>
<td>The underlying type. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoArray"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoArray</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L578" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>pointedType</td>
<td>const *Swag.TypeInfo</td>
<td>The underlying type. </td>
</tr>
<tr>
<td>finalType</td>
<td>const *Swag.TypeInfo</td>
<td>If this is an array with multiple dimensions, this will be the final type. </td>
</tr>
<tr>
<td>count</td>
<td>u64</td>
<td>Number of elements in the array. </td>
</tr>
<tr>
<td>totalCount</td>
<td>u64</td>
<td>The total number of elements in case this has multiple dimensions. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoEnum"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoEnum</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L570" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>values</td>
<td>const [..] Swag.TypeValue</td>
<td>All the values. </td>
</tr>
<tr>
<td>rawType</td>
<td>const *Swag.TypeInfo</td>
<td>The underlying enum type. </td>
</tr>
<tr>
<td>attributes</td>
<td>const [..] Swag.Attribute</td>
<td>All the attributes. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoFlags"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L465" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Flags in each <a href="#Swag_TypeInfo">TypeInfo</a>. </p>
<table class="enumeration">
<tr>
<td>Zero</td>
<td></td>
</tr>
<tr>
<td>PointerTypeInfo</td>
<td>This is a pointer to a <a href="#Swag_TypeInfo">TypeInfo</a> struct. </td>
</tr>
<tr>
<td>Integer</td>
<td>This is a native integer. </td>
</tr>
<tr>
<td>Float</td>
<td>This is a native float. </td>
</tr>
<tr>
<td>Unsigned</td>
<td>This is a native unsigned integer. </td>
</tr>
<tr>
<td>HasPostCopy</td>
<td>This is a struct with a <code class="incode">opPostCopy</code>. </td>
</tr>
<tr>
<td>HasPostMove</td>
<td>This is a struct with a <code class="incode">opPostMove</code>. </td>
</tr>
<tr>
<td>HasDrop</td>
<td>This is a struct with a <code class="incode">opDrop</code>. </td>
</tr>
<tr>
<td>Strict</td>
<td>This is a typealias with the Swag.Strict attribute]. </td>
</tr>
<tr>
<td>CanCopy</td>
<td>This is a struct that can be copied. </td>
</tr>
<tr>
<td>Tuple</td>
<td>This is a tuple. </td>
</tr>
<tr>
<td>CString</td>
<td>This is a <code class="incode">cstring</code>. </td>
</tr>
<tr>
<td>Generic</td>
<td>This is a generic type. </td>
</tr>
<tr>
<td>PointerRef</td>
<td>This is a reference. </td>
</tr>
<tr>
<td>PointerMoveRef</td>
<td>This is a move reference. </td>
</tr>
<tr>
<td>PointerArithmetic</td>
<td>This is a pointer to multiple values. </td>
</tr>
<tr>
<td>Character</td>
<td>This is a 32 bits character. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoFunc"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoFunc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L561" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>generics</td>
<td>const [..] Swag.TypeValue</td>
<td>Generic parameters. </td>
</tr>
<tr>
<td>parameters</td>
<td>const [..] Swag.TypeValue</td>
<td>Parameters. </td>
</tr>
<tr>
<td>returnType</td>
<td>const *Swag.TypeInfo</td>
<td>The return type, or <code class="incode">null</code>. </td>
</tr>
<tr>
<td>attributes</td>
<td>const [..] Swag.Attribute</td>
<td>All the attributes. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoGeneric"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoGeneric</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L599" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>rawType</td>
<td>const *Swag.TypeInfo</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoKind"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L416" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The kind of the typeinfo. </p>
<table class="enumeration">
<tr>
<td>Invalid</td>
<td></td>
</tr>
<tr>
<td>Native</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoNative">TypeInfoNative</a>. See <a href="#Swag_TypeInfoNativeKind">TypeInfoNativeKind</a> for the underlying type. </td>
</tr>
<tr>
<td>Namespace</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoNamespace">TypeInfoNamespace</a>. </td>
</tr>
<tr>
<td>Enum</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoEnum">TypeInfoEnum</a>. </td>
</tr>
<tr>
<td>Func</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoFunc">TypeInfoFunc</a>. </td>
</tr>
<tr>
<td>Lambda</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoFunc">TypeInfoFunc</a>, but this is a lambda or a closure. </td>
</tr>
<tr>
<td>Pointer</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoPointer">TypeInfoPointer</a>. </td>
</tr>
<tr>
<td>Array</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoArray">TypeInfoArray</a>. </td>
</tr>
<tr>
<td>Slice</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoSlice">TypeInfoSlice</a>. </td>
</tr>
<tr>
<td>TypeListTuple</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoStruct">TypeInfoStruct</a>. </td>
</tr>
<tr>
<td>TypeListArray</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoArray">TypeInfoArray</a>. </td>
</tr>
<tr>
<td>Variadic</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoVariadic">TypeInfoVariadic</a>. </td>
</tr>
<tr>
<td>TypedVariadic</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoVariadic">TypeInfoVariadic</a>. </td>
</tr>
<tr>
<td>CVariadic</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoVariadic">TypeInfoVariadic</a>. </td>
</tr>
<tr>
<td>Struct</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoStruct">TypeInfoStruct</a>. </td>
</tr>
<tr>
<td>Generic</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoGeneric">TypeInfoGeneric</a>. </td>
</tr>
<tr>
<td>Alias</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoAlias">TypeInfoAlias</a>. </td>
</tr>
<tr>
<td>Code</td>
<td>The typeinfo is a <a href="#Swag_TypeInfo">TypeInfo</a>. </td>
</tr>
<tr>
<td>Interface</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoStruct">TypeInfoStruct</a>, but this is an interface. </td>
</tr>
<tr>
<td>Attribute</td>
<td>The typeinfo is a <a href="#Swag_TypeInfoFunc">TypeInfoFunc</a>, but this is an attribute. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoNamespace"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoNamespace</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L605" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoNative"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoNative</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L527" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>nativeKind</td>
<td><a href="#Swag_TypeInfoNativeKind">Swag.TypeInfoNativeKind</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoNativeKind"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoNativeKind</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L442" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>The native type if the type is... native. </p>
<table class="enumeration">
<tr>
<td>Void</td>
<td></td>
</tr>
<tr>
<td>S8</td>
<td></td>
</tr>
<tr>
<td>S16</td>
<td></td>
</tr>
<tr>
<td>S32</td>
<td></td>
</tr>
<tr>
<td>S64</td>
<td></td>
</tr>
<tr>
<td>U8</td>
<td></td>
</tr>
<tr>
<td>U16</td>
<td></td>
</tr>
<tr>
<td>U32</td>
<td></td>
</tr>
<tr>
<td>U64</td>
<td></td>
</tr>
<tr>
<td>F32</td>
<td></td>
</tr>
<tr>
<td>F64</td>
<td></td>
</tr>
<tr>
<td>Bool</td>
<td></td>
</tr>
<tr>
<td>Rune</td>
<td></td>
</tr>
<tr>
<td>String</td>
<td></td>
</tr>
<tr>
<td>Any</td>
<td></td>
</tr>
<tr>
<td>CString</td>
<td></td>
</tr>
<tr>
<td>Undefined</td>
<td></td>
</tr>
</table>
<p> If the <code class="incode">kind</code> of the type is <code class="incode">TypeInfoKind.Native</code>, then this is the real native type. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoPointer"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoPointer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L533" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>pointedType</td>
<td>const *Swag.TypeInfo</td>
<td>The pointed type. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoSlice"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoSlice</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L587" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>pointedType</td>
<td>const *Swag.TypeInfo</td>
<td>The underlying type. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoStruct"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoStruct</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L545" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>opInit</td>
<td>func(*void)</td>
<td>Pointer to the function to initialize an instance of that struct. </td>
</tr>
<tr>
<td>opDrop</td>
<td>func(*void)</td>
<td>Pointer to the function to drop an instance of that struct. </td>
</tr>
<tr>
<td>opPostCopy</td>
<td>func(*void)</td>
<td>Pointer to the function to call after a copy. </td>
</tr>
<tr>
<td>opPostMove</td>
<td>func(*void)</td>
<td>Pointer to the function to call after a move. </td>
</tr>
<tr>
<td>structName</td>
<td>string</td>
<td>User name of the struct. </td>
</tr>
<tr>
<td>fromGeneric</td>
<td>const *Swag.TypeInfo</td>
<td>If it comes from a generic instantiation, this is the original generic struct. </td>
</tr>
<tr>
<td>generics</td>
<td>const [..] Swag.TypeValue</td>
<td>Generic parameters. </td>
</tr>
<tr>
<td>fields</td>
<td>const [..] Swag.TypeValue</td>
<td>All the fields. </td>
</tr>
<tr>
<td>methods</td>
<td>const [..] Swag.TypeValue</td>
<td>All the methods, if exported. </td>
</tr>
<tr>
<td>interfaces</td>
<td>const [..] Swag.TypeValue</td>
<td>All the interfaces. </td>
</tr>
<tr>
<td>attributes</td>
<td>const [..] Swag.Attribute</td>
<td>All the attributes. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeInfoVariadic"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeInfoVariadic</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L593" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> base</td>
<td><a href="#Swag_TypeInfo">Swag.TypeInfo</a></td>
<td></td>
</tr>
<tr>
<td>rawType</td>
<td>const *Swag.TypeInfo</td>
<td>The underlying type. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_TypeValue"><span class="titletype">struct</span> <span class="titlelight">Swag.</span><span class="titlestrong">TypeValue</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L505" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Represents a value, like a function parameter or an enum value. </p>
<table class="enumeration">
<tr>
<td>name</td>
<td>string</td>
<td>Name of the value. </td>
</tr>
<tr>
<td>pointedType</td>
<td>const *Swag.TypeInfo</td>
<td>Type of the value. </td>
</tr>
<tr>
<td>value</td>
<td>const *void</td>
<td>Pointer to the constant value. </td>
</tr>
<tr>
<td>attributes</td>
<td>const [..] Swag.Attribute</td>
<td>Associated attributes. </td>
</tr>
<tr>
<td>offset</td>
<td>u32</td>
<td>Offset, in bytes. </td>
</tr>
<tr>
<td>crc32</td>
<td>u32</td>
<td>Unique crc32. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_U16"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">U16</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_U32"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">U32</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_U64"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">U64</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_U8"><span class="titletype">namespace</span> <span class="titlelight">Swag.</span><span class="titlestrong">U8</span></span>
</td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Using"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Using</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\runtime.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="addinfos"><b>Usage</b>: all 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Using">Using</a></span><span class="SyntaxCode">(what: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_TypeInfo">TypeInfo</a></span><span class="SyntaxCode">...)</code>
</pre>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_Warn"><span class="titletype">attr</span> <span class="titlelight">Swag.</span><span class="titlestrong">Warn</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L183" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Change the behavior of a given warning or list of warnings. </p>
<div class="addinfos"><b>Usage</b>: all multi 
</div>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">attr</span><span class="SyntaxCode"> </span><span class="SyntaxConstant"><a href="#Swag_Warn">Warn</a></span><span class="SyntaxCode">(what: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, level: </span><span class="SyntaxConstant"><a href="#Swag">Swag</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Swag_WarnLevel">WarnLevel</a></span><span class="SyntaxCode">)</code>
</pre>
<p> For example: </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxAttribute">#[Swag.Warn("wrn0004", Swag.WarnLevel.Error)
 #[Swag.Warn("wrn0003|wrn0004", Swag.WarnLevel.Disable)
 #global #[Swag.Warn("wrn0001", Swag.WarnLevel.Enable)]</span><span class="SyntaxCode"></code>
</pre>
<p> You can also change the warning behaviors for the whole module in your <a href="#Swag_BuildCfg">BuildCfg</a> </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Swag_WarnLevel"><span class="titletype">enum</span> <span class="titlelight">Swag.</span><span class="titlestrong">WarnLevel</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/runtime\bootstrap.swg#L166" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Warning behavior for <a href="#Swag_Warn">Warn</a> attribute. </p>
<table class="enumeration">
<tr>
<td>Enable</td>
<td>Enable the given warning. </td>
</tr>
<tr>
<td>Disable</td>
<td>Disable the given warning. </td>
</tr>
<tr>
<td>Error</td>
<td>Force the given warning to be raised as an error. </td>
</tr>
</table>
</div>
</div>
</div>
</body>
</html>
