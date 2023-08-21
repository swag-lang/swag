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
            @media only screen and (max-width: 600px) {
                td {
                    display: block;
                    width:   100%;
                }
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
        .container table.enumeration {
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              100%;
            font-size:          90%;
        }
        .container td.enumeration {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              30%;
        }
        .container td.tdname {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              20%;
            background-color:   #f8f8f8;
        }
        .container td.tdtype {
            padding:            6px;
            border:             1px solid LightGrey;
            border-collapse:    collapse;
            width:              auto;
        }
        .container td:last-child {
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
<h1>Module audio</h1>
<h2>Structs</h2>
<h3></h3>
<ul>
<li><a href="#Audio_Bus">Bus</a></li>
<li><a href="#Audio_Voice">Voice</a></li>
</ul>
<h3>codec</h3>
<ul>
<li><a href="#Audio_Codec">Codec</a></li>
</ul>
<h3>file</h3>
<ul>
<li><a href="#Audio_SoundFile">SoundFile</a></li>
</ul>
<h2>Interfaces</h2>
<h3>codec</h3>
<ul>
<li><a href="#Audio_ICodec">ICodec</a></li>
</ul>
<h2>Enums</h2>
<h3></h3>
<ul>
<li><a href="#Audio_VoiceCreateFlags">VoiceCreateFlags</a></li>
<li><a href="#Audio_VoicePlayFlags">VoicePlayFlags</a></li>
<li><a href="#Audio_VoiceState">VoiceState</a></li>
</ul>
<h3>file</h3>
<ul>
<li><a href="#Audio_SoundFileEncoding">SoundFileEncoding</a></li>
<li><a href="#Audio_SoundFileValidityMask">SoundFileValidityMask</a></li>
</ul>
<h2>Functions</h2>
<h3></h3>
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
<h3>codec</h3>
<ul>
<li><a href="#Audio_addCodec">Audio.addCodec</a></li>
</ul>
<h3>driver</h3>
<ul>
<li><a href="#Audio_Bus_getVolume">Bus.getVolume</a></li>
<li><a href="#Audio_Bus_setVolume">Bus.setVolume</a></li>
<li><a href="#Audio_Voice_getVolume">Voice.getVolume</a></li>
<li><a href="#Audio_Voice_setFrequencyRatio">Voice.setFrequencyRatio</a></li>
<li><a href="#Audio_Voice_setRooting">Voice.setRooting</a></li>
<li><a href="#Audio_Voice_setVolume">Voice.setVolume</a></li>
</ul>
<h3>file</h3>
<ul>
<li><a href="#Audio_SoundFile_load">SoundFile.load</a></li>
<li><a href="#Audio_Wav_loadFile">Wav.loadFile</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Overview</h1>
<p><code class="incode">Std.Audio</code> is a module to decode and play sound files. </p>
<p>Under windows, it is based on the <code class="incode">xaudio2</code> library. </p>
<h2>How to play a sound </h2>
<p>First, you have to initialize the audio engine by calling <a href="#Audio_createEngine">Audio.createEngine</a>. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">createEngine</span><span class="SyntaxCode">()
</span><span class="SyntaxLogic">defer</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">destroyEngine</span><span class="SyntaxCode">() </span><span class="SyntaxComment">// Don't forget to destroy the engine when you are done</span><span class="SyntaxCode"></code>
</pre>
<p>You then have to load a sound file. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> soundFile = </span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SoundFile</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Load</span><span class="SyntaxCode">(</span><span class="SyntaxString">"mySound.wav"</span><span class="SyntaxCode">)</code>
</pre>
<p>Note that by default, the sound file will load all of its datas in memory. If you want the sound to be loaded only when played, set <code class="incode">loadDatas</code> to false. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> soundFile = </span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SoundFile</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Load</span><span class="SyntaxCode">(</span><span class="SyntaxString">"mySound.wav"</span><span class="SyntaxCode">, loadDatas = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)</code>
</pre>
<p>Once you have a sound file, the simplest way to play it is by calling <a href="#Audio_Voice_play">Voice.play</a>. The sound will be played once, until the end, and will be destroyed. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Voice</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">play</span><span class="SyntaxCode">(soundFile)</code>
</pre>
<p>To have more control, you could also use <a href="#Audio_Voice_create">Voice.create</a> then [Voice.Play] on the created sound. That way you will recieve a <a href="#Audio_Voice">Voice</a> object you can play with. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">let</span><span class="SyntaxCode"> voice = </span><span class="SyntaxConstant">Voice</span><span class="SyntaxCode">.</span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(&soundFile)
voice.</span><span class="SyntaxFunction">setVolume</span><span class="SyntaxCode">(</span><span class="SyntaxNumber">0.5</span><span class="SyntaxCode">)
voice.</span><span class="SyntaxFunction">play</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">Loop</span><span class="SyntaxCode">)</code>
</pre>
<h1>Content</h1>
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
<td class="tdname">
handle</td>
<td class="tdtype">
Audio.BusHandle</td>
<td class="enumeration">
</td>
</tr>
</table>
<p> A <a href="#Audio_Voice">Voice</a> can be assigned to one or more buses. If you then change some parameters of the bus (like the volume), then all the voices assigned to it will be impacted. </p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Audio_Bus_create">create(u32, *Bus)</a></td>
<td class="enumeration">
<p>Creates an audio bus. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Bus_destroy">destroy(self)</a></td>
<td class="enumeration">
<p>Destroy the bus (immediatly). </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Bus_getVolume">getVolume(self)</a></td>
<td class="enumeration">
<p>Returns the actual volume. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Bus_getVolumeDB">getVolumeDB(self)</a></td>
<td class="enumeration">
<p>Returns the actual volume, in DB. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Bus_setVolume">setVolume(self, f32, u32)</a></td>
<td class="enumeration">
<p>Set the playing bus volume between [0..1]. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Bus_setVolumeDB">setVolumeDB(self, f32, u32)</a></td>
<td class="enumeration">
<p>Set the playing bus volume in DB. </p>
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(numChannels: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">, parent: *</span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Audio_Bus">Bus</a></span><span class="SyntaxCode"> = </span><span class="SyntaxKeyword">null</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Audio_Bus">Bus</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">destroy</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getVolume</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getVolumeDB</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setVolume</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, volume: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, batchID: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setVolumeDB</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, volumeDB: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, batchID: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
srcEncoding</td>
<td class="tdtype">
<a href="#Audio_SoundFileEncoding">Audio.SoundFileEncoding</a></td>
<td class="enumeration">
<p>The original encoding. </p>
</td>
</tr>
<tr>
<td class="tdname">
dstEncoding</td>
<td class="tdtype">
<a href="#Audio_SoundFileEncoding">Audio.SoundFileEncoding</a></td>
<td class="enumeration">
<p>The requested encoding. </p>
</td>
</tr>
<tr>
<td class="tdname">
type</td>
<td class="tdtype">
const *Swag.TypeInfoStruct</td>
<td class="enumeration">
<p>The real type of the codec. </p>
</td>
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
<td class="tdname">
canEncode</td>
<td class="tdtype">
func(*Audio.ICodec, Audio.SoundFileEncoding)->bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
canDecode</td>
<td class="tdtype">
func(*Audio.ICodec, Audio.SoundFileEncoding)->bool</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
init</td>
<td class="tdtype">
func(*Audio.ICodec, ^void, u64)->u64 throw</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
decode</td>
<td class="tdtype">
func(*Audio.ICodec, ^void, u64, ^void, u64)->{write: u64, read: u64} throw</td>
<td class="enumeration">
</td>
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
<td class="tdname">
fullname</td>
<td class="tdtype">
Core.String</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
datas</td>
<td class="tdtype">
Core.Array'(u8)</td>
<td class="enumeration">
<p>Prefetched datas (in encoding format). </p>
</td>
</tr>
<tr>
<td class="tdname">
sampleCount</td>
<td class="tdtype">
u64</td>
<td class="enumeration">
<p>Total number of samples. </p>
</td>
</tr>
<tr>
<td class="tdname">
dataSize</td>
<td class="tdtype">
u64</td>
<td class="enumeration">
<p>Total size, in bytes, of datas. </p>
</td>
</tr>
<tr>
<td class="tdname">
dataSeek</td>
<td class="tdtype">
u64</td>
<td class="enumeration">
<p>The position in the file where the datas are stored. </p>
</td>
</tr>
<tr>
<td class="tdname">
encoding</td>
<td class="tdtype">
<a href="#Audio_SoundFileEncoding">Audio.SoundFileEncoding</a></td>
<td class="enumeration">
<p>Encoding type of the datas. </p>
</td>
</tr>
<tr>
<td class="tdname">
validity</td>
<td class="tdtype">
<a href="#Audio_SoundFileValidityMask">Audio.SoundFileValidityMask</a></td>
<td class="enumeration">
<p>What informations in this struct are valid. </p>
</td>
</tr>
<tr>
<td class="tdname">
frequency</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Sound frequency. </p>
</td>
</tr>
<tr>
<td class="tdname">
channelCount</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Number of channels (2 for stereo...). </p>
</td>
</tr>
<tr>
<td class="tdname">
channelMask</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Identifier of the channels. </p>
</td>
</tr>
<tr>
<td class="tdname">
bitsPerSample</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Number of bits per sample in the datas. </p>
</td>
</tr>
<tr>
<td class="tdname">
duration</td>
<td class="tdtype">
f32</td>
<td class="enumeration">
<p>Duration, in seconds, of the sound. </p>
</td>
</tr>
<tr>
<td class="tdname">
validBitsPerSample</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
<p>Number of valid bits per sample (&lt;= bitsPerSample). </p>
</td>
</tr>
</table>
<p> The <code class="incode">SoundFile</code> is not necessary fully loaded in memory, in case we want it to be streamed. </p>
<h3>Functions</h3>
<table class="enumeration">
<tr>
<td class="enumeration">
<a href="#Audio_SoundFile_load">load(string, bool, bool)</a></td>
<td class="enumeration">
<p>Load a <code class="incode">SoundFile</code> from disk. </p>
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">load</span><span class="SyntaxCode">(fullname: </span><span class="SyntaxType">string</span><span class="SyntaxCode">, loadDatas = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">, loadMetaDatas = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Audio_SoundFile">SoundFile</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
Unknown</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Pcm8</td>
<td class="enumeration">
<p>Pcm, uncompressed, 8 bits per sample. </p>
</td>
</tr>
<tr>
<td class="tdname">
Pcm16</td>
<td class="enumeration">
<p>Pcm, uncompressed, 16 bits per sample. </p>
</td>
</tr>
<tr>
<td class="tdname">
Pcm24</td>
<td class="enumeration">
<p>Pcm, uncompressed, 24 bits per sample. </p>
</td>
</tr>
<tr>
<td class="tdname">
Pcm32</td>
<td class="enumeration">
<p>Pcm, uncompressed, 32 bits per sample. </p>
</td>
</tr>
<tr>
<td class="tdname">
Float32</td>
<td class="enumeration">
<p>Pcm, uncompressed, float 32 bits per sample. </p>
</td>
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
<td class="tdname">
Zero</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Format</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Frequency</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ChannelCount</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ChannelMask</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
BitsPerSample</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Duration</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
SampleCount</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ValidBitsPerSample</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Data</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
MetaData</td>
<td class="enumeration">
</td>
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
<td class="enumeration">
<a href="#Audio_Voice_create">create(*SoundFile, VoiceCreateFlags)</a></td>
<td class="enumeration">
<p>Creates a new voice for a given sound file. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_destroy">destroy(self)</a></td>
<td class="enumeration">
<p>Destroy the voice. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_getVolume">getVolume(self)</a></td>
<td class="enumeration">
<p>Returns the actual volume. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_getVolumeDB">getVolumeDB(self)</a></td>
<td class="enumeration">
<p>Returns the actual volume, in DB. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_isPlaying">isPlaying(self)</a></td>
<td class="enumeration">
<p>Returns true if the voice is currently playing. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_pause">pause(self)</a></td>
<td class="enumeration">
<p>Pause the playing voice. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_play">play(*SoundFile, VoiceCreateFlags, VoicePlayFlags)</a></td>
<td class="enumeration">
<p>Creates a voice and plays it. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_play">play(self, VoicePlayFlags)</a></td>
<td class="enumeration">
<p>Plays a voice. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_setFrequencyRatio">setFrequencyRatio(self, f32, u32)</a></td>
<td class="enumeration">
<p>Set the playing pitch. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_setRooting">setRooting(self, const [..] const *Bus)</a></td>
<td class="enumeration">
<p>Root a voice to a given list of buses. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_setVolume">setVolume(self, f32, u32)</a></td>
<td class="enumeration">
<p>Set the playing voice volume between [0..1]. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_setVolumeDB">setVolumeDB(self, f32, u32)</a></td>
<td class="enumeration">
<p>Set the playing voice volume. </p>
</td>
</tr>
<tr>
<td class="enumeration">
<a href="#Audio_Voice_stop">stop(self)</a></td>
<td class="enumeration">
<p>Stop the playing voice. </p>
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">create</span><span class="SyntaxCode">(file: *</span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Audio_SoundFile">SoundFile</a></span><span class="SyntaxCode">, createFlags = </span><span class="SyntaxConstant"><a href="#Audio_VoiceCreateFlags">VoiceCreateFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Default</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Audio_Voice">Voice</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">destroy</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getVolume</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getVolumeDB</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">isPlaying</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self)-&gt;</span><span class="SyntaxType">bool</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">pause</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">play</span><span class="SyntaxCode">(file: *</span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Audio_SoundFile">SoundFile</a></span><span class="SyntaxCode">, createFlags = </span><span class="SyntaxConstant"><a href="#Audio_VoiceCreateFlags">VoiceCreateFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Default</span><span class="SyntaxCode">, playFlags = </span><span class="SyntaxConstant"><a href="#Audio_VoicePlayFlags">VoicePlayFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Default</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Audio_Voice">Voice</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
<p>Plays a voice. </p>
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">play</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, playFlags = </span><span class="SyntaxConstant"><a href="#Audio_VoicePlayFlags">VoicePlayFlags</a></span><span class="SyntaxCode">.</span><span class="SyntaxConstant">Zero</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setFrequencyRatio</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, ratio: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, batchID: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setRooting</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, buses: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> [..] </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Audio_Bus">Bus</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setVolume</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, volume: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, batchID: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setVolumeDB</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self, volumeDB: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">, batchID: </span><span class="SyntaxType">u32</span><span class="SyntaxCode"> = </span><span class="SyntaxNumber">0</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">stop</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> self) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
Zero</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
AcceptPitch</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
AcceptFilters</td>
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
<span class="content" id="Audio_VoicePlayFlags"><span class="titletype">enum</span> <span class="titlelight">Audio.</span><span class="titlestrong">VoicePlayFlags</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L13" class="src">[src]</a></td>
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
Loop</td>
<td class="enumeration">
<p>Play in loops. </p>
</td>
</tr>
<tr>
<td class="tdname">
DestroyOnStop</td>
<td class="enumeration">
<p>Destroy the voice once the sound has been played. </p>
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
<span class="content" id="Audio_VoiceState"><span class="titletype">enum</span> <span class="titlelight">Audio.</span><span class="titlestrong">VoiceState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/audio\src\voice.swg#L22" class="src">[src]</a></td>
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
PlayedOnce</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Playing</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
PendingDestroy</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">loadFile</span><span class="SyntaxCode">(</span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> file: *</span><span class="SyntaxConstant">Audio</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Audio_SoundFile">SoundFile</a></span><span class="SyntaxCode">, stream: *</span><span class="SyntaxConstant">Core</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">File</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">FileStream</span><span class="SyntaxCode">, loadDatas = </span><span class="SyntaxKeyword">true</span><span class="SyntaxCode">, loadMetaDatas = </span><span class="SyntaxKeyword">false</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">T</span><span class="SyntaxCode">) </span><span class="SyntaxFunction">addCodec</span><span class="SyntaxCode">()</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">convertDBToPercent</span><span class="SyntaxCode">(dbVolume: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">convertPercentToDB</span><span class="SyntaxCode">(percentVolume: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">f32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">createEngine</span><span class="SyntaxCode">() </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">destroyEngine</span><span class="SyntaxCode">()</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">getOutputVolume</span><span class="SyntaxCode">()</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setOutputVolume</span><span class="SyntaxCode">(volume: </span><span class="SyntaxType">f32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
</div>
</div>
</div>
</body>
</html>
