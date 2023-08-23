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
<td id="XAudio2_FORMAT_EXTENSIBLE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FORMAT_EXTENSIBLE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor1" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor1</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor10" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor10</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor11" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor11</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor12" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor12</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor13" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor13</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor14" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor14</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor15" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor15</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor16" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor16</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor17" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor17</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor18" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor18</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor19" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor19</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor2" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor2</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor20" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor20</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor21" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor21</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor22" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor22</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor23" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor23</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor24" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor24</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor25" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor25</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor26" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor26</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor27" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor27</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor28" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor28</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor29" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor29</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor3" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor3</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor30" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor30</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor31" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor31</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor32" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor32</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor4" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor4</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor5" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor5</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor6" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor6</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor7" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor7</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor8" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor8</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor9" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Processor9</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_HANDLE_BYTESIZE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">X3DAUDIO_HANDLE_BYTESIZE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_SPEED_OF_SOUND" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">X3DAUDIO_SPEED_OF_SOUND</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_1024_QUANTUM" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_1024_QUANTUM</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in XAudio2Create to specify nondefault processing quantum of 21.33 ms (1024 samples at 48KHz). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_ANY_PROCESSOR" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_ANY_PROCESSOR</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_COMMIT_ALL" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_COMMIT_ALL</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Used in IXAudio2::CommitChanges. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_COMMIT_NOW" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_COMMIT_NOW</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Numeric values with special meanings Used as an OperationSet argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEBUG_ENGINE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_DEBUG_ENGINE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in XAudio2Create. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_CHANNELS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_DEFAULT_CHANNELS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Used in CreateMasteringVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_FREQ_RATIO" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_DEFAULT_FREQ_RATIO</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td>Default MaxFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_SAMPLERATE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_DEFAULT_SAMPLERATE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Used in CreateMasteringVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_END_OF_STREAM" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_END_OF_STREAM</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in XAUDIO2_BUFFER.Flags. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_INVALID_OPSET" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_INVALID_OPSET</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Not allowed for OperationSet arguments. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_API_CALLS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_API_CALLS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Public API function entries and exits. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_DETAIL" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_DETAIL</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>More detailed chit-chat. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_ERRORS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_ERRORS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Values for the TraceMask and BreakMask bitmaps.  Only ERRORS and WARNINGS are valid in BreakMask.  WARNINGS implies ERRORS, DETAIL implies INFO, and FUNC_CALLS implies API_CALLS.  By default, TraceMask is ERRORS and WARNINGS and all the other settings are zero. For handled errors with serious effects. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_FUNC_CALLS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_FUNC_CALLS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Internal function entries and exits. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_INFO" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_INFO</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Informational chit-chat (e.g. state changes). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_LOCKS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_LOCKS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Usage of critical sections and mutexes. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_MEMORY" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_MEMORY</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Memory heap usage information. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_STREAMING" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_STREAMING</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Audio streaming information. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_TIMING" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_TIMING</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Delays detected and other timing data. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_WARNINGS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOG_WARNINGS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>For handled errors that may be recoverable. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOOP_INFINITE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_LOOP_INFINITE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Used in XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_AUDIO_CHANNELS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_AUDIO_CHANNELS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Maximum channels in an audio stream. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_BUFFERS_SYSTEM" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_BUFFERS_SYSTEM</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Maximum buffers allowed for system threads (Xbox 360 only). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_BUFFER_BYTES" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_BUFFER_BYTES</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Maximum bytes allowed in a source buffer. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FILTER_FREQUENCY" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_FILTER_FREQUENCY</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td>Maximum XAUDIO2_FILTER_PARAMETERS.Frequency. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FILTER_ONEOVERQ" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_FILTER_ONEOVERQ</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td>Maximum XAUDIO2_FILTER_PARAMETERS.OneOverQ. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FREQ_RATIO" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_FREQ_RATIO</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td>Maximum MaxFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_INSTANCES" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_INSTANCES</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Maximum simultaneous XAudio2 objects on Xbox 360. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_LOOP_COUNT" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_LOOP_COUNT</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Maximum non-infinite XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_QUEUED_BUFFERS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_QUEUED_BUFFERS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Maximum buffers allowed in a voice queue. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_SAMPLE_RATE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_SAMPLE_RATE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Maximum audio sample rate supported. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_VOLUME_LEVEL" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MAX_VOLUME_LEVEL</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td>Maximum acceptable volume level (2^24). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MIN_FREQ_RATIO" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MIN_FREQ_RATIO</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td>Minimum SetFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MIN_SAMPLE_RATE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_MIN_SAMPLE_RATE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Minimum audio sample rate supported. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_NO_LOOP_REGION" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_NO_LOOP_REGION</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td>Used in XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in CreateMasteringVoice to create a virtual audio client. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_PLAY_TAILS" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_PLAY_TAILS</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in IXAudio2SourceVoice::Stop. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_SEND_USEFILTER" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_SEND_USEFILTER</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in XAUDIO2_SEND_DESCRIPTOR.Flags. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_STOP_ENGINE_WHEN_IDLE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_STOP_ENGINE_WHEN_IDLE</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in XAudio2Create to force the engine to Stop when no source voices are Started, and Start when a voice is Started. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOPITCH" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_VOICE_NOPITCH</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in IXAudio2::CreateSourceVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOSAMPLESPLAYED" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_VOICE_NOSAMPLESPLAYED</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in IXAudio2SourceVoice::GetState. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOSRC" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_VOICE_NOSRC</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Used in IXAudio2::CreateSourceVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_USEFILTER" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_VOICE_USEFILTER</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
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
<td id="XAudio2_FLOAT32" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">FLOAT32</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_UINT32" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">UINT32</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_UINT64" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">UINT64</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_HANDLE" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">X3DAUDIO_HANDLE</span></span></td>
<td class="codetype"><span class="SyntaxCode">[<span class="SyntaxNumber">20</span>] <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">BYTE</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_PROCESSOR" class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAUDIO2_PROCESSOR</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_Other</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_ForegroundOnlyMedia</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_BackgroundCapableMedia</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_Communications</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_Alerts</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_SoundEffects</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_GameEffects</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_GameMedia</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_GameChat</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_Speech</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_Movie</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCategory_Media</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">vtbl</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Itf">IXAudio2Itf</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode">vtbl</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2EngineCallbackItf">IXAudio2EngineCallbackItf</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode">onProcessingPassStart</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">onProcessingPassEnd</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">onCriticalError</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>, <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span>)</span></td>
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
<td class="codetype"><span class="SyntaxCode">queryInterface</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">addRef</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">ULONG</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">release</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">ULONG</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">registerForCallbacks</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">unregisterForCallbacks</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">createSourceVoice</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, **<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_WAVEFORMATEX">WAVEFORMATEX</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxType">f32</span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">createSubmixVoice</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, **<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SubmixVoice">IXAudio2SubmixVoice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">createMasteringVoice</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, **<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2MasteringVoice">IXAudio2MasteringVoice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">LPCWSTR</span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_AUDIO_STREAM_CATEGORY">AUDIO_STREAM_CATEGORY</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">startEngine</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">stopEngine</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">commitChanges</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getPerformanceData</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_PERFORMANCE_DATA">XAUDIO2_PERFORMANCE_DATA</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setDebugConfiguration</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_DEBUG_CONFIGURATION">XAUDIO2_DEBUG_CONFIGURATION</a></span>, *<span class="SyntaxType">void</span>)</span></td>
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
<td class="codetype"><span class="SyntaxCode">vtbl</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2MasteringVoiceItf">IXAudio2MasteringVoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">using</span> voice</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getChannelMask</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2MasteringVoice">IXAudio2MasteringVoice</a></span>, *<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">DWORD</span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">vtbl</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoiceItf">IXAudio2SourceVoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">using</span> voice</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">start</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">stop</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">submitSourceBuffer</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_BUFFER">XAUDIO2_BUFFER</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_BUFFER_WMA">XAUDIO2_BUFFER_WMA</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">glushSourceBuffers</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">discontinuity</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">exitLoop</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getState</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_VOICE_STATE">XAUDIO2_VOICE_STATE</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setFrequencyRatio</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SyntaxType">f32</span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getFrequencyRatio</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, *<span class="SyntaxType">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setSourceSampleRate</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">vtbl</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2SubmixVoiceItf">IXAudio2SubmixVoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">using</span> voice</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode">vtbl</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode">vtbl</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceCallbackItf">IXAudio2VoiceCallbackItf</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode">onVoiceProcessingPassStart</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">onVoiceProcessingPassEnd</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">onStreamEnd</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">onBufferStart</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="SyntaxType">void</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">onBufferEnd</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="SyntaxType">void</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">onLoopEnd</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="SyntaxType">void</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">onVoiceError</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="SyntaxType">void</span>, <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span>)</span></td>
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
<td class="codetype"><span class="SyntaxCode">getVoiceDetails</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_VOICE_DETAILS">XAUDIO2_VOICE_DETAILS</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setOutputVoices</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setEffectChain</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">enableEffect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">disableEffect</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getEffectState</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">BOOL</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setEffectParameters</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxType">void</span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getEffectParameters</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="SyntaxType">void</span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setFilterParameters</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getFilterParameters</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setOutputFilterParameters</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getOutputFilterParameters</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setVolume</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxType">f32</span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getVolume</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SyntaxType">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setChannelVolumes</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxType">f32</span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getChannelVolumes</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="SyntaxType">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">setOutputMatrix</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxKeyword">const</span> *<span class="SyntaxType">f32</span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">getOutputMatrix</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="SyntaxType">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">destroyVoice</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">func</span>(*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>)</span></td>
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
<td class="codetype"><span class="SyntaxCode">wFormatTag</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">nChannels</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">nSamplesPerSec</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">nAvgBytesPerSec</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">nBlockAlign</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">wBitsPerSample</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">cbSize</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u16</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">format</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_WAVEFORMATEX">WAVEFORMATEX</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">samples</span></td>
<td class="codetype"><span class="SyntaxCode">{wValidBitsPerSample: <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">WORD</span>, wSamplesPerBlock: <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">WORD</span>, wReserved: <span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">WORD</span>}</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">dwChannelMask</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">subFormat</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">GUID</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">X3DAudioInitialize</span>(speakerChannelMask: <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, speedOfSound: <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_FLOAT32">FLOAT32</a></span>, instance: <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_X3DAUDIO_HANDLE">X3DAUDIO_HANDLE</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></code>
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
<td class="codetype"><span class="SyntaxCode">flags</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Either 0 or XAUDIO2_END_OF_STREAM. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">audioBytes</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Size of the audio data buffer in bytes. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pAudioData</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">BYTE</span></span></td>
<td>Pointer to the audio data buffer. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">playBegin</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>First sample in this buffer to be played. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">playLength</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Length of the region to be played in samples, or 0 to play the whole buffer. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">loopBegin</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>First sample of the region to be looped. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">loopLength</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Length of the desired loop region in samples, or 0 to loop the entire buffer. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">loopCount</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td>Number of times to repeat the loop region, or XAUDIO2_LOOP_INFINITE to loop forever. </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pContext</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">pDecodedPacketCumulativeBytes</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxKeyword">const</span> *<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">PacketCount</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">traceMask</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">breakMask</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">logThreadID</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">logFileline</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">logFunctionName</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">logTiming</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">s32</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">type</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_FILTER_TYPE">XAUDIO2_FILTER_TYPE</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">frequency</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">oneOverQ</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">f32</span></span></td>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LowPassFilter</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">BandPassFilter</span></span></td>
<td>Attenuates frequencies above the cutoff frequency (state-variable filter). </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HighPassFilter</span></span></td>
<td>Attenuates frequencies outside a given range      (state-variable filter). </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">NotchFilter</span></span></td>
<td>Attenuates frequencies below the cutoff frequency (state-variable filter). </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">LowPassOnePoleFilter</span></span></td>
<td>Attenuates frequencies inside a given range       (state-variable filter). </td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">HighPassOnePoleFilter</span></span></td>
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
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">AudioCyclesSinceLastQuery</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">TotalCyclesSinceLastQuery</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MinimumCyclesPerQuantum</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MaximumCyclesPerQuantum</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">MemoryUsageInBytes</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">CurrentLatencyInSamples</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">GlitchesSinceEngineStarted</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ActiveSourceVoiceCount</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">TotalSourceVoiceCount</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ActiveSubmixVoiceCount</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ActiveResamplerCount</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ActiveMatrixMixCount</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ActiveXmaSourceVoices</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxConstant">ActiveXmaStreams</span></span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">flags</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pOutputVoice</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode">creationFlags</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">activeFlags</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">inputChannels</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">inputSampleRate</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
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
<td class="codetype"><span class="SyntaxCode">sendCount</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">pSends</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_SEND_DESCRIPTOR">XAUDIO2_SEND_DESCRIPTOR</a></span></span></td>
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
<td class="codetype"><span class="SyntaxCode">pCurrentBufferContext</span></td>
<td class="codetype"><span class="SyntaxCode">*<span class="SyntaxType">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">buffersQueued</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SyntaxCode">samplesPlayed</span></td>
<td class="codetype"><span class="SyntaxCode"><span class="SyntaxType">u64</span></span></td>
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
<div class="precode"><code><span class="SyntaxCode"><span class="SyntaxKeyword">func</span> <span class="SyntaxConstant">XAudio2Create</span>(ppXAudio2: **<span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, flags: <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_UINT32">UINT32</a></span>, processor: <span class="SyntaxConstant">XAudio2</span>.<span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_PROCESSOR">XAUDIO2_PROCESSOR</a></span> = <span class="SyntaxConstant"><a href="#XAudio2_XAUDIO2_ANY_PROCESSOR">XAUDIO2_ANY_PROCESSOR</a></span>)-&gt;<span class="SyntaxConstant">Win32</span>.<span class="SyntaxConstant">HRESULT</span></span></code>
</div>
</div>
</div>
</div>
</body>
</html>
