<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module xaudio2</title>
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
        
        .container a        { color:           DoggerBlue; }
        .container a:hover  { text-decoration: underline; }
        .container img      { margin:          0 auto; }
        
        .left a     { text-decoration: none; }
        .left ul    { list-style-type: none; margin-left: -20px; }
        .left h3    { background-color: Black; color: White; padding: 6px; }
        .right h1   { margin-top: 50px; margin-bottom: 50px; }
        .right h2   { margin-top: 35px; }
        
        .strikethrough-text     { text-decoration: line-through; }
        .swag-watermak          { text-align:right; font-size: 80%; }
        .swag-watermak a        { text-decoration: none; color: inherit; }
        
        .blockquote {
            border-radius:      5px;
            border:             1px solid;
            margin:             20px;
            margin-left:        20px;
            margin-right:       20px;
            padding:            10px;
        }
        .blockquote-default     { border-color: Orange; border-left: 6px solid Orange; background-color: LightYellow; }
        .blockquote-note        { border-color: #ADCEDD; background-color: #CDEEFD; }
        .blockquote-tip         { border-color: #BCCFBC; background-color: #DCEFDC; }
        .blockquote-warning     { border-color: #DFBDB3; background-color: #FFDDD3; }
        .blockquote-attention   { border-color: #DDBAB8; background-color: #FDDAD8; }
        .blockquote-example     { border: 2px solid LightGrey; }
        .blockquote-title-block { margin-bottom: 10px; }
        .blockquote-title       { font-weight: bold; }
        .blockquote-default     p:first-child { margin-top: 0px; }
        .blockquote-default     p:last-child  { margin-bottom: 0px; }
        .blockquote             p:last-child  { margin-bottom: 0px; }
        
        .description-list-title   { font-weight: bold; font-style: italic; }
        .description-list-block   { margin-left: 30px; }
        
        table.api-item            { border-collapse: separate; background-color: Black; color: White; width: 100%; margin-top: 70px; margin-right: 0px; font-size: 110%; }
        .api-item td:first-child  { width: 33%; white-space: nowrap; }
        .api-item-title-src-ref   { text-align:  right; }
        .api-item-title-src-ref a { color:       inherit; }
        .api-item-title-kind      { font-weight: normal; font-size: 80%; }
        .api-item-title-light     { font-weight: normal; }
        .api-item-title-strong    { font-weight: bold; font-size: 100%; }
        .api-additional-infos     { font-size: 90%; white-space: break-spaces; overflow-wrap: break-word; }
        
        table.table-enumeration           { border: 1px solid LightGrey; border-collapse: collapse; width: calc(100% - 40px); font-size: 90%; margin-left: 20px; margin-right: 20px; }
        .table-enumeration td             { border: 1px solid LightGrey; border-collapse: collapse; padding: 6px; min-width: 100px; }
        .table-enumeration td:first-child { background-color: #f8f8f8; white-space: nowrap; }
        .table-enumeration td:last-child  { width: 100%; }
        .table-enumeration td.code-type   { background-color: #eeeeee; }
        .table-enumeration a              { text-decoration: none; color: inherit; }
        
        .code-inline            { font-size: 110%; font-family: monospace; display: inline-block; background-color: #eeeeee; padding: 0px 8px; border-radius: 5px; border: 1px dotted #cccccc; }
        .code-block {
            background-color:   #eeeeee;
            border-radius:      5px;
            border:             1px solid LightGrey;
            padding:            10px;
            margin:             20px;
            white-space:        pre;
            overflow-x:         auto;
        }
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
<div class="right-page">
<h1>Module xaudio2</h1>
<p> Wrapper for Microsoft XAudio2 library. </p>
<h1>Content</h1>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_FORMAT_EXTENSIBLE"><span class="api-item-title-kind">const</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">Constants</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L308" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td id="XAudio2_FORMAT_EXTENSIBLE">FORMAT_EXTENSIBLE</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor1">Processor1</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor10">Processor10</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor11">Processor11</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor12">Processor12</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor13">Processor13</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor14">Processor14</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor15">Processor15</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor16">Processor16</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor17">Processor17</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor18">Processor18</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor19">Processor19</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor2">Processor2</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor20">Processor20</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor21">Processor21</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor22">Processor22</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor23">Processor23</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor24">Processor24</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor25">Processor25</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor26">Processor26</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor27">Processor27</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor28">Processor28</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor29">Processor29</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor3">Processor3</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor30">Processor30</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor31">Processor31</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor32">Processor32</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor4">Processor4</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor5">Processor5</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor6">Processor6</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor7">Processor7</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor8">Processor8</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_Processor9">Processor9</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_HANDLE_BYTESIZE">X3DAUDIO_HANDLE_BYTESIZE</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_SPEED_OF_SOUND">X3DAUDIO_SPEED_OF_SOUND</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_1024_QUANTUM">XAUDIO2_1024_QUANTUM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in XAudio2Create to specify nondefault processing quantum of 21.33 ms (1024 samples at 48KHz). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_ANY_PROCESSOR">XAUDIO2_ANY_PROCESSOR</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_COMMIT_ALL">XAUDIO2_COMMIT_ALL</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Used in IXAudio2::CommitChanges. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_COMMIT_NOW">XAUDIO2_COMMIT_NOW</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Numeric values with special meanings Used as an OperationSet argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEBUG_ENGINE">XAUDIO2_DEBUG_ENGINE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in XAudio2Create. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_CHANNELS">XAUDIO2_DEFAULT_CHANNELS</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Used in CreateMasteringVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_FREQ_RATIO">XAUDIO2_DEFAULT_FREQ_RATIO</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td>Default MaxFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_DEFAULT_SAMPLERATE">XAUDIO2_DEFAULT_SAMPLERATE</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Used in CreateMasteringVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_END_OF_STREAM">XAUDIO2_END_OF_STREAM</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in XAUDIO2_BUFFER.Flags. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_INVALID_OPSET">XAUDIO2_INVALID_OPSET</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Not allowed for OperationSet arguments. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_API_CALLS">XAUDIO2_LOG_API_CALLS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Public API function entries and exits. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_DETAIL">XAUDIO2_LOG_DETAIL</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>More detailed chit-chat. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_ERRORS">XAUDIO2_LOG_ERRORS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Values for the TraceMask and BreakMask bitmaps.  Only ERRORS and WARNINGS are valid in BreakMask.  WARNINGS implies ERRORS, DETAIL implies INFO, and FUNC_CALLS implies API_CALLS.  By default, TraceMask is ERRORS and WARNINGS and all the other settings are zero. For handled errors with serious effects. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_FUNC_CALLS">XAUDIO2_LOG_FUNC_CALLS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Internal function entries and exits. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_INFO">XAUDIO2_LOG_INFO</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Informational chit-chat (e.g. state changes). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_LOCKS">XAUDIO2_LOG_LOCKS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Usage of critical sections and mutexes. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_MEMORY">XAUDIO2_LOG_MEMORY</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Memory heap usage information. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_STREAMING">XAUDIO2_LOG_STREAMING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Audio streaming information. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_TIMING">XAUDIO2_LOG_TIMING</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Delays detected and other timing data. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOG_WARNINGS">XAUDIO2_LOG_WARNINGS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>For handled errors that may be recoverable. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_LOOP_INFINITE">XAUDIO2_LOOP_INFINITE</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Used in XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_AUDIO_CHANNELS">XAUDIO2_MAX_AUDIO_CHANNELS</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Maximum channels in an audio stream. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_BUFFERS_SYSTEM">XAUDIO2_MAX_BUFFERS_SYSTEM</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Maximum buffers allowed for system threads (Xbox 360 only). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_BUFFER_BYTES">XAUDIO2_MAX_BUFFER_BYTES</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Maximum bytes allowed in a source buffer. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FILTER_FREQUENCY">XAUDIO2_MAX_FILTER_FREQUENCY</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td>Maximum XAUDIO2_FILTER_PARAMETERS.Frequency. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FILTER_ONEOVERQ">XAUDIO2_MAX_FILTER_ONEOVERQ</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td>Maximum XAUDIO2_FILTER_PARAMETERS.OneOverQ. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_FREQ_RATIO">XAUDIO2_MAX_FREQ_RATIO</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td>Maximum MaxFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_INSTANCES">XAUDIO2_MAX_INSTANCES</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Maximum simultaneous XAudio2 objects on Xbox 360. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_LOOP_COUNT">XAUDIO2_MAX_LOOP_COUNT</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Maximum non-infinite XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_QUEUED_BUFFERS">XAUDIO2_MAX_QUEUED_BUFFERS</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Maximum buffers allowed in a voice queue. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_SAMPLE_RATE">XAUDIO2_MAX_SAMPLE_RATE</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Maximum audio sample rate supported. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MAX_VOLUME_LEVEL">XAUDIO2_MAX_VOLUME_LEVEL</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td>Maximum acceptable volume level (2^24). </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MIN_FREQ_RATIO">XAUDIO2_MIN_FREQ_RATIO</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td>Minimum SetFrequencyRatio argument. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_MIN_SAMPLE_RATE">XAUDIO2_MIN_SAMPLE_RATE</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Minimum audio sample rate supported. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_NO_LOOP_REGION">XAUDIO2_NO_LOOP_REGION</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td>Used in XAUDIO2_BUFFER.LoopCount. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT">XAUDIO2_NO_VIRTUAL_AUDIO_CLIENT</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in CreateMasteringVoice to create a virtual audio client. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_PLAY_TAILS">XAUDIO2_PLAY_TAILS</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in IXAudio2SourceVoice::Stop. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_SEND_USEFILTER">XAUDIO2_SEND_USEFILTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in XAUDIO2_SEND_DESCRIPTOR.Flags. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_STOP_ENGINE_WHEN_IDLE">XAUDIO2_STOP_ENGINE_WHEN_IDLE</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in XAudio2Create to force the engine to Stop when no source voices are Started, and Start when a voice is Started. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOPITCH">XAUDIO2_VOICE_NOPITCH</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in IXAudio2::CreateSourceVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOSAMPLESPLAYED">XAUDIO2_VOICE_NOSAMPLESPLAYED</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in IXAudio2SourceVoice::GetState. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_NOSRC">XAUDIO2_VOICE_NOSRC</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in IXAudio2::CreateSourceVoice. </td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_VOICE_USEFILTER">XAUDIO2_VOICE_USEFILTER</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Used in IXAudio2::CreateSource/SubmixVoice. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_FLOAT32"><span class="api-item-title-kind">type alias</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">Type Aliases</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td id="XAudio2_FLOAT32">FLOAT32</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_UINT32">UINT32</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_UINT64">UINT64</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_X3DAUDIO_HANDLE">X3DAUDIO_HANDLE</td>
<td class="code-type"><span class="SCde">[<span class="SNum">20</span>] <span class="SCst">Win32</span>.<span class="SCst">BYTE</span></span></td>
<td></td>
</tr>
<tr>
<td id="XAudio2_XAUDIO2_PROCESSOR">XAUDIO2_PROCESSOR</td>
<td class="code-type"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_AUDIO_STREAM_CATEGORY"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">AUDIO_STREAM_CATEGORY</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L133" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
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
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L216" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>vtbl</td>
<td class="code-type"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Itf">IXAudio2Itf</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2EngineCallback"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2EngineCallback</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L220" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>vtbl</td>
<td class="code-type"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallbackItf">IXAudio2EngineCallbackItf</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2EngineCallbackItf"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2EngineCallbackItf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L224" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>onProcessingPassStart</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>onProcessingPassEnd</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>onCriticalError</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>, <span class="SCst">Win32</span>.<span class="SCst">HRESULT</span>)</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2Itf"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2Itf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L242" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>queryInterface</td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td>addRef</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">ULONG</span></span></td>
<td></td>
</tr>
<tr>
<td>release</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">ULONG</span></span></td>
<td></td>
</tr>
<tr>
<td>registerForCallbacks</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>unregisterForCallbacks</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2EngineCallback">IXAudio2EngineCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>createSourceVoice</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, **<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_WAVEFORMATEX">WAVEFORMATEX</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="STpe">f32</span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>createSubmixVoice</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, **<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SubmixVoice">IXAudio2SubmixVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>createMasteringVoice</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, **<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2MasteringVoice">IXAudio2MasteringVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">Win32</span>.<span class="SCst">LPCWSTR</span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_AUDIO_STREAM_CATEGORY">AUDIO_STREAM_CATEGORY</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>startEngine</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>stopEngine</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>commitChanges</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getPerformanceData</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_PERFORMANCE_DATA">XAUDIO2_PERFORMANCE_DATA</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>setDebugConfiguration</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_DEBUG_CONFIGURATION">XAUDIO2_DEBUG_CONFIGURATION</a></span>, *<span class="STpe">void</span>)</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2MasteringVoice"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2MasteringVoice</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L218" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>vtbl</td>
<td class="code-type"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2MasteringVoiceItf">IXAudio2MasteringVoiceItf</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2MasteringVoiceItf"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2MasteringVoiceItf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L282" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>using voice</td>
<td class="code-type"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
<td></td>
</tr>
<tr>
<td>getChannelMask</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2MasteringVoice">IXAudio2MasteringVoice</a></span>, *<span class="SCst">Win32</span>.<span class="SCst">DWORD</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2SourceVoice"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2SourceVoice</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L219" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>vtbl</td>
<td class="code-type"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoiceItf">IXAudio2SourceVoiceItf</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2SourceVoiceItf"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2SourceVoiceItf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L293" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>using voice</td>
<td class="code-type"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
<td></td>
</tr>
<tr>
<td>start</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>stop</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>submitSourceBuffer</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_BUFFER">XAUDIO2_BUFFER</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_BUFFER_WMA">XAUDIO2_BUFFER_WMA</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>glushSourceBuffers</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>discontinuity</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>exitLoop</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getState</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_STATE">XAUDIO2_VOICE_STATE</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>setFrequencyRatio</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="STpe">f32</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getFrequencyRatio</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, *<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td>setSourceSampleRate</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SourceVoice">IXAudio2SourceVoice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2SubmixVoice"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2SubmixVoice</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L222" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>vtbl</td>
<td class="code-type"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2SubmixVoiceItf">IXAudio2SubmixVoiceItf</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2SubmixVoiceItf"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2SubmixVoiceItf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L288" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>using voice</td>
<td class="code-type"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2Voice"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2Voice</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L217" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>vtbl</td>
<td class="code-type"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceItf">IXAudio2VoiceItf</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2VoiceCallback"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2VoiceCallback</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L221" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>vtbl</td>
<td class="code-type"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallbackItf">IXAudio2VoiceCallbackItf</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2VoiceCallbackItf"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2VoiceCallbackItf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L231" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>onVoiceProcessingPassStart</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>onVoiceProcessingPassEnd</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>onStreamEnd</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>onBufferStart</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="STpe">void</span>)</span></td>
<td></td>
</tr>
<tr>
<td>onBufferEnd</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="STpe">void</span>)</span></td>
<td></td>
</tr>
<tr>
<td>onLoopEnd</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="STpe">void</span>)</span></td>
<td></td>
</tr>
<tr>
<td>onVoiceError</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2VoiceCallback">IXAudio2VoiceCallback</a></span>, *<span class="STpe">void</span>, <span class="SCst">Win32</span>.<span class="SCst">HRESULT</span>)</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_IXAudio2VoiceItf"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">IXAudio2VoiceItf</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L259" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>getVoiceDetails</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_DETAILS">XAUDIO2_VOICE_DETAILS</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>setOutputVoices</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_VOICE_SENDS">XAUDIO2_VOICE_SENDS</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>setEffectChain</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_EFFECT_CHAIN">XAUDIO2_EFFECT_CHAIN</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>enableEffect</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>disableEffect</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getEffectState</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="SCst">Win32</span>.<span class="SCst">BOOL</span>)</span></td>
<td></td>
</tr>
<tr>
<td>setEffectParameters</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SKwd">const</span> *<span class="STpe">void</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getEffectParameters</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="STpe">void</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>setFilterParameters</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getFilterParameters</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>setOutputFilterParameters</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getOutputFilterParameters</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_PARAMETERS">XAUDIO2_FILTER_PARAMETERS</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td>setVolume</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="STpe">f32</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getVolume</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td>setChannelVolumes</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SKwd">const</span> *<span class="STpe">f32</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getChannelVolumes</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td>setOutputMatrix</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SKwd">const</span> *<span class="STpe">f32</span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></td>
<td></td>
</tr>
<tr>
<td>getOutputMatrix</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, *<span class="STpe">f32</span>)</span></td>
<td></td>
</tr>
<tr>
<td>destroyVoice</td>
<td class="code-type"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span>)</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_WAVEFORMATEX"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">WAVEFORMATEX</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L311" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>wFormatTag</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td>nChannels</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td>nSamplesPerSec</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>nAvgBytesPerSec</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>nBlockAlign</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td>wBitsPerSample</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
<tr>
<td>cbSize</td>
<td class="code-type"><span class="STpe">u16</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_WAVEFORMATEXTENSIBLE"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">WAVEFORMATEXTENSIBLE</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L322" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>format</td>
<td class="code-type"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_WAVEFORMATEX">WAVEFORMATEX</a></span></span></td>
<td></td>
</tr>
<tr>
<td>samples</td>
<td class="code-type"><span class="SCde">{wValidBitsPerSample: <span class="SCst">Win32</span>.<span class="SCst">WORD</span>, wSamplesPerBlock: <span class="SCst">Win32</span>.<span class="SCst">WORD</span>, wReserved: <span class="SCst">Win32</span>.<span class="SCst">WORD</span>}</span></td>
<td></td>
</tr>
<tr>
<td>dwChannelMask</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>subFormat</td>
<td class="code-type"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst">GUID</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_X3DAudioInitialize"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">X3DAudioInitialize</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L340" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">X3DAudioInitialize</span>(speakerChannelMask: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, speedOfSound: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_FLOAT32">FLOAT32</a></span>, instance: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_X3DAUDIO_HANDLE">X3DAUDIO_HANDLE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></code>
</div>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_BUFFER"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_BUFFER</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L172" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>flags</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Either 0 or XAUDIO2_END_OF_STREAM. </td>
</tr>
<tr>
<td>audioBytes</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Size of the audio data buffer in bytes. </td>
</tr>
<tr>
<td>pAudioData</td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst">BYTE</span></span></td>
<td>Pointer to the audio data buffer. </td>
</tr>
<tr>
<td>playBegin</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>First sample in this buffer to be played. </td>
</tr>
<tr>
<td>playLength</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Length of the region to be played in samples, or 0 to play the whole buffer. </td>
</tr>
<tr>
<td>loopBegin</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>First sample of the region to be looped. </td>
</tr>
<tr>
<td>loopLength</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Length of the desired loop region in samples, or 0 to loop the entire buffer. </td>
</tr>
<tr>
<td>loopCount</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td>Number of times to repeat the loop region, or XAUDIO2_LOOP_INFINITE to loop forever. </td>
</tr>
<tr>
<td>pContext</td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td>Context value to be passed back in callbacks. </td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_BUFFER_WMA"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_BUFFER_WMA</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L185" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>pDecodedPacketCumulativeBytes</td>
<td class="code-type"><span class="SCde"><span class="SKwd">const</span> *<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span></span></td>
<td></td>
</tr>
<tr>
<td>PacketCount</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_DEBUG_CONFIGURATION"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_DEBUG_CONFIGURATION</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L123" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>traceMask</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>breakMask</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>logThreadID</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>logFileline</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>logFunctionName</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
<tr>
<td>logTiming</td>
<td class="code-type"><span class="STpe">s32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_EFFECT_CHAIN"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_EFFECT_CHAIN</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L101" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_FILTER_PARAMETERS"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_FILTER_PARAMETERS</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L209" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>type</td>
<td class="code-type"><span class="SCde"><span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_FILTER_TYPE">XAUDIO2_FILTER_TYPE</a></span></span></td>
<td></td>
</tr>
<tr>
<td>frequency</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
<tr>
<td>oneOverQ</td>
<td class="code-type"><span class="STpe">f32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_FILTER_TYPE"><span class="api-item-title-kind">enum</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_FILTER_TYPE</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L199" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Used in XAUDIO2_FILTER_PARAMETERS below. </p>
<table class="table-enumeration">
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
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_PERFORMANCE_DATA"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_PERFORMANCE_DATA</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L105" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>AudioCyclesSinceLastQuery</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td>TotalCyclesSinceLastQuery</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
<tr>
<td>MinimumCyclesPerQuantum</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>MaximumCyclesPerQuantum</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>MemoryUsageInBytes</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>CurrentLatencyInSamples</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>GlitchesSinceEngineStarted</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>ActiveSourceVoiceCount</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>TotalSourceVoiceCount</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>ActiveSubmixVoiceCount</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>ActiveResamplerCount</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>ActiveMatrixMixCount</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>ActiveXmaSourceVoices</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>ActiveXmaStreams</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_SEND_DESCRIPTOR"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_SEND_DESCRIPTOR</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L158" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>flags</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>pOutputVoice</td>
<td class="code-type"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2Voice">IXAudio2Voice</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_VOICE_DETAILS"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_VOICE_DETAILS</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>creationFlags</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>activeFlags</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>inputChannels</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>inputSampleRate</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_VOICE_SENDS"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_VOICE_SENDS</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L165" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>sendCount</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>pSends</td>
<td class="code-type"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_SEND_DESCRIPTOR">XAUDIO2_SEND_DESCRIPTOR</a></span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAUDIO2_VOICE_STATE"><span class="api-item-title-kind">struct</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAUDIO2_VOICE_STATE</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L191" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="table-enumeration">
<tr>
<td>pCurrentBufferContext</td>
<td class="code-type"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td>buffersQueued</td>
<td class="code-type"><span class="STpe">u32</span></td>
<td></td>
</tr>
<tr>
<td>samplesPlayed</td>
<td class="code-type"><span class="STpe">u64</span></td>
<td></td>
</tr>
</table>
<p>
<table class="api-item">
<tr>
<td class="api-item">
<span id="XAudio2_XAudio2Create"><span class="api-item-title-kind">func</span> <span class="api-item-title-light">XAudio2.</span><span class="api-item-title-strong">XAudio2Create</span></span>
</td>
<td class="api-item-title-src-ref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/xaudio2\src\xaudio2.swg#L339" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="code-block"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">XAudio2Create</span>(ppXAudio2: **<span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_IXAudio2">IXAudio2</a></span>, flags: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_UINT32">UINT32</a></span>, processor: <span class="SCst">XAudio2</span>.<span class="SCst"><a href="#XAudio2_XAUDIO2_PROCESSOR">XAUDIO2_PROCESSOR</a></span> = <span class="SCst"><a href="#XAudio2_XAUDIO2_ANY_PROCESSOR">XAUDIO2_ANY_PROCESSOR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst">HRESULT</span></span></code>
</div>
<div class="swag-watermak">
Generated on 03-09-2023 07-49-27 with <a href="https://swag-lang.org/index.php">swag</a> 0.24.0</div>
</div>
</div>
</div>
</body>
</html>
