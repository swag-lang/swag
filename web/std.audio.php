<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module audio</title>
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
<li><a href="#Audio_Bus">Bus</a></li>
<li><a href="#Audio_Voice">Voice</a></li>
</ul>
<h4>codec</h4>
<ul>
<li><a href="#Audio_Codec">Codec</a></li>
</ul>
<h4>file</h4>
<ul>
<li><a href="#Audio_SoundFile">SoundFile</a></li>
</ul>
<h3>Interfaces</h3>
<h4>codec</h4>
<ul>
<li><a href="#Audio_ICodec">ICodec</a></li>
</ul>
<h3>Enums</h3>
<h4></h4>
<ul>
<li><a href="#Audio_VoiceCreateFlags">VoiceCreateFlags</a></li>
<li><a href="#Audio_VoicePlayFlags">VoicePlayFlags</a></li>
<li><a href="#Audio_VoiceState">VoiceState</a></li>
</ul>
<h4>file</h4>
<ul>
<li><a href="#Audio_SoundFileEncoding">SoundFileEncoding</a></li>
<li><a href="#Audio_SoundFileValidityMask">SoundFileValidityMask</a></li>
</ul>
<h3>Type Aliases</h3>
<h4>driver</h4>
<ul>
<li><a href="#Audio_BusHandle">BusHandle</a></li>
<li><a href="#Audio_VoiceHandle">VoiceHandle</a></li>
</ul>
<h3>Functions</h3>
<h4></h4>
<ul>
<li><a href="#Audio_convertDBToPercent">Audio.convertDBToPercent</a></li>
<li><a href="#Audio_convertPercentToDB">Audio.convertPercentToDB</a></li>
<li><a href="#Audio_createEngine">Audio.createEngine</a></li>
<li><a href="#Audio_destroyEngine">Audio.destroyEngine</a></li>
<li><a href="#Audio_getOutputVolume">Audio.getOutputVolume</a></li>
<li><a href="#Audio_setOutputVolume">Audio.setOutputVolume</a></li>
<li><a href="#Audio_Bus_create">Bus.create</a></li>
<li><a href="#Audio_Bus_destroy">Bus.destroy</a></li>
<li><a href="#Audio_Bus_getVolumeDB">Bus.getVolumeDB</a></li>
<li><a href="#Audio_Bus_setVolumeDB">Bus.setVolumeDB</a></li>
<li><a href="#Audio_Voice_create">Voice.create</a></li>
<li><a href="#Audio_Voice_destroy">Voice.destroy</a></li>
<li><a href="#Audio_Voice_getVolumeDB">Voice.getVolumeDB</a></li>
<li><a href="#Audio_Voice_isPlaying">Voice.isPlaying</a></li>
<li><a href="#Audio_Voice_pause">Voice.pause</a></li>
<li><a href="#Audio_Voice_play">Voice.play</a></li>
<li><a href="#Audio_Voice_setVolumeDB">Voice.setVolumeDB</a></li>
<li><a href="#Audio_Voice_stop">Voice.stop</a></li>
</ul>
<h4>codec</h4>
<ul>
<li><a href="#Audio_addCodec">Audio.addCodec</a></li>
</ul>
<h4>driver</h4>
<ul>
<li><a href="#Audio_Bus_getVolume">Bus.getVolume</a></li>
<li><a href="#Audio_Bus_setVolume">Bus.setVolume</a></li>
<li><a href="#Audio_Voice_getVolume">Voice.getVolume</a></li>
<li><a href="#Audio_Voice_setFrequencyRatio">Voice.setFrequencyRatio</a></li>
<li><a href="#Audio_Voice_setRooting">Voice.setRooting</a></li>
<li><a href="#Audio_Voice_setVolume">Voice.setVolume</a></li>
</ul>
<h4>file</h4>
<ul>
<li><a href="#Audio_SoundFile_load">SoundFile.load</a></li>
<li><a href="#Audio_Wav_loadFile">Wav.loadFile</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</blockquote>
<h1>Module audio</h1>
<p><code class="incode">Std.Audio</code> is a module to decode and play sound files. Under windows, it is based on the <code class="incode">xaudio2</code> library. </p>
<h2 id="How_to_play_a_sound">How to play a sound </h2>
<p>First, you have to initialize the audio engine by calling <a href="#Audio_createEngine">Audio.createEngine</a>. </p>
<div class="precode"><code><span class="SCde"><span class="SCst">Audio</span>.<span class="SFct">createEngine</span>()
<span class="SLgc">defer</span> <span class="SCst">Audio</span>.<span class="SFct">destroyEngine</span>() <span class="SCmt">// Don't forget to destroy the engine when you are done</span></span></code>
</div>
<p>You then have to load a sound file. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">let</span> soundFile = <span class="SCst">Audio</span>.<span class="SCst">SoundFile</span>.<span class="SCst">Load</span>(<span class="SStr">"mySound.wav"</span>)</span></code>
</div>
<p>Note that by default, the sound file will load all of its datas in memory. If you want the sound to be loaded only when played, set <code class="incode">loadDatas</code> to false. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">let</span> soundFile = <span class="SCst">Audio</span>.<span class="SCst">SoundFile</span>.<span class="SCst">Load</span>(<span class="SStr">"mySound.wav"</span>, loadDatas = <span class="SKwd">false</span>)</span></code>
</div>
<p>Once you have a sound file, the simplest way to play it is by calling <a href="#Audio_Voice_play">Voice.play</a>. The sound will be played once, until the end, and will be destroyed. </p>
<div class="precode"><code><span class="SCde"><span class="SCst">Voice</span>.<span class="SFct">play</span>(soundFile)</span></code>
</div>
<p>To have more control, you could also use <a href="#Audio_Voice_create">Voice.create</a> then [Voice.Play] on the created sound. That way you will recieve a <a href="#Audio_Voice">Voice</a> object you can play with. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">let</span> voice = <span class="SCst">Voice</span>.<span class="SFct">create</span>(&soundFile)
voice.<span class="SFct">setVolume</span>(<span class="SNum">0.5</span>)
voice.<span class="SFct">play</span>(<span class="SCst">Loop</span>)</span></code>
</div>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_BusHandle"><span class="titletype">type alias</span> <span class="titlelight">Audio.</span><span class="titlestrong">Type Aliases</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\driver\xaudio2.swg#L323" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Audio_BusHandle" class="codetype"><span class="SCst">BusHandle</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst">IXAudio2SubmixVoice</span></span></td>
<td></td>
</tr>
<tr>
<td id="Audio_VoiceHandle" class="codetype"><span class="SCst">VoiceHandle</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst">IXAudio2SourceVoice</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Bus"><span class="titletype">struct</span> <span class="titlelight">Audio.</span><span class="titlestrong">Bus</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\bus.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Represents a bus. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">handle</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">XAudio2</span>.<span class="SCst">IXAudio2SubmixVoice</span></span></td>
<td></td>
</tr>
</table>
<p> A <a href="#Audio_Voice">Voice</a> can be assigned to one or more buses. If you then change some parameters of the bus (like the volume), then all the voices assigned to it will be impacted. </p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Audio_Bus_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Creates an audio bus. </td>
</tr>
<tr>
<td><a href="#Audio_Bus_destroy"><span class="SCde"><span class="SFct">destroy</span>()</spa</a></td>
<td>Destroy the bus (immediatly). </td>
</tr>
<tr>
<td><a href="#Audio_Bus_getVolume"><span class="SCde"><span class="SFct">getVolume</span>()</spa</a></td>
<td>Returns the actual volume. </td>
</tr>
<tr>
<td><a href="#Audio_Bus_getVolumeDB"><span class="SCde"><span class="SFct">getVolumeDB</span>()</spa</a></td>
<td>Returns the actual volume, in DB. </td>
</tr>
<tr>
<td><a href="#Audio_Bus_setVolume"><span class="SCde"><span class="SFct">setVolume</span>()</spa</a></td>
<td>Set the playing bus volume between [0..1]. </td>
</tr>
<tr>
<td><a href="#Audio_Bus_setVolumeDB"><span class="SCde"><span class="SFct">setVolumeDB</span>()</spa</a></td>
<td>Set the playing bus volume in DB. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Bus_create"><span class="titletype">func</span> <span class="titlelight">Bus.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\bus.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates an audio bus. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(numChannels: <span class="STpe">u32</span>, parent: *<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_Bus">Bus</a></span> = <span class="SKwd">null</span>)-&gt;*<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_Bus">Bus</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p> You can then associate a <a href="#Audio_Voice">Voice</a> to that bus with <a href="#Audio_Voice_setRooting">Voice.setRooting</a>  Note that you can have a graph of buses, because a bus can have another bus as <code class="incode">parent</code>. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Bus_destroy"><span class="titletype">func</span> <span class="titlelight">Bus.</span><span class="titlestrong">destroy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\bus.swg#L27" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy the bus (immediatly). </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">destroy</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Bus_getVolume"><span class="titletype">func</span> <span class="titlelight">Bus.</span><span class="titlestrong">getVolume</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\driver\xaudio2.swg#L372" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the actual volume. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getVolume</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Bus_getVolumeDB"><span class="titletype">func</span> <span class="titlelight">Bus.</span><span class="titlestrong">getVolumeDB</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\bus.swg#L43" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the actual volume, in DB. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getVolumeDB</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Bus_setVolume"><span class="titletype">func</span> <span class="titlelight">Bus.</span><span class="titlestrong">setVolume</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\driver\xaudio2.swg#L361" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the playing bus volume between [0..1]. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setVolume</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, volume: <span class="STpe">f32</span>, batchID: <span class="STpe">u32</span> = <span class="SNum">0</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Bus_setVolumeDB"><span class="titletype">func</span> <span class="titlelight">Bus.</span><span class="titlestrong">setVolumeDB</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\bus.swg#L37" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the playing bus volume in DB. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setVolumeDB</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, volumeDB: <span class="STpe">f32</span>, batchID: <span class="STpe">u32</span> = <span class="SNum">0</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Codec"><span class="titletype">struct</span> <span class="titlelight">Audio.</span><span class="titlestrong">Codec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\codec\codec.swg#L48" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Base struct for all codec instances. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">srcEncoding</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFileEncoding">SoundFileEncoding</a></span></span></td>
<td>The original encoding. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dstEncoding</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFileEncoding">SoundFileEncoding</a></span></span></td>
<td>The requested encoding. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">type</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="SCst">Swag</span>.<span class="SCst">TypeInfoStruct</span></span></td>
<td>The real type of the codec. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_ICodec"><span class="titletype">interface</span> <span class="titlelight">Audio.</span><span class="titlestrong">ICodec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\codec\codec.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Interface to describe a codec. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">canEncode</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_ICodec">ICodec</a></span>, <span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFileEncoding">SoundFileEncoding</a></span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">canDecode</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_ICodec">ICodec</a></span>, <span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFileEncoding">SoundFileEncoding</a></span>)-&gt;<span class="STpe">bool</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">init</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_ICodec">ICodec</a></span>, ^<span class="STpe">void</span>, <span class="STpe">u64</span>)-&gt;<span class="STpe">u64</span> <span class="SKwd">throw</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">decode</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_ICodec">ICodec</a></span>, ^<span class="STpe">void</span>, <span class="STpe">u64</span>, ^<span class="STpe">void</span>, <span class="STpe">u64</span>)-&gt;{write: <span class="STpe">u64</span>, read: <span class="STpe">u64</span>} <span class="SKwd">throw</span></span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_SoundFile"><span class="titletype">struct</span> <span class="titlelight">Audio.</span><span class="titlestrong">SoundFile</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\file\soundfile.swg#L34" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Represents a sound file. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCde">fullname</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">String</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">datas</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Core</span>.<span class="SCst">Array</span>'(<span class="STpe">u8</span>)</span></td>
<td>Prefetched datas (in encoding format). </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">sampleCount</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td>Total number of samples. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dataSize</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td>Total size, in bytes, of datas. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dataSeek</span></td>
<td class="codetype"><span class="STpe">u64</span></td>
<td>The position in the file where the datas are stored. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">encoding</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFileEncoding">SoundFileEncoding</a></span></span></td>
<td>Encoding type of the datas. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">validity</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFileValidityMask">SoundFileValidityMask</a></span></span></td>
<td>What informations in this struct are valid. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">frequency</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Sound frequency. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">channelCount</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Number of channels (2 for stereo...). </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">channelMask</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Identifier of the channels. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bitsPerSample</span></td>
<td class="codetype"><span class="STpe">u32</span></td>
<td>Number of bits per sample in the datas. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">duration</span></td>
<td class="codetype"><span class="STpe">f32</span></td>
<td>Duration, in seconds, of the sound. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">validBitsPerSample</span></td>
<td class="codetype"><span class="STpe">u16</span></td>
<td>Number of valid bits per sample (&lt;= bitsPerSample). </td>
</tr>
</table>
<p> The <code class="incode">SoundFile</code> is not necessary fully loaded in memory, in case we want it to be streamed. </p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Audio_SoundFile_load"><span class="SCde"><span class="SFct">load</span>()</spa</a></td>
<td>Load a <code class="incode">SoundFile</code> from disk. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_SoundFile_load"><span class="titletype">func</span> <span class="titlelight">SoundFile.</span><span class="titlestrong">load</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\file\soundfile.swg#L59" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Load a <code class="incode">SoundFile</code> from disk. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">load</span>(fullname: <span class="STpe">string</span>, loadDatas = <span class="SKwd">true</span>, loadMetaDatas = <span class="SKwd">false</span>)-&gt;<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFile">SoundFile</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p> Will load the sound datas if <code class="incode">loadDatas</code> is true.  Will load the sound metadatas if <code class="incode">loadMetaData</code> is true. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_SoundFileEncoding"><span class="titletype">enum</span> <span class="titlelight">Audio.</span><span class="titlestrong">SoundFileEncoding</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\file\soundfile.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>SoundFile internal format. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCst">Unknown</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Pcm8</span></td>
<td>Pcm, uncompressed, 8 bits per sample. </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Pcm16</span></td>
<td>Pcm, uncompressed, 16 bits per sample. </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Pcm24</span></td>
<td>Pcm, uncompressed, 24 bits per sample. </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Pcm32</span></td>
<td>Pcm, uncompressed, 32 bits per sample. </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Float32</span></td>
<td>Pcm, uncompressed, float 32 bits per sample. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_SoundFileValidityMask"><span class="titletype">enum</span> <span class="titlelight">Audio.</span><span class="titlestrong">SoundFileValidityMask</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\file\soundfile.swg#L6" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Determins which informations in the <a href="#Audio_SoundFile">SoundFile</a> struct are valid. </p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Format</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Frequency</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">ChannelCount</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">ChannelMask</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">BitsPerSample</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Duration</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">SampleCount</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">ValidBitsPerSample</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Data</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">MetaData</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice"><span class="titletype">struct</span> <span class="titlelight">Audio.</span><span class="titlestrong">Voice</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L32" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Represents a playing sound. </p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td><a href="#Audio_Voice_create"><span class="SCde"><span class="SFct">create</span>()</spa</a></td>
<td>Creates a new voice for a given sound file. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_destroy"><span class="SCde"><span class="SFct">destroy</span>()</spa</a></td>
<td>Destroy the voice. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_getVolume"><span class="SCde"><span class="SFct">getVolume</span>()</spa</a></td>
<td>Returns the actual volume. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_getVolumeDB"><span class="SCde"><span class="SFct">getVolumeDB</span>()</spa</a></td>
<td>Returns the actual volume, in DB. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_isPlaying"><span class="SCde"><span class="SFct">isPlaying</span>()</spa</a></td>
<td>Returns true if the voice is currently playing. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_pause"><span class="SCde"><span class="SFct">pause</span>()</spa</a></td>
<td>Pause the playing voice. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_play"><span class="SCde"><span class="SFct">play</span>(*<span class="SCst">SoundFile</span>, <span class="SCst">VoiceCreateFlags</span>, <span class="SCst">VoicePlayFlags</span>)</span></a></td>
<td>Creates a voice and plays it. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_play"><span class="SCde"><span class="SFct">play</span>(<span class="SKwd">self</span>, <span class="SCst">VoicePlayFlags</span>)</span></a></td>
<td>Plays a voice. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_setFrequencyRatio"><span class="SCde"><span class="SFct">setFrequencyRatio</span>()</spa</a></td>
<td>Set the playing pitch. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_setRooting"><span class="SCde"><span class="SFct">setRooting</span>()</spa</a></td>
<td>Root a voice to a given list of buses. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_setVolume"><span class="SCde"><span class="SFct">setVolume</span>()</spa</a></td>
<td>Set the playing voice volume between [0..1]. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_setVolumeDB"><span class="SCde"><span class="SFct">setVolumeDB</span>()</spa</a></td>
<td>Set the playing voice volume. </td>
</tr>
<tr>
<td><a href="#Audio_Voice_stop"><span class="SCde"><span class="SFct">stop</span>()</spa</a></td>
<td>Stop the playing voice. </td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_create"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">create</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L94" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a new voice for a given sound file. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">create</span>(file: *<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFile">SoundFile</a></span>, createFlags = <span class="SCst"><a href="#Audio_VoiceCreateFlags">VoiceCreateFlags</a></span>.<span class="SCst">Default</span>)-&gt;*<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_Voice">Voice</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p> A voice is what will be actually played. You can have as many voices as you want for one unique <a href="#Audio_SoundFile">SoundFile</a>. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_destroy"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">destroy</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L154" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy the voice. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">destroy</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_getVolume"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">getVolume</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\driver\xaudio2.swg#L265" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the actual volume. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getVolume</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_getVolumeDB"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">getVolumeDB</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L177" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns the actual volume, in DB. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getVolumeDB</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">f32</span> <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_isPlaying"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">isPlaying</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L163" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Returns true if the voice is currently playing. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">isPlaying</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>)-&gt;<span class="STpe">bool</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_pause"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">pause</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L134" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Pause the playing voice. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">pause</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_play"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">play</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L108" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates a voice and plays it. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">play</span>(file: *<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFile">SoundFile</a></span>, createFlags = <span class="SCst"><a href="#Audio_VoiceCreateFlags">VoiceCreateFlags</a></span>.<span class="SCst">Default</span>, playFlags = <span class="SCst"><a href="#Audio_VoicePlayFlags">VoicePlayFlags</a></span>.<span class="SCst">Default</span>)-&gt;*<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_Voice">Voice</a></span> <span class="SKwd">throw</span></span></code>
</div>
<p>Plays a voice. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">play</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, playFlags = <span class="SCst"><a href="#Audio_VoicePlayFlags">VoicePlayFlags</a></span>.<span class="SCst">Zero</span>) <span class="SKwd">throw</span></span></code>
</div>
<p> By default, the voice will be destroyed when stopped or finished. </p>
<p> By default, you will have to destroy the voice yourself when no more needed, for example if <a href="#Audio_Voice_isPlaying">Voice.isPlaying</a> returns false. But if you want the voice to be destroyed automatically when done, set the <code class="incode">DestroyOnStop</code> flag in <code class="incode">playFlags</code>.  See <a href="#Audio_Voice_create">Voice.create</a> </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_setFrequencyRatio"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">setFrequencyRatio</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\driver\xaudio2.swg#L277" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the playing pitch. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setFrequencyRatio</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, ratio: <span class="STpe">f32</span>, batchID: <span class="STpe">u32</span> = <span class="SNum">0</span>) <span class="SKwd">throw</span></span></code>
</div>
<p> The voice should have been created with <code class="incode">VoiceCreateFlags.AcceptPitch</code>.  See <a href="#Audio_Voice_create">Voice.create</a> </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_setRooting"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">setRooting</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\driver\xaudio2.swg#L289" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Root a voice to a given list of buses. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setRooting</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, buses: <span class="SKwd">const</span> [..] <span class="SKwd">const</span> *<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_Bus">Bus</a></span>) <span class="SKwd">throw</span></span></code>
</div>
<p> You can also set <code class="incode">buses</code> to null if you want to root the voice only to the  main bus (which is the default). </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_setVolume"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">setVolume</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\driver\xaudio2.swg#L254" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the playing voice volume between [0..1]. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setVolume</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, volume: <span class="STpe">f32</span>, batchID: <span class="STpe">u32</span> = <span class="SNum">0</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_setVolumeDB"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">setVolumeDB</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L171" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the playing voice volume. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setVolumeDB</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>, volumeDB: <span class="STpe">f32</span>, batchID: <span class="STpe">u32</span> = <span class="SNum">0</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Voice_stop"><span class="titletype">func</span> <span class="titlelight">Voice.</span><span class="titlestrong">stop</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L143" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Stop the playing voice. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">stop</span>(<span class="SKwd">using</span> <span class="SKwd">self</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_VoiceCreateFlags"><span class="titletype">enum</span> <span class="titlelight">Audio.</span><span class="titlestrong">VoiceCreateFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AcceptPitch</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">AcceptFilters</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Default</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_VoicePlayFlags"><span class="titletype">enum</span> <span class="titlelight">Audio.</span><span class="titlestrong">VoicePlayFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L13" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Loop</span></td>
<td>Play in loops. </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">DestroyOnStop</span></td>
<td>Destroy the voice once the sound has been played. </td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Default</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_VoiceState"><span class="titletype">enum</span> <span class="titlelight">Audio.</span><span class="titlestrong">VoiceState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L22" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"><span class="SCst">Zero</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">PlayedOnce</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">Playing</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCst">PendingDestroy</span></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_Wav_loadFile"><span class="titletype">func</span> <span class="titlelight">Wav.</span><span class="titlestrong">loadFile</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\file\wav.swg#L231" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Load a wav file. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">loadFile</span>(<span class="SKwd">using</span> file: *<span class="SCst">Audio</span>.<span class="SCst"><a href="#Audio_SoundFile">SoundFile</a></span>, stream: *<span class="SCst">Core</span>.<span class="SCst">File</span>.<span class="SCst">FileStream</span>, loadDatas = <span class="SKwd">true</span>, loadMetaDatas = <span class="SKwd">false</span>) <span class="SKwd">throw</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_addCodec"><span class="titletype">func</span> <span class="titlelight">Audio.</span><span class="titlestrong">addCodec</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\codec\codec.swg#L21" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Register a codec. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span>(<span class="SCst">T</span>) <span class="SFct">addCodec</span>()</span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_convertDBToPercent"><span class="titletype">func</span> <span class="titlelight">Audio.</span><span class="titlestrong">convertDBToPercent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\helpers.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a DB value to a percent. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">convertDBToPercent</span>(dbVolume: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_convertPercentToDB"><span class="titletype">func</span> <span class="titlelight">Audio.</span><span class="titlestrong">convertPercentToDB</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\helpers.swg#L12" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Convert a percent value to DB. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">convertPercentToDB</span>(percentVolume: <span class="STpe">f32</span>)-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_createEngine"><span class="titletype">func</span> <span class="titlelight">Audio.</span><span class="titlestrong">createEngine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\audio.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Creates the audio engine. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">createEngine</span>() <span class="SKwd">throw</span></span></code>
</div>
<p> Must be called once, before anything else. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_destroyEngine"><span class="titletype">func</span> <span class="titlelight">Audio.</span><span class="titlestrong">destroyEngine</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\audio.swg#L14" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Destroy the audio engine. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">destroyEngine</span>()</span></code>
</div>
<p> Must be called at the end, when engine is no more used. </p>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_getOutputVolume"><span class="titletype">func</span> <span class="titlelight">Audio.</span><span class="titlestrong">getOutputVolume</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\audio.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Get the general output volume. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">getOutputVolume</span>()-&gt;<span class="STpe">f32</span></span></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Audio_setOutputVolume"><span class="titletype">func</span> <span class="titlelight">Audio.</span><span class="titlestrong">setOutputVolume</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\audio.swg#L20" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Set the general output volume. </p>
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setOutputVolume</span>(volume: <span class="STpe">f32</span>) <span class="SKwd">throw</span></span></code>
</div>
</div>
</div>
</div>
</body>
</html>
