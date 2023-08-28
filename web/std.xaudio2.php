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
            border-radius:      5px;
            border:             1px solid Orange;
            background-color:   LightYellow;
            border-left:        6px solid Orange;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .container a {
            color:              DoggerBlue;
        }
        .precode a {
            color:              inherit;
        }
        .codetype a {
            color:              inherit;
        }
        .left a {
            text-decoration:    none;
        }
        .container a:hover {
            text-decoration:    underline;
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
            border-radius:      5px;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
        }
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
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</blockquote>
<h1>Module xaudio2</h1>
<p> Wrapper for Microsoft XAudio2 library. </p>
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
<td id="XAudio2_FORMAT_EXTENSIBLE" class="codetype"><span class="SCst">FORMAT_EXTENSIBLE</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor1" class="codetype"><span class="SCst">Processor1</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor10" class="codetype"><span class="SCst">Processor10</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor11" class="codetype"><span class="SCst">Processor11</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor12" class="codetype"><span class="SCst">Processor12</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor13" class="codetype"><span class="SCst">Processor13</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor14" class="codetype"><span class="SCst">Processor14</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor15" class="codetype"><span class="SCst">Processor15</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor16" class="codetype"><span class="SCst">Processor16</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor17" class="codetype"><span class="SCst">Processor17</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor18" class="codetype"><span class="SCst">Processor18</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor19" class="codetype"><span class="SCst">Processor19</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor2" class="codetype"><span class="SCst">Processor2</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor20" class="codetype"><span class="SCst">Processor20</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor21" class="codetype"><span class="SCst">Processor21</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor22" class="codetype"><span class="SCst">Processor22</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor23" class="codetype"><span class="SCst">Processor23</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor24" class="codetype"><span class="SCst">Processor24</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor25" class="codetype"><span class="SCst">Processor25</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor26" class="codetype"><span class="SCst">Processor26</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor27" class="codetype"><span class="SCst">Processor27</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor28" class="codetype"><span class="SCst">Processor28</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor29" class="codetype"><span class="SCst">Processor29</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor3" class="codetype"><span class="SCst">Processor3</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor30" class="codetype"><span class="SCst">Processor30</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor31" class="codetype"><span class="SCst">Processor31</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor32" class="codetype"><span class="SCst">Processor32</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor4" class="codetype"><span class="SCst">Processor4</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor5" class="codetype"><span class="SCst">Processor5</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor6" class="codetype"><span class="SCst">Processor6</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor7" class="codetype"><span class="SCst">Processor7</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor8" class="codetype"><span class="SCst">Processor8</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor9" class="codetype"><span class="SCst">Processor9</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_HANDLE_BYTESIZE" class="codetype"><span class="SCst">X3DAUDIO_HANDLE_BYTESIZE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_SPEED_OF_SOUND" class="codetype"><span class="SCst">X3DAUDIO_SPEED_OF_SOUND</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_1024_QUANTUM" class="codetype"><span class="SCst">XAUDIO2_1024_QUANTUM</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in XAudio2Create to specify nondefault processing quantum of 21.33 ms (1024 samples at 48KHz). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_ANY_PROCESSOR" class="codetype"><span class="SCst">XAUDIO2_ANY_PROCESSOR</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_COMMIT_ALL" class="codetype"><span class="SCst">XAUDIO2_COMMIT_ALL</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Used in IXAudio2::CommitChanges. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_COMMIT_NOW" class="codetype"><span class="SCst">XAUDIO2_COMMIT_NOW</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Numeric values with special meanings Used as an OperationSet argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEBUG_ENGINE" class="codetype"><span class="SCst">XAUDIO2_DEBUG_ENGINE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in XAudio2Create. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_CHANNELS" class="codetype"><span class="SCst">XAUDIO2_DEFAULT_CHANNELS</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Used in CreateMasteringVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_FREQ_RATIO" class="codetype"><span class="SCst">XAUDIO2_DEFAULT_FREQ_RATIO</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td>Default MaxFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_SAMPLERATE" class="codetype"><span class="SCst">XAUDIO2_DEFAULT_SAMPLERATE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Used in CreateMasteringVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_END_OF_STREAM" class="codetype"><span class="SCst">XAUDIO2_END_OF_STREAM</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in XAUDIO2_BUFFER.Flags. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_INVALID_OPSET" class="codetype"><span class="SCst">XAUDIO2_INVALID_OPSET</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Not allowed for OperationSet arguments. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_API_CALLS" class="codetype"><span class="SCst">XAUDIO2_LOG_API_CALLS</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Public API function entries and exits. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_DETAIL" class="codetype"><span class="SCst">XAUDIO2_LOG_DETAIL</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>More detailed chit-chat. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_ERRORS" class="codetype"><span class="SCst">XAUDIO2_LOG_ERRORS</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Values for the TraceMask and BreakMask bitmaps.  Only ERRORS and WARNINGS are valid in BreakMask.  WARNINGS implies ERRORS, DETAIL implies INFO, and FUNC_CALLS implies API_CALLS.  By default, TraceMask is ERRORS and WARNINGS and all the other settings are zero. For handled errors with serious effects. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_FUNC_CALLS" class="codetype"><span class="SCst">XAUDIO2_LOG_FUNC_CALLS</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Internal function entries and exits. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_INFO" class="codetype"><span class="SCst">XAUDIO2_LOG_INFO</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Informational chit-chat (e.g. state changes). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_LOCKS" class="codetype"><span class="SCst">XAUDIO2_LOG_LOCKS</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Usage of critical sections and mutexes. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_MEMORY" class="codetype"><span class="SCst">XAUDIO2_LOG_MEMORY</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Memory heap usage information. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_STREAMING" class="codetype"><span class="SCst">XAUDIO2_LOG_STREAMING</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Audio streaming information. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_TIMING" class="codetype"><span class="SCst">XAUDIO2_LOG_TIMING</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Delays detected and other timing data. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_WARNINGS" class="codetype"><span class="SCst">XAUDIO2_LOG_WARNINGS</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>For handled errors that may be recoverable. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOOP_INFINITE" class="codetype"><span class="SCst">XAUDIO2_LOOP_INFINITE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Used in XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_AUDIO_CHANNELS" class="codetype"><span class="SCst">XAUDIO2_MAX_AUDIO_CHANNELS</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Maximum channels in an audio stream. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_BUFFERS_SYSTEM" class="codetype"><span class="SCst">XAUDIO2_MAX_BUFFERS_SYSTEM</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Maximum buffers allowed for system threads (Xbox 360 only). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_BUFFER_BYTES" class="codetype"><span class="SCst">XAUDIO2_MAX_BUFFER_BYTES</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Maximum bytes allowed in a source buffer. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FILTER_FREQUENCY" class="codetype"><span class="SCst">XAUDIO2_MAX_FILTER_FREQUENCY</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td>Maximum XAUDIO2_FILTER_PARAMETERS.Frequency. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FILTER_ONEOVERQ" class="codetype"><span class="SCst">XAUDIO2_MAX_FILTER_ONEOVERQ</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td>Maximum XAUDIO2_FILTER_PARAMETERS.OneOverQ. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FREQ_RATIO" class="codetype"><span class="SCst">XAUDIO2_MAX_FREQ_RATIO</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td>Maximum MaxFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_INSTANCES" class="codetype"><span class="SCst">XAUDIO2_MAX_INSTANCES</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Maximum simultaneous XAudio2 objects on Xbox 360. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_LOOP_COUNT" class="codetype"><span class="SCst">XAUDIO2_MAX_LOOP_COUNT</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Maximum non-infinite XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_QUEUED_BUFFERS" class="codetype"><span class="SCst">XAUDIO2_MAX_QUEUED_BUFFERS</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Maximum buffers allowed in a voice queue. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_SAMPLE_RATE" class="codetype"><span class="SCst">XAUDIO2_MAX_SAMPLE_RATE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Maximum audio sample rate supported. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_VOLUME_LEVEL" class="codetype"><span class="SCst">XAUDIO2_MAX_VOLUME_LEVEL</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td>Maximum acceptable volume level (2^24). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MIN_FREQ_RATIO" class="codetype"><span class="SCst">XAUDIO2_MIN_FREQ_RATIO</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td>Minimum SetFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MIN_SAMPLE_RATE" class="codetype"><span class="SCst">XAUDIO2_MIN_SAMPLE_RATE</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Minimum audio sample rate supported. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_NO_LOOP_REGION" class="codetype"><span class="SCst">XAUDIO2_NO_LOOP_REGION</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td>Used in XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT" class="codetype"><span class="SCst">XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in CreateMasteringVoice to create a virtual audio client. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_PLAY_TAILS" class="codetype"><span class="SCst">XAUDIO2_PLAY_TAILS</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in IXAudio2SourceVoice::Stop. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_SEND_USEFILTER" class="codetype"><span class="SCst">XAUDIO2_SEND_USEFILTER</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in XAUDIO2_SEND_DESCRIPTOR.Flags. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_STOP_ENGINE_WHEN_IDLE" class="codetype"><span class="SCst">XAUDIO2_STOP_ENGINE_WHEN_IDLE</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in XAudio2Create to force the engine to Stop when no source voices are Started, and Start when a voice is Started. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOPITCH" class="codetype"><span class="SCst">XAUDIO2_VOICE_NOPITCH</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in IXAudio2::CreateSourceVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOSAMPLESPLAYED" class="codetype"><span class="SCst">XAUDIO2_VOICE_NOSAMPLESPLAYED</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in IXAudio2SourceVoice::GetState. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOSRC" class="codetype"><span class="SCst">XAUDIO2_VOICE_NOSRC</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Used in IXAudio2::CreateSourceVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_USEFILTER" class="codetype"><span class="SCst">XAUDIO2_VOICE_USEFILTER</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
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
<td id="XAudio2_FLOAT32" class="codetype"><span class="SCst">FLOAT32</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_UINT32" class="codetype"><span class="SCst">UINT32</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_UINT64" class="codetype"><span class="SCst">UINT64</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_HANDLE" class="codetype"><span class="SCst">X3DAUDIO_HANDLE</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">20</span>] <span class="SCst">Win32</span>.<span class="SCst">BYTE</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_PROCESSOR" class="codetype"><span class="SCst">XAUDIO2_PROCESSOR</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span></span></td>
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
<td class="codetype"><span class="SCst">AudioCategory_Other</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_ForegroundOnlyMedia</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_BackgroundCapableMedia</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_Communications</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_Alerts</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_SoundEffects</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_GameEffects</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_GameMedia</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_GameChat</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_Speech</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_Movie</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AudioCategory_Media</span></td>
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
<td class="codetype"><span class="SCde">vtbl</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Itf">IXAudio2Itf</a></span></span></td>
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
<td class="codetype"><span class="SCde">vtbl</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallbackItf">IXAudio2EngineCallbackItf</a></span></span></td>
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
<td class="codetype"><span class="SCde">onProcessingPassStart</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">onProcessingPassEnd</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">onCriticalError</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>, <span class="SCst">Win32</span>.<span class="SCst">HRESULT</span>)</span></td>
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
<td class="codetype"><span class="SCde">queryInterface</span></td>
<td class="codetype"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">addRef</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">ULONG</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">release</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">ULONG</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">registerForCallbacks</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">unregisterForCallbacks</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">createSourceVoice</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, **<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_WAVEFORMATEX">WAVEFORMATEX</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="STpe">f32</span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">createSubmixVoice</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, **<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SubmixVoice">IXAudio2SubmixVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">createMasteringVoice</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, **<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2MasteringVoice">IXAudio2MasteringVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">Win32</span>.<span class="SCst">LPCWSTR</span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_AUDIO_STREAM_CATEGORY">AUDIO_STREAM_CATEGORY</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">startEngine</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">stopEngine</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">commitChanges</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getPerformanceData</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_PERFORMANCE_DATA">XAUDIO2_PERFORMANCE_DATA</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setDebugConfiguration</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_DEBUG_CONFIGURATION">XAUDIO2_DEBUG_CONFIGURATION</a></span>, *<span class="STpe">void</span>)</span></td>
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
<td class="codetype"><span class="SCde">vtbl</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2MasteringVoiceItf">IXAudio2MasteringVoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SCde"><span class="SKwd">using</span> voice</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getChannelMask</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2MasteringVoice">IXAudio2MasteringVoice</a></span>, *<span class="SCst">Win32</span>.<span class="SCst">DWORD</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
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
<td class="codetype"><span class="SCde">vtbl</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoiceItf">IXAudio2SourceVoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SCde"><span class="SKwd">using</span> voice</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">start</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">stop</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">submitSourceBuffer</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_BUFFER">XAUDIO2_BUFFER</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_BUFFER_WMA">XAUDIO2_BUFFER_WMA</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">glushSourceBuffers</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">discontinuity</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">exitLoop</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getState</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_STATE">XAUDIO2_VOICE_STATE</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setFrequencyRatio</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="STpe">f32</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getFrequencyRatio</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, *<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setSourceSampleRate</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
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
<td class="codetype"><span class="SCde">vtbl</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SubmixVoiceItf">IXAudio2SubmixVoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SCde"><span class="SKwd">using</span> voice</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SCde">vtbl</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
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
<td class="codetype"><span class="SCde">vtbl</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallbackItf">IXAudio2VoiceCallbackItf</a></span></span></td>
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
<td class="codetype"><span class="SCde">onVoiceProcessingPassStart</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">onVoiceProcessingPassEnd</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">onStreamEnd</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">onBufferStart</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="STpe">void</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">onBufferEnd</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="STpe">void</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">onLoopEnd</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="STpe">void</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">onVoiceError</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="STpe">void</span>, <span class="SCst">Win32</span>.<span class="SCst">HRESULT</span>)</span></td>
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
<td class="codetype"><span class="SCde">getVoiceDetails</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_DETAILS">XAUDIO2_VOICE_DETAILS</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setOutputVoices</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setEffectChain</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">enableEffect</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">disableEffect</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getEffectState</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="SCst">Win32</span>.<span class="SCst">BOOL</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setEffectParameters</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SKwd">const</span> *<span class="STpe">void</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getEffectParameters</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="STpe">void</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setFilterParameters</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getFilterParameters</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setOutputFilterParameters</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getOutputFilterParameters</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setVolume</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="STpe">f32</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getVolume</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setChannelVolumes</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SKwd">const</span> *<span class="STpe">f32</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getChannelVolumes</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">setOutputMatrix</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SKwd">const</span> *<span class="STpe">f32</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">getOutputMatrix</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">destroyVoice</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>)</span></td>
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
<td class="codetype"><span class="SCde">wFormatTag</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nChannels</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nSamplesPerSec</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nAvgBytesPerSec</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nBlockAlign</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wBitsPerSample</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cbSize</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
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
<td class="codetype"><span class="SCde">format</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_WAVEFORMATEX">WAVEFORMATEX</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">samples</span></td>
<td class="codetype"><span class="SCde">{wValidBitsPerSample: <span class="SCst">Win32</span>.<span class="SCst">WORD</span>, wSamplesPerBlock: <span class="SCst">Win32</span>.<span class="SCst">WORD</span>, wReserved: <span class="SCst">Win32</span>.<span class="SCst">WORD</span>}</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwChannelMask</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">subFormat</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">GUID</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">X3DAudioInitialize</span>(speakerChannelMask: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, speedOfSound: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_FLOAT32">FLOAT32</a></span>, instance: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_X3DAUDIO_HANDLE">X3DAUDIO_HANDLE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></code>
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
<td class="codetype"><span class="SCde">flags</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Either 0 or XAUDIO2_END_OF_STREAM. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">audioBytes</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Size of the audio data buffer in bytes. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">pAudioData</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst">BYTE</span></span></td>
<td>Pointer to the audio data buffer. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">playBegin</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>First sample in this buffer to be played. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">playLength</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Length of the region to be played in samples, or 0 to play the whole buffer. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">loopBegin</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>First sample of the region to be looped. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">loopLength</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Length of the desired loop region in samples, or 0 to loop the entire buffer. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">loopCount</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Number of times to repeat the loop region, or XAUDIO2_LOOP_INFINITE to loop forever. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">pContext</span></td>
<td class="codetype"><span class="SCde">*<span class="STpe">void</span></span></td>
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
<td class="codetype"><span class="SCde">pDecodedPacketCumulativeBytes</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">PacketCount</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
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
<td class="codetype"><span class="SCde">traceMask</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">breakMask</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">logThreadID</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">logFileline</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">logFunctionName</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">logTiming</span></td>
<td class="codetype"><span class="STpe">s32</span></td>
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
<td class="codetype"><span class="SCde">type</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_TYPE">XAUDIO2_FILTER_TYPE</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">frequency</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">oneOverQ</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
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
<td class="codetype"><span class="SCst">LowPassFilter</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">BandPassFilter</span></td>
<td>Attenuates frequencies above the cutoff frequency (state-variable filter). </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">HighPassFilter</span></td>
<td>Attenuates frequencies outside a given range      (state-variable filter). </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">NotchFilter</span></td>
<td>Attenuates frequencies below the cutoff frequency (state-variable filter). </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">LowPassOnePoleFilter</span></td>
<td>Attenuates frequencies inside a given range       (state-variable filter). </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">HighPassOnePoleFilter</span></td>
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
<td class="codetype"><span class="SCst">AudioCyclesSinceLastQuery</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">TotalCyclesSinceLastQuery</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">MinimumCyclesPerQuantum</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">MaximumCyclesPerQuantum</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">MemoryUsageInBytes</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">CurrentLatencyInSamples</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">GlitchesSinceEngineStarted</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">ActiveSourceVoiceCount</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">TotalSourceVoiceCount</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">ActiveSubmixVoiceCount</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">ActiveResamplerCount</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">ActiveMatrixMixCount</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">ActiveXmaSourceVoices</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">ActiveXmaStreams</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
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
<td class="codetype"><span class="SCde">flags</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">pOutputVoice</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span></span></td>
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
<td class="codetype"><span class="SCde">creationFlags</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">activeFlags</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">inputChannels</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">inputSampleRate</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
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
<td class="codetype"><span class="SCde">sendCount</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">pSends</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_SEND_DESCRIPTOR">XAUDIO2_SEND_DESCRIPTOR</a></span></span></td>
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
<td class="codetype"><span class="SCde">pCurrentBufferContext</span></td>
<td class="codetype"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">buffersQueued</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">samplesPlayed</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">XAudio2Create</span>(ppXAudio2: **<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, flags: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, processor: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_PROCESSOR">XAUDIO2_PROCESSOR</a></span> = <span class="SCst"><a href="#XAudio2_XAUDIO2_ANY_PROCESSOR">XAUDIO2_ANY_PROCESSOR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></code>
</div>
</div>
</div>
</div>
</body>
</html>
