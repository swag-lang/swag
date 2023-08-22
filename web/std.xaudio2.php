<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module xaudio2</title>
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
<li><a href="#XAudio2_IXAudio2">IXAudio2</a></li>
<li><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></li>
<li><a href="#XAudio2_IXAudio2EngineCallbackItf">IXAudio2EngineCallbackItf</a></li>
<li><a href="#XAudio2_IXAudio2Itf">IXAudio2Itf</a></li>
<li><a href="#XAudio2_IXAudio2MasteringVoice">IXAudio2MasteringVoice</a></li>
<li><a href="#XAudio2_IXAudio2MasteringVoiceItf">IXAudio2MasteringVoiceItf</a></li>
<li><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></li>
<li><a href="#XAudio2_IXAudio2SourceVoiceItf">IXAudio2SourceVoiceItf</a></li>
<li><a href="#XAudio2_IXAudio2SubmixVoice">IXAudio2SubmixVoice</a></li>
<li><a href="#XAudio2_IXAudio2SubmixVoiceItf">IXAudio2SubmixVoiceItf</a></li>
<li><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></li>
<li><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></li>
<li><a href="#XAudio2_IXAudio2VoiceCallbackItf">IXAudio2VoiceCallbackItf</a></li>
<li><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></li>
<li><a href="#XAudio2_WAVEFORMATEX">WAVEFORMATEX</a></li>
<li><a href="#XAudio2_WAVEFORMATEXTENSIBLE">WAVEFORMATEXTENSIBLE</a></li>
<li><a href="#XAudio2_XAUDIO2_BUFFER">XAUDIO2_BUFFER</a></li>
<li><a href="#XAudio2_XAUDIO2_BUFFER_WMA">XAUDIO2_BUFFER_WMA</a></li>
<li><a href="#XAudio2_XAUDIO2_DEBUG_CONFIGURATION">XAUDIO2_DEBUG_CONFIGURATION</a></li>
<li><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></li>
<li><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></li>
<li><a href="#XAudio2_XAUDIO2_PERFORMANCE_DATA">XAUDIO2_PERFORMANCE_DATA</a></li>
<li><a href="#XAudio2_XAUDIO2_SEND_DESCRIPTOR">XAUDIO2_SEND_DESCRIPTOR</a></li>
<li><a href="#XAudio2_XAUDIO2_VOICE_DETAILS">XAUDIO2_VOICE_DETAILS</a></li>
<li><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></li>
<li><a href="#XAudio2_XAUDIO2_VOICE_STATE">XAUDIO2_VOICE_STATE</a></li>
</ul>
<h3>Enums</h3>
<h4></h4>
<ul>
<li><a href="#XAudio2_AUDIO_STREAM_CATEGORY">AUDIO_STREAM_CATEGORY</a></li>
<li><a href="#XAudio2_XAUDIO2_FILTER_TYPE">XAUDIO2_FILTER_TYPE</a></li>
</ul>
<h3>Constants</h3>
<h4></h4>
<ul>
<li><a href="#XAudio2_FORMAT_EXTENSIBLE">FORMAT_EXTENSIBLE</a></li>
<li><a href="#XAudio2_Processor1">Processor1</a></li>
<li><a href="#XAudio2_Processor10">Processor10</a></li>
<li><a href="#XAudio2_Processor11">Processor11</a></li>
<li><a href="#XAudio2_Processor12">Processor12</a></li>
<li><a href="#XAudio2_Processor13">Processor13</a></li>
<li><a href="#XAudio2_Processor14">Processor14</a></li>
<li><a href="#XAudio2_Processor15">Processor15</a></li>
<li><a href="#XAudio2_Processor16">Processor16</a></li>
<li><a href="#XAudio2_Processor17">Processor17</a></li>
<li><a href="#XAudio2_Processor18">Processor18</a></li>
<li><a href="#XAudio2_Processor19">Processor19</a></li>
<li><a href="#XAudio2_Processor2">Processor2</a></li>
<li><a href="#XAudio2_Processor20">Processor20</a></li>
<li><a href="#XAudio2_Processor21">Processor21</a></li>
<li><a href="#XAudio2_Processor22">Processor22</a></li>
<li><a href="#XAudio2_Processor23">Processor23</a></li>
<li><a href="#XAudio2_Processor24">Processor24</a></li>
<li><a href="#XAudio2_Processor25">Processor25</a></li>
<li><a href="#XAudio2_Processor26">Processor26</a></li>
<li><a href="#XAudio2_Processor27">Processor27</a></li>
<li><a href="#XAudio2_Processor28">Processor28</a></li>
<li><a href="#XAudio2_Processor29">Processor29</a></li>
<li><a href="#XAudio2_Processor3">Processor3</a></li>
<li><a href="#XAudio2_Processor30">Processor30</a></li>
<li><a href="#XAudio2_Processor31">Processor31</a></li>
<li><a href="#XAudio2_Processor32">Processor32</a></li>
<li><a href="#XAudio2_Processor4">Processor4</a></li>
<li><a href="#XAudio2_Processor5">Processor5</a></li>
<li><a href="#XAudio2_Processor6">Processor6</a></li>
<li><a href="#XAudio2_Processor7">Processor7</a></li>
<li><a href="#XAudio2_Processor8">Processor8</a></li>
<li><a href="#XAudio2_Processor9">Processor9</a></li>
<li><a href="#XAudio2_X3DAUDIO_HANDLE_BYTESIZE">X3DAUDIO_HANDLE_BYTESIZE</a></li>
<li><a href="#XAudio2_X3DAUDIO_SPEED_OF_SOUND">X3DAUDIO_SPEED_OF_SOUND</a></li>
<li><a href="#XAudio2_XAUDIO2_1024_QUANTUM">XAUDIO2_1024_QUANTUM</a></li>
<li><a href="#XAudio2_XAUDIO2_ANY_PROCESSOR">XAUDIO2_ANY_PROCESSOR</a></li>
<li><a href="#XAudio2_XAUDIO2_COMMIT_ALL">XAUDIO2_COMMIT_ALL</a></li>
<li><a href="#XAudio2_XAUDIO2_COMMIT_NOW">XAUDIO2_COMMIT_NOW</a></li>
<li><a href="#XAudio2_XAUDIO2_DEBUG_ENGINE">XAUDIO2_DEBUG_ENGINE</a></li>
<li><a href="#XAudio2_XAUDIO2_DEFAULT_CHANNELS">XAUDIO2_DEFAULT_CHANNELS</a></li>
<li><a href="#XAudio2_XAUDIO2_DEFAULT_FREQ_RATIO">XAUDIO2_DEFAULT_FREQ_RATIO</a></li>
<li><a href="#XAudio2_XAUDIO2_DEFAULT_SAMPLERATE">XAUDIO2_DEFAULT_SAMPLERATE</a></li>
<li><a href="#XAudio2_XAUDIO2_END_OF_STREAM">XAUDIO2_END_OF_STREAM</a></li>
<li><a href="#XAudio2_XAUDIO2_INVALID_OPSET">XAUDIO2_INVALID_OPSET</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_API_CALLS">XAUDIO2_LOG_API_CALLS</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_DETAIL">XAUDIO2_LOG_DETAIL</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_ERRORS">XAUDIO2_LOG_ERRORS</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_FUNC_CALLS">XAUDIO2_LOG_FUNC_CALLS</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_INFO">XAUDIO2_LOG_INFO</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_LOCKS">XAUDIO2_LOG_LOCKS</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_MEMORY">XAUDIO2_LOG_MEMORY</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_STREAMING">XAUDIO2_LOG_STREAMING</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_TIMING">XAUDIO2_LOG_TIMING</a></li>
<li><a href="#XAudio2_XAUDIO2_LOG_WARNINGS">XAUDIO2_LOG_WARNINGS</a></li>
<li><a href="#XAudio2_XAUDIO2_LOOP_INFINITE">XAUDIO2_LOOP_INFINITE</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_AUDIO_CHANNELS">XAUDIO2_MAX_AUDIO_CHANNELS</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_BUFFERS_SYSTEM">XAUDIO2_MAX_BUFFERS_SYSTEM</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_BUFFER_BYTES">XAUDIO2_MAX_BUFFER_BYTES</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_FILTER_FREQUENCY">XAUDIO2_MAX_FILTER_FREQUENCY</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_FILTER_ONEOVERQ">XAUDIO2_MAX_FILTER_ONEOVERQ</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_FREQ_RATIO">XAUDIO2_MAX_FREQ_RATIO</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_INSTANCES">XAUDIO2_MAX_INSTANCES</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_LOOP_COUNT">XAUDIO2_MAX_LOOP_COUNT</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_QUEUED_BUFFERS">XAUDIO2_MAX_QUEUED_BUFFERS</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_SAMPLE_RATE">XAUDIO2_MAX_SAMPLE_RATE</a></li>
<li><a href="#XAudio2_XAUDIO2_MAX_VOLUME_LEVEL">XAUDIO2_MAX_VOLUME_LEVEL</a></li>
<li><a href="#XAudio2_XAUDIO2_MIN_FREQ_RATIO">XAUDIO2_MIN_FREQ_RATIO</a></li>
<li><a href="#XAudio2_XAUDIO2_MIN_SAMPLE_RATE">XAUDIO2_MIN_SAMPLE_RATE</a></li>
<li><a href="#XAudio2_XAUDIO2_NO_LOOP_REGION">XAUDIO2_NO_LOOP_REGION</a></li>
<li><a href="#XAudio2_XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT">XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT</a></li>
<li><a href="#XAudio2_XAUDIO2_PLAY_TAILS">XAUDIO2_PLAY_TAILS</a></li>
<li><a href="#XAudio2_XAUDIO2_SEND_USEFILTER">XAUDIO2_SEND_USEFILTER</a></li>
<li><a href="#XAudio2_XAUDIO2_STOP_ENGINE_WHEN_IDLE">XAUDIO2_STOP_ENGINE_WHEN_IDLE</a></li>
<li><a href="#XAudio2_XAUDIO2_VOICE_NOPITCH">XAUDIO2_VOICE_NOPITCH</a></li>
<li><a href="#XAudio2_XAUDIO2_VOICE_NOSAMPLESPLAYED">XAUDIO2_VOICE_NOSAMPLESPLAYED</a></li>
<li><a href="#XAudio2_XAUDIO2_VOICE_NOSRC">XAUDIO2_VOICE_NOSRC</a></li>
<li><a href="#XAudio2_XAUDIO2_VOICE_USEFILTER">XAUDIO2_VOICE_USEFILTER</a></li>
</ul>
<h3>Type Aliases</h3>
<h4></h4>
<ul>
<li><a href="#XAudio2_FLOAT32">FLOAT32</a></li>
<li><a href="#XAudio2_UINT32">UINT32</a></li>
<li><a href="#XAudio2_UINT64">UINT64</a></li>
<li><a href="#XAudio2_X3DAUDIO_HANDLE">X3DAUDIO_HANDLE</a></li>
<li><a href="#XAudio2_XAUDIO2_PROCESSOR">XAUDIO2_PROCESSOR</a></li>
</ul>
<h3>Functions</h3>
<h4></h4>
<ul>
<li><a href="#XAudio2_X3DAudioInitialize">XAudio2.X3DAudioInitialize</a></li>
<li><a href="#XAudio2_XAudio2Create">XAudio2.XAudio2Create</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Module xaudio2</h1>
<p>Wrapper for Microsoft XAudio2 library. </p>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_FORMAT_EXTENSIBLE"><span class="titletype">const</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">Constants</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L308" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="XAudio2_FORMAT_EXTENSIBLE">FORMAT_EXTENSIBLE</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor1">Processor1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor10">Processor10</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor11">Processor11</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor12">Processor12</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor13">Processor13</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor14">Processor14</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor15">Processor15</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor16">Processor16</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor17">Processor17</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor18">Processor18</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor19">Processor19</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor2">Processor2</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor20">Processor20</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor21">Processor21</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor22">Processor22</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor23">Processor23</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor24">Processor24</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor25">Processor25</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor26">Processor26</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor27">Processor27</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor28">Processor28</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor29">Processor29</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor3">Processor3</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor30">Processor30</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor31">Processor31</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor32">Processor32</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor4">Processor4</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor5">Processor5</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor6">Processor6</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor7">Processor7</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor8">Processor8</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor9">Processor9</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_HANDLE_BYTESIZE">X3DAUDIO_HANDLE_BYTESIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_SPEED_OF_SOUND">X3DAUDIO_SPEED_OF_SOUND</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_1024_QUANTUM">XAUDIO2_1024_QUANTUM</td>
<td>u32</td>
<td>Used in XAudio2Create to specify nondefault processing quantum of 21.33 ms (1024 samples at 48KHz). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_ANY_PROCESSOR">XAUDIO2_ANY_PROCESSOR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_COMMIT_ALL">XAUDIO2_COMMIT_ALL</td>
<td>s32</td>
<td>Used in IXAudio2::CommitChanges. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_COMMIT_NOW">XAUDIO2_COMMIT_NOW</td>
<td>s32</td>
<td>Numeric values with special meanings Used as an OperationSet argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEBUG_ENGINE">XAUDIO2_DEBUG_ENGINE</td>
<td>u32</td>
<td>Used in XAudio2Create. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_CHANNELS">XAUDIO2_DEFAULT_CHANNELS</td>
<td>s32</td>
<td>Used in CreateMasteringVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_FREQ_RATIO">XAUDIO2_DEFAULT_FREQ_RATIO</td>
<td>f32</td>
<td>Default MaxFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_SAMPLERATE">XAUDIO2_DEFAULT_SAMPLERATE</td>
<td>s32</td>
<td>Used in CreateMasteringVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_END_OF_STREAM">XAUDIO2_END_OF_STREAM</td>
<td>u32</td>
<td>Used in XAUDIO2_BUFFER.Flags. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_INVALID_OPSET">XAUDIO2_INVALID_OPSET</td>
<td>u32</td>
<td>Not allowed for OperationSet arguments. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_API_CALLS">XAUDIO2_LOG_API_CALLS</td>
<td>u32</td>
<td>Public API function entries and exits. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_DETAIL">XAUDIO2_LOG_DETAIL</td>
<td>u32</td>
<td>More detailed chit-chat. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_ERRORS">XAUDIO2_LOG_ERRORS</td>
<td>u32</td>
<td>Values for the TraceMask and BreakMask bitmaps.  Only ERRORS and WARNINGS are valid in BreakMask.  WARNINGS implies ERRORS, DETAIL implies INFO, and FUNC_CALLS implies API_CALLS.  By default, TraceMask is ERRORS and WARNINGS and all the other settings are zero. For handled errors with serious effects. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_FUNC_CALLS">XAUDIO2_LOG_FUNC_CALLS</td>
<td>u32</td>
<td>Internal function entries and exits. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_INFO">XAUDIO2_LOG_INFO</td>
<td>u32</td>
<td>Informational chit-chat (e.g. state changes). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_LOCKS">XAUDIO2_LOG_LOCKS</td>
<td>u32</td>
<td>Usage of critical sections and mutexes. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_MEMORY">XAUDIO2_LOG_MEMORY</td>
<td>u32</td>
<td>Memory heap usage information. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_STREAMING">XAUDIO2_LOG_STREAMING</td>
<td>u32</td>
<td>Audio streaming information. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_TIMING">XAUDIO2_LOG_TIMING</td>
<td>u32</td>
<td>Delays detected and other timing data. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_WARNINGS">XAUDIO2_LOG_WARNINGS</td>
<td>u32</td>
<td>For handled errors that may be recoverable. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOOP_INFINITE">XAUDIO2_LOOP_INFINITE</td>
<td>s32</td>
<td>Used in XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_AUDIO_CHANNELS">XAUDIO2_MAX_AUDIO_CHANNELS</td>
<td>s32</td>
<td>Maximum channels in an audio stream. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_BUFFERS_SYSTEM">XAUDIO2_MAX_BUFFERS_SYSTEM</td>
<td>s32</td>
<td>Maximum buffers allowed for system threads (Xbox 360 only). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_BUFFER_BYTES">XAUDIO2_MAX_BUFFER_BYTES</td>
<td>u32</td>
<td>Maximum bytes allowed in a source buffer. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FILTER_FREQUENCY">XAUDIO2_MAX_FILTER_FREQUENCY</td>
<td>f32</td>
<td>Maximum XAUDIO2_FILTER_PARAMETERS.Frequency. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FILTER_ONEOVERQ">XAUDIO2_MAX_FILTER_ONEOVERQ</td>
<td>f32</td>
<td>Maximum XAUDIO2_FILTER_PARAMETERS.OneOverQ. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FREQ_RATIO">XAUDIO2_MAX_FREQ_RATIO</td>
<td>f32</td>
<td>Maximum MaxFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_INSTANCES">XAUDIO2_MAX_INSTANCES</td>
<td>s32</td>
<td>Maximum simultaneous XAudio2 objects on Xbox 360. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_LOOP_COUNT">XAUDIO2_MAX_LOOP_COUNT</td>
<td>s32</td>
<td>Maximum non-infinite XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_QUEUED_BUFFERS">XAUDIO2_MAX_QUEUED_BUFFERS</td>
<td>s32</td>
<td>Maximum buffers allowed in a voice queue. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_SAMPLE_RATE">XAUDIO2_MAX_SAMPLE_RATE</td>
<td>s32</td>
<td>Maximum audio sample rate supported. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_VOLUME_LEVEL">XAUDIO2_MAX_VOLUME_LEVEL</td>
<td>f32</td>
<td>Maximum acceptable volume level (2^24). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MIN_FREQ_RATIO">XAUDIO2_MIN_FREQ_RATIO</td>
<td>f32</td>
<td>Minimum SetFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MIN_SAMPLE_RATE">XAUDIO2_MIN_SAMPLE_RATE</td>
<td>s32</td>
<td>Minimum audio sample rate supported. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_NO_LOOP_REGION">XAUDIO2_NO_LOOP_REGION</td>
<td>s32</td>
<td>Used in XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT">XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT</td>
<td>u32</td>
<td>Used in CreateMasteringVoice to create a virtual audio client. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_PLAY_TAILS">XAUDIO2_PLAY_TAILS</td>
<td>u32</td>
<td>Used in IXAudio2SourceVoice::Stop. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_SEND_USEFILTER">XAUDIO2_SEND_USEFILTER</td>
<td>u32</td>
<td>Used in XAUDIO2_SEND_DESCRIPTOR.Flags. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_STOP_ENGINE_WHEN_IDLE">XAUDIO2_STOP_ENGINE_WHEN_IDLE</td>
<td>u32</td>
<td>Used in XAudio2Create to force the engine to Stop when no source voices are Started, and Start when a voice is Started. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOPITCH">XAUDIO2_VOICE_NOPITCH</td>
<td>u32</td>
<td>Used in IXAudio2::CreateSourceVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOSAMPLESPLAYED">XAUDIO2_VOICE_NOSAMPLESPLAYED</td>
<td>u32</td>
<td>Used in IXAudio2SourceVoice::GetState. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOSRC">XAUDIO2_VOICE_NOSRC</td>
<td>u32</td>
<td>Used in IXAudio2::CreateSourceVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_USEFILTER">XAUDIO2_VOICE_USEFILTER</td>
<td>u32</td>
<td>Used in IXAudio2::CreateSource/SubmixVoice. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_FLOAT32"><span class="titletype">type alias</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">Type Aliases</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="XAudio2_FLOAT32">FLOAT32</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_UINT32">UINT32</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_UINT64">UINT64</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_HANDLE">X3DAUDIO_HANDLE</td>
<td>[20] Win32.BYTE</td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_PROCESSOR">XAUDIO2_PROCESSOR</td>
<td><a href="#XAudio2_UINT32">XAudio2.UINT32</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_AUDIO_STREAM_CATEGORY"><span class="titletype">enum</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">AUDIO_STREAM_CATEGORY</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L133" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>AudioCategory_Other</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_ForegroundOnlyMedia</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_BackgroundCapableMedia</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_Communications</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_Alerts</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_SoundEffects</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_GameEffects</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_GameMedia</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_GameChat</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_Speech</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_Movie</td>
<td></td>
</tr>
<tr>
<td>AudioCategory_Media</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L216" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>vtbl</td>
<td>*XAudio2.IXAudio2Itf</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2EngineCallback"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2EngineCallback</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L220" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>vtbl</td>
<td>*XAudio2.IXAudio2EngineCallbackItf</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2EngineCallbackItf"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2EngineCallbackItf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L224" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>onProcessingPassStart</td>
<td>func(*XAudio2.IXAudio2EngineCallback)</td>
<td></td>
</tr>
<tr>
<td>onProcessingPassEnd</td>
<td>func(*XAudio2.IXAudio2EngineCallback)</td>
<td></td>
</tr>
<tr>
<td>onCriticalError</td>
<td>func(*XAudio2.IXAudio2EngineCallback, Win32.HRESULT)</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2Itf"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2Itf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L242" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>queryInterface</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>addRef</td>
<td>func(*XAudio2.IXAudio2)->Win32.ULONG</td>
<td></td>
</tr>
<tr>
<td>release</td>
<td>func(*XAudio2.IXAudio2)->Win32.ULONG</td>
<td></td>
</tr>
<tr>
<td>registerForCallbacks</td>
<td>func(*XAudio2.IXAudio2, *XAudio2.IXAudio2EngineCallback)</td>
<td></td>
</tr>
<tr>
<td>unregisterForCallbacks</td>
<td>func(*XAudio2.IXAudio2, *XAudio2.IXAudio2EngineCallback)</td>
<td></td>
</tr>
<tr>
<td>createSourceVoice</td>
<td>func(*XAudio2.IXAudio2, **XAudio2.IXAudio2SourceVoice, const *XAudio2.WAVEFORMATEX, XAudio2.UINT32, f32, *XAudio2.IXAudio2VoiceCallback, const *XAudio2.XAUDIO2_VOICE_SENDS, const *XAudio2.XAUDIO2_EFFECT_CHAIN)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>createSubmixVoice</td>
<td>func(*XAudio2.IXAudio2, **XAudio2.IXAudio2SubmixVoice, XAudio2.UINT32, XAudio2.UINT32, XAudio2.UINT32, XAudio2.UINT32, const *XAudio2.XAUDIO2_VOICE_SENDS, const *XAudio2.XAUDIO2_EFFECT_CHAIN)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>createMasteringVoice</td>
<td>func(*XAudio2.IXAudio2, **XAudio2.IXAudio2MasteringVoice, XAudio2.UINT32, XAudio2.UINT32, XAudio2.UINT32, Win32.LPCWSTR, *XAudio2.XAUDIO2_EFFECT_CHAIN, XAudio2.AUDIO_STREAM_CATEGORY)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>startEngine</td>
<td>func(*XAudio2.IXAudio2)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>stopEngine</td>
<td>func(*XAudio2.IXAudio2)</td>
<td></td>
</tr>
<tr>
<td>commitChanges</td>
<td>func(*XAudio2.IXAudio2, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getPerformanceData</td>
<td>func(*XAudio2.IXAudio2, *XAudio2.XAUDIO2_PERFORMANCE_DATA)</td>
<td></td>
</tr>
<tr>
<td>setDebugConfiguration</td>
<td>func(*XAudio2.IXAudio2, const *XAudio2.XAUDIO2_DEBUG_CONFIGURATION, *void)</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2MasteringVoice"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2MasteringVoice</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L218" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>vtbl</td>
<td>*XAudio2.IXAudio2MasteringVoiceItf</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2MasteringVoiceItf"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2MasteringVoiceItf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L282" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> voice</td>
<td><a href="#XAudio2_IXAudio2VoiceItf">XAudio2.IXAudio2VoiceItf</a></td>
<td></td>
</tr>
<tr>
<td>getChannelMask</td>
<td>func(*XAudio2.IXAudio2MasteringVoice, *Win32.DWORD)->Win32.HRESULT</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2SourceVoice"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2SourceVoice</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L219" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>vtbl</td>
<td>*XAudio2.IXAudio2SourceVoiceItf</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2SourceVoiceItf"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2SourceVoiceItf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L293" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> voice</td>
<td><a href="#XAudio2_IXAudio2VoiceItf">XAudio2.IXAudio2VoiceItf</a></td>
<td></td>
</tr>
<tr>
<td>start</td>
<td>func(*XAudio2.IXAudio2SourceVoice, XAudio2.UINT32, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>stop</td>
<td>func(*XAudio2.IXAudio2SourceVoice, XAudio2.UINT32, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>submitSourceBuffer</td>
<td>func(*XAudio2.IXAudio2SourceVoice, const *XAudio2.XAUDIO2_BUFFER, const *XAudio2.XAUDIO2_BUFFER_WMA)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>glushSourceBuffers</td>
<td>func(*XAudio2.IXAudio2SourceVoice)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>discontinuity</td>
<td>func(*XAudio2.IXAudio2SourceVoice)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>exitLoop</td>
<td>func(*XAudio2.IXAudio2SourceVoice, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getState</td>
<td>func(*XAudio2.IXAudio2SourceVoice, *XAudio2.XAUDIO2_VOICE_STATE, XAudio2.UINT32)</td>
<td></td>
</tr>
<tr>
<td>setFrequencyRatio</td>
<td>func(*XAudio2.IXAudio2SourceVoice, f32, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getFrequencyRatio</td>
<td>func(*XAudio2.IXAudio2SourceVoice, *f32)</td>
<td></td>
</tr>
<tr>
<td>setSourceSampleRate</td>
<td>func(*XAudio2.IXAudio2SourceVoice, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2SubmixVoice"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2SubmixVoice</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L222" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>vtbl</td>
<td>*XAudio2.IXAudio2SubmixVoiceItf</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2SubmixVoiceItf"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2SubmixVoiceItf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L288" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td><b>using</b> voice</td>
<td><a href="#XAudio2_IXAudio2VoiceItf">XAudio2.IXAudio2VoiceItf</a></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2Voice"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2Voice</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L217" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>vtbl</td>
<td>*XAudio2.IXAudio2VoiceItf</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2VoiceCallback"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2VoiceCallback</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L221" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>vtbl</td>
<td>*XAudio2.IXAudio2VoiceCallbackItf</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2VoiceCallbackItf"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2VoiceCallbackItf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L231" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>onVoiceProcessingPassStart</td>
<td>func(*XAudio2.IXAudio2VoiceCallback, XAudio2.UINT32)</td>
<td></td>
</tr>
<tr>
<td>onVoiceProcessingPassEnd</td>
<td>func(*XAudio2.IXAudio2VoiceCallback)</td>
<td></td>
</tr>
<tr>
<td>onStreamEnd</td>
<td>func(*XAudio2.IXAudio2VoiceCallback)</td>
<td></td>
</tr>
<tr>
<td>onBufferStart</td>
<td>func(*XAudio2.IXAudio2VoiceCallback, *void)</td>
<td></td>
</tr>
<tr>
<td>onBufferEnd</td>
<td>func(*XAudio2.IXAudio2VoiceCallback, *void)</td>
<td></td>
</tr>
<tr>
<td>onLoopEnd</td>
<td>func(*XAudio2.IXAudio2VoiceCallback, *void)</td>
<td></td>
</tr>
<tr>
<td>onVoiceError</td>
<td>func(*XAudio2.IXAudio2VoiceCallback, *void, Win32.HRESULT)</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_IXAudio2VoiceItf"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">IXAudio2VoiceItf</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L259" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>getVoiceDetails</td>
<td>func(*XAudio2.IXAudio2Voice, *XAudio2.XAUDIO2_VOICE_DETAILS)</td>
<td></td>
</tr>
<tr>
<td>setOutputVoices</td>
<td>func(*XAudio2.IXAudio2Voice, const *XAudio2.XAUDIO2_VOICE_SENDS)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>setEffectChain</td>
<td>func(*XAudio2.IXAudio2Voice, const *XAudio2.XAUDIO2_EFFECT_CHAIN)</td>
<td></td>
</tr>
<tr>
<td>enableEffect</td>
<td>func(*XAudio2.IXAudio2Voice, XAudio2.UINT32, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>disableEffect</td>
<td>func(*XAudio2.IXAudio2Voice, XAudio2.UINT32, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getEffectState</td>
<td>func(*XAudio2.IXAudio2Voice, XAudio2.UINT32, *Win32.BOOL)</td>
<td></td>
</tr>
<tr>
<td>setEffectParameters</td>
<td>func(*XAudio2.IXAudio2Voice, XAudio2.UINT32, const *void, XAudio2.UINT32, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getEffectParameters</td>
<td>func(*XAudio2.IXAudio2Voice, XAudio2.UINT32, *void, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>setFilterParameters</td>
<td>func(*XAudio2.IXAudio2Voice, const *XAudio2.XAUDIO2_FILTER_PARAMETERS, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getFilterParameters</td>
<td>func(*XAudio2.IXAudio2Voice, *XAudio2.XAUDIO2_FILTER_PARAMETERS)</td>
<td></td>
</tr>
<tr>
<td>setOutputFilterParameters</td>
<td>func(*XAudio2.IXAudio2Voice, *XAudio2.IXAudio2Voice, const *XAudio2.XAUDIO2_FILTER_PARAMETERS, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getOutputFilterParameters</td>
<td>func(*XAudio2.IXAudio2Voice, *XAudio2.IXAudio2Voice, *XAudio2.XAUDIO2_FILTER_PARAMETERS)</td>
<td></td>
</tr>
<tr>
<td>setVolume</td>
<td>func(*XAudio2.IXAudio2Voice, f32, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getVolume</td>
<td>func(*XAudio2.IXAudio2Voice, *f32)</td>
<td></td>
</tr>
<tr>
<td>setChannelVolumes</td>
<td>func(*XAudio2.IXAudio2Voice, XAudio2.UINT32, const *f32, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getChannelVolumes</td>
<td>func(*XAudio2.IXAudio2Voice, XAudio2.UINT32, *f32)</td>
<td></td>
</tr>
<tr>
<td>setOutputMatrix</td>
<td>func(*XAudio2.IXAudio2Voice, *XAudio2.IXAudio2Voice, XAudio2.UINT32, XAudio2.UINT32, const *f32, XAudio2.UINT32)->Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td>getOutputMatrix</td>
<td>func(*XAudio2.IXAudio2Voice, *XAudio2.IXAudio2Voice, XAudio2.UINT32, XAudio2.UINT32, *f32)</td>
<td></td>
</tr>
<tr>
<td>destroyVoice</td>
<td>func(*XAudio2.IXAudio2Voice)</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_WAVEFORMATEX"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">WAVEFORMATEX</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L311" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>wFormatTag</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>nChannels</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>nSamplesPerSec</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>nAvgBytesPerSec</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>nBlockAlign</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>wBitsPerSample</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>cbSize</td>
<td>u16</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_WAVEFORMATEXTENSIBLE"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">WAVEFORMATEXTENSIBLE</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L322" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>format</td>
<td><a href="#XAudio2_WAVEFORMATEX">XAudio2.WAVEFORMATEX</a></td>
<td></td>
</tr>
<tr>
<td>samples</td>
<td>{wValidBitsPerSample: Win32.WORD, wSamplesPerBlock: Win32.WORD, wReserved: Win32.WORD}</td>
<td></td>
</tr>
<tr>
<td>dwChannelMask</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>subFormat</td>
<td>Win32.GUID</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_X3DAudioInitialize"><span class="titletype">func</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">X3DAudioInitialize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L340" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">X3DAudioInitialize</span><span class="SyntaxCode">(speakerChannelMask: </span><span class="SyntaxConstant">XAudio2</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span><span class="SyntaxCode">, speedOfSound: </span><span class="SyntaxConstant">XAudio2</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#XAudio2_FLOAT32">FLOAT32</a></span><span class="SyntaxCode">, instance: </span><span class="SyntaxConstant">XAudio2</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#XAudio2_X3DAUDIO_HANDLE">X3DAUDIO_HANDLE</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HRESULT</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_BUFFER"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_BUFFER</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L172" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>flags</td>
<td>u32</td>
<td>Either 0 or XAUDIO2_END_OF_STREAM. </td>
</tr>
<tr>
<td>audioBytes</td>
<td>u32</td>
<td>Size of the audio data buffer in bytes. </td>
</tr>
<tr>
<td>pAudioData</td>
<td>const *Win32.BYTE</td>
<td>Pointer to the audio data buffer. </td>
</tr>
<tr>
<td>playBegin</td>
<td>u32</td>
<td>First sample in this buffer to be played. </td>
</tr>
<tr>
<td>playLength</td>
<td>u32</td>
<td>Length of the region to be played in samples, or 0 to play the whole buffer. </td>
</tr>
<tr>
<td>loopBegin</td>
<td>u32</td>
<td>First sample of the region to be looped. </td>
</tr>
<tr>
<td>loopLength</td>
<td>u32</td>
<td>Length of the desired loop region in samples, or 0 to loop the entire buffer. </td>
</tr>
<tr>
<td>loopCount</td>
<td>u32</td>
<td>Number of times to repeat the loop region, or XAUDIO2_LOOP_INFINITE to loop forever. </td>
</tr>
<tr>
<td>pContext</td>
<td>*void</td>
<td>Context value to be passed back in callbacks. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_BUFFER_WMA"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_BUFFER_WMA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L185" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>pDecodedPacketCumulativeBytes</td>
<td>const *XAudio2.UINT32</td>
<td></td>
</tr>
<tr>
<td>PacketCount</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_DEBUG_CONFIGURATION"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_DEBUG_CONFIGURATION</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L123" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>traceMask</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>breakMask</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>logThreadID</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>logFileline</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>logFunctionName</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>logTiming</td>
<td>s32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_EFFECT_CHAIN"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_EFFECT_CHAIN</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L101" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_FILTER_PARAMETERS"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_FILTER_PARAMETERS</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L209" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>type</td>
<td><a href="#XAudio2_XAUDIO2_FILTER_TYPE">XAudio2.XAUDIO2_FILTER_TYPE</a></td>
<td></td>
</tr>
<tr>
<td>frequency</td>
<td>f32</td>
<td></td>
</tr>
<tr>
<td>oneOverQ</td>
<td>f32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_FILTER_TYPE"><span class="titletype">enum</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_FILTER_TYPE</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L199" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Used in XAUDIO2_FILTER_PARAMETERS below. </p>
<table class="enumeration">
<tr>
<td>LowPassFilter</td>
<td></td>
</tr>
<tr>
<td>BandPassFilter</td>
<td>Attenuates frequencies above the cutoff frequency (state-variable filter). </td>
</tr>
<tr>
<td>HighPassFilter</td>
<td>Attenuates frequencies outside a given range      (state-variable filter). </td>
</tr>
<tr>
<td>NotchFilter</td>
<td>Attenuates frequencies below the cutoff frequency (state-variable filter). </td>
</tr>
<tr>
<td>LowPassOnePoleFilter</td>
<td>Attenuates frequencies inside a given range       (state-variable filter). </td>
</tr>
<tr>
<td>HighPassOnePoleFilter</td>
<td>Attenuates frequencies above the cutoff frequency (one-pole filter, XAUDIO2_FILTER_PARAMETERS.OneOverQ has no effect)  Attenuates frequencies below the cutoff frequency (one-pole filter, XAUDIO2_FILTER_PARAMETERS.OneOverQ has no effect). </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_PERFORMANCE_DATA"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_PERFORMANCE_DATA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>AudioCyclesSinceLastQuery</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>TotalCyclesSinceLastQuery</td>
<td>u64</td>
<td></td>
</tr>
<tr>
<td>MinimumCyclesPerQuantum</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>MaximumCyclesPerQuantum</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>MemoryUsageInBytes</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>CurrentLatencyInSamples</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>GlitchesSinceEngineStarted</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ActiveSourceVoiceCount</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>TotalSourceVoiceCount</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ActiveSubmixVoiceCount</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ActiveResamplerCount</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ActiveMatrixMixCount</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ActiveXmaSourceVoices</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>ActiveXmaStreams</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_SEND_DESCRIPTOR"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_SEND_DESCRIPTOR</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>flags</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>pOutputVoice</td>
<td>*XAudio2.IXAudio2Voice</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_VOICE_DETAILS"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_VOICE_DETAILS</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>creationFlags</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>activeFlags</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>inputChannels</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>inputSampleRate</td>
<td>u32</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_VOICE_SENDS"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_VOICE_SENDS</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L165" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>sendCount</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>pSends</td>
<td>*XAudio2.XAUDIO2_SEND_DESCRIPTOR</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAUDIO2_VOICE_STATE"><span class="titletype">struct</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAUDIO2_VOICE_STATE</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L191" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td>pCurrentBufferContext</td>
<td>*void</td>
<td></td>
</tr>
<tr>
<td>buffersQueued</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>samplesPlayed</td>
<td>u64</td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="XAudio2_XAudio2Create"><span class="titletype">func</span> <span class="titlelight">XAudio2.</span><span class="titlestrong">XAudio2Create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L339" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">XAudio2Create</span><span class="SyntaxCode">(ppXAudio2: **</span><span class="SyntaxConstant">XAudio2</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span><span class="SyntaxCode">, flags: </span><span class="SyntaxConstant">XAudio2</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span><span class="SyntaxCode">, processor: </span><span class="SyntaxConstant">XAudio2</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_PROCESSOR">XAUDIO2_PROCESSOR</a></span><span class="SyntaxCode"> = </span><span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_ANY_PROCESSOR">XAUDIO2_ANY_PROCESSOR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HRESULT</span><span class="SyntaxCode"></code>
</div>
</div>
</div>
</div>
</body>
</html>
