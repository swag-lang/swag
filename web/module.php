<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Swag</title>
<link rel="icon" type="image/x-icon" href="favicon.ico">
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
        .left a, .enumeration a {
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
</head>
<body>
<?php include('common/start-body.php'); ?><div class="container">
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.24.0)</blockquote>
<div class="precode"><code><span class="SCde"><span class="SFct">#dependencies</span>
{
    <span class="SFct">#run</span>
    {
        <span class="SKwd">let</span> itf = <span class="SItr">@compiler</span>()
        <span class="SKwd">let</span> cfg = itf.<span class="SFct">getBuildCfg</span>()
        cfg.moduleVersion  = <span class="SNum">1</span>
        cfg.moduleRevision = <span class="SNum">0</span>
        cfg.moduleBuildNum = <span class="SNum">0</span>
        cfg.backendKind    = .<span class="SCst">Executable</span>

        cfg.genDoc.kind = .<span class="SCst">Pages</span>
        cfg.genDoc.outputExtension = <span class="SStr">".php"</span>

        cfg.genDoc.titleContent = <span class="SStr">"Swag"</span>
        cfg.genDoc.css = <span class="SStr">"css/style.css"</span>
        cfg.genDoc.icon = <span class="SStr">"favicon.ico"</span>
        cfg.genDoc.startHead = <span class="SStr">"&lt;?php include('common/start-head.php'); ?&gt;"</span>
        cfg.genDoc.startBody = <span class="SStr">"&lt;?php include('common/start-body.php'); ?&gt;"</span>

        cfg.genDoc.morePages = <span class="SStr">"../../../examples/scripts/flappy.swgs"</span>
    }
}</span></code>
</div>
</div>
</div>
</div>
</body>
</html>
