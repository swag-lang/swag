<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php include('common/start-head.php'); ?><title>Module win32</title>
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
<li><a href="#Win32_CONSOLE_SCREEN_BUFFER_INFO">CONSOLE_SCREEN_BUFFER_INFO</a></li>
<li><a href="#Win32_COORD">COORD</a></li>
<li><a href="#Win32_CREATESTRUCTA">CREATESTRUCTA</a></li>
<li><a href="#Win32_CREATESTRUCTW">CREATESTRUCTW</a></li>
<li><a href="#Win32_FILETIME">FILETIME</a></li>
<li><a href="#Win32_GUID">GUID</a></li>
<li><a href="#Win32_ICONINFO">ICONINFO</a></li>
<li><a href="#Win32_IP_ADAPTER_INFO">IP_ADAPTER_INFO</a></li>
<li><a href="#Win32_IP_ADDRESS_STRING">IP_ADDRESS_STRING</a></li>
<li><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></li>
<li><a href="#Win32_MINMAXINFO">MINMAXINFO</a></li>
<li><a href="#Win32_MONITORINFO">MONITORINFO</a></li>
<li><a href="#Win32_MSG">MSG</a></li>
<li><a href="#Win32_NOTIFYICONDATAA">NOTIFYICONDATAA</a></li>
<li><a href="#Win32_NOTIFYICONDATAW">NOTIFYICONDATAW</a></li>
<li><a href="#Win32_PAINTSTRUCT">PAINTSTRUCT</a></li>
<li><a href="#Win32_POINT">POINT</a></li>
<li><a href="#Win32_PROCESS_INFORMATION">PROCESS_INFORMATION</a></li>
<li><a href="#Win32_RECT">RECT</a></li>
<li><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></li>
<li><a href="#Win32_SHFILEINFOA">SHFILEINFOA</a></li>
<li><a href="#Win32_SHFILEINFOW">SHFILEINFOW</a></li>
<li><a href="#Win32_SHSTOCKICONINFO">SHSTOCKICONINFO</a></li>
<li><a href="#Win32_SMALL_RECT">SMALL_RECT</a></li>
<li><a href="#Win32_STARTUPINFOA">STARTUPINFOA</a></li>
<li><a href="#Win32_STARTUPINFOW">STARTUPINFOW</a></li>
<li><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></li>
<li><a href="#Win32_SYSTEM_INFO">SYSTEM_INFO</a></li>
<li><a href="#Win32_WIN32_FILE_ATTRIBUTE_DATA">WIN32_FILE_ATTRIBUTE_DATA</a></li>
<li><a href="#Win32_WIN32_FIND_DATAA">WIN32_FIND_DATAA</a></li>
<li><a href="#Win32_WIN32_FIND_DATAW">WIN32_FIND_DATAW</a></li>
<li><a href="#Win32_WINDOWPOS">WINDOWPOS</a></li>
<li><a href="#Win32_WNDCLASSA">WNDCLASSA</a></li>
<li><a href="#Win32_WNDCLASSW">WNDCLASSW</a></li>
</ul>
<h3>Enums</h3>
<h4></h4>
<ul>
<li><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></li>
<li><a href="#Win32_SHSTOCKICONID">SHSTOCKICONID</a></li>
</ul>
<h3>Constants</h3>
<h4></h4>
<ul>
<li><a href="#Win32_BACKGROUND_BLUE">BACKGROUND_BLUE</a></li>
<li><a href="#Win32_BACKGROUND_GREEN">BACKGROUND_GREEN</a></li>
<li><a href="#Win32_BACKGROUND_INTENSITY">BACKGROUND_INTENSITY</a></li>
<li><a href="#Win32_BACKGROUND_MASK">BACKGROUND_MASK</a></li>
<li><a href="#Win32_BACKGROUND_RED">BACKGROUND_RED</a></li>
<li><a href="#Win32_CF_BITMAP">CF_BITMAP</a></li>
<li><a href="#Win32_CF_DIB">CF_DIB</a></li>
<li><a href="#Win32_CF_DIBV5">CF_DIBV5</a></li>
<li><a href="#Win32_CF_DIF">CF_DIF</a></li>
<li><a href="#Win32_CF_ENHMETAFILE">CF_ENHMETAFILE</a></li>
<li><a href="#Win32_CF_HDROP">CF_HDROP</a></li>
<li><a href="#Win32_CF_LOCALE">CF_LOCALE</a></li>
<li><a href="#Win32_CF_METAFILEPICT">CF_METAFILEPICT</a></li>
<li><a href="#Win32_CF_OEMTEXT">CF_OEMTEXT</a></li>
<li><a href="#Win32_CF_PALETTE">CF_PALETTE</a></li>
<li><a href="#Win32_CF_PENDATA">CF_PENDATA</a></li>
<li><a href="#Win32_CF_RIFF">CF_RIFF</a></li>
<li><a href="#Win32_CF_SYLK">CF_SYLK</a></li>
<li><a href="#Win32_CF_TEXT">CF_TEXT</a></li>
<li><a href="#Win32_CF_TIFF">CF_TIFF</a></li>
<li><a href="#Win32_CF_UNICODETEXT">CF_UNICODETEXT</a></li>
<li><a href="#Win32_CF_WAVE">CF_WAVE</a></li>
<li><a href="#Win32_CONSOLE_TEXTMODE_BUFFER">CONSOLE_TEXTMODE_BUFFER</a></li>
<li><a href="#Win32_CREATE_ALWAYS">CREATE_ALWAYS</a></li>
<li><a href="#Win32_CREATE_NEW">CREATE_NEW</a></li>
<li><a href="#Win32_CREATE_SUSPENDED">CREATE_SUSPENDED</a></li>
<li><a href="#Win32_CSIDL_ADMINTOOLS">CSIDL_ADMINTOOLS</a></li>
<li><a href="#Win32_CSIDL_ALTSTARTUP">CSIDL_ALTSTARTUP</a></li>
<li><a href="#Win32_CSIDL_APPDATA">CSIDL_APPDATA</a></li>
<li><a href="#Win32_CSIDL_BITBUCKET">CSIDL_BITBUCKET</a></li>
<li><a href="#Win32_CSIDL_CDBURN_AREA">CSIDL_CDBURN_AREA</a></li>
<li><a href="#Win32_CSIDL_COMMON_ADMINTOOLS">CSIDL_COMMON_ADMINTOOLS</a></li>
<li><a href="#Win32_CSIDL_COMMON_ALTSTARTUP">CSIDL_COMMON_ALTSTARTUP</a></li>
<li><a href="#Win32_CSIDL_COMMON_APPDATA">CSIDL_COMMON_APPDATA</a></li>
<li><a href="#Win32_CSIDL_COMMON_DESKTOPDIRECTORY">CSIDL_COMMON_DESKTOPDIRECTORY</a></li>
<li><a href="#Win32_CSIDL_COMMON_DOCUMENTS">CSIDL_COMMON_DOCUMENTS</a></li>
<li><a href="#Win32_CSIDL_COMMON_FAVORITES">CSIDL_COMMON_FAVORITES</a></li>
<li><a href="#Win32_CSIDL_COMMON_MUSIC">CSIDL_COMMON_MUSIC</a></li>
<li><a href="#Win32_CSIDL_COMMON_OEM_LINKS">CSIDL_COMMON_OEM_LINKS</a></li>
<li><a href="#Win32_CSIDL_COMMON_PICTURES">CSIDL_COMMON_PICTURES</a></li>
<li><a href="#Win32_CSIDL_COMMON_PROGRAMS">CSIDL_COMMON_PROGRAMS</a></li>
<li><a href="#Win32_CSIDL_COMMON_STARTMENU">CSIDL_COMMON_STARTMENU</a></li>
<li><a href="#Win32_CSIDL_COMMON_STARTUP">CSIDL_COMMON_STARTUP</a></li>
<li><a href="#Win32_CSIDL_COMMON_TEMPLATES">CSIDL_COMMON_TEMPLATES</a></li>
<li><a href="#Win32_CSIDL_COMMON_VIDEO">CSIDL_COMMON_VIDEO</a></li>
<li><a href="#Win32_CSIDL_COMPUTERSNEARME">CSIDL_COMPUTERSNEARME</a></li>
<li><a href="#Win32_CSIDL_CONNECTIONS">CSIDL_CONNECTIONS</a></li>
<li><a href="#Win32_CSIDL_CONTROLS">CSIDL_CONTROLS</a></li>
<li><a href="#Win32_CSIDL_COOKIES">CSIDL_COOKIES</a></li>
<li><a href="#Win32_CSIDL_DESKTOP">CSIDL_DESKTOP</a></li>
<li><a href="#Win32_CSIDL_DESKTOPDIRECTORY">CSIDL_DESKTOPDIRECTORY</a></li>
<li><a href="#Win32_CSIDL_DRIVES">CSIDL_DRIVES</a></li>
<li><a href="#Win32_CSIDL_FAVORITES">CSIDL_FAVORITES</a></li>
<li><a href="#Win32_CSIDL_FLAG_CREATE">CSIDL_FLAG_CREATE</a></li>
<li><a href="#Win32_CSIDL_FLAG_DONT_UNEXPAND">CSIDL_FLAG_DONT_UNEXPAND</a></li>
<li><a href="#Win32_CSIDL_FLAG_DONT_VERIFY">CSIDL_FLAG_DONT_VERIFY</a></li>
<li><a href="#Win32_CSIDL_FLAG_MASK">CSIDL_FLAG_MASK</a></li>
<li><a href="#Win32_CSIDL_FLAG_NO_ALIAS">CSIDL_FLAG_NO_ALIAS</a></li>
<li><a href="#Win32_CSIDL_FLAG_PER_USER_INIT">CSIDL_FLAG_PER_USER_INIT</a></li>
<li><a href="#Win32_CSIDL_FONTS">CSIDL_FONTS</a></li>
<li><a href="#Win32_CSIDL_HISTORY">CSIDL_HISTORY</a></li>
<li><a href="#Win32_CSIDL_INTERNET">CSIDL_INTERNET</a></li>
<li><a href="#Win32_CSIDL_INTERNET_CACHE">CSIDL_INTERNET_CACHE</a></li>
<li><a href="#Win32_CSIDL_LOCAL_APPDATA">CSIDL_LOCAL_APPDATA</a></li>
<li><a href="#Win32_CSIDL_MYDOCUMENTS">CSIDL_MYDOCUMENTS</a></li>
<li><a href="#Win32_CSIDL_MYMUSIC">CSIDL_MYMUSIC</a></li>
<li><a href="#Win32_CSIDL_MYPICTURES">CSIDL_MYPICTURES</a></li>
<li><a href="#Win32_CSIDL_MYVIDEO">CSIDL_MYVIDEO</a></li>
<li><a href="#Win32_CSIDL_NETHOOD">CSIDL_NETHOOD</a></li>
<li><a href="#Win32_CSIDL_NETWORK">CSIDL_NETWORK</a></li>
<li><a href="#Win32_CSIDL_PERSONAL">CSIDL_PERSONAL</a></li>
<li><a href="#Win32_CSIDL_PRINTERS">CSIDL_PRINTERS</a></li>
<li><a href="#Win32_CSIDL_PRINTHOOD">CSIDL_PRINTHOOD</a></li>
<li><a href="#Win32_CSIDL_PROFILE">CSIDL_PROFILE</a></li>
<li><a href="#Win32_CSIDL_PROGRAMS">CSIDL_PROGRAMS</a></li>
<li><a href="#Win32_CSIDL_PROGRAM_FILES">CSIDL_PROGRAM_FILES</a></li>
<li><a href="#Win32_CSIDL_PROGRAM_FILESX86">CSIDL_PROGRAM_FILESX86</a></li>
<li><a href="#Win32_CSIDL_PROGRAM_FILES_COMMON">CSIDL_PROGRAM_FILES_COMMON</a></li>
<li><a href="#Win32_CSIDL_PROGRAM_FILES_COMMONX86">CSIDL_PROGRAM_FILES_COMMONX86</a></li>
<li><a href="#Win32_CSIDL_RECENT">CSIDL_RECENT</a></li>
<li><a href="#Win32_CSIDL_RESOURCES">CSIDL_RESOURCES</a></li>
<li><a href="#Win32_CSIDL_RESOURCES_LOCALIZED">CSIDL_RESOURCES_LOCALIZED</a></li>
<li><a href="#Win32_CSIDL_SENDTO">CSIDL_SENDTO</a></li>
<li><a href="#Win32_CSIDL_STARTMENU">CSIDL_STARTMENU</a></li>
<li><a href="#Win32_CSIDL_STARTUP">CSIDL_STARTUP</a></li>
<li><a href="#Win32_CSIDL_SYSTEM">CSIDL_SYSTEM</a></li>
<li><a href="#Win32_CSIDL_SYSTEMX86">CSIDL_SYSTEMX86</a></li>
<li><a href="#Win32_CSIDL_TEMPLATES">CSIDL_TEMPLATES</a></li>
<li><a href="#Win32_CSIDL_WINDOWS">CSIDL_WINDOWS</a></li>
<li><a href="#Win32_CS_BYTEALIGNCLIENT">CS_BYTEALIGNCLIENT</a></li>
<li><a href="#Win32_CS_BYTEALIGNWINDOW">CS_BYTEALIGNWINDOW</a></li>
<li><a href="#Win32_CS_CLASSDC">CS_CLASSDC</a></li>
<li><a href="#Win32_CS_DBLCLKS">CS_DBLCLKS</a></li>
<li><a href="#Win32_CS_DROPSHADOW">CS_DROPSHADOW</a></li>
<li><a href="#Win32_CS_GLOBALCLASS">CS_GLOBALCLASS</a></li>
<li><a href="#Win32_CS_HREDRAW">CS_HREDRAW</a></li>
<li><a href="#Win32_CS_IME">CS_IME</a></li>
<li><a href="#Win32_CS_NOCLOSE">CS_NOCLOSE</a></li>
<li><a href="#Win32_CS_OWNDC">CS_OWNDC</a></li>
<li><a href="#Win32_CS_PARENTDC">CS_PARENTDC</a></li>
<li><a href="#Win32_CS_SAVEBITS">CS_SAVEBITS</a></li>
<li><a href="#Win32_CS_VREDRAW">CS_VREDRAW</a></li>
<li><a href="#Win32_CWP_ALL">CWP_ALL</a></li>
<li><a href="#Win32_CWP_SKIPDISABLED">CWP_SKIPDISABLED</a></li>
<li><a href="#Win32_CWP_SKIPINVISIBLE">CWP_SKIPINVISIBLE</a></li>
<li><a href="#Win32_CWP_SKIPTRANSPARENT">CWP_SKIPTRANSPARENT</a></li>
<li><a href="#Win32_CW_USEDEFAULT">CW_USEDEFAULT</a></li>
<li><a href="#Win32_DEFAULT_MINIMUM_ENTITIES">DEFAULT_MINIMUM_ENTITIES</a></li>
<li><a href="#Win32_DM_POINTERHITTEST">DM_POINTERHITTEST</a></li>
<li><a href="#Win32_ERROR_DEVICE_NOT_CONNECTED">ERROR_DEVICE_NOT_CONNECTED</a></li>
<li><a href="#Win32_ERROR_FILE_NOT_FOUND">ERROR_FILE_NOT_FOUND</a></li>
<li><a href="#Win32_ERROR_IO_PENDING">ERROR_IO_PENDING</a></li>
<li><a href="#Win32_ERROR_NO_MORE_FILES">ERROR_NO_MORE_FILES</a></li>
<li><a href="#Win32_ERROR_SUCCESS">ERROR_SUCCESS</a></li>
<li><a href="#Win32_FALSE">FALSE</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_ARCHIVE">FILE_ATTRIBUTE_ARCHIVE</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_COMPRESSED">FILE_ATTRIBUTE_COMPRESSED</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_DEVICE">FILE_ATTRIBUTE_DEVICE</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_DIRECTORY">FILE_ATTRIBUTE_DIRECTORY</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_EA">FILE_ATTRIBUTE_EA</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_ENCRYPTED">FILE_ATTRIBUTE_ENCRYPTED</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_HIDDEN">FILE_ATTRIBUTE_HIDDEN</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_INTEGRITY_STREAM">FILE_ATTRIBUTE_INTEGRITY_STREAM</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_NORMAL">FILE_ATTRIBUTE_NORMAL</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_NOT_CONTENT_INDEXED">FILE_ATTRIBUTE_NOT_CONTENT_INDEXED</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_NO_SCRUB_DATA">FILE_ATTRIBUTE_NO_SCRUB_DATA</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_OFFLINE">FILE_ATTRIBUTE_OFFLINE</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_PINNED">FILE_ATTRIBUTE_PINNED</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_READONLY">FILE_ATTRIBUTE_READONLY</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS">FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_RECALL_ON_OPEN">FILE_ATTRIBUTE_RECALL_ON_OPEN</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_REPARSE_POINT">FILE_ATTRIBUTE_REPARSE_POINT</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_SPARSE_FILE">FILE_ATTRIBUTE_SPARSE_FILE</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_SYSTEM">FILE_ATTRIBUTE_SYSTEM</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_TEMPORARY">FILE_ATTRIBUTE_TEMPORARY</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_UNPINNED">FILE_ATTRIBUTE_UNPINNED</a></li>
<li><a href="#Win32_FILE_ATTRIBUTE_VIRTUAL">FILE_ATTRIBUTE_VIRTUAL</a></li>
<li><a href="#Win32_FILE_BEGIN">FILE_BEGIN</a></li>
<li><a href="#Win32_FILE_CURRENT">FILE_CURRENT</a></li>
<li><a href="#Win32_FILE_END">FILE_END</a></li>
<li><a href="#Win32_FILE_READ_ATTRIBUTES">FILE_READ_ATTRIBUTES</a></li>
<li><a href="#Win32_FILE_SHARE_DELETE">FILE_SHARE_DELETE</a></li>
<li><a href="#Win32_FILE_SHARE_READ">FILE_SHARE_READ</a></li>
<li><a href="#Win32_FILE_SHARE_WRITE">FILE_SHARE_WRITE</a></li>
<li><a href="#Win32_FILE_WRITE_ATTRIBUTES">FILE_WRITE_ATTRIBUTES</a></li>
<li><a href="#Win32_FOREGROUND_BLUE">FOREGROUND_BLUE</a></li>
<li><a href="#Win32_FOREGROUND_GREEN">FOREGROUND_GREEN</a></li>
<li><a href="#Win32_FOREGROUND_INTENSITY">FOREGROUND_INTENSITY</a></li>
<li><a href="#Win32_FOREGROUND_MASK">FOREGROUND_MASK</a></li>
<li><a href="#Win32_FOREGROUND_RED">FOREGROUND_RED</a></li>
<li><a href="#Win32_FORMAT_MESSAGE_ALLOCATE_BUFFER">FORMAT_MESSAGE_ALLOCATE_BUFFER</a></li>
<li><a href="#Win32_FORMAT_MESSAGE_ARGUMENT_ARRAY">FORMAT_MESSAGE_ARGUMENT_ARRAY</a></li>
<li><a href="#Win32_FORMAT_MESSAGE_FROM_HMODULE">FORMAT_MESSAGE_FROM_HMODULE</a></li>
<li><a href="#Win32_FORMAT_MESSAGE_FROM_STRING">FORMAT_MESSAGE_FROM_STRING</a></li>
<li><a href="#Win32_FORMAT_MESSAGE_FROM_SYSTEM">FORMAT_MESSAGE_FROM_SYSTEM</a></li>
<li><a href="#Win32_FORMAT_MESSAGE_IGNORE_INSERTS">FORMAT_MESSAGE_IGNORE_INSERTS</a></li>
<li><a href="#Win32_FORMAT_MESSAGE_MAX_WIDTH_MASK">FORMAT_MESSAGE_MAX_WIDTH_MASK</a></li>
<li><a href="#Win32_GENERIC_ALL">GENERIC_ALL</a></li>
<li><a href="#Win32_GENERIC_EXECUTE">GENERIC_EXECUTE</a></li>
<li><a href="#Win32_GENERIC_READ">GENERIC_READ</a></li>
<li><a href="#Win32_GENERIC_WRITE">GENERIC_WRITE</a></li>
<li><a href="#Win32_GMEM_DDESHARE">GMEM_DDESHARE</a></li>
<li><a href="#Win32_GMEM_DISCARDABLE">GMEM_DISCARDABLE</a></li>
<li><a href="#Win32_GMEM_FIXED">GMEM_FIXED</a></li>
<li><a href="#Win32_GMEM_INVALID_HANDLE">GMEM_INVALID_HANDLE</a></li>
<li><a href="#Win32_GMEM_LOWER">GMEM_LOWER</a></li>
<li><a href="#Win32_GMEM_MODIFY">GMEM_MODIFY</a></li>
<li><a href="#Win32_GMEM_MOVEABLE">GMEM_MOVEABLE</a></li>
<li><a href="#Win32_GMEM_NOCOMPACT">GMEM_NOCOMPACT</a></li>
<li><a href="#Win32_GMEM_NODISCARD">GMEM_NODISCARD</a></li>
<li><a href="#Win32_GMEM_NOTIFY">GMEM_NOTIFY</a></li>
<li><a href="#Win32_GMEM_NOT_BANKED">GMEM_NOT_BANKED</a></li>
<li><a href="#Win32_GMEM_SHARE">GMEM_SHARE</a></li>
<li><a href="#Win32_GMEM_VALID_FLAGS">GMEM_VALID_FLAGS</a></li>
<li><a href="#Win32_GMEM_ZEROINIT">GMEM_ZEROINIT</a></li>
<li><a href="#Win32_GWLP_HINSTANCE">GWLP_HINSTANCE</a></li>
<li><a href="#Win32_GWLP_HWNDPARENT">GWLP_HWNDPARENT</a></li>
<li><a href="#Win32_GWLP_ID">GWLP_ID</a></li>
<li><a href="#Win32_GWLP_USERDATA">GWLP_USERDATA</a></li>
<li><a href="#Win32_GWLP_WNDPROC">GWLP_WNDPROC</a></li>
<li><a href="#Win32_GW_CHILD">GW_CHILD</a></li>
<li><a href="#Win32_GW_ENABLEDPOPUP">GW_ENABLEDPOPUP</a></li>
<li><a href="#Win32_GW_HWNDFIRST">GW_HWNDFIRST</a></li>
<li><a href="#Win32_GW_HWNDLAST">GW_HWNDLAST</a></li>
<li><a href="#Win32_GW_HWNDNEXT">GW_HWNDNEXT</a></li>
<li><a href="#Win32_GW_HWNDPREV">GW_HWNDPREV</a></li>
<li><a href="#Win32_GW_MAX">GW_MAX</a></li>
<li><a href="#Win32_GW_OWNER">GW_OWNER</a></li>
<li><a href="#Win32_HWND_BOTTOM">HWND_BOTTOM</a></li>
<li><a href="#Win32_HWND_NOTOPMOST">HWND_NOTOPMOST</a></li>
<li><a href="#Win32_HWND_TOP">HWND_TOP</a></li>
<li><a href="#Win32_HWND_TOPMOST">HWND_TOPMOST</a></li>
<li><a href="#Win32_IDABORT">IDABORT</a></li>
<li><a href="#Win32_IDCANCEL">IDCANCEL</a></li>
<li><a href="#Win32_IDCLOSE">IDCLOSE</a></li>
<li><a href="#Win32_IDCONTINUE">IDCONTINUE</a></li>
<li><a href="#Win32_IDC_APPSTARTING">IDC_APPSTARTING</a></li>
<li><a href="#Win32_IDC_ARROW">IDC_ARROW</a></li>
<li><a href="#Win32_IDC_CROSS">IDC_CROSS</a></li>
<li><a href="#Win32_IDC_HAND">IDC_HAND</a></li>
<li><a href="#Win32_IDC_HELP">IDC_HELP</a></li>
<li><a href="#Win32_IDC_IBEAM">IDC_IBEAM</a></li>
<li><a href="#Win32_IDC_ICON">IDC_ICON</a></li>
<li><a href="#Win32_IDC_NO">IDC_NO</a></li>
<li><a href="#Win32_IDC_PERSON">IDC_PERSON</a></li>
<li><a href="#Win32_IDC_PIN">IDC_PIN</a></li>
<li><a href="#Win32_IDC_SIZE">IDC_SIZE</a></li>
<li><a href="#Win32_IDC_SIZEALL">IDC_SIZEALL</a></li>
<li><a href="#Win32_IDC_SIZENESW">IDC_SIZENESW</a></li>
<li><a href="#Win32_IDC_SIZENS">IDC_SIZENS</a></li>
<li><a href="#Win32_IDC_SIZENWSE">IDC_SIZENWSE</a></li>
<li><a href="#Win32_IDC_SIZEWE">IDC_SIZEWE</a></li>
<li><a href="#Win32_IDC_UPARROW">IDC_UPARROW</a></li>
<li><a href="#Win32_IDC_WAIT">IDC_WAIT</a></li>
<li><a href="#Win32_IDHELP">IDHELP</a></li>
<li><a href="#Win32_IDIGNORE">IDIGNORE</a></li>
<li><a href="#Win32_IDNO">IDNO</a></li>
<li><a href="#Win32_IDOK">IDOK</a></li>
<li><a href="#Win32_IDRETRY">IDRETRY</a></li>
<li><a href="#Win32_IDTIMEOUT">IDTIMEOUT</a></li>
<li><a href="#Win32_IDTRYAGAIN">IDTRYAGAIN</a></li>
<li><a href="#Win32_IDYES">IDYES</a></li>
<li><a href="#Win32_INFINITE">INFINITE</a></li>
<li><a href="#Win32_INVALID_FILE_ATTRIBUTES">INVALID_FILE_ATTRIBUTES</a></li>
<li><a href="#Win32_INVALID_FILE_SIZE">INVALID_FILE_SIZE</a></li>
<li><a href="#Win32_INVALID_HANDLE_VALUE">INVALID_HANDLE_VALUE</a></li>
<li><a href="#Win32_INVALID_SET_FILE_POINTER">INVALID_SET_FILE_POINTER</a></li>
<li><a href="#Win32_LANG_ENGLISH">LANG_ENGLISH</a></li>
<li><a href="#Win32_LWA_ALPHA">LWA_ALPHA</a></li>
<li><a href="#Win32_LWA_COLORKEY">LWA_COLORKEY</a></li>
<li><a href="#Win32_MAPVK_VK_TO_CHAR">MAPVK_VK_TO_CHAR</a></li>
<li><a href="#Win32_MAPVK_VK_TO_VSC">MAPVK_VK_TO_VSC</a></li>
<li><a href="#Win32_MAPVK_VK_TO_VSC_EX">MAPVK_VK_TO_VSC_EX</a></li>
<li><a href="#Win32_MAPVK_VSC_TO_VK">MAPVK_VSC_TO_VK</a></li>
<li><a href="#Win32_MAPVK_VSC_TO_VK_EX">MAPVK_VSC_TO_VK_EX</a></li>
<li><a href="#Win32_MAXIMUM_WAIT_OBJECTS">MAXIMUM_WAIT_OBJECTS</a></li>
<li><a href="#Win32_MAX_ADAPTER_ADDRESS_LENGTH">MAX_ADAPTER_ADDRESS_LENGTH</a></li>
<li><a href="#Win32_MAX_ADAPTER_DESCRIPTION_LENGTH">MAX_ADAPTER_DESCRIPTION_LENGTH</a></li>
<li><a href="#Win32_MAX_ADAPTER_NAME_LENGTH">MAX_ADAPTER_NAME_LENGTH</a></li>
<li><a href="#Win32_MAX_DHCPV6_DUID_LENGTH">MAX_DHCPV6_DUID_LENGTH</a></li>
<li><a href="#Win32_MAX_DNS_SUFFIX_STRING_LENGTH">MAX_DNS_SUFFIX_STRING_LENGTH</a></li>
<li><a href="#Win32_MAX_DOMAIN_NAME_LEN">MAX_DOMAIN_NAME_LEN</a></li>
<li><a href="#Win32_MAX_HOSTNAME_LEN">MAX_HOSTNAME_LEN</a></li>
<li><a href="#Win32_MAX_PATH">MAX_PATH</a></li>
<li><a href="#Win32_MAX_SCOPE_ID_LEN">MAX_SCOPE_ID_LEN</a></li>
<li><a href="#Win32_MB_ABORTRETRYIGNORE">MB_ABORTRETRYIGNORE</a></li>
<li><a href="#Win32_MB_APPLMODAL">MB_APPLMODAL</a></li>
<li><a href="#Win32_MB_CANCELTRYCONTINUE">MB_CANCELTRYCONTINUE</a></li>
<li><a href="#Win32_MB_DEFAULT_DESKTOP_ONLY">MB_DEFAULT_DESKTOP_ONLY</a></li>
<li><a href="#Win32_MB_DEFBUTTON1">MB_DEFBUTTON1</a></li>
<li><a href="#Win32_MB_DEFBUTTON2">MB_DEFBUTTON2</a></li>
<li><a href="#Win32_MB_DEFBUTTON3">MB_DEFBUTTON3</a></li>
<li><a href="#Win32_MB_DEFBUTTON4">MB_DEFBUTTON4</a></li>
<li><a href="#Win32_MB_HELP">MB_HELP</a></li>
<li><a href="#Win32_MB_ICONASTERISK">MB_ICONASTERISK</a></li>
<li><a href="#Win32_MB_ICONERROR">MB_ICONERROR</a></li>
<li><a href="#Win32_MB_ICONEXCLAMATION">MB_ICONEXCLAMATION</a></li>
<li><a href="#Win32_MB_ICONHAND">MB_ICONHAND</a></li>
<li><a href="#Win32_MB_ICONINFORMATION">MB_ICONINFORMATION</a></li>
<li><a href="#Win32_MB_ICONQUESTION">MB_ICONQUESTION</a></li>
<li><a href="#Win32_MB_ICONSTOP">MB_ICONSTOP</a></li>
<li><a href="#Win32_MB_ICONWARNING">MB_ICONWARNING</a></li>
<li><a href="#Win32_MB_NOFOCUS">MB_NOFOCUS</a></li>
<li><a href="#Win32_MB_OK">MB_OK</a></li>
<li><a href="#Win32_MB_OKCANCEL">MB_OKCANCEL</a></li>
<li><a href="#Win32_MB_RETRYCANCEL">MB_RETRYCANCEL</a></li>
<li><a href="#Win32_MB_RIGHT">MB_RIGHT</a></li>
<li><a href="#Win32_MB_RTLREADING">MB_RTLREADING</a></li>
<li><a href="#Win32_MB_SETFOREGROUND">MB_SETFOREGROUND</a></li>
<li><a href="#Win32_MB_SYSTEMMODAL">MB_SYSTEMMODAL</a></li>
<li><a href="#Win32_MB_TASKMODAL">MB_TASKMODAL</a></li>
<li><a href="#Win32_MB_TOPMOST">MB_TOPMOST</a></li>
<li><a href="#Win32_MB_USERICON">MB_USERICON</a></li>
<li><a href="#Win32_MB_YESNO">MB_YESNO</a></li>
<li><a href="#Win32_MB_YESNOCANCEL">MB_YESNOCANCEL</a></li>
<li><a href="#Win32_MN_GETHMENU">MN_GETHMENU</a></li>
<li><a href="#Win32_MOD_ALT">MOD_ALT</a></li>
<li><a href="#Win32_MOD_CONTROL">MOD_CONTROL</a></li>
<li><a href="#Win32_MOD_NOREPEAT">MOD_NOREPEAT</a></li>
<li><a href="#Win32_MOD_SHIFT">MOD_SHIFT</a></li>
<li><a href="#Win32_MOD_WIN">MOD_WIN</a></li>
<li><a href="#Win32_MONITOR_DEFAULTTONEAREST">MONITOR_DEFAULTTONEAREST</a></li>
<li><a href="#Win32_MONITOR_DEFAULTTONULL">MONITOR_DEFAULTTONULL</a></li>
<li><a href="#Win32_MONITOR_DEFAULTTOPRIMARY">MONITOR_DEFAULTTOPRIMARY</a></li>
<li><a href="#Win32_NIF_GUID">NIF_GUID</a></li>
<li><a href="#Win32_NIF_ICON">NIF_ICON</a></li>
<li><a href="#Win32_NIF_INFO">NIF_INFO</a></li>
<li><a href="#Win32_NIF_MESSAGE">NIF_MESSAGE</a></li>
<li><a href="#Win32_NIF_REALTIME">NIF_REALTIME</a></li>
<li><a href="#Win32_NIF_SHOWTIP">NIF_SHOWTIP</a></li>
<li><a href="#Win32_NIF_STATE">NIF_STATE</a></li>
<li><a href="#Win32_NIF_TIP">NIF_TIP</a></li>
<li><a href="#Win32_NIM_ADD">NIM_ADD</a></li>
<li><a href="#Win32_NIM_DELETE">NIM_DELETE</a></li>
<li><a href="#Win32_NIM_MODIFY">NIM_MODIFY</a></li>
<li><a href="#Win32_NIM_SETFOCUS">NIM_SETFOCUS</a></li>
<li><a href="#Win32_NIM_SETVERSION">NIM_SETVERSION</a></li>
<li><a href="#Win32_OPEN_ALWAYS">OPEN_ALWAYS</a></li>
<li><a href="#Win32_OPEN_EXISTING">OPEN_EXISTING</a></li>
<li><a href="#Win32_PM_NOREMOVE">PM_NOREMOVE</a></li>
<li><a href="#Win32_PM_NOYIELD">PM_NOYIELD</a></li>
<li><a href="#Win32_PM_REMOVE">PM_REMOVE</a></li>
<li><a href="#Win32_SC_ARRANGE">SC_ARRANGE</a></li>
<li><a href="#Win32_SC_CLOSE">SC_CLOSE</a></li>
<li><a href="#Win32_SC_CONTEXTHELP">SC_CONTEXTHELP</a></li>
<li><a href="#Win32_SC_DEFAULT">SC_DEFAULT</a></li>
<li><a href="#Win32_SC_HOTKEY">SC_HOTKEY</a></li>
<li><a href="#Win32_SC_HSCROLL">SC_HSCROLL</a></li>
<li><a href="#Win32_SC_KEYMENU">SC_KEYMENU</a></li>
<li><a href="#Win32_SC_MAXIMIZE">SC_MAXIMIZE</a></li>
<li><a href="#Win32_SC_MINIMIZE">SC_MINIMIZE</a></li>
<li><a href="#Win32_SC_MONITORPOWER">SC_MONITORPOWER</a></li>
<li><a href="#Win32_SC_MOUSEMENU">SC_MOUSEMENU</a></li>
<li><a href="#Win32_SC_MOVE">SC_MOVE</a></li>
<li><a href="#Win32_SC_NEXTWINDOW">SC_NEXTWINDOW</a></li>
<li><a href="#Win32_SC_PREVWINDOW">SC_PREVWINDOW</a></li>
<li><a href="#Win32_SC_RESTORE">SC_RESTORE</a></li>
<li><a href="#Win32_SC_SCREENSAVE">SC_SCREENSAVE</a></li>
<li><a href="#Win32_SC_SEPARATOR">SC_SEPARATOR</a></li>
<li><a href="#Win32_SC_SIZE">SC_SIZE</a></li>
<li><a href="#Win32_SC_TASKLIST">SC_TASKLIST</a></li>
<li><a href="#Win32_SC_VSCROLL">SC_VSCROLL</a></li>
<li><a href="#Win32_SHGFI_ADDOVERLAYS">SHGFI_ADDOVERLAYS</a></li>
<li><a href="#Win32_SHGFI_ATTRIBUTES">SHGFI_ATTRIBUTES</a></li>
<li><a href="#Win32_SHGFI_ATTR_SPECIFIED">SHGFI_ATTR_SPECIFIED</a></li>
<li><a href="#Win32_SHGFI_DISPLAYNAME">SHGFI_DISPLAYNAME</a></li>
<li><a href="#Win32_SHGFI_EXETYPE">SHGFI_EXETYPE</a></li>
<li><a href="#Win32_SHGFI_ICON">SHGFI_ICON</a></li>
<li><a href="#Win32_SHGFI_ICONLOCATION">SHGFI_ICONLOCATION</a></li>
<li><a href="#Win32_SHGFI_LARGEICON">SHGFI_LARGEICON</a></li>
<li><a href="#Win32_SHGFI_LINKOVERLAY">SHGFI_LINKOVERLAY</a></li>
<li><a href="#Win32_SHGFI_OPENICON">SHGFI_OPENICON</a></li>
<li><a href="#Win32_SHGFI_OVERLAYINDEX">SHGFI_OVERLAYINDEX</a></li>
<li><a href="#Win32_SHGFI_PIDL">SHGFI_PIDL</a></li>
<li><a href="#Win32_SHGFI_SELECTED">SHGFI_SELECTED</a></li>
<li><a href="#Win32_SHGFI_SHELLICONSIZE">SHGFI_SHELLICONSIZE</a></li>
<li><a href="#Win32_SHGFI_SMALLICON">SHGFI_SMALLICON</a></li>
<li><a href="#Win32_SHGFI_SYSICONINDEX">SHGFI_SYSICONINDEX</a></li>
<li><a href="#Win32_SHGFI_TYPENAME">SHGFI_TYPENAME</a></li>
<li><a href="#Win32_SHGFI_USEFILEATTRIBUTES">SHGFI_USEFILEATTRIBUTES</a></li>
<li><a href="#Win32_SHGSI_ICON">SHGSI_ICON</a></li>
<li><a href="#Win32_SHGSI_ICONLOCATION">SHGSI_ICONLOCATION</a></li>
<li><a href="#Win32_SHGSI_LARGEICON">SHGSI_LARGEICON</a></li>
<li><a href="#Win32_SHGSI_LINKOVERLAY">SHGSI_LINKOVERLAY</a></li>
<li><a href="#Win32_SHGSI_SELECTED">SHGSI_SELECTED</a></li>
<li><a href="#Win32_SHGSI_SHELLICONSIZE">SHGSI_SHELLICONSIZE</a></li>
<li><a href="#Win32_SHGSI_SMALLICON">SHGSI_SMALLICON</a></li>
<li><a href="#Win32_SHGSI_SYSICONINDEX">SHGSI_SYSICONINDEX</a></li>
<li><a href="#Win32_SM_ARRANGE">SM_ARRANGE</a></li>
<li><a href="#Win32_SM_CARETBLINKINGENABLED">SM_CARETBLINKINGENABLED</a></li>
<li><a href="#Win32_SM_CLEANBOOT">SM_CLEANBOOT</a></li>
<li><a href="#Win32_SM_CMETRICS">SM_CMETRICS</a></li>
<li><a href="#Win32_SM_CMONITORS">SM_CMONITORS</a></li>
<li><a href="#Win32_SM_CMOUSEBUTTONS">SM_CMOUSEBUTTONS</a></li>
<li><a href="#Win32_SM_CONVERTIBLESLATEMODE">SM_CONVERTIBLESLATEMODE</a></li>
<li><a href="#Win32_SM_CXBORDER">SM_CXBORDER</a></li>
<li><a href="#Win32_SM_CXCURSOR">SM_CXCURSOR</a></li>
<li><a href="#Win32_SM_CXDLGFRAME">SM_CXDLGFRAME</a></li>
<li><a href="#Win32_SM_CXDOUBLECLK">SM_CXDOUBLECLK</a></li>
<li><a href="#Win32_SM_CXDRAG">SM_CXDRAG</a></li>
<li><a href="#Win32_SM_CXEDGE">SM_CXEDGE</a></li>
<li><a href="#Win32_SM_CXFIXEDFRAME">SM_CXFIXEDFRAME</a></li>
<li><a href="#Win32_SM_CXFOCUSBORDER">SM_CXFOCUSBORDER</a></li>
<li><a href="#Win32_SM_CXFRAME">SM_CXFRAME</a></li>
<li><a href="#Win32_SM_CXFULLSCREEN">SM_CXFULLSCREEN</a></li>
<li><a href="#Win32_SM_CXHSCROLL">SM_CXHSCROLL</a></li>
<li><a href="#Win32_SM_CXHTHUMB">SM_CXHTHUMB</a></li>
<li><a href="#Win32_SM_CXICON">SM_CXICON</a></li>
<li><a href="#Win32_SM_CXICONSPACING">SM_CXICONSPACING</a></li>
<li><a href="#Win32_SM_CXMAXIMIZED">SM_CXMAXIMIZED</a></li>
<li><a href="#Win32_SM_CXMAXTRACK">SM_CXMAXTRACK</a></li>
<li><a href="#Win32_SM_CXMENUCHECK">SM_CXMENUCHECK</a></li>
<li><a href="#Win32_SM_CXMENUSIZE">SM_CXMENUSIZE</a></li>
<li><a href="#Win32_SM_CXMIN">SM_CXMIN</a></li>
<li><a href="#Win32_SM_CXMINIMIZED">SM_CXMINIMIZED</a></li>
<li><a href="#Win32_SM_CXMINSPACING">SM_CXMINSPACING</a></li>
<li><a href="#Win32_SM_CXMINTRACK">SM_CXMINTRACK</a></li>
<li><a href="#Win32_SM_CXPADDEDBORDER">SM_CXPADDEDBORDER</a></li>
<li><a href="#Win32_SM_CXSCREEN">SM_CXSCREEN</a></li>
<li><a href="#Win32_SM_CXSIZE">SM_CXSIZE</a></li>
<li><a href="#Win32_SM_CXSIZEFRAME">SM_CXSIZEFRAME</a></li>
<li><a href="#Win32_SM_CXSMICON">SM_CXSMICON</a></li>
<li><a href="#Win32_SM_CXSMSIZE">SM_CXSMSIZE</a></li>
<li><a href="#Win32_SM_CXVIRTUALSCREEN">SM_CXVIRTUALSCREEN</a></li>
<li><a href="#Win32_SM_CXVSCROLL">SM_CXVSCROLL</a></li>
<li><a href="#Win32_SM_CYBORDER">SM_CYBORDER</a></li>
<li><a href="#Win32_SM_CYCAPTION">SM_CYCAPTION</a></li>
<li><a href="#Win32_SM_CYCURSOR">SM_CYCURSOR</a></li>
<li><a href="#Win32_SM_CYDLGFRAME">SM_CYDLGFRAME</a></li>
<li><a href="#Win32_SM_CYDOUBLECLK">SM_CYDOUBLECLK</a></li>
<li><a href="#Win32_SM_CYDRAG">SM_CYDRAG</a></li>
<li><a href="#Win32_SM_CYEDGE">SM_CYEDGE</a></li>
<li><a href="#Win32_SM_CYFIXEDFRAME">SM_CYFIXEDFRAME</a></li>
<li><a href="#Win32_SM_CYFOCUSBORDER">SM_CYFOCUSBORDER</a></li>
<li><a href="#Win32_SM_CYFRAME">SM_CYFRAME</a></li>
<li><a href="#Win32_SM_CYFULLSCREEN">SM_CYFULLSCREEN</a></li>
<li><a href="#Win32_SM_CYHSCROLL">SM_CYHSCROLL</a></li>
<li><a href="#Win32_SM_CYICON">SM_CYICON</a></li>
<li><a href="#Win32_SM_CYICONSPACING">SM_CYICONSPACING</a></li>
<li><a href="#Win32_SM_CYKANJIWINDOW">SM_CYKANJIWINDOW</a></li>
<li><a href="#Win32_SM_CYMAXIMIZED">SM_CYMAXIMIZED</a></li>
<li><a href="#Win32_SM_CYMAXTRACK">SM_CYMAXTRACK</a></li>
<li><a href="#Win32_SM_CYMENU">SM_CYMENU</a></li>
<li><a href="#Win32_SM_CYMENUCHECK">SM_CYMENUCHECK</a></li>
<li><a href="#Win32_SM_CYMENUSIZE">SM_CYMENUSIZE</a></li>
<li><a href="#Win32_SM_CYMIN">SM_CYMIN</a></li>
<li><a href="#Win32_SM_CYMINIMIZED">SM_CYMINIMIZED</a></li>
<li><a href="#Win32_SM_CYMINSPACING">SM_CYMINSPACING</a></li>
<li><a href="#Win32_SM_CYMINTRACK">SM_CYMINTRACK</a></li>
<li><a href="#Win32_SM_CYSCREEN">SM_CYSCREEN</a></li>
<li><a href="#Win32_SM_CYSIZE">SM_CYSIZE</a></li>
<li><a href="#Win32_SM_CYSIZEFRAME">SM_CYSIZEFRAME</a></li>
<li><a href="#Win32_SM_CYSMCAPTION">SM_CYSMCAPTION</a></li>
<li><a href="#Win32_SM_CYSMICON">SM_CYSMICON</a></li>
<li><a href="#Win32_SM_CYSMSIZE">SM_CYSMSIZE</a></li>
<li><a href="#Win32_SM_CYVIRTUALSCREEN">SM_CYVIRTUALSCREEN</a></li>
<li><a href="#Win32_SM_CYVSCROLL">SM_CYVSCROLL</a></li>
<li><a href="#Win32_SM_CYVTHUMB">SM_CYVTHUMB</a></li>
<li><a href="#Win32_SM_DBCSENABLED">SM_DBCSENABLED</a></li>
<li><a href="#Win32_SM_DEBUG">SM_DEBUG</a></li>
<li><a href="#Win32_SM_DIGITIZER">SM_DIGITIZER</a></li>
<li><a href="#Win32_SM_IMMENABLED">SM_IMMENABLED</a></li>
<li><a href="#Win32_SM_MAXIMUMTOUCHES">SM_MAXIMUMTOUCHES</a></li>
<li><a href="#Win32_SM_MEDIACENTER">SM_MEDIACENTER</a></li>
<li><a href="#Win32_SM_MENUDROPALIGNMENT">SM_MENUDROPALIGNMENT</a></li>
<li><a href="#Win32_SM_MIDEASTENABLED">SM_MIDEASTENABLED</a></li>
<li><a href="#Win32_SM_MOUSEHORIZONTALWHEELPRESENT">SM_MOUSEHORIZONTALWHEELPRESENT</a></li>
<li><a href="#Win32_SM_MOUSEPRESENT">SM_MOUSEPRESENT</a></li>
<li><a href="#Win32_SM_MOUSEWHEELPRESENT">SM_MOUSEWHEELPRESENT</a></li>
<li><a href="#Win32_SM_NETWORK">SM_NETWORK</a></li>
<li><a href="#Win32_SM_PENWINDOWS">SM_PENWINDOWS</a></li>
<li><a href="#Win32_SM_REMOTECONTROL">SM_REMOTECONTROL</a></li>
<li><a href="#Win32_SM_REMOTESESSION">SM_REMOTESESSION</a></li>
<li><a href="#Win32_SM_RESERVED1">SM_RESERVED1</a></li>
<li><a href="#Win32_SM_RESERVED2">SM_RESERVED2</a></li>
<li><a href="#Win32_SM_RESERVED3">SM_RESERVED3</a></li>
<li><a href="#Win32_SM_RESERVED4">SM_RESERVED4</a></li>
<li><a href="#Win32_SM_SAMEDISPLAYFORMAT">SM_SAMEDISPLAYFORMAT</a></li>
<li><a href="#Win32_SM_SECURE">SM_SECURE</a></li>
<li><a href="#Win32_SM_SERVERR2">SM_SERVERR2</a></li>
<li><a href="#Win32_SM_SHOWSOUNDS">SM_SHOWSOUNDS</a></li>
<li><a href="#Win32_SM_SHUTTINGDOWN">SM_SHUTTINGDOWN</a></li>
<li><a href="#Win32_SM_SLOWMACHINE">SM_SLOWMACHINE</a></li>
<li><a href="#Win32_SM_STARTER">SM_STARTER</a></li>
<li><a href="#Win32_SM_SWAPBUTTON">SM_SWAPBUTTON</a></li>
<li><a href="#Win32_SM_SYSTEMDOCKED">SM_SYSTEMDOCKED</a></li>
<li><a href="#Win32_SM_TABLETPC">SM_TABLETPC</a></li>
<li><a href="#Win32_SM_XVIRTUALSCREEN">SM_XVIRTUALSCREEN</a></li>
<li><a href="#Win32_SM_YVIRTUALSCREEN">SM_YVIRTUALSCREEN</a></li>
<li><a href="#Win32_SRWLOCK_INIT">SRWLOCK_INIT</a></li>
<li><a href="#Win32_STD_INPUT_HANDLE">STD_INPUT_HANDLE</a></li>
<li><a href="#Win32_STD_OUTPUT_HANDLE">STD_OUTPUT_HANDLE</a></li>
<li><a href="#Win32_SUBLANG_DEFAULT">SUBLANG_DEFAULT</a></li>
<li><a href="#Win32_SWP_ASYNCWINDOWPOS">SWP_ASYNCWINDOWPOS</a></li>
<li><a href="#Win32_SWP_DEFERERASE">SWP_DEFERERASE</a></li>
<li><a href="#Win32_SWP_DRAWFRAME">SWP_DRAWFRAME</a></li>
<li><a href="#Win32_SWP_FRAMECHANGED">SWP_FRAMECHANGED</a></li>
<li><a href="#Win32_SWP_HIDEWINDOW">SWP_HIDEWINDOW</a></li>
<li><a href="#Win32_SWP_NOACTIVATE">SWP_NOACTIVATE</a></li>
<li><a href="#Win32_SWP_NOCOPYBITS">SWP_NOCOPYBITS</a></li>
<li><a href="#Win32_SWP_NOMOVE">SWP_NOMOVE</a></li>
<li><a href="#Win32_SWP_NOOWNERZORDER">SWP_NOOWNERZORDER</a></li>
<li><a href="#Win32_SWP_NOREDRAW">SWP_NOREDRAW</a></li>
<li><a href="#Win32_SWP_NOREPOSITION">SWP_NOREPOSITION</a></li>
<li><a href="#Win32_SWP_NOSENDCHANGING">SWP_NOSENDCHANGING</a></li>
<li><a href="#Win32_SWP_NOSIZE">SWP_NOSIZE</a></li>
<li><a href="#Win32_SWP_NOZORDER">SWP_NOZORDER</a></li>
<li><a href="#Win32_SWP_SHOWWINDOW">SWP_SHOWWINDOW</a></li>
<li><a href="#Win32_SW_FORCEMINIMIZE">SW_FORCEMINIMIZE</a></li>
<li><a href="#Win32_SW_HIDE">SW_HIDE</a></li>
<li><a href="#Win32_SW_MAX">SW_MAX</a></li>
<li><a href="#Win32_SW_MAXIMIZE">SW_MAXIMIZE</a></li>
<li><a href="#Win32_SW_MINIMIZE">SW_MINIMIZE</a></li>
<li><a href="#Win32_SW_NORMAL">SW_NORMAL</a></li>
<li><a href="#Win32_SW_RESTORE">SW_RESTORE</a></li>
<li><a href="#Win32_SW_SHOW">SW_SHOW</a></li>
<li><a href="#Win32_SW_SHOWDEFAULT">SW_SHOWDEFAULT</a></li>
<li><a href="#Win32_SW_SHOWMAXIMIZED">SW_SHOWMAXIMIZED</a></li>
<li><a href="#Win32_SW_SHOWMINIMIZED">SW_SHOWMINIMIZED</a></li>
<li><a href="#Win32_SW_SHOWMINNOACTIVE">SW_SHOWMINNOACTIVE</a></li>
<li><a href="#Win32_SW_SHOWNA">SW_SHOWNA</a></li>
<li><a href="#Win32_SW_SHOWNOACTIVATE">SW_SHOWNOACTIVATE</a></li>
<li><a href="#Win32_SW_SHOWNORMAL">SW_SHOWNORMAL</a></li>
<li><a href="#Win32_S_OK">S_OK</a></li>
<li><a href="#Win32_THREAD_PRIORITY_ABOVE_NORMAL">THREAD_PRIORITY_ABOVE_NORMAL</a></li>
<li><a href="#Win32_THREAD_PRIORITY_BELOW_NORMAL">THREAD_PRIORITY_BELOW_NORMAL</a></li>
<li><a href="#Win32_THREAD_PRIORITY_HIGHEST">THREAD_PRIORITY_HIGHEST</a></li>
<li><a href="#Win32_THREAD_PRIORITY_LOWEST">THREAD_PRIORITY_LOWEST</a></li>
<li><a href="#Win32_THREAD_PRIORITY_NORMAL">THREAD_PRIORITY_NORMAL</a></li>
<li><a href="#Win32_TRUE">TRUE</a></li>
<li><a href="#Win32_TRUNCATE_EXISTING">TRUNCATE_EXISTING</a></li>
<li><a href="#Win32_VK_ACCEPT">VK_ACCEPT</a></li>
<li><a href="#Win32_VK_ADD">VK_ADD</a></li>
<li><a href="#Win32_VK_APPS">VK_APPS</a></li>
<li><a href="#Win32_VK_ATTN">VK_ATTN</a></li>
<li><a href="#Win32_VK_BACK">VK_BACK</a></li>
<li><a href="#Win32_VK_BROWSER_BACK">VK_BROWSER_BACK</a></li>
<li><a href="#Win32_VK_BROWSER_FAVORITES">VK_BROWSER_FAVORITES</a></li>
<li><a href="#Win32_VK_BROWSER_FORWARD">VK_BROWSER_FORWARD</a></li>
<li><a href="#Win32_VK_BROWSER_HOME">VK_BROWSER_HOME</a></li>
<li><a href="#Win32_VK_BROWSER_REFRESH">VK_BROWSER_REFRESH</a></li>
<li><a href="#Win32_VK_BROWSER_SEARCH">VK_BROWSER_SEARCH</a></li>
<li><a href="#Win32_VK_BROWSER_STOP">VK_BROWSER_STOP</a></li>
<li><a href="#Win32_VK_CANCEL">VK_CANCEL</a></li>
<li><a href="#Win32_VK_CAPITAL">VK_CAPITAL</a></li>
<li><a href="#Win32_VK_CLEAR">VK_CLEAR</a></li>
<li><a href="#Win32_VK_CONTROL">VK_CONTROL</a></li>
<li><a href="#Win32_VK_CONVERT">VK_CONVERT</a></li>
<li><a href="#Win32_VK_CRSEL">VK_CRSEL</a></li>
<li><a href="#Win32_VK_DECIMAL">VK_DECIMAL</a></li>
<li><a href="#Win32_VK_DELETE">VK_DELETE</a></li>
<li><a href="#Win32_VK_DIVIDE">VK_DIVIDE</a></li>
<li><a href="#Win32_VK_DOWN">VK_DOWN</a></li>
<li><a href="#Win32_VK_END">VK_END</a></li>
<li><a href="#Win32_VK_EREOF">VK_EREOF</a></li>
<li><a href="#Win32_VK_ESCAPE">VK_ESCAPE</a></li>
<li><a href="#Win32_VK_EXECUTE">VK_EXECUTE</a></li>
<li><a href="#Win32_VK_EXSEL">VK_EXSEL</a></li>
<li><a href="#Win32_VK_F1">VK_F1</a></li>
<li><a href="#Win32_VK_F10">VK_F10</a></li>
<li><a href="#Win32_VK_F11">VK_F11</a></li>
<li><a href="#Win32_VK_F12">VK_F12</a></li>
<li><a href="#Win32_VK_F13">VK_F13</a></li>
<li><a href="#Win32_VK_F14">VK_F14</a></li>
<li><a href="#Win32_VK_F15">VK_F15</a></li>
<li><a href="#Win32_VK_F16">VK_F16</a></li>
<li><a href="#Win32_VK_F17">VK_F17</a></li>
<li><a href="#Win32_VK_F18">VK_F18</a></li>
<li><a href="#Win32_VK_F19">VK_F19</a></li>
<li><a href="#Win32_VK_F2">VK_F2</a></li>
<li><a href="#Win32_VK_F20">VK_F20</a></li>
<li><a href="#Win32_VK_F21">VK_F21</a></li>
<li><a href="#Win32_VK_F22">VK_F22</a></li>
<li><a href="#Win32_VK_F23">VK_F23</a></li>
<li><a href="#Win32_VK_F24">VK_F24</a></li>
<li><a href="#Win32_VK_F3">VK_F3</a></li>
<li><a href="#Win32_VK_F4">VK_F4</a></li>
<li><a href="#Win32_VK_F5">VK_F5</a></li>
<li><a href="#Win32_VK_F6">VK_F6</a></li>
<li><a href="#Win32_VK_F7">VK_F7</a></li>
<li><a href="#Win32_VK_F8">VK_F8</a></li>
<li><a href="#Win32_VK_F9">VK_F9</a></li>
<li><a href="#Win32_VK_FINAL">VK_FINAL</a></li>
<li><a href="#Win32_VK_GAMEPAD_A">VK_GAMEPAD_A</a></li>
<li><a href="#Win32_VK_GAMEPAD_B">VK_GAMEPAD_B</a></li>
<li><a href="#Win32_VK_GAMEPAD_DPAD_DOWN">VK_GAMEPAD_DPAD_DOWN</a></li>
<li><a href="#Win32_VK_GAMEPAD_DPAD_LEFT">VK_GAMEPAD_DPAD_LEFT</a></li>
<li><a href="#Win32_VK_GAMEPAD_DPAD_RIGHT">VK_GAMEPAD_DPAD_RIGHT</a></li>
<li><a href="#Win32_VK_GAMEPAD_DPAD_UP">VK_GAMEPAD_DPAD_UP</a></li>
<li><a href="#Win32_VK_GAMEPAD_LEFT_SHOULDER">VK_GAMEPAD_LEFT_SHOULDER</a></li>
<li><a href="#Win32_VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON">VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON</a></li>
<li><a href="#Win32_VK_GAMEPAD_LEFT_THUMBSTICK_DOWN">VK_GAMEPAD_LEFT_THUMBSTICK_DOWN</a></li>
<li><a href="#Win32_VK_GAMEPAD_LEFT_THUMBSTICK_LEFT">VK_GAMEPAD_LEFT_THUMBSTICK_LEFT</a></li>
<li><a href="#Win32_VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT">VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT</a></li>
<li><a href="#Win32_VK_GAMEPAD_LEFT_THUMBSTICK_UP">VK_GAMEPAD_LEFT_THUMBSTICK_UP</a></li>
<li><a href="#Win32_VK_GAMEPAD_LEFT_TRIGGER">VK_GAMEPAD_LEFT_TRIGGER</a></li>
<li><a href="#Win32_VK_GAMEPAD_MENU">VK_GAMEPAD_MENU</a></li>
<li><a href="#Win32_VK_GAMEPAD_RIGHT_SHOULDER">VK_GAMEPAD_RIGHT_SHOULDER</a></li>
<li><a href="#Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON">VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON</a></li>
<li><a href="#Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN">VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN</a></li>
<li><a href="#Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT">VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT</a></li>
<li><a href="#Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT">VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT</a></li>
<li><a href="#Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_UP">VK_GAMEPAD_RIGHT_THUMBSTICK_UP</a></li>
<li><a href="#Win32_VK_GAMEPAD_RIGHT_TRIGGER">VK_GAMEPAD_RIGHT_TRIGGER</a></li>
<li><a href="#Win32_VK_GAMEPAD_VIEW">VK_GAMEPAD_VIEW</a></li>
<li><a href="#Win32_VK_GAMEPAD_X">VK_GAMEPAD_X</a></li>
<li><a href="#Win32_VK_GAMEPAD_Y">VK_GAMEPAD_Y</a></li>
<li><a href="#Win32_VK_HANGEUL">VK_HANGEUL</a></li>
<li><a href="#Win32_VK_HANGUL">VK_HANGUL</a></li>
<li><a href="#Win32_VK_HANJA">VK_HANJA</a></li>
<li><a href="#Win32_VK_HELP">VK_HELP</a></li>
<li><a href="#Win32_VK_HOME">VK_HOME</a></li>
<li><a href="#Win32_VK_ICO_00">VK_ICO_00</a></li>
<li><a href="#Win32_VK_ICO_CLEAR">VK_ICO_CLEAR</a></li>
<li><a href="#Win32_VK_ICO_HELP">VK_ICO_HELP</a></li>
<li><a href="#Win32_VK_INSERT">VK_INSERT</a></li>
<li><a href="#Win32_VK_JUNJA">VK_JUNJA</a></li>
<li><a href="#Win32_VK_KANA">VK_KANA</a></li>
<li><a href="#Win32_VK_KANJI">VK_KANJI</a></li>
<li><a href="#Win32_VK_LAUNCH_APP1">VK_LAUNCH_APP1</a></li>
<li><a href="#Win32_VK_LAUNCH_APP2">VK_LAUNCH_APP2</a></li>
<li><a href="#Win32_VK_LAUNCH_MAIL">VK_LAUNCH_MAIL</a></li>
<li><a href="#Win32_VK_LAUNCH_MEDIA_SELECT">VK_LAUNCH_MEDIA_SELECT</a></li>
<li><a href="#Win32_VK_LBUTTON">VK_LBUTTON</a></li>
<li><a href="#Win32_VK_LCONTROL">VK_LCONTROL</a></li>
<li><a href="#Win32_VK_LEFT">VK_LEFT</a></li>
<li><a href="#Win32_VK_LMENU">VK_LMENU</a></li>
<li><a href="#Win32_VK_LSHIFT">VK_LSHIFT</a></li>
<li><a href="#Win32_VK_LWIN">VK_LWIN</a></li>
<li><a href="#Win32_VK_MBUTTON">VK_MBUTTON</a></li>
<li><a href="#Win32_VK_MEDIA_NEXT_TRACK">VK_MEDIA_NEXT_TRACK</a></li>
<li><a href="#Win32_VK_MEDIA_PLAY_PAUSE">VK_MEDIA_PLAY_PAUSE</a></li>
<li><a href="#Win32_VK_MEDIA_PREV_TRACK">VK_MEDIA_PREV_TRACK</a></li>
<li><a href="#Win32_VK_MEDIA_STOP">VK_MEDIA_STOP</a></li>
<li><a href="#Win32_VK_MENU">VK_MENU</a></li>
<li><a href="#Win32_VK_MODECHANGE">VK_MODECHANGE</a></li>
<li><a href="#Win32_VK_MULTIPLY">VK_MULTIPLY</a></li>
<li><a href="#Win32_VK_NAVIGATION_ACCEPT">VK_NAVIGATION_ACCEPT</a></li>
<li><a href="#Win32_VK_NAVIGATION_CANCEL">VK_NAVIGATION_CANCEL</a></li>
<li><a href="#Win32_VK_NAVIGATION_DOWN">VK_NAVIGATION_DOWN</a></li>
<li><a href="#Win32_VK_NAVIGATION_LEFT">VK_NAVIGATION_LEFT</a></li>
<li><a href="#Win32_VK_NAVIGATION_MENU">VK_NAVIGATION_MENU</a></li>
<li><a href="#Win32_VK_NAVIGATION_RIGHT">VK_NAVIGATION_RIGHT</a></li>
<li><a href="#Win32_VK_NAVIGATION_UP">VK_NAVIGATION_UP</a></li>
<li><a href="#Win32_VK_NAVIGATION_VIEW">VK_NAVIGATION_VIEW</a></li>
<li><a href="#Win32_VK_NEXT">VK_NEXT</a></li>
<li><a href="#Win32_VK_NONAME">VK_NONAME</a></li>
<li><a href="#Win32_VK_NONCONVERT">VK_NONCONVERT</a></li>
<li><a href="#Win32_VK_NUMLOCK">VK_NUMLOCK</a></li>
<li><a href="#Win32_VK_NUMPAD0">VK_NUMPAD0</a></li>
<li><a href="#Win32_VK_NUMPAD1">VK_NUMPAD1</a></li>
<li><a href="#Win32_VK_NUMPAD2">VK_NUMPAD2</a></li>
<li><a href="#Win32_VK_NUMPAD3">VK_NUMPAD3</a></li>
<li><a href="#Win32_VK_NUMPAD4">VK_NUMPAD4</a></li>
<li><a href="#Win32_VK_NUMPAD5">VK_NUMPAD5</a></li>
<li><a href="#Win32_VK_NUMPAD6">VK_NUMPAD6</a></li>
<li><a href="#Win32_VK_NUMPAD7">VK_NUMPAD7</a></li>
<li><a href="#Win32_VK_NUMPAD8">VK_NUMPAD8</a></li>
<li><a href="#Win32_VK_NUMPAD9">VK_NUMPAD9</a></li>
<li><a href="#Win32_VK_OEM_1">VK_OEM_1</a></li>
<li><a href="#Win32_VK_OEM_102">VK_OEM_102</a></li>
<li><a href="#Win32_VK_OEM_2">VK_OEM_2</a></li>
<li><a href="#Win32_VK_OEM_3">VK_OEM_3</a></li>
<li><a href="#Win32_VK_OEM_4">VK_OEM_4</a></li>
<li><a href="#Win32_VK_OEM_5">VK_OEM_5</a></li>
<li><a href="#Win32_VK_OEM_6">VK_OEM_6</a></li>
<li><a href="#Win32_VK_OEM_7">VK_OEM_7</a></li>
<li><a href="#Win32_VK_OEM_8">VK_OEM_8</a></li>
<li><a href="#Win32_VK_OEM_ATTN">VK_OEM_ATTN</a></li>
<li><a href="#Win32_VK_OEM_AUTO">VK_OEM_AUTO</a></li>
<li><a href="#Win32_VK_OEM_AX">VK_OEM_AX</a></li>
<li><a href="#Win32_VK_OEM_BACKTAB">VK_OEM_BACKTAB</a></li>
<li><a href="#Win32_VK_OEM_CLEAR">VK_OEM_CLEAR</a></li>
<li><a href="#Win32_VK_OEM_COMMA">VK_OEM_COMMA</a></li>
<li><a href="#Win32_VK_OEM_COPY">VK_OEM_COPY</a></li>
<li><a href="#Win32_VK_OEM_CUSEL">VK_OEM_CUSEL</a></li>
<li><a href="#Win32_VK_OEM_ENLW">VK_OEM_ENLW</a></li>
<li><a href="#Win32_VK_OEM_FINISH">VK_OEM_FINISH</a></li>
<li><a href="#Win32_VK_OEM_FJ_JISHO">VK_OEM_FJ_JISHO</a></li>
<li><a href="#Win32_VK_OEM_FJ_LOYA">VK_OEM_FJ_LOYA</a></li>
<li><a href="#Win32_VK_OEM_FJ_MASSHOU">VK_OEM_FJ_MASSHOU</a></li>
<li><a href="#Win32_VK_OEM_FJ_ROYA">VK_OEM_FJ_ROYA</a></li>
<li><a href="#Win32_VK_OEM_FJ_TOUROKU">VK_OEM_FJ_TOUROKU</a></li>
<li><a href="#Win32_VK_OEM_JUMP">VK_OEM_JUMP</a></li>
<li><a href="#Win32_VK_OEM_MINUS">VK_OEM_MINUS</a></li>
<li><a href="#Win32_VK_OEM_NEC_EQUAL">VK_OEM_NEC_EQUAL</a></li>
<li><a href="#Win32_VK_OEM_PA1">VK_OEM_PA1</a></li>
<li><a href="#Win32_VK_OEM_PA2">VK_OEM_PA2</a></li>
<li><a href="#Win32_VK_OEM_PA3">VK_OEM_PA3</a></li>
<li><a href="#Win32_VK_OEM_PERIOD">VK_OEM_PERIOD</a></li>
<li><a href="#Win32_VK_OEM_PLUS">VK_OEM_PLUS</a></li>
<li><a href="#Win32_VK_OEM_RESET">VK_OEM_RESET</a></li>
<li><a href="#Win32_VK_OEM_WSCTRL">VK_OEM_WSCTRL</a></li>
<li><a href="#Win32_VK_PA1">VK_PA1</a></li>
<li><a href="#Win32_VK_PACKET">VK_PACKET</a></li>
<li><a href="#Win32_VK_PAUSE">VK_PAUSE</a></li>
<li><a href="#Win32_VK_PLAY">VK_PLAY</a></li>
<li><a href="#Win32_VK_PRINT">VK_PRINT</a></li>
<li><a href="#Win32_VK_PRIOR">VK_PRIOR</a></li>
<li><a href="#Win32_VK_PROCESSKEY">VK_PROCESSKEY</a></li>
<li><a href="#Win32_VK_RBUTTON">VK_RBUTTON</a></li>
<li><a href="#Win32_VK_RCONTROL">VK_RCONTROL</a></li>
<li><a href="#Win32_VK_RETURN">VK_RETURN</a></li>
<li><a href="#Win32_VK_RIGHT">VK_RIGHT</a></li>
<li><a href="#Win32_VK_RMENU">VK_RMENU</a></li>
<li><a href="#Win32_VK_RSHIFT">VK_RSHIFT</a></li>
<li><a href="#Win32_VK_RWIN">VK_RWIN</a></li>
<li><a href="#Win32_VK_SCROLL">VK_SCROLL</a></li>
<li><a href="#Win32_VK_SELECT">VK_SELECT</a></li>
<li><a href="#Win32_VK_SEPARATOR">VK_SEPARATOR</a></li>
<li><a href="#Win32_VK_SHIFT">VK_SHIFT</a></li>
<li><a href="#Win32_VK_SLEEP">VK_SLEEP</a></li>
<li><a href="#Win32_VK_SNAPSHOT">VK_SNAPSHOT</a></li>
<li><a href="#Win32_VK_SPACE">VK_SPACE</a></li>
<li><a href="#Win32_VK_SUBTRACT">VK_SUBTRACT</a></li>
<li><a href="#Win32_VK_TAB">VK_TAB</a></li>
<li><a href="#Win32_VK_UP">VK_UP</a></li>
<li><a href="#Win32_VK_VOLUME_DOWN">VK_VOLUME_DOWN</a></li>
<li><a href="#Win32_VK_VOLUME_MUTE">VK_VOLUME_MUTE</a></li>
<li><a href="#Win32_VK_VOLUME_UP">VK_VOLUME_UP</a></li>
<li><a href="#Win32_VK_XBUTTON1">VK_XBUTTON1</a></li>
<li><a href="#Win32_VK_XBUTTON2">VK_XBUTTON2</a></li>
<li><a href="#Win32_VK_ZOOM">VK_ZOOM</a></li>
<li><a href="#Win32_WAIT_ABANDONED">WAIT_ABANDONED</a></li>
<li><a href="#Win32_WAIT_FAILED">WAIT_FAILED</a></li>
<li><a href="#Win32_WAIT_OBJECT_0">WAIT_OBJECT_0</a></li>
<li><a href="#Win32_WAIT_TIMEOUT">WAIT_TIMEOUT</a></li>
<li><a href="#Win32_WA_ACTIVE">WA_ACTIVE</a></li>
<li><a href="#Win32_WA_CLICKACTIVE">WA_CLICKACTIVE</a></li>
<li><a href="#Win32_WA_INACTIVE">WA_INACTIVE</a></li>
<li><a href="#Win32_WHEEL_DELTA">WHEEL_DELTA</a></li>
<li><a href="#Win32_WM_ACTIVATE">WM_ACTIVATE</a></li>
<li><a href="#Win32_WM_ACTIVATEAPP">WM_ACTIVATEAPP</a></li>
<li><a href="#Win32_WM_AFXFIRST">WM_AFXFIRST</a></li>
<li><a href="#Win32_WM_AFXLAST">WM_AFXLAST</a></li>
<li><a href="#Win32_WM_APP">WM_APP</a></li>
<li><a href="#Win32_WM_APPCOMMAND">WM_APPCOMMAND</a></li>
<li><a href="#Win32_WM_ASKCBFORMATNAME">WM_ASKCBFORMATNAME</a></li>
<li><a href="#Win32_WM_CANCELJOURNAL">WM_CANCELJOURNAL</a></li>
<li><a href="#Win32_WM_CANCELMODE">WM_CANCELMODE</a></li>
<li><a href="#Win32_WM_CAPTURECHANGED">WM_CAPTURECHANGED</a></li>
<li><a href="#Win32_WM_CHANGECBCHAIN">WM_CHANGECBCHAIN</a></li>
<li><a href="#Win32_WM_CHANGEUISTATE">WM_CHANGEUISTATE</a></li>
<li><a href="#Win32_WM_CHAR">WM_CHAR</a></li>
<li><a href="#Win32_WM_CHARTOITEM">WM_CHARTOITEM</a></li>
<li><a href="#Win32_WM_CHILDACTIVATE">WM_CHILDACTIVATE</a></li>
<li><a href="#Win32_WM_CLEAR">WM_CLEAR</a></li>
<li><a href="#Win32_WM_CLIPBOARDUPDATE">WM_CLIPBOARDUPDATE</a></li>
<li><a href="#Win32_WM_CLOSE">WM_CLOSE</a></li>
<li><a href="#Win32_WM_COMMAND">WM_COMMAND</a></li>
<li><a href="#Win32_WM_COMMNOTIFY">WM_COMMNOTIFY</a></li>
<li><a href="#Win32_WM_COMPACTING">WM_COMPACTING</a></li>
<li><a href="#Win32_WM_COMPAREITEM">WM_COMPAREITEM</a></li>
<li><a href="#Win32_WM_CONTEXTMENU">WM_CONTEXTMENU</a></li>
<li><a href="#Win32_WM_COPY">WM_COPY</a></li>
<li><a href="#Win32_WM_COPYDATA">WM_COPYDATA</a></li>
<li><a href="#Win32_WM_CREATE">WM_CREATE</a></li>
<li><a href="#Win32_WM_CTLCOLORBTN">WM_CTLCOLORBTN</a></li>
<li><a href="#Win32_WM_CTLCOLORDLG">WM_CTLCOLORDLG</a></li>
<li><a href="#Win32_WM_CTLCOLOREDIT">WM_CTLCOLOREDIT</a></li>
<li><a href="#Win32_WM_CTLCOLORLISTBOX">WM_CTLCOLORLISTBOX</a></li>
<li><a href="#Win32_WM_CTLCOLORMSGBOX">WM_CTLCOLORMSGBOX</a></li>
<li><a href="#Win32_WM_CTLCOLORSCROLLBAR">WM_CTLCOLORSCROLLBAR</a></li>
<li><a href="#Win32_WM_CTLCOLORSTATIC">WM_CTLCOLORSTATIC</a></li>
<li><a href="#Win32_WM_CUT">WM_CUT</a></li>
<li><a href="#Win32_WM_DEADCHAR">WM_DEADCHAR</a></li>
<li><a href="#Win32_WM_DELETEITEM">WM_DELETEITEM</a></li>
<li><a href="#Win32_WM_DESTROY">WM_DESTROY</a></li>
<li><a href="#Win32_WM_DESTROYCLIPBOARD">WM_DESTROYCLIPBOARD</a></li>
<li><a href="#Win32_WM_DEVICECHANGE">WM_DEVICECHANGE</a></li>
<li><a href="#Win32_WM_DEVMODECHANGE">WM_DEVMODECHANGE</a></li>
<li><a href="#Win32_WM_DISPLAYCHANGE">WM_DISPLAYCHANGE</a></li>
<li><a href="#Win32_WM_DPICHANGED">WM_DPICHANGED</a></li>
<li><a href="#Win32_WM_DPICHANGED_AFTERPARENT">WM_DPICHANGED_AFTERPARENT</a></li>
<li><a href="#Win32_WM_DPICHANGED_BEFOREPARENT">WM_DPICHANGED_BEFOREPARENT</a></li>
<li><a href="#Win32_WM_DRAWCLIPBOARD">WM_DRAWCLIPBOARD</a></li>
<li><a href="#Win32_WM_DRAWITEM">WM_DRAWITEM</a></li>
<li><a href="#Win32_WM_DROPFILES">WM_DROPFILES</a></li>
<li><a href="#Win32_WM_DWMCOLORIZATIONCOLORCHANGED">WM_DWMCOLORIZATIONCOLORCHANGED</a></li>
<li><a href="#Win32_WM_DWMCOMPOSITIONCHANGED">WM_DWMCOMPOSITIONCHANGED</a></li>
<li><a href="#Win32_WM_DWMNCRENDERINGCHANGED">WM_DWMNCRENDERINGCHANGED</a></li>
<li><a href="#Win32_WM_DWMSENDICONICLIVEPREVIEWBITMAP">WM_DWMSENDICONICLIVEPREVIEWBITMAP</a></li>
<li><a href="#Win32_WM_DWMSENDICONICTHUMBNAIL">WM_DWMSENDICONICTHUMBNAIL</a></li>
<li><a href="#Win32_WM_DWMWINDOWMAXIMIZEDCHANGE">WM_DWMWINDOWMAXIMIZEDCHANGE</a></li>
<li><a href="#Win32_WM_ENABLE">WM_ENABLE</a></li>
<li><a href="#Win32_WM_ENTERIDLE">WM_ENTERIDLE</a></li>
<li><a href="#Win32_WM_ENTERMENULOOP">WM_ENTERMENULOOP</a></li>
<li><a href="#Win32_WM_ENTERSIZEMOVE">WM_ENTERSIZEMOVE</a></li>
<li><a href="#Win32_WM_ERASEBKGND">WM_ERASEBKGND</a></li>
<li><a href="#Win32_WM_EXITMENULOOP">WM_EXITMENULOOP</a></li>
<li><a href="#Win32_WM_EXITSIZEMOVE">WM_EXITSIZEMOVE</a></li>
<li><a href="#Win32_WM_FONTCHANGE">WM_FONTCHANGE</a></li>
<li><a href="#Win32_WM_GESTURE">WM_GESTURE</a></li>
<li><a href="#Win32_WM_GESTURENOTIFY">WM_GESTURENOTIFY</a></li>
<li><a href="#Win32_WM_GETDLGCODE">WM_GETDLGCODE</a></li>
<li><a href="#Win32_WM_GETDPISCALEDSIZE">WM_GETDPISCALEDSIZE</a></li>
<li><a href="#Win32_WM_GETFONT">WM_GETFONT</a></li>
<li><a href="#Win32_WM_GETHOTKEY">WM_GETHOTKEY</a></li>
<li><a href="#Win32_WM_GETICON">WM_GETICON</a></li>
<li><a href="#Win32_WM_GETMINMAXINFO">WM_GETMINMAXINFO</a></li>
<li><a href="#Win32_WM_GETOBJECT">WM_GETOBJECT</a></li>
<li><a href="#Win32_WM_GETTEXT">WM_GETTEXT</a></li>
<li><a href="#Win32_WM_GETTEXTLENGTH">WM_GETTEXTLENGTH</a></li>
<li><a href="#Win32_WM_GETTITLEBARINFOEX">WM_GETTITLEBARINFOEX</a></li>
<li><a href="#Win32_WM_HANDHELDFIRST">WM_HANDHELDFIRST</a></li>
<li><a href="#Win32_WM_HANDHELDLAST">WM_HANDHELDLAST</a></li>
<li><a href="#Win32_WM_HELP">WM_HELP</a></li>
<li><a href="#Win32_WM_HOTKEY">WM_HOTKEY</a></li>
<li><a href="#Win32_WM_HSCROLL">WM_HSCROLL</a></li>
<li><a href="#Win32_WM_HSCROLLCLIPBOARD">WM_HSCROLLCLIPBOARD</a></li>
<li><a href="#Win32_WM_ICONERASEBKGND">WM_ICONERASEBKGND</a></li>
<li><a href="#Win32_WM_IME_CHAR">WM_IME_CHAR</a></li>
<li><a href="#Win32_WM_IME_COMPOSITION">WM_IME_COMPOSITION</a></li>
<li><a href="#Win32_WM_IME_COMPOSITIONFULL">WM_IME_COMPOSITIONFULL</a></li>
<li><a href="#Win32_WM_IME_CONTROL">WM_IME_CONTROL</a></li>
<li><a href="#Win32_WM_IME_ENDCOMPOSITION">WM_IME_ENDCOMPOSITION</a></li>
<li><a href="#Win32_WM_IME_KEYDOWN">WM_IME_KEYDOWN</a></li>
<li><a href="#Win32_WM_IME_KEYLAST">WM_IME_KEYLAST</a></li>
<li><a href="#Win32_WM_IME_KEYUP">WM_IME_KEYUP</a></li>
<li><a href="#Win32_WM_IME_NOTIFY">WM_IME_NOTIFY</a></li>
<li><a href="#Win32_WM_IME_REQUEST">WM_IME_REQUEST</a></li>
<li><a href="#Win32_WM_IME_SELECT">WM_IME_SELECT</a></li>
<li><a href="#Win32_WM_IME_SETCONTEXT">WM_IME_SETCONTEXT</a></li>
<li><a href="#Win32_WM_IME_STARTCOMPOSITION">WM_IME_STARTCOMPOSITION</a></li>
<li><a href="#Win32_WM_INITDIALOG">WM_INITDIALOG</a></li>
<li><a href="#Win32_WM_INITMENU">WM_INITMENU</a></li>
<li><a href="#Win32_WM_INITMENUPOPUP">WM_INITMENUPOPUP</a></li>
<li><a href="#Win32_WM_INPUT">WM_INPUT</a></li>
<li><a href="#Win32_WM_INPUTLANGCHANGE">WM_INPUTLANGCHANGE</a></li>
<li><a href="#Win32_WM_INPUTLANGCHANGEREQUEST">WM_INPUTLANGCHANGEREQUEST</a></li>
<li><a href="#Win32_WM_INPUT_DEVICE_CHANGE">WM_INPUT_DEVICE_CHANGE</a></li>
<li><a href="#Win32_WM_KEYDOWN">WM_KEYDOWN</a></li>
<li><a href="#Win32_WM_KEYFIRST">WM_KEYFIRST</a></li>
<li><a href="#Win32_WM_KEYLAST">WM_KEYLAST</a></li>
<li><a href="#Win32_WM_KEYUP">WM_KEYUP</a></li>
<li><a href="#Win32_WM_KILLFOCUS">WM_KILLFOCUS</a></li>
<li><a href="#Win32_WM_LBUTTONDBLCLK">WM_LBUTTONDBLCLK</a></li>
<li><a href="#Win32_WM_LBUTTONDOWN">WM_LBUTTONDOWN</a></li>
<li><a href="#Win32_WM_LBUTTONUP">WM_LBUTTONUP</a></li>
<li><a href="#Win32_WM_MBUTTONDBLCLK">WM_MBUTTONDBLCLK</a></li>
<li><a href="#Win32_WM_MBUTTONDOWN">WM_MBUTTONDOWN</a></li>
<li><a href="#Win32_WM_MBUTTONUP">WM_MBUTTONUP</a></li>
<li><a href="#Win32_WM_MDIACTIVATE">WM_MDIACTIVATE</a></li>
<li><a href="#Win32_WM_MDICASCADE">WM_MDICASCADE</a></li>
<li><a href="#Win32_WM_MDICREATE">WM_MDICREATE</a></li>
<li><a href="#Win32_WM_MDIDESTROY">WM_MDIDESTROY</a></li>
<li><a href="#Win32_WM_MDIGETACTIVE">WM_MDIGETACTIVE</a></li>
<li><a href="#Win32_WM_MDIICONARRANGE">WM_MDIICONARRANGE</a></li>
<li><a href="#Win32_WM_MDIMAXIMIZE">WM_MDIMAXIMIZE</a></li>
<li><a href="#Win32_WM_MDINEXT">WM_MDINEXT</a></li>
<li><a href="#Win32_WM_MDIREFRESHMENU">WM_MDIREFRESHMENU</a></li>
<li><a href="#Win32_WM_MDIRESTORE">WM_MDIRESTORE</a></li>
<li><a href="#Win32_WM_MDISETMENU">WM_MDISETMENU</a></li>
<li><a href="#Win32_WM_MDITILE">WM_MDITILE</a></li>
<li><a href="#Win32_WM_MEASUREITEM">WM_MEASUREITEM</a></li>
<li><a href="#Win32_WM_MENUCHAR">WM_MENUCHAR</a></li>
<li><a href="#Win32_WM_MENUCOMMAND">WM_MENUCOMMAND</a></li>
<li><a href="#Win32_WM_MENUDRAG">WM_MENUDRAG</a></li>
<li><a href="#Win32_WM_MENUGETOBJECT">WM_MENUGETOBJECT</a></li>
<li><a href="#Win32_WM_MENURBUTTONUP">WM_MENURBUTTONUP</a></li>
<li><a href="#Win32_WM_MENUSELECT">WM_MENUSELECT</a></li>
<li><a href="#Win32_WM_MOUSEACTIVATE">WM_MOUSEACTIVATE</a></li>
<li><a href="#Win32_WM_MOUSEFIRST">WM_MOUSEFIRST</a></li>
<li><a href="#Win32_WM_MOUSEHOVER">WM_MOUSEHOVER</a></li>
<li><a href="#Win32_WM_MOUSEHWHEEL">WM_MOUSEHWHEEL</a></li>
<li><a href="#Win32_WM_MOUSELAST">WM_MOUSELAST</a></li>
<li><a href="#Win32_WM_MOUSELEAVE">WM_MOUSELEAVE</a></li>
<li><a href="#Win32_WM_MOUSEMOVE">WM_MOUSEMOVE</a></li>
<li><a href="#Win32_WM_MOUSEWHEEL">WM_MOUSEWHEEL</a></li>
<li><a href="#Win32_WM_MOVE">WM_MOVE</a></li>
<li><a href="#Win32_WM_MOVING">WM_MOVING</a></li>
<li><a href="#Win32_WM_NCACTIVATE">WM_NCACTIVATE</a></li>
<li><a href="#Win32_WM_NCCALCSIZE">WM_NCCALCSIZE</a></li>
<li><a href="#Win32_WM_NCCREATE">WM_NCCREATE</a></li>
<li><a href="#Win32_WM_NCDESTROY">WM_NCDESTROY</a></li>
<li><a href="#Win32_WM_NCHITTEST">WM_NCHITTEST</a></li>
<li><a href="#Win32_WM_NCLBUTTONDBLCLK">WM_NCLBUTTONDBLCLK</a></li>
<li><a href="#Win32_WM_NCLBUTTONDOWN">WM_NCLBUTTONDOWN</a></li>
<li><a href="#Win32_WM_NCLBUTTONUP">WM_NCLBUTTONUP</a></li>
<li><a href="#Win32_WM_NCMBUTTONDBLCLK">WM_NCMBUTTONDBLCLK</a></li>
<li><a href="#Win32_WM_NCMBUTTONDOWN">WM_NCMBUTTONDOWN</a></li>
<li><a href="#Win32_WM_NCMBUTTONUP">WM_NCMBUTTONUP</a></li>
<li><a href="#Win32_WM_NCMOUSEHOVER">WM_NCMOUSEHOVER</a></li>
<li><a href="#Win32_WM_NCMOUSELEAVE">WM_NCMOUSELEAVE</a></li>
<li><a href="#Win32_WM_NCMOUSEMOVE">WM_NCMOUSEMOVE</a></li>
<li><a href="#Win32_WM_NCPAINT">WM_NCPAINT</a></li>
<li><a href="#Win32_WM_NCPOINTERDOWN">WM_NCPOINTERDOWN</a></li>
<li><a href="#Win32_WM_NCPOINTERUP">WM_NCPOINTERUP</a></li>
<li><a href="#Win32_WM_NCPOINTERUPDATE">WM_NCPOINTERUPDATE</a></li>
<li><a href="#Win32_WM_NCRBUTTONDBLCLK">WM_NCRBUTTONDBLCLK</a></li>
<li><a href="#Win32_WM_NCRBUTTONDOWN">WM_NCRBUTTONDOWN</a></li>
<li><a href="#Win32_WM_NCRBUTTONUP">WM_NCRBUTTONUP</a></li>
<li><a href="#Win32_WM_NCXBUTTONDBLCLK">WM_NCXBUTTONDBLCLK</a></li>
<li><a href="#Win32_WM_NCXBUTTONDOWN">WM_NCXBUTTONDOWN</a></li>
<li><a href="#Win32_WM_NCXBUTTONUP">WM_NCXBUTTONUP</a></li>
<li><a href="#Win32_WM_NEXTDLGCTL">WM_NEXTDLGCTL</a></li>
<li><a href="#Win32_WM_NEXTMENU">WM_NEXTMENU</a></li>
<li><a href="#Win32_WM_NOTIFY">WM_NOTIFY</a></li>
<li><a href="#Win32_WM_NOTIFYFORMAT">WM_NOTIFYFORMAT</a></li>
<li><a href="#Win32_WM_PAINT">WM_PAINT</a></li>
<li><a href="#Win32_WM_PAINTCLIPBOARD">WM_PAINTCLIPBOARD</a></li>
<li><a href="#Win32_WM_PAINTICON">WM_PAINTICON</a></li>
<li><a href="#Win32_WM_PALETTECHANGED">WM_PALETTECHANGED</a></li>
<li><a href="#Win32_WM_PALETTEISCHANGING">WM_PALETTEISCHANGING</a></li>
<li><a href="#Win32_WM_PARENTNOTIFY">WM_PARENTNOTIFY</a></li>
<li><a href="#Win32_WM_PASTE">WM_PASTE</a></li>
<li><a href="#Win32_WM_PENWINFIRST">WM_PENWINFIRST</a></li>
<li><a href="#Win32_WM_PENWINLAST">WM_PENWINLAST</a></li>
<li><a href="#Win32_WM_POINTERACTIVATE">WM_POINTERACTIVATE</a></li>
<li><a href="#Win32_WM_POINTERCAPTURECHANGED">WM_POINTERCAPTURECHANGED</a></li>
<li><a href="#Win32_WM_POINTERDEVICECHANGE">WM_POINTERDEVICECHANGE</a></li>
<li><a href="#Win32_WM_POINTERDEVICEINRANGE">WM_POINTERDEVICEINRANGE</a></li>
<li><a href="#Win32_WM_POINTERDEVICEOUTOFRANGE">WM_POINTERDEVICEOUTOFRANGE</a></li>
<li><a href="#Win32_WM_POINTERDOWN">WM_POINTERDOWN</a></li>
<li><a href="#Win32_WM_POINTERENTER">WM_POINTERENTER</a></li>
<li><a href="#Win32_WM_POINTERHWHEEL">WM_POINTERHWHEEL</a></li>
<li><a href="#Win32_WM_POINTERLEAVE">WM_POINTERLEAVE</a></li>
<li><a href="#Win32_WM_POINTERROUTEDAWAY">WM_POINTERROUTEDAWAY</a></li>
<li><a href="#Win32_WM_POINTERROUTEDRELEASED">WM_POINTERROUTEDRELEASED</a></li>
<li><a href="#Win32_WM_POINTERROUTEDTO">WM_POINTERROUTEDTO</a></li>
<li><a href="#Win32_WM_POINTERUP">WM_POINTERUP</a></li>
<li><a href="#Win32_WM_POINTERUPDATE">WM_POINTERUPDATE</a></li>
<li><a href="#Win32_WM_POINTERWHEEL">WM_POINTERWHEEL</a></li>
<li><a href="#Win32_WM_POWER">WM_POWER</a></li>
<li><a href="#Win32_WM_POWERBROADCAST">WM_POWERBROADCAST</a></li>
<li><a href="#Win32_WM_PRINT">WM_PRINT</a></li>
<li><a href="#Win32_WM_PRINTCLIENT">WM_PRINTCLIENT</a></li>
<li><a href="#Win32_WM_QUERYDRAGICON">WM_QUERYDRAGICON</a></li>
<li><a href="#Win32_WM_QUERYNEWPALETTE">WM_QUERYNEWPALETTE</a></li>
<li><a href="#Win32_WM_QUERYUISTATE">WM_QUERYUISTATE</a></li>
<li><a href="#Win32_WM_QUEUESYNC">WM_QUEUESYNC</a></li>
<li><a href="#Win32_WM_QUIT">WM_QUIT</a></li>
<li><a href="#Win32_WM_RBUTTONDBLCLK">WM_RBUTTONDBLCLK</a></li>
<li><a href="#Win32_WM_RBUTTONDOWN">WM_RBUTTONDOWN</a></li>
<li><a href="#Win32_WM_RBUTTONUP">WM_RBUTTONUP</a></li>
<li><a href="#Win32_WM_RENDERALLFORMATS">WM_RENDERALLFORMATS</a></li>
<li><a href="#Win32_WM_RENDERFORMAT">WM_RENDERFORMAT</a></li>
<li><a href="#Win32_WM_SETCURSOR">WM_SETCURSOR</a></li>
<li><a href="#Win32_WM_SETFOCUS">WM_SETFOCUS</a></li>
<li><a href="#Win32_WM_SETFONT">WM_SETFONT</a></li>
<li><a href="#Win32_WM_SETHOTKEY">WM_SETHOTKEY</a></li>
<li><a href="#Win32_WM_SETICON">WM_SETICON</a></li>
<li><a href="#Win32_WM_SETREDRAW">WM_SETREDRAW</a></li>
<li><a href="#Win32_WM_SETTEXT">WM_SETTEXT</a></li>
<li><a href="#Win32_WM_SETTINGCHANGE">WM_SETTINGCHANGE</a></li>
<li><a href="#Win32_WM_SHOWWINDOW">WM_SHOWWINDOW</a></li>
<li><a href="#Win32_WM_SIZE">WM_SIZE</a></li>
<li><a href="#Win32_WM_SIZECLIPBOARD">WM_SIZECLIPBOARD</a></li>
<li><a href="#Win32_WM_SIZING">WM_SIZING</a></li>
<li><a href="#Win32_WM_SPOOLERSTATUS">WM_SPOOLERSTATUS</a></li>
<li><a href="#Win32_WM_STYLECHANGED">WM_STYLECHANGED</a></li>
<li><a href="#Win32_WM_STYLECHANGING">WM_STYLECHANGING</a></li>
<li><a href="#Win32_WM_SYNCPAINT">WM_SYNCPAINT</a></li>
<li><a href="#Win32_WM_SYSCHAR">WM_SYSCHAR</a></li>
<li><a href="#Win32_WM_SYSCOLORCHANGE">WM_SYSCOLORCHANGE</a></li>
<li><a href="#Win32_WM_SYSCOMMAND">WM_SYSCOMMAND</a></li>
<li><a href="#Win32_WM_SYSDEADCHAR">WM_SYSDEADCHAR</a></li>
<li><a href="#Win32_WM_SYSKEYDOWN">WM_SYSKEYDOWN</a></li>
<li><a href="#Win32_WM_SYSKEYUP">WM_SYSKEYUP</a></li>
<li><a href="#Win32_WM_TABLET_FIRST">WM_TABLET_FIRST</a></li>
<li><a href="#Win32_WM_TABLET_LAST">WM_TABLET_LAST</a></li>
<li><a href="#Win32_WM_TCARD">WM_TCARD</a></li>
<li><a href="#Win32_WM_THEMECHANGED">WM_THEMECHANGED</a></li>
<li><a href="#Win32_WM_TIMECHANGE">WM_TIMECHANGE</a></li>
<li><a href="#Win32_WM_TIMER">WM_TIMER</a></li>
<li><a href="#Win32_WM_TOUCH">WM_TOUCH</a></li>
<li><a href="#Win32_WM_TOUCHHITTESTING">WM_TOUCHHITTESTING</a></li>
<li><a href="#Win32_WM_UNDO">WM_UNDO</a></li>
<li><a href="#Win32_WM_UNICHAR">WM_UNICHAR</a></li>
<li><a href="#Win32_WM_UNINITMENUPOPUP">WM_UNINITMENUPOPUP</a></li>
<li><a href="#Win32_WM_UPDATEUISTATE">WM_UPDATEUISTATE</a></li>
<li><a href="#Win32_WM_USER">WM_USER</a></li>
<li><a href="#Win32_WM_USERCHANGED">WM_USERCHANGED</a></li>
<li><a href="#Win32_WM_VKEYTOITEM">WM_VKEYTOITEM</a></li>
<li><a href="#Win32_WM_VSCROLL">WM_VSCROLL</a></li>
<li><a href="#Win32_WM_VSCROLLCLIPBOARD">WM_VSCROLLCLIPBOARD</a></li>
<li><a href="#Win32_WM_WINDOWPOSCHANGED">WM_WINDOWPOSCHANGED</a></li>
<li><a href="#Win32_WM_WINDOWPOSCHANGING">WM_WINDOWPOSCHANGING</a></li>
<li><a href="#Win32_WM_WININICHANGE">WM_WININICHANGE</a></li>
<li><a href="#Win32_WM_WTSSESSION_CHANGE">WM_WTSSESSION_CHANGE</a></li>
<li><a href="#Win32_WM_XBUTTONDBLCLK">WM_XBUTTONDBLCLK</a></li>
<li><a href="#Win32_WM_XBUTTONDOWN">WM_XBUTTONDOWN</a></li>
<li><a href="#Win32_WM_XBUTTONUP">WM_XBUTTONUP</a></li>
<li><a href="#Win32_WS_BORDER">WS_BORDER</a></li>
<li><a href="#Win32_WS_CAPTION">WS_CAPTION</a></li>
<li><a href="#Win32_WS_CHILD">WS_CHILD</a></li>
<li><a href="#Win32_WS_CLIPCHILDREN">WS_CLIPCHILDREN</a></li>
<li><a href="#Win32_WS_CLIPSIBLINGS">WS_CLIPSIBLINGS</a></li>
<li><a href="#Win32_WS_DISABLED">WS_DISABLED</a></li>
<li><a href="#Win32_WS_DLGFRAME">WS_DLGFRAME</a></li>
<li><a href="#Win32_WS_EX_ACCEPTFILES">WS_EX_ACCEPTFILES</a></li>
<li><a href="#Win32_WS_EX_APPWINDOW">WS_EX_APPWINDOW</a></li>
<li><a href="#Win32_WS_EX_CLIENTEDGE">WS_EX_CLIENTEDGE</a></li>
<li><a href="#Win32_WS_EX_COMPOSITED">WS_EX_COMPOSITED</a></li>
<li><a href="#Win32_WS_EX_CONTEXTHELP">WS_EX_CONTEXTHELP</a></li>
<li><a href="#Win32_WS_EX_CONTROLPARENT">WS_EX_CONTROLPARENT</a></li>
<li><a href="#Win32_WS_EX_DLGMODALFRAME">WS_EX_DLGMODALFRAME</a></li>
<li><a href="#Win32_WS_EX_LAYERED">WS_EX_LAYERED</a></li>
<li><a href="#Win32_WS_EX_LAYOUTRTL">WS_EX_LAYOUTRTL</a></li>
<li><a href="#Win32_WS_EX_LEFT">WS_EX_LEFT</a></li>
<li><a href="#Win32_WS_EX_LEFTSCROLLBAR">WS_EX_LEFTSCROLLBAR</a></li>
<li><a href="#Win32_WS_EX_LTRREADING">WS_EX_LTRREADING</a></li>
<li><a href="#Win32_WS_EX_MDICHILD">WS_EX_MDICHILD</a></li>
<li><a href="#Win32_WS_EX_NOACTIVATE">WS_EX_NOACTIVATE</a></li>
<li><a href="#Win32_WS_EX_NOINHERITLAYOUT">WS_EX_NOINHERITLAYOUT</a></li>
<li><a href="#Win32_WS_EX_NOPARENTNOTIFY">WS_EX_NOPARENTNOTIFY</a></li>
<li><a href="#Win32_WS_EX_NOREDIRECTIONBITMAP">WS_EX_NOREDIRECTIONBITMAP</a></li>
<li><a href="#Win32_WS_EX_OVERLAPPEDWINDOW">WS_EX_OVERLAPPEDWINDOW</a></li>
<li><a href="#Win32_WS_EX_PALETTEWINDOW">WS_EX_PALETTEWINDOW</a></li>
<li><a href="#Win32_WS_EX_RIGHT">WS_EX_RIGHT</a></li>
<li><a href="#Win32_WS_EX_RIGHTSCROLLBAR">WS_EX_RIGHTSCROLLBAR</a></li>
<li><a href="#Win32_WS_EX_RTLREADING">WS_EX_RTLREADING</a></li>
<li><a href="#Win32_WS_EX_STATICEDGE">WS_EX_STATICEDGE</a></li>
<li><a href="#Win32_WS_EX_TOOLWINDOW">WS_EX_TOOLWINDOW</a></li>
<li><a href="#Win32_WS_EX_TOPMOST">WS_EX_TOPMOST</a></li>
<li><a href="#Win32_WS_EX_TRANSPARENT">WS_EX_TRANSPARENT</a></li>
<li><a href="#Win32_WS_EX_WINDOWEDGE">WS_EX_WINDOWEDGE</a></li>
<li><a href="#Win32_WS_GROUP">WS_GROUP</a></li>
<li><a href="#Win32_WS_HSCROLL">WS_HSCROLL</a></li>
<li><a href="#Win32_WS_MAXIMIZE">WS_MAXIMIZE</a></li>
<li><a href="#Win32_WS_MAXIMIZEBOX">WS_MAXIMIZEBOX</a></li>
<li><a href="#Win32_WS_MINIMIZE">WS_MINIMIZE</a></li>
<li><a href="#Win32_WS_MINIMIZEBOX">WS_MINIMIZEBOX</a></li>
<li><a href="#Win32_WS_OVERLAPPED">WS_OVERLAPPED</a></li>
<li><a href="#Win32_WS_OVERLAPPEDWINDOW">WS_OVERLAPPEDWINDOW</a></li>
<li><a href="#Win32_WS_POPUP">WS_POPUP</a></li>
<li><a href="#Win32_WS_POPUPWINDOW">WS_POPUPWINDOW</a></li>
<li><a href="#Win32_WS_SYSMENU">WS_SYSMENU</a></li>
<li><a href="#Win32_WS_TABSTOP">WS_TABSTOP</a></li>
<li><a href="#Win32_WS_THICKFRAME">WS_THICKFRAME</a></li>
<li><a href="#Win32_WS_VISIBLE">WS_VISIBLE</a></li>
<li><a href="#Win32_WS_VSCROLL">WS_VSCROLL</a></li>
</ul>
<h3>Type Aliases</h3>
<h4></h4>
<ul>
<li><a href="#Win32_ATOM">ATOM</a></li>
<li><a href="#Win32_BOOL">BOOL</a></li>
<li><a href="#Win32_BYTE">BYTE</a></li>
<li><a href="#Win32_CHAR">CHAR</a></li>
<li><a href="#Win32_COLORREF">COLORREF</a></li>
<li><a href="#Win32_DWORD">DWORD</a></li>
<li><a href="#Win32_DWORD64">DWORD64</a></li>
<li><a href="#Win32_DWORD_PTR">DWORD_PTR</a></li>
<li><a href="#Win32_HANDLE">HANDLE</a></li>
<li><a href="#Win32_HBITMAP">HBITMAP</a></li>
<li><a href="#Win32_HBRUSH">HBRUSH</a></li>
<li><a href="#Win32_HCURSOR">HCURSOR</a></li>
<li><a href="#Win32_HDC">HDC</a></li>
<li><a href="#Win32_HGLOBAL">HGLOBAL</a></li>
<li><a href="#Win32_HICON">HICON</a></li>
<li><a href="#Win32_HINSTANCE">HINSTANCE</a></li>
<li><a href="#Win32_HLOCAL">HLOCAL</a></li>
<li><a href="#Win32_HMENU">HMENU</a></li>
<li><a href="#Win32_HMODULE">HMODULE</a></li>
<li><a href="#Win32_HMONITOR">HMONITOR</a></li>
<li><a href="#Win32_HRESULT">HRESULT</a></li>
<li><a href="#Win32_HWND">HWND</a></li>
<li><a href="#Win32_IP_MASK_STRING">IP_MASK_STRING</a></li>
<li><a href="#Win32_LARGE_INTEGER">LARGE_INTEGER</a></li>
<li><a href="#Win32_LONG">LONG</a></li>
<li><a href="#Win32_LPARAM">LPARAM</a></li>
<li><a href="#Win32_LPBYTE">LPBYTE</a></li>
<li><a href="#Win32_LPCH">LPCH</a></li>
<li><a href="#Win32_LPCSTR">LPCSTR</a></li>
<li><a href="#Win32_LPCTSTR">LPCTSTR</a></li>
<li><a href="#Win32_LPCVOID">LPCVOID</a></li>
<li><a href="#Win32_LPCWSTR">LPCWSTR</a></li>
<li><a href="#Win32_LPDWORD">LPDWORD</a></li>
<li><a href="#Win32_LPOVERLAPPED">LPOVERLAPPED</a></li>
<li><a href="#Win32_LPSTR">LPSTR</a></li>
<li><a href="#Win32_LPTHREAD_START_ROUTINE">LPTHREAD_START_ROUTINE</a></li>
<li><a href="#Win32_LPVOID">LPVOID</a></li>
<li><a href="#Win32_LPWCH">LPWCH</a></li>
<li><a href="#Win32_LPWSTR">LPWSTR</a></li>
<li><a href="#Win32_LRESULT">LRESULT</a></li>
<li><a href="#Win32_PCSTR">PCSTR</a></li>
<li><a href="#Win32_PCWSTR">PCWSTR</a></li>
<li><a href="#Win32_PLONG">PLONG</a></li>
<li><a href="#Win32_SHORT">SHORT</a></li>
<li><a href="#Win32_SHSTDAPI">SHSTDAPI</a></li>
<li><a href="#Win32_SIZE_T">SIZE_T</a></li>
<li><a href="#Win32_SRWLOCK">SRWLOCK</a></li>
<li><a href="#Win32_UINT">UINT</a></li>
<li><a href="#Win32_ULONG">ULONG</a></li>
<li><a href="#Win32_ULONG64">ULONG64</a></li>
<li><a href="#Win32_USHORT">USHORT</a></li>
<li><a href="#Win32_WAITORTIMERCALLBACK">WAITORTIMERCALLBACK</a></li>
<li><a href="#Win32_WCHAR">WCHAR</a></li>
<li><a href="#Win32_WORD">WORD</a></li>
<li><a href="#Win32_WPARAM">WPARAM</a></li>
<li><a href="#Win32_time_t">time_t</a></li>
<li><a href="#Win32_va_list">va_list</a></li>
</ul>
<h3>Functions</h3>
<h4></h4>
<ul>
<li><a href="#Win32_AcquireSRWLockExclusive">Win32.AcquireSRWLockExclusive</a></li>
<li><a href="#Win32_AcquireSRWLockShared">Win32.AcquireSRWLockShared</a></li>
<li><a href="#Win32_BeginPaint">Win32.BeginPaint</a></li>
<li><a href="#Win32_BringWindowToTop">Win32.BringWindowToTop</a></li>
<li><a href="#Win32_CancelWaitableTimer">Win32.CancelWaitableTimer</a></li>
<li><a href="#Win32_ChildWindowFromPoint">Win32.ChildWindowFromPoint</a></li>
<li><a href="#Win32_ChildWindowFromPointEx">Win32.ChildWindowFromPointEx</a></li>
<li><a href="#Win32_ClientToScreen">Win32.ClientToScreen</a></li>
<li><a href="#Win32_CloseClipboard">Win32.CloseClipboard</a></li>
<li><a href="#Win32_CloseHandle">Win32.CloseHandle</a></li>
<li><a href="#Win32_CoInitializeEx">Win32.CoInitializeEx</a></li>
<li><a href="#Win32_CoUninitialize">Win32.CoUninitialize</a></li>
<li><a href="#Win32_CreateConsoleScreenBuffer">Win32.CreateConsoleScreenBuffer</a></li>
<li><a href="#Win32_CreateDirectoryA">Win32.CreateDirectoryA</a></li>
<li><a href="#Win32_CreateDirectoryW">Win32.CreateDirectoryW</a></li>
<li><a href="#Win32_CreateEventA">Win32.CreateEventA</a></li>
<li><a href="#Win32_CreateEventW">Win32.CreateEventW</a></li>
<li><a href="#Win32_CreateFileA">Win32.CreateFileA</a></li>
<li><a href="#Win32_CreateFileW">Win32.CreateFileW</a></li>
<li><a href="#Win32_CreateIconIndirect">Win32.CreateIconIndirect</a></li>
<li><a href="#Win32_CreateMutexA">Win32.CreateMutexA</a></li>
<li><a href="#Win32_CreateMutexW">Win32.CreateMutexW</a></li>
<li><a href="#Win32_CreateProcessA">Win32.CreateProcessA</a></li>
<li><a href="#Win32_CreateProcessW">Win32.CreateProcessW</a></li>
<li><a href="#Win32_CreateThread">Win32.CreateThread</a></li>
<li><a href="#Win32_CreateTimerQueueTimer">Win32.CreateTimerQueueTimer</a></li>
<li><a href="#Win32_CreateWaitableTimerA">Win32.CreateWaitableTimerA</a></li>
<li><a href="#Win32_CreateWaitableTimerW">Win32.CreateWaitableTimerW</a></li>
<li><a href="#Win32_CreateWindowExA">Win32.CreateWindowExA</a></li>
<li><a href="#Win32_CreateWindowExW">Win32.CreateWindowExW</a></li>
<li><a href="#Win32_DebugActiveProcess">Win32.DebugActiveProcess</a></li>
<li><a href="#Win32_DebugBreak">Win32.DebugBreak</a></li>
<li><a href="#Win32_DefWindowProcA">Win32.DefWindowProcA</a></li>
<li><a href="#Win32_DefWindowProcW">Win32.DefWindowProcW</a></li>
<li><a href="#Win32_DeleteFileA">Win32.DeleteFileA</a></li>
<li><a href="#Win32_DeleteFileW">Win32.DeleteFileW</a></li>
<li><a href="#Win32_DeleteTimerQueueTimer">Win32.DeleteTimerQueueTimer</a></li>
<li><a href="#Win32_DestroyIcon">Win32.DestroyIcon</a></li>
<li><a href="#Win32_DestroyWindow">Win32.DestroyWindow</a></li>
<li><a href="#Win32_DispatchMessageA">Win32.DispatchMessageA</a></li>
<li><a href="#Win32_DispatchMessageW">Win32.DispatchMessageW</a></li>
<li><a href="#Win32_EmptyClipboard">Win32.EmptyClipboard</a></li>
<li><a href="#Win32_EnableWindow">Win32.EnableWindow</a></li>
<li><a href="#Win32_EndPaint">Win32.EndPaint</a></li>
<li><a href="#Win32_EnumDisplayMonitors">Win32.EnumDisplayMonitors</a></li>
<li><a href="#Win32_EnumWindows">Win32.EnumWindows</a></li>
<li><a href="#Win32_ExitProcess">Win32.ExitProcess</a></li>
<li><a href="#Win32_ExtractIconExA">Win32.ExtractIconExA</a></li>
<li><a href="#Win32_ExtractIconExW">Win32.ExtractIconExW</a></li>
<li><a href="#Win32_FileTimeToLocalFileTime">Win32.FileTimeToLocalFileTime</a></li>
<li><a href="#Win32_FileTimeToSystemTime">Win32.FileTimeToSystemTime</a></li>
<li><a href="#Win32_FillRect">Win32.FillRect</a></li>
<li><a href="#Win32_FindClose">Win32.FindClose</a></li>
<li><a href="#Win32_FindFirstFileA">Win32.FindFirstFileA</a></li>
<li><a href="#Win32_FindFirstFileW">Win32.FindFirstFileW</a></li>
<li><a href="#Win32_FindFirstVolumeA">Win32.FindFirstVolumeA</a></li>
<li><a href="#Win32_FindFirstVolumeW">Win32.FindFirstVolumeW</a></li>
<li><a href="#Win32_FindNextFileA">Win32.FindNextFileA</a></li>
<li><a href="#Win32_FindNextFileW">Win32.FindNextFileW</a></li>
<li><a href="#Win32_FindNextVolumeA">Win32.FindNextVolumeA</a></li>
<li><a href="#Win32_FindNextVolumeW">Win32.FindNextVolumeW</a></li>
<li><a href="#Win32_FindVolumeClose">Win32.FindVolumeClose</a></li>
<li><a href="#Win32_FindWindowA">Win32.FindWindowA</a></li>
<li><a href="#Win32_FindWindowW">Win32.FindWindowW</a></li>
<li><a href="#Win32_FormatMessageA">Win32.FormatMessageA</a></li>
<li><a href="#Win32_GET_WHEEL_DELTA_WPARAM">Win32.GET_WHEEL_DELTA_WPARAM</a></li>
<li><a href="#Win32_GET_X_LPARAM">Win32.GET_X_LPARAM</a></li>
<li><a href="#Win32_GET_Y_LPARAM">Win32.GET_Y_LPARAM</a></li>
<li><a href="#Win32_GetActiveWindow">Win32.GetActiveWindow</a></li>
<li><a href="#Win32_GetAdaptersInfo">Win32.GetAdaptersInfo</a></li>
<li><a href="#Win32_GetAsyncKeyState">Win32.GetAsyncKeyState</a></li>
<li><a href="#Win32_GetClientRect">Win32.GetClientRect</a></li>
<li><a href="#Win32_GetClipboardData">Win32.GetClipboardData</a></li>
<li><a href="#Win32_GetConsoleScreenBufferInfo">Win32.GetConsoleScreenBufferInfo</a></li>
<li><a href="#Win32_GetCurrentDirectoryA">Win32.GetCurrentDirectoryA</a></li>
<li><a href="#Win32_GetCurrentDirectoryW">Win32.GetCurrentDirectoryW</a></li>
<li><a href="#Win32_GetCurrentProcess">Win32.GetCurrentProcess</a></li>
<li><a href="#Win32_GetCurrentProcessId">Win32.GetCurrentProcessId</a></li>
<li><a href="#Win32_GetCurrentThread">Win32.GetCurrentThread</a></li>
<li><a href="#Win32_GetCurrentThreadId">Win32.GetCurrentThreadId</a></li>
<li><a href="#Win32_GetCursorPos">Win32.GetCursorPos</a></li>
<li><a href="#Win32_GetDC">Win32.GetDC</a></li>
<li><a href="#Win32_GetDesktopWindow">Win32.GetDesktopWindow</a></li>
<li><a href="#Win32_GetFileAttributesA">Win32.GetFileAttributesA</a></li>
<li><a href="#Win32_GetFileAttributesExA">Win32.GetFileAttributesExA</a></li>
<li><a href="#Win32_GetFileAttributesExW">Win32.GetFileAttributesExW</a></li>
<li><a href="#Win32_GetFileAttributesW">Win32.GetFileAttributesW</a></li>
<li><a href="#Win32_GetFileSize">Win32.GetFileSize</a></li>
<li><a href="#Win32_GetForegroundWindow">Win32.GetForegroundWindow</a></li>
<li><a href="#Win32_GetFullPathNameA">Win32.GetFullPathNameA</a></li>
<li><a href="#Win32_GetFullPathNameW">Win32.GetFullPathNameW</a></li>
<li><a href="#Win32_GetIconInfo">Win32.GetIconInfo</a></li>
<li><a href="#Win32_GetKeyboardState">Win32.GetKeyboardState</a></li>
<li><a href="#Win32_GetLastError">Win32.GetLastError</a></li>
<li><a href="#Win32_GetLocalTime">Win32.GetLocalTime</a></li>
<li><a href="#Win32_GetMessageA">Win32.GetMessageA</a></li>
<li><a href="#Win32_GetMessageW">Win32.GetMessageW</a></li>
<li><a href="#Win32_GetModuleHandleA">Win32.GetModuleHandleA</a></li>
<li><a href="#Win32_GetModuleHandleW">Win32.GetModuleHandleW</a></li>
<li><a href="#Win32_GetMonitorInfoA">Win32.GetMonitorInfoA</a></li>
<li><a href="#Win32_GetMonitorInfoW">Win32.GetMonitorInfoW</a></li>
<li><a href="#Win32_GetNextWindow">Win32.GetNextWindow</a></li>
<li><a href="#Win32_GetProcAddress">Win32.GetProcAddress</a></li>
<li><a href="#Win32_GetStdHandle">Win32.GetStdHandle</a></li>
<li><a href="#Win32_GetSystemInfo">Win32.GetSystemInfo</a></li>
<li><a href="#Win32_GetSystemMetrics">Win32.GetSystemMetrics</a></li>
<li><a href="#Win32_GetSystemTime">Win32.GetSystemTime</a></li>
<li><a href="#Win32_GetTopWindow">Win32.GetTopWindow</a></li>
<li><a href="#Win32_GetVolumePathNamesForVolumeNameA">Win32.GetVolumePathNamesForVolumeNameA</a></li>
<li><a href="#Win32_GetVolumePathNamesForVolumeNameW">Win32.GetVolumePathNamesForVolumeNameW</a></li>
<li><a href="#Win32_GetWindow">Win32.GetWindow</a></li>
<li><a href="#Win32_GetWindowLongPtrA">Win32.GetWindowLongPtrA</a></li>
<li><a href="#Win32_GetWindowLongPtrW">Win32.GetWindowLongPtrW</a></li>
<li><a href="#Win32_GetWindowRect">Win32.GetWindowRect</a></li>
<li><a href="#Win32_GetWindowTextA">Win32.GetWindowTextA</a></li>
<li><a href="#Win32_GetWindowTextW">Win32.GetWindowTextW</a></li>
<li><a href="#Win32_GlobalAlloc">Win32.GlobalAlloc</a></li>
<li><a href="#Win32_GlobalLock">Win32.GlobalLock</a></li>
<li><a href="#Win32_GlobalSize">Win32.GlobalSize</a></li>
<li><a href="#Win32_GlobalUnlock">Win32.GlobalUnlock</a></li>
<li><a href="#Win32_HIWORD">Win32.HIWORD</a></li>
<li><a href="#Win32_InitializeSRWLock">Win32.InitializeSRWLock</a></li>
<li><a href="#Win32_InvalidateRect">Win32.InvalidateRect</a></li>
<li><a href="#Win32_IsClipboardFormatAvailable">Win32.IsClipboardFormatAvailable</a></li>
<li><a href="#Win32_IsDebuggerPresent">Win32.IsDebuggerPresent</a></li>
<li><a href="#Win32_IsWindowVisible">Win32.IsWindowVisible</a></li>
<li><a href="#Win32_LOWORD">Win32.LOWORD</a></li>
<li><a href="#Win32_LoadCursorA">Win32.LoadCursorA</a></li>
<li><a href="#Win32_LoadCursorW">Win32.LoadCursorW</a></li>
<li><a href="#Win32_LocalFree">Win32.LocalFree</a></li>
<li><a href="#Win32_MAKEINTRESOURCEA">Win32.MAKEINTRESOURCEA</a></li>
<li><a href="#Win32_MAKEINTRESOURCEW">Win32.MAKEINTRESOURCEW</a></li>
<li><a href="#Win32_MAKELANGID">Win32.MAKELANGID</a></li>
<li><a href="#Win32_MapVirtualKeyA">Win32.MapVirtualKeyA</a></li>
<li><a href="#Win32_MapVirtualKeyW">Win32.MapVirtualKeyW</a></li>
<li><a href="#Win32_MessageBoxA">Win32.MessageBoxA</a></li>
<li><a href="#Win32_MessageBoxW">Win32.MessageBoxW</a></li>
<li><a href="#Win32_MonitorFromPoint">Win32.MonitorFromPoint</a></li>
<li><a href="#Win32_MonitorFromWindow">Win32.MonitorFromWindow</a></li>
<li><a href="#Win32_MoveWindow">Win32.MoveWindow</a></li>
<li><a href="#Win32_OpenClipboard">Win32.OpenClipboard</a></li>
<li><a href="#Win32_OutputDebugStringA">Win32.OutputDebugStringA</a></li>
<li><a href="#Win32_OutputDebugStringW">Win32.OutputDebugStringW</a></li>
<li><a href="#Win32_PeekMessageA">Win32.PeekMessageA</a></li>
<li><a href="#Win32_PeekMessageW">Win32.PeekMessageW</a></li>
<li><a href="#Win32_PostMessageA">Win32.PostMessageA</a></li>
<li><a href="#Win32_PostMessageW">Win32.PostMessageW</a></li>
<li><a href="#Win32_PostQuitMessage">Win32.PostQuitMessage</a></li>
<li><a href="#Win32_QueryPerformanceCounter">Win32.QueryPerformanceCounter</a></li>
<li><a href="#Win32_QueryPerformanceFrequency">Win32.QueryPerformanceFrequency</a></li>
<li><a href="#Win32_ReadFile">Win32.ReadFile</a></li>
<li><a href="#Win32_RegisterClassA">Win32.RegisterClassA</a></li>
<li><a href="#Win32_RegisterClassW">Win32.RegisterClassW</a></li>
<li><a href="#Win32_RegisterClipboardFormatA">Win32.RegisterClipboardFormatA</a></li>
<li><a href="#Win32_RegisterClipboardFormatW">Win32.RegisterClipboardFormatW</a></li>
<li><a href="#Win32_RegisterHotKey">Win32.RegisterHotKey</a></li>
<li><a href="#Win32_ReleaseCapture">Win32.ReleaseCapture</a></li>
<li><a href="#Win32_ReleaseDC">Win32.ReleaseDC</a></li>
<li><a href="#Win32_ReleaseMutex">Win32.ReleaseMutex</a></li>
<li><a href="#Win32_ReleaseSRWLockExclusive">Win32.ReleaseSRWLockExclusive</a></li>
<li><a href="#Win32_ReleaseSRWLockShared">Win32.ReleaseSRWLockShared</a></li>
<li><a href="#Win32_ResetEvent">Win32.ResetEvent</a></li>
<li><a href="#Win32_ResumeThread">Win32.ResumeThread</a></li>
<li><a href="#Win32_SHGetFileInfoA">Win32.SHGetFileInfoA</a></li>
<li><a href="#Win32_SHGetFileInfoW">Win32.SHGetFileInfoW</a></li>
<li><a href="#Win32_SHGetSpecialFolderPathA">Win32.SHGetSpecialFolderPathA</a></li>
<li><a href="#Win32_SHGetSpecialFolderPathW">Win32.SHGetSpecialFolderPathW</a></li>
<li><a href="#Win32_SHGetStockIconInfo">Win32.SHGetStockIconInfo</a></li>
<li><a href="#Win32_ScreenToClient">Win32.ScreenToClient</a></li>
<li><a href="#Win32_SetActiveWindow">Win32.SetActiveWindow</a></li>
<li><a href="#Win32_SetCapture">Win32.SetCapture</a></li>
<li><a href="#Win32_SetClipboardData">Win32.SetClipboardData</a></li>
<li><a href="#Win32_SetConsoleActiveScreenBuffer">Win32.SetConsoleActiveScreenBuffer</a></li>
<li><a href="#Win32_SetConsoleOutputCP">Win32.SetConsoleOutputCP</a></li>
<li><a href="#Win32_SetConsoleTextAttribute">Win32.SetConsoleTextAttribute</a></li>
<li><a href="#Win32_SetCurrentDirectoryA">Win32.SetCurrentDirectoryA</a></li>
<li><a href="#Win32_SetCurrentDirectoryW">Win32.SetCurrentDirectoryW</a></li>
<li><a href="#Win32_SetCursor">Win32.SetCursor</a></li>
<li><a href="#Win32_SetCursorPos">Win32.SetCursorPos</a></li>
<li><a href="#Win32_SetEvent">Win32.SetEvent</a></li>
<li><a href="#Win32_SetFilePointer">Win32.SetFilePointer</a></li>
<li><a href="#Win32_SetFileTime">Win32.SetFileTime</a></li>
<li><a href="#Win32_SetFocus">Win32.SetFocus</a></li>
<li><a href="#Win32_SetLayeredWindowAttributes">Win32.SetLayeredWindowAttributes</a></li>
<li><a href="#Win32_SetThreadPriority">Win32.SetThreadPriority</a></li>
<li><a href="#Win32_SetWaitableTimer">Win32.SetWaitableTimer</a></li>
<li><a href="#Win32_SetWindowLongPtrA">Win32.SetWindowLongPtrA</a></li>
<li><a href="#Win32_SetWindowLongPtrW">Win32.SetWindowLongPtrW</a></li>
<li><a href="#Win32_SetWindowPos">Win32.SetWindowPos</a></li>
<li><a href="#Win32_Shell_NotifyIconA">Win32.Shell_NotifyIconA</a></li>
<li><a href="#Win32_Shell_NotifyIconW">Win32.Shell_NotifyIconW</a></li>
<li><a href="#Win32_ShowCursor">Win32.ShowCursor</a></li>
<li><a href="#Win32_ShowWindow">Win32.ShowWindow</a></li>
<li><a href="#Win32_Sleep">Win32.Sleep</a></li>
<li><a href="#Win32_SleepEx">Win32.SleepEx</a></li>
<li><a href="#Win32_SwitchToThread">Win32.SwitchToThread</a></li>
<li><a href="#Win32_SystemTimeToFileTime">Win32.SystemTimeToFileTime</a></li>
<li><a href="#Win32_ToUnicode">Win32.ToUnicode</a></li>
<li><a href="#Win32_TranslateMessage">Win32.TranslateMessage</a></li>
<li><a href="#Win32_TryAcquireSRWLockExclusive">Win32.TryAcquireSRWLockExclusive</a></li>
<li><a href="#Win32_TryAcquireSRWLockShared">Win32.TryAcquireSRWLockShared</a></li>
<li><a href="#Win32_UnregisterHotKey">Win32.UnregisterHotKey</a></li>
<li><a href="#Win32_UpdateWindow">Win32.UpdateWindow</a></li>
<li><a href="#Win32_WaitForMultipleObjects">Win32.WaitForMultipleObjects</a></li>
<li><a href="#Win32_WaitForSingleObject">Win32.WaitForSingleObject</a></li>
<li><a href="#Win32_WindowFromPoint">Win32.WindowFromPoint</a></li>
<li><a href="#Win32_WriteConsoleOutputCharacterA">Win32.WriteConsoleOutputCharacterA</a></li>
<li><a href="#Win32_WriteConsoleOutputCharacterW">Win32.WriteConsoleOutputCharacterW</a></li>
<li><a href="#Win32_WriteFile">Win32.WriteFile</a></li>
<li><a href="#Win32_setError">Win32.setError</a></li>
</ul>
</div>
</div>
<div class="right">
<div class="rightpage">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.23.0)</blockquote>
<h1>Module win32</h1>
<p>Wrappers for Microsoft Windows Sdk Libraries. </p>
<p> 10.0.19041.0 </p>
<h1>Content</h1>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_BACKGROUND_BLUE"><span class="titletype">const</span> <span class="titlelight">Win32.</span><span class="titlestrong">Constants</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L65" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Win32_BACKGROUND_BLUE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">BACKGROUND_BLUE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_GREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">BACKGROUND_GREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_INTENSITY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">BACKGROUND_INTENSITY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_MASK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">BACKGROUND_MASK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_RED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">BACKGROUND_RED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_BITMAP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_BITMAP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_DIB" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_DIB</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_DIBV5" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_DIBV5</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_DIF" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_DIF</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_ENHMETAFILE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_ENHMETAFILE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_HDROP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_HDROP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_LOCALE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_LOCALE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_METAFILEPICT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_METAFILEPICT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_OEMTEXT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_OEMTEXT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_PALETTE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_PALETTE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_PENDATA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_PENDATA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_RIFF" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_RIFF</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_SYLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_SYLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_TEXT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_TEXT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_TIFF" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_TIFF</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_UNICODETEXT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_UNICODETEXT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_WAVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CF_WAVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CONSOLE_TEXTMODE_BUFFER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CONSOLE_TEXTMODE_BUFFER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CREATE_ALWAYS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CREATE_ALWAYS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CREATE_NEW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CREATE_NEW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CREATE_SUSPENDED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CREATE_SUSPENDED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_ADMINTOOLS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_ADMINTOOLS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\Start Menu\Programs\Administrative Tools. </td>
</tr>
<tr>
<td id="Win32_CSIDL_ALTSTARTUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_ALTSTARTUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>non localized startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_APPDATA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_APPDATA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\Application Data. </td>
</tr>
<tr>
<td id="Win32_CSIDL_BITBUCKET" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_BITBUCKET</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;desktop&gt;\Recycle Bin. </td>
</tr>
<tr>
<td id="Win32_CSIDL_CDBURN_AREA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_CDBURN_AREA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>USERPROFILE\Local Settings\Application Data\Microsoft\CD Burning. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_ADMINTOOLS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_ADMINTOOLS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\Start Menu\Programs\Administrative Tools. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_ALTSTARTUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_ALTSTARTUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>non localized common startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_APPDATA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_APPDATA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\Application Data. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_DESKTOPDIRECTORY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_DESKTOPDIRECTORY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\Desktop. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_DOCUMENTS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_DOCUMENTS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\Documents. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_FAVORITES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_FAVORITES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_MUSIC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_MUSIC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\My Music. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_OEM_LINKS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_OEM_LINKS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Links to All Users OEM specific apps. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_PICTURES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_PICTURES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\My Pictures. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_PROGRAMS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_PROGRAMS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\Start Menu\Programs. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_STARTMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_STARTMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\Start Menu. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_STARTUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_STARTUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\Startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_TEMPLATES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_TEMPLATES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\Templates. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_VIDEO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMMON_VIDEO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>All Users\My Video. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMPUTERSNEARME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COMPUTERSNEARME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Computers Near Me (computered from Workgroup membership). </td>
</tr>
<tr>
<td id="Win32_CSIDL_CONNECTIONS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_CONNECTIONS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Network and Dial-up Connections. </td>
</tr>
<tr>
<td id="Win32_CSIDL_CONTROLS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_CONTROLS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>My Computer\Control Panel. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COOKIES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_COOKIES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_DESKTOP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_DESKTOP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;desktop&gt;. </td>
</tr>
<tr>
<td id="Win32_CSIDL_DESKTOPDIRECTORY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_DESKTOPDIRECTORY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\Desktop. </td>
</tr>
<tr>
<td id="Win32_CSIDL_DRIVES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_DRIVES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>My Computer. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FAVORITES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_FAVORITES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\Favorites. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_CREATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_FLAG_CREATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>combine with CSIDL_ value to force folder creation in SHGetFolderPath(). </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_DONT_UNEXPAND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_FLAG_DONT_UNEXPAND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>combine with CSIDL_ value to avoid unexpanding environment variables. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_DONT_VERIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_FLAG_DONT_VERIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>combine with CSIDL_ value to return an unverified folder path. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_MASK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_FLAG_MASK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>mask for all possible flag values. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_NO_ALIAS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_FLAG_NO_ALIAS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>combine with CSIDL_ value to insure non-typealias versions of the pidl. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_PER_USER_INIT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_FLAG_PER_USER_INIT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>combine with CSIDL_ value to indicate per-user init (eg. upgrade). </td>
</tr>
<tr>
<td id="Win32_CSIDL_FONTS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_FONTS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>windows\fonts. </td>
</tr>
<tr>
<td id="Win32_CSIDL_HISTORY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_HISTORY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_INTERNET" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_INTERNET</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Internet Explorer (icon on desktop). </td>
</tr>
<tr>
<td id="Win32_CSIDL_INTERNET_CACHE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_INTERNET_CACHE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_LOCAL_APPDATA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_LOCAL_APPDATA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\Local Settings\Applicaiton Data (non roaming). </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYDOCUMENTS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_MYDOCUMENTS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Personal was just a silly name for My Documents. </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYMUSIC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_MYMUSIC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>"My Music" folder. </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYPICTURES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_MYPICTURES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>C:\Program Files\My Pictures. </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYVIDEO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_MYVIDEO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>"My Videos" folder. </td>
</tr>
<tr>
<td id="Win32_CSIDL_NETHOOD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_NETHOOD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\nethood. </td>
</tr>
<tr>
<td id="Win32_CSIDL_NETWORK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_NETWORK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Network Neighborhood (My Network Places). </td>
</tr>
<tr>
<td id="Win32_CSIDL_PERSONAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_PERSONAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>My Documents. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PRINTERS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_PRINTERS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>My Computer\Printers. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PRINTHOOD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_PRINTHOOD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\PrintHood. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROFILE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_PROFILE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>USERPROFILE. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAMS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_PROGRAMS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Start Menu\Programs. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_PROGRAM_FILES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>C:\Program Files. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILESX86" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_PROGRAM_FILESX86</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>x86 C:\Program Files on RISC. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES_COMMON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_PROGRAM_FILES_COMMON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>C:\Program Files\Common. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES_COMMONX86" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_PROGRAM_FILES_COMMONX86</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>x86 Program Files\Common on RISC. </td>
</tr>
<tr>
<td id="Win32_CSIDL_RECENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_RECENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\Recent. </td>
</tr>
<tr>
<td id="Win32_CSIDL_RESOURCES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_RESOURCES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Resource Direcotry. </td>
</tr>
<tr>
<td id="Win32_CSIDL_RESOURCES_LOCALIZED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_RESOURCES_LOCALIZED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Localized Resource Direcotry. </td>
</tr>
<tr>
<td id="Win32_CSIDL_SENDTO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_SENDTO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\SendTo. </td>
</tr>
<tr>
<td id="Win32_CSIDL_STARTMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_STARTMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>&lt;user name&gt;\Start Menu. </td>
</tr>
<tr>
<td id="Win32_CSIDL_STARTUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_STARTUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Start Menu\Programs\Startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_SYSTEM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_SYSTEM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>GetSystemDirectory(). </td>
</tr>
<tr>
<td id="Win32_CSIDL_SYSTEMX86" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_SYSTEMX86</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>x86 system directory on RISC. </td>
</tr>
<tr>
<td id="Win32_CSIDL_TEMPLATES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_TEMPLATES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_WINDOWS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CSIDL_WINDOWS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>GetWindowsDirectory(). </td>
</tr>
<tr>
<td id="Win32_CS_BYTEALIGNCLIENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_BYTEALIGNCLIENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_BYTEALIGNWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_BYTEALIGNWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_CLASSDC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_CLASSDC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_DBLCLKS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_DBLCLKS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_DROPSHADOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_DROPSHADOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_GLOBALCLASS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_GLOBALCLASS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_HREDRAW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_HREDRAW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_IME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_IME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_NOCLOSE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_NOCLOSE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_OWNDC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_OWNDC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_PARENTDC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_PARENTDC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_SAVEBITS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_SAVEBITS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_VREDRAW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CS_VREDRAW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_ALL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CWP_ALL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_SKIPDISABLED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CWP_SKIPDISABLED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_SKIPINVISIBLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CWP_SKIPINVISIBLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_SKIPTRANSPARENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CWP_SKIPTRANSPARENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CW_USEDEFAULT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CW_USEDEFAULT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_DEFAULT_MINIMUM_ENTITIES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">DEFAULT_MINIMUM_ENTITIES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_DM_POINTERHITTEST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">DM_POINTERHITTEST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_DEVICE_NOT_CONNECTED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">ERROR_DEVICE_NOT_CONNECTED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_FILE_NOT_FOUND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">ERROR_FILE_NOT_FOUND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_IO_PENDING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">ERROR_IO_PENDING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_NO_MORE_FILES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">ERROR_NO_MORE_FILES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_SUCCESS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">ERROR_SUCCESS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FALSE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FALSE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_ARCHIVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_ARCHIVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_COMPRESSED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_COMPRESSED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_DEVICE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_DEVICE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_DIRECTORY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_DIRECTORY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_EA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_EA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_ENCRYPTED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_ENCRYPTED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_HIDDEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_HIDDEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_INTEGRITY_STREAM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_INTEGRITY_STREAM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NORMAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_NORMAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NOT_CONTENT_INDEXED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_NOT_CONTENT_INDEXED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NO_SCRUB_DATA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_NO_SCRUB_DATA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_OFFLINE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_OFFLINE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_PINNED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_PINNED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_READONLY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_READONLY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_RECALL_ON_OPEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_RECALL_ON_OPEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_REPARSE_POINT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_REPARSE_POINT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_SPARSE_FILE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_SPARSE_FILE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_SYSTEM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_SYSTEM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_TEMPORARY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_TEMPORARY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_UNPINNED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_UNPINNED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_VIRTUAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_ATTRIBUTE_VIRTUAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_BEGIN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_BEGIN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_CURRENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_CURRENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_END" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_END</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_READ_ATTRIBUTES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_READ_ATTRIBUTES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_DELETE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_SHARE_DELETE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_READ" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_SHARE_READ</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_WRITE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_SHARE_WRITE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_WRITE_ATTRIBUTES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FILE_WRITE_ATTRIBUTES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_BLUE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FOREGROUND_BLUE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_GREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FOREGROUND_GREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_INTENSITY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FOREGROUND_INTENSITY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_MASK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FOREGROUND_MASK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_RED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FOREGROUND_RED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_ALLOCATE_BUFFER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FORMAT_MESSAGE_ALLOCATE_BUFFER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_ARGUMENT_ARRAY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FORMAT_MESSAGE_ARGUMENT_ARRAY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_HMODULE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FORMAT_MESSAGE_FROM_HMODULE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_STRING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FORMAT_MESSAGE_FROM_STRING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_SYSTEM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FORMAT_MESSAGE_FROM_SYSTEM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_IGNORE_INSERTS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FORMAT_MESSAGE_IGNORE_INSERTS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_MAX_WIDTH_MASK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">FORMAT_MESSAGE_MAX_WIDTH_MASK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_ALL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GENERIC_ALL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_EXECUTE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GENERIC_EXECUTE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_READ" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GENERIC_READ</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_WRITE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GENERIC_WRITE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_DDESHARE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_DDESHARE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_DISCARDABLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_DISCARDABLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_FIXED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_FIXED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_INVALID_HANDLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_INVALID_HANDLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_LOWER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_LOWER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_MODIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_MODIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_MOVEABLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_MOVEABLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NOCOMPACT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_NOCOMPACT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NODISCARD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_NODISCARD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NOTIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_NOTIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NOT_BANKED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_NOT_BANKED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_SHARE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_SHARE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_VALID_FLAGS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_VALID_FLAGS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_ZEROINIT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GMEM_ZEROINIT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_HINSTANCE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GWLP_HINSTANCE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_HWNDPARENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GWLP_HWNDPARENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_ID" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GWLP_ID</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_USERDATA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GWLP_USERDATA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_WNDPROC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GWLP_WNDPROC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_CHILD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GW_CHILD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_ENABLEDPOPUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GW_ENABLEDPOPUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDFIRST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GW_HWNDFIRST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDLAST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GW_HWNDLAST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDNEXT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GW_HWNDNEXT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDPREV" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GW_HWNDPREV</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_MAX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GW_MAX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_OWNER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GW_OWNER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_BOTTOM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HWND_BOTTOM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_NOTOPMOST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HWND_NOTOPMOST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_TOP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HWND_TOP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_TOPMOST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HWND_TOPMOST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDABORT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDABORT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDCANCEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDCANCEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDCLOSE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDCLOSE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDCONTINUE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDCONTINUE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_APPSTARTING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_APPSTARTING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_ARROW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_ARROW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_CROSS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_CROSS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_HAND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_HAND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_HELP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_HELP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_IBEAM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_IBEAM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_ICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_ICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_NO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_NO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_PERSON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_PERSON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_PIN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_PIN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_SIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZEALL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_SIZEALL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZENESW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_SIZENESW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZENS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_SIZENS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZENWSE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_SIZENWSE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZEWE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_SIZEWE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_UPARROW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_UPARROW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_WAIT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDC_WAIT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDHELP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDHELP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDIGNORE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDIGNORE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDNO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDNO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDOK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDOK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDRETRY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDRETRY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDTIMEOUT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDTIMEOUT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDTRYAGAIN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDTRYAGAIN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDYES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IDYES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_INFINITE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">INFINITE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_FILE_ATTRIBUTES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">INVALID_FILE_ATTRIBUTES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_FILE_SIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">INVALID_FILE_SIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_HANDLE_VALUE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">INVALID_HANDLE_VALUE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_SET_FILE_POINTER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">INVALID_SET_FILE_POINTER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LANG_ENGLISH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LANG_ENGLISH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LWA_ALPHA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LWA_ALPHA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LWA_COLORKEY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LWA_COLORKEY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_CHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAPVK_VK_TO_CHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_VSC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAPVK_VK_TO_VSC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_VSC_EX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAPVK_VK_TO_VSC_EX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VSC_TO_VK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAPVK_VSC_TO_VK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VSC_TO_VK_EX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAPVK_VSC_TO_VK_EX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAXIMUM_WAIT_OBJECTS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAXIMUM_WAIT_OBJECTS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_ADDRESS_LENGTH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAX_ADAPTER_ADDRESS_LENGTH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_DESCRIPTION_LENGTH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAX_ADAPTER_DESCRIPTION_LENGTH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_NAME_LENGTH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAX_ADAPTER_NAME_LENGTH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_DHCPV6_DUID_LENGTH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAX_DHCPV6_DUID_LENGTH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_DNS_SUFFIX_STRING_LENGTH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAX_DNS_SUFFIX_STRING_LENGTH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_DOMAIN_NAME_LEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAX_DOMAIN_NAME_LEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_HOSTNAME_LEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAX_HOSTNAME_LEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_PATH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAX_PATH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_SCOPE_ID_LEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MAX_SCOPE_ID_LEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ABORTRETRYIGNORE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_ABORTRETRYIGNORE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_APPLMODAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_APPLMODAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_CANCELTRYCONTINUE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_CANCELTRYCONTINUE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFAULT_DESKTOP_ONLY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_DEFAULT_DESKTOP_ONLY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON1" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_DEFBUTTON1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON2" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_DEFBUTTON2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON3" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_DEFBUTTON3</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON4" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_DEFBUTTON4</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_HELP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_HELP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONASTERISK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_ICONASTERISK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONERROR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_ICONERROR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONEXCLAMATION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_ICONEXCLAMATION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONHAND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_ICONHAND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONINFORMATION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_ICONINFORMATION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONQUESTION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_ICONQUESTION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONSTOP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_ICONSTOP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONWARNING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_ICONWARNING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_NOFOCUS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_NOFOCUS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_OK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_OK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_OKCANCEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_OKCANCEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_RETRYCANCEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_RETRYCANCEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_RIGHT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_RIGHT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_RTLREADING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_RTLREADING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_SETFOREGROUND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_SETFOREGROUND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_SYSTEMMODAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_SYSTEMMODAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_TASKMODAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_TASKMODAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_TOPMOST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_TOPMOST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_USERICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_USERICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_YESNO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_YESNO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_YESNOCANCEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MB_YESNOCANCEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MN_GETHMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MN_GETHMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_ALT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MOD_ALT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_CONTROL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MOD_CONTROL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_NOREPEAT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MOD_NOREPEAT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_SHIFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MOD_SHIFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_WIN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MOD_WIN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTONEAREST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MONITOR_DEFAULTTONEAREST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTONULL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MONITOR_DEFAULTTONULL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTOPRIMARY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">MONITOR_DEFAULTTOPRIMARY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_GUID" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIF_GUID</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_ICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIF_ICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_INFO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIF_INFO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_MESSAGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIF_MESSAGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_REALTIME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIF_REALTIME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_SHOWTIP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIF_SHOWTIP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_STATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIF_STATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_TIP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIF_TIP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_ADD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIM_ADD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_DELETE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIM_DELETE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_MODIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIM_MODIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_SETFOCUS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIM_SETFOCUS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_SETVERSION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">NIM_SETVERSION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_OPEN_ALWAYS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">OPEN_ALWAYS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_OPEN_EXISTING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">OPEN_EXISTING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_PM_NOREMOVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">PM_NOREMOVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_PM_NOYIELD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">PM_NOYIELD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_PM_REMOVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">PM_REMOVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_ARRANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_ARRANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_CLOSE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_CLOSE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_CONTEXTHELP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_CONTEXTHELP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_DEFAULT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_DEFAULT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_HOTKEY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_HOTKEY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_HSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_HSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_KEYMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_KEYMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MAXIMIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_MAXIMIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MINIMIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_MINIMIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MONITORPOWER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_MONITORPOWER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MOUSEMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_MOUSEMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MOVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_MOVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_NEXTWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_NEXTWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_PREVWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_PREVWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_RESTORE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_RESTORE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_SCREENSAVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_SCREENSAVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_SEPARATOR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_SEPARATOR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_SIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_SIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_TASKLIST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_TASKLIST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_VSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SC_VSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGFI_ADDOVERLAYS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_ADDOVERLAYS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>apply the appropriate overlays. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ATTRIBUTES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_ATTRIBUTES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get attributes. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ATTR_SPECIFIED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_ATTR_SPECIFIED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get only specified attributes. </td>
</tr>
<tr>
<td id="Win32_SHGFI_DISPLAYNAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_DISPLAYNAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get display name. </td>
</tr>
<tr>
<td id="Win32_SHGFI_EXETYPE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_EXETYPE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>return exe type. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_ICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ICONLOCATION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_ICONLOCATION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get icon location. </td>
</tr>
<tr>
<td id="Win32_SHGFI_LARGEICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_LARGEICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get large icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_LINKOVERLAY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_LINKOVERLAY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>put a link overlay on icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_OPENICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_OPENICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get open icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_OVERLAYINDEX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_OVERLAYINDEX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Get the index of the overlay. </td>
</tr>
<tr>
<td id="Win32_SHGFI_PIDL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_PIDL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>pszPath is a pidl. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SELECTED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_SELECTED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>show icon in selected state. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SHELLICONSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_SHELLICONSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get shell size icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SMALLICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_SMALLICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get small icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SYSICONINDEX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_SYSICONINDEX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get system icon index. </td>
</tr>
<tr>
<td id="Win32_SHGFI_TYPENAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_TYPENAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>get type name. </td>
</tr>
<tr>
<td id="Win32_SHGFI_USEFILEATTRIBUTES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGFI_USEFILEATTRIBUTES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>use passed dwFileAttribute. </td>
</tr>
<tr>
<td id="Win32_SHGSI_ICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGSI_ICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_ICONLOCATION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGSI_ICONLOCATION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td>you always get the icon location. </td>
</tr>
<tr>
<td id="Win32_SHGSI_LARGEICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGSI_LARGEICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_LINKOVERLAY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGSI_LINKOVERLAY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SELECTED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGSI_SELECTED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SHELLICONSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGSI_SHELLICONSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SMALLICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGSI_SMALLICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SYSICONINDEX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHGSI_SYSICONINDEX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_ARRANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_ARRANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CARETBLINKINGENABLED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CARETBLINKINGENABLED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CLEANBOOT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CLEANBOOT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CMETRICS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CMETRICS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CMONITORS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CMONITORS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CMOUSEBUTTONS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CMOUSEBUTTONS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CONVERTIBLESLATEMODE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CONVERTIBLESLATEMODE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXBORDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXBORDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXCURSOR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXCURSOR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXDLGFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXDLGFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXDOUBLECLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXDOUBLECLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXDRAG" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXDRAG</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXEDGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXEDGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFIXEDFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXFIXEDFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFOCUSBORDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXFOCUSBORDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFULLSCREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXFULLSCREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXHSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXHSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXHTHUMB" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXHTHUMB</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXICONSPACING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXICONSPACING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMAXIMIZED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXMAXIMIZED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMAXTRACK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXMAXTRACK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMENUCHECK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXMENUCHECK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMENUSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXMENUSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMIN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXMIN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMINIMIZED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXMINIMIZED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMINSPACING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXMINSPACING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMINTRACK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXMINTRACK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXPADDEDBORDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXPADDEDBORDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSCREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXSCREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSIZEFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXSIZEFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSMICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXSMICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSMSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXSMSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXVIRTUALSCREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXVIRTUALSCREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXVSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CXVSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYBORDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYBORDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYCAPTION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYCAPTION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYCURSOR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYCURSOR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYDLGFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYDLGFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYDOUBLECLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYDOUBLECLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYDRAG" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYDRAG</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYEDGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYEDGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFIXEDFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYFIXEDFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFOCUSBORDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYFOCUSBORDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFULLSCREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYFULLSCREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYHSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYHSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYICONSPACING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYICONSPACING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYKANJIWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYKANJIWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMAXIMIZED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYMAXIMIZED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMAXTRACK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYMAXTRACK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMENUCHECK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYMENUCHECK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMENUSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYMENUSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMIN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYMIN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMINIMIZED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYMINIMIZED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMINSPACING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYMINSPACING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMINTRACK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYMINTRACK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSCREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYSCREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSIZEFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYSIZEFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSMCAPTION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYSMCAPTION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSMICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYSMICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSMSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYSMSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYVIRTUALSCREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYVIRTUALSCREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYVSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYVSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYVTHUMB" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_CYVTHUMB</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_DBCSENABLED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_DBCSENABLED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_DEBUG" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_DEBUG</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_DIGITIZER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_DIGITIZER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_IMMENABLED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_IMMENABLED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MAXIMUMTOUCHES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_MAXIMUMTOUCHES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MEDIACENTER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_MEDIACENTER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MENUDROPALIGNMENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_MENUDROPALIGNMENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MIDEASTENABLED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_MIDEASTENABLED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MOUSEHORIZONTALWHEELPRESENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_MOUSEHORIZONTALWHEELPRESENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MOUSEPRESENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_MOUSEPRESENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MOUSEWHEELPRESENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_MOUSEWHEELPRESENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_NETWORK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_NETWORK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_PENWINDOWS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_PENWINDOWS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_REMOTECONTROL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_REMOTECONTROL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_REMOTESESSION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_REMOTESESSION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED1" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_RESERVED1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED2" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_RESERVED2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED3" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_RESERVED3</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED4" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_RESERVED4</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SAMEDISPLAYFORMAT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_SAMEDISPLAYFORMAT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SECURE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_SECURE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SERVERR2" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_SERVERR2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SHOWSOUNDS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_SHOWSOUNDS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SHUTTINGDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_SHUTTINGDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SLOWMACHINE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_SLOWMACHINE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_STARTER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_STARTER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SWAPBUTTON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_SWAPBUTTON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SYSTEMDOCKED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_SYSTEMDOCKED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_TABLETPC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_TABLETPC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_XVIRTUALSCREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_XVIRTUALSCREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_YVIRTUALSCREEN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SM_YVIRTUALSCREEN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SRWLOCK_INIT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SRWLOCK_INIT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_STD_INPUT_HANDLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">STD_INPUT_HANDLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_STD_OUTPUT_HANDLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">STD_OUTPUT_HANDLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SUBLANG_DEFAULT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SUBLANG_DEFAULT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_ASYNCWINDOWPOS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_ASYNCWINDOWPOS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_DEFERERASE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_DEFERERASE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_DRAWFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_DRAWFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_FRAMECHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_FRAMECHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_HIDEWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_HIDEWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOACTIVATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_NOACTIVATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOCOPYBITS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_NOCOPYBITS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOMOVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_NOMOVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOOWNERZORDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_NOOWNERZORDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOREDRAW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_NOREDRAW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOREPOSITION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_NOREPOSITION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOSENDCHANGING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_NOSENDCHANGING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_NOSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOZORDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_NOZORDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_SHOWWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SWP_SHOWWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_FORCEMINIMIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_FORCEMINIMIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_HIDE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_HIDE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_MAX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_MAX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_MAXIMIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_MAXIMIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_MINIMIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_MINIMIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_NORMAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_NORMAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_RESTORE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_RESTORE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_SHOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWDEFAULT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_SHOWDEFAULT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWMAXIMIZED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_SHOWMAXIMIZED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWMINIMIZED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_SHOWMINIMIZED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWMINNOACTIVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_SHOWMINNOACTIVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWNA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_SHOWNA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWNOACTIVATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_SHOWNOACTIVATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWNORMAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SW_SHOWNORMAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_S_OK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">S_OK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_ABOVE_NORMAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">THREAD_PRIORITY_ABOVE_NORMAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_BELOW_NORMAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">THREAD_PRIORITY_BELOW_NORMAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_HIGHEST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">THREAD_PRIORITY_HIGHEST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_LOWEST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">THREAD_PRIORITY_LOWEST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_NORMAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">THREAD_PRIORITY_NORMAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_TRUE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">TRUE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_TRUNCATE_EXISTING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">TRUNCATE_EXISTING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ACCEPT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_ACCEPT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ADD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_ADD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_APPS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_APPS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ATTN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_ATTN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BACK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_BACK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_BACK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_BROWSER_BACK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_FAVORITES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_BROWSER_FAVORITES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_FORWARD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_BROWSER_FORWARD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_HOME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_BROWSER_HOME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_REFRESH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_BROWSER_REFRESH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_SEARCH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_BROWSER_SEARCH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_STOP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_BROWSER_STOP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CANCEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_CANCEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CAPITAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_CAPITAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CLEAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_CLEAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CONTROL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_CONTROL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CONVERT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_CONVERT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CRSEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_CRSEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DECIMAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_DECIMAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DELETE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_DELETE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DIVIDE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_DIVIDE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_DOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_END" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_END</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_EREOF" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_EREOF</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ESCAPE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_ESCAPE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_EXECUTE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_EXECUTE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_EXSEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_EXSEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F1" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F10" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F10</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F11" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F11</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F12" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F12</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F13" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F13</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F14" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F14</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F15" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F15</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F16" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F16</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F17" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F17</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F18" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F18</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F19" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F19</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F2" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F20" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F20</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F21" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F21</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F22" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F22</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F23" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F23</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F24" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F24</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F3" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F3</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F4" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F4</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F5" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F5</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F6" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F6</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F7" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F7</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F8" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F8</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F9" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_F9</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_FINAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_FINAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_A" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_A</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_B" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_B</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_DOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_DPAD_DOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_LEFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_DPAD_LEFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_RIGHT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_DPAD_RIGHT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_UP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_DPAD_UP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_SHOULDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_LEFT_SHOULDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_DOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_LEFT_THUMBSTICK_DOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_LEFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_LEFT_THUMBSTICK_LEFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_UP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_LEFT_THUMBSTICK_UP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_TRIGGER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_LEFT_TRIGGER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_MENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_MENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_SHOULDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_RIGHT_SHOULDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_UP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_RIGHT_THUMBSTICK_UP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_TRIGGER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_RIGHT_TRIGGER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_VIEW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_VIEW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_X" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_X</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_Y" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_GAMEPAD_Y</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HANGEUL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_HANGEUL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HANGUL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_HANGUL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HANJA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_HANJA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HELP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_HELP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HOME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_HOME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ICO_00" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_ICO_00</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ICO_CLEAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_ICO_CLEAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ICO_HELP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_ICO_HELP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_INSERT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_INSERT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_JUNJA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_JUNJA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_KANA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_KANA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_KANJI" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_KANJI</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_APP1" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LAUNCH_APP1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_APP2" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LAUNCH_APP2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_MAIL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LAUNCH_MAIL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_MEDIA_SELECT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LAUNCH_MEDIA_SELECT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LBUTTON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LBUTTON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LCONTROL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LCONTROL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LEFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LEFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LSHIFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LSHIFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LWIN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_LWIN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MBUTTON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_MBUTTON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_NEXT_TRACK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_MEDIA_NEXT_TRACK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_PLAY_PAUSE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_MEDIA_PLAY_PAUSE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_PREV_TRACK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_MEDIA_PREV_TRACK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_STOP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_MEDIA_STOP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_MENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MODECHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_MODECHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MULTIPLY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_MULTIPLY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_ACCEPT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NAVIGATION_ACCEPT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_CANCEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NAVIGATION_CANCEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_DOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NAVIGATION_DOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_LEFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NAVIGATION_LEFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_MENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NAVIGATION_MENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_RIGHT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NAVIGATION_RIGHT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_UP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NAVIGATION_UP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_VIEW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NAVIGATION_VIEW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NEXT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NEXT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NONAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NONAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NONCONVERT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NONCONVERT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMLOCK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMLOCK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD0" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD0</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD1" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD2" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD3" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD3</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD4" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD4</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD5" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD5</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD6" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD6</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD7" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD7</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD8" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD8</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD9" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_NUMPAD9</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_1" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_102" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_102</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_2" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_3" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_3</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_4" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_4</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_5" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_5</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_6" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_6</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_7" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_7</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_8" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_8</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_ATTN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_ATTN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_AUTO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_AUTO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_AX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_AX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_BACKTAB" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_BACKTAB</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_CLEAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_CLEAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_COMMA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_COMMA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_COPY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_COPY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_CUSEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_CUSEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_ENLW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_ENLW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FINISH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_FINISH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_JISHO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_FJ_JISHO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_LOYA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_FJ_LOYA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_MASSHOU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_FJ_MASSHOU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_ROYA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_FJ_ROYA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_TOUROKU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_FJ_TOUROKU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_JUMP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_JUMP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_MINUS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_MINUS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_NEC_EQUAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_NEC_EQUAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA1" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_PA1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA2" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_PA2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA3" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_PA3</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PERIOD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_PERIOD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PLUS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_PLUS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_RESET" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_RESET</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_WSCTRL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_OEM_WSCTRL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PA1" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_PA1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PACKET" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_PACKET</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PAUSE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_PAUSE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PLAY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_PLAY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PRINT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_PRINT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PRIOR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_PRIOR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PROCESSKEY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_PROCESSKEY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RBUTTON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_RBUTTON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RCONTROL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_RCONTROL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RETURN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_RETURN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RIGHT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_RIGHT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_RMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RSHIFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_RSHIFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RWIN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_RWIN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_SCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SELECT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_SELECT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SEPARATOR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_SEPARATOR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SHIFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_SHIFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SLEEP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_SLEEP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SNAPSHOT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_SNAPSHOT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SPACE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_SPACE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SUBTRACT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_SUBTRACT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_TAB" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_TAB</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_UP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_UP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_DOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_VOLUME_DOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_MUTE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_VOLUME_MUTE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_UP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_VOLUME_UP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_XBUTTON1" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_XBUTTON1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_XBUTTON2" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_XBUTTON2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ZOOM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">VK_ZOOM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_ABANDONED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WAIT_ABANDONED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_FAILED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WAIT_FAILED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_OBJECT_0" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WAIT_OBJECT_0</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_TIMEOUT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WAIT_TIMEOUT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WA_ACTIVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WA_ACTIVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WA_CLICKACTIVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WA_CLICKACTIVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WA_INACTIVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WA_INACTIVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WHEEL_DELTA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WHEEL_DELTA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ACTIVATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_ACTIVATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ACTIVATEAPP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_ACTIVATEAPP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_AFXFIRST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_AFXFIRST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_AFXLAST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_AFXLAST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_APP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_APP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_APPCOMMAND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_APPCOMMAND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ASKCBFORMATNAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_ASKCBFORMATNAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CANCELJOURNAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CANCELJOURNAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CANCELMODE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CANCELMODE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CAPTURECHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CAPTURECHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHANGECBCHAIN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CHANGECBCHAIN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHANGEUISTATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CHANGEUISTATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHARTOITEM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CHARTOITEM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHILDACTIVATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CHILDACTIVATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CLEAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CLEAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CLIPBOARDUPDATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CLIPBOARDUPDATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CLOSE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CLOSE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMMAND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_COMMAND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMMNOTIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_COMMNOTIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMPACTING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_COMPACTING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMPAREITEM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_COMPAREITEM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CONTEXTMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CONTEXTMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COPY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_COPY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COPYDATA" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_COPYDATA</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CREATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CREATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORBTN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CTLCOLORBTN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORDLG" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CTLCOLORDLG</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLOREDIT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CTLCOLOREDIT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORLISTBOX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CTLCOLORLISTBOX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORMSGBOX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CTLCOLORMSGBOX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORSCROLLBAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CTLCOLORSCROLLBAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORSTATIC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CTLCOLORSTATIC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CUT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_CUT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DEADCHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DEADCHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DELETEITEM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DELETEITEM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DESTROY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DESTROY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DESTROYCLIPBOARD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DESTROYCLIPBOARD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DEVICECHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DEVICECHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DEVMODECHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DEVMODECHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DISPLAYCHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DISPLAYCHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DPICHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED_AFTERPARENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DPICHANGED_AFTERPARENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED_BEFOREPARENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DPICHANGED_BEFOREPARENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DRAWCLIPBOARD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DRAWCLIPBOARD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DRAWITEM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DRAWITEM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DROPFILES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DROPFILES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMCOLORIZATIONCOLORCHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DWMCOLORIZATIONCOLORCHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMCOMPOSITIONCHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DWMCOMPOSITIONCHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMNCRENDERINGCHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DWMNCRENDERINGCHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMSENDICONICLIVEPREVIEWBITMAP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DWMSENDICONICLIVEPREVIEWBITMAP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMSENDICONICTHUMBNAIL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DWMSENDICONICTHUMBNAIL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMWINDOWMAXIMIZEDCHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_DWMWINDOWMAXIMIZEDCHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENABLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_ENABLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENTERIDLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_ENTERIDLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENTERMENULOOP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_ENTERMENULOOP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENTERSIZEMOVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_ENTERSIZEMOVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ERASEBKGND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_ERASEBKGND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_EXITMENULOOP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_EXITMENULOOP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_EXITSIZEMOVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_EXITSIZEMOVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_FONTCHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_FONTCHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GESTURE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GESTURE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GESTURENOTIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GESTURENOTIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETDLGCODE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETDLGCODE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETDPISCALEDSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETDPISCALEDSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETFONT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETFONT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETHOTKEY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETHOTKEY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETMINMAXINFO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETMINMAXINFO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETOBJECT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETOBJECT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETTEXT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETTEXT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETTEXTLENGTH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETTEXTLENGTH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETTITLEBARINFOEX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_GETTITLEBARINFOEX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HANDHELDFIRST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_HANDHELDFIRST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HANDHELDLAST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_HANDHELDLAST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HELP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_HELP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HOTKEY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_HOTKEY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_HSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HSCROLLCLIPBOARD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_HSCROLLCLIPBOARD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ICONERASEBKGND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_ICONERASEBKGND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_CHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_CHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_COMPOSITION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_COMPOSITION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_COMPOSITIONFULL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_COMPOSITIONFULL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_CONTROL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_CONTROL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_ENDCOMPOSITION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_ENDCOMPOSITION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_KEYDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYLAST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_KEYLAST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_KEYUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_NOTIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_NOTIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_REQUEST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_REQUEST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_SELECT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_SELECT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_SETCONTEXT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_SETCONTEXT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_STARTCOMPOSITION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_IME_STARTCOMPOSITION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INITDIALOG" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_INITDIALOG</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INITMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_INITMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INITMENUPOPUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_INITMENUPOPUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_INPUT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUTLANGCHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_INPUTLANGCHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUTLANGCHANGEREQUEST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_INPUTLANGCHANGEREQUEST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUT_DEVICE_CHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_INPUT_DEVICE_CHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_KEYDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYFIRST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_KEYFIRST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYLAST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_KEYLAST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_KEYUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KILLFOCUS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_KILLFOCUS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONDBLCLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_LBUTTONDBLCLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_LBUTTONDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_LBUTTONUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONDBLCLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MBUTTONDBLCLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MBUTTONDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MBUTTONUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIACTIVATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDIACTIVATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDICASCADE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDICASCADE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDICREATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDICREATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIDESTROY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDIDESTROY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIGETACTIVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDIGETACTIVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIICONARRANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDIICONARRANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIMAXIMIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDIMAXIMIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDINEXT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDINEXT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIREFRESHMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDIREFRESHMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIRESTORE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDIRESTORE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDISETMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDISETMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDITILE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MDITILE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MEASUREITEM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MEASUREITEM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUCHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MENUCHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUCOMMAND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MENUCOMMAND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUDRAG" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MENUDRAG</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUGETOBJECT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MENUGETOBJECT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENURBUTTONUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MENURBUTTONUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUSELECT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MENUSELECT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEACTIVATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOUSEACTIVATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEFIRST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOUSEFIRST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEHOVER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOUSEHOVER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEHWHEEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOUSEHWHEEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSELAST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOUSELAST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSELEAVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOUSELEAVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEMOVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOUSEMOVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEWHEEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOUSEWHEEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOVING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_MOVING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCACTIVATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCACTIVATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCCALCSIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCCALCSIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCCREATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCCREATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCDESTROY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCDESTROY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCHITTEST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCHITTEST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONDBLCLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCLBUTTONDBLCLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCLBUTTONDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCLBUTTONUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONDBLCLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCMBUTTONDBLCLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCMBUTTONDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCMBUTTONUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSEHOVER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCMOUSEHOVER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSELEAVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCMOUSELEAVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSEMOVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCMOUSEMOVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPAINT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCPAINT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCPOINTERDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCPOINTERUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERUPDATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCPOINTERUPDATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONDBLCLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCRBUTTONDBLCLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCRBUTTONDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCRBUTTONUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONDBLCLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCXBUTTONDBLCLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCXBUTTONDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NCXBUTTONUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NEXTDLGCTL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NEXTDLGCTL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NEXTMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NEXTMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NOTIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NOTIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NOTIFYFORMAT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_NOTIFYFORMAT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PAINT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PAINT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PAINTCLIPBOARD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PAINTCLIPBOARD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PAINTICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PAINTICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PALETTECHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PALETTECHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PALETTEISCHANGING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PALETTEISCHANGING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PARENTNOTIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PARENTNOTIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PASTE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PASTE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PENWINFIRST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PENWINFIRST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PENWINLAST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PENWINLAST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERACTIVATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERACTIVATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERCAPTURECHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERCAPTURECHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICECHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERDEVICECHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICEINRANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERDEVICEINRANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICEOUTOFRANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERDEVICEOUTOFRANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERENTER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERENTER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERHWHEEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERHWHEEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERLEAVE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERLEAVE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDAWAY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERROUTEDAWAY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDRELEASED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERROUTEDRELEASED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDTO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERROUTEDTO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERUPDATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERUPDATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERWHEEL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POINTERWHEEL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POWER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POWER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POWERBROADCAST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_POWERBROADCAST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PRINT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PRINT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PRINTCLIENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_PRINTCLIENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUERYDRAGICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_QUERYDRAGICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUERYNEWPALETTE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_QUERYNEWPALETTE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUERYUISTATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_QUERYUISTATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUEUESYNC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_QUEUESYNC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUIT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_QUIT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONDBLCLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_RBUTTONDBLCLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_RBUTTONDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_RBUTTONUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RENDERALLFORMATS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_RENDERALLFORMATS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RENDERFORMAT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_RENDERFORMAT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETCURSOR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SETCURSOR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETFOCUS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SETFOCUS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETFONT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SETFONT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETHOTKEY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SETHOTKEY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SETICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETREDRAW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SETREDRAW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETTEXT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SETTEXT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETTINGCHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SETTINGCHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SHOWWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SHOWWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SIZECLIPBOARD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SIZECLIPBOARD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SIZING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SIZING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SPOOLERSTATUS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SPOOLERSTATUS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_STYLECHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_STYLECHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_STYLECHANGING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_STYLECHANGING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYNCPAINT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SYNCPAINT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSCHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SYSCHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSCOLORCHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SYSCOLORCHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSCOMMAND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SYSCOMMAND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSDEADCHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SYSDEADCHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSKEYDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SYSKEYDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSKEYUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_SYSKEYUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TABLET_FIRST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_TABLET_FIRST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TABLET_LAST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_TABLET_LAST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TCARD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_TCARD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_THEMECHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_THEMECHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TIMECHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_TIMECHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TIMER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_TIMER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TOUCH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_TOUCH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TOUCHHITTESTING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_TOUCHHITTESTING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UNDO" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_UNDO</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UNICHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_UNICHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UNINITMENUPOPUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_UNINITMENUPOPUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UPDATEUISTATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_UPDATEUISTATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_USER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_USER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_USERCHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_USERCHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_VKEYTOITEM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_VKEYTOITEM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_VSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_VSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_VSCROLLCLIPBOARD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_VSCROLLCLIPBOARD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WINDOWPOSCHANGED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_WINDOWPOSCHANGED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WINDOWPOSCHANGING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_WINDOWPOSCHANGING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WININICHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_WININICHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WTSSESSION_CHANGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_WTSSESSION_CHANGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONDBLCLK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_XBUTTONDBLCLK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONDOWN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_XBUTTONDOWN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WM_XBUTTONUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_BORDER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_BORDER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CAPTION" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_CAPTION</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CHILD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_CHILD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CLIPCHILDREN" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_CLIPCHILDREN</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CLIPSIBLINGS" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_CLIPSIBLINGS</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_DISABLED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_DISABLED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_DLGFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_DLGFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_ACCEPTFILES" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_ACCEPTFILES</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_APPWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_APPWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_CLIENTEDGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_CLIENTEDGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_COMPOSITED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_COMPOSITED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_CONTEXTHELP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_CONTEXTHELP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_CONTROLPARENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_CONTROLPARENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_DLGMODALFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_DLGMODALFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LAYERED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_LAYERED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LAYOUTRTL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_LAYOUTRTL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LEFT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_LEFT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LEFTSCROLLBAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_LEFTSCROLLBAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LTRREADING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_LTRREADING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_MDICHILD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_MDICHILD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOACTIVATE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_NOACTIVATE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOINHERITLAYOUT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_NOINHERITLAYOUT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOPARENTNOTIFY" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_NOPARENTNOTIFY</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOREDIRECTIONBITMAP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_NOREDIRECTIONBITMAP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_OVERLAPPEDWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_OVERLAPPEDWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_PALETTEWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_PALETTEWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_RIGHT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_RIGHT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_RIGHTSCROLLBAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_RIGHTSCROLLBAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_RTLREADING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_RTLREADING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_STATICEDGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_STATICEDGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_TOOLWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_TOOLWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_TOPMOST" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_TOPMOST</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_TRANSPARENT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_TRANSPARENT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_WINDOWEDGE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_EX_WINDOWEDGE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_GROUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_GROUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_HSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_HSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MAXIMIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_MAXIMIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MAXIMIZEBOX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_MAXIMIZEBOX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MINIMIZE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_MINIMIZE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MINIMIZEBOX" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_MINIMIZEBOX</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_OVERLAPPED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_OVERLAPPED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_OVERLAPPEDWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_OVERLAPPEDWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_POPUP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_POPUP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_POPUPWINDOW" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_POPUPWINDOW</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_SYSMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_SYSMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_TABSTOP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_TABSTOP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_THICKFRAME" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_THICKFRAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_VISIBLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_VISIBLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_VSCROLL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WS_VSCROLL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ATOM"><span class="titletype">type alias</span> <span class="titlelight">Win32.</span><span class="titlestrong">Type Aliases</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L5" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td id="Win32_ATOM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">ATOM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_BOOL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_BYTE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">BYTE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_CHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">CHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_COLORREF" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_DWORD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_DWORD64" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">DWORD64</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_DWORD_PTR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">DWORD_PTR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HANDLE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HBITMAP" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HBITMAP</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HBRUSH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HBRUSH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HCURSOR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HCURSOR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HDC" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HGLOBAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HGLOBAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HICON" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HINSTANCE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HINSTANCE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HLOCAL" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HLOCAL</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HMENU" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HMENU</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HMODULE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HMODULE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HMONITOR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HMONITOR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HRESULT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HRESULT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_IP_MASK_STRING" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">IP_MASK_STRING</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDRESS_STRING">IP_ADDRESS_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LARGE_INTEGER" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LARGE_INTEGER</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LONG" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPARAM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPBYTE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPBYTE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BYTE">BYTE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPCH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCSTR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCTSTR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPCTSTR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCVOID" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPCVOID</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCWSTR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPDWORD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPOVERLAPPED" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPOVERLAPPED</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPSTR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPTHREAD_START_ROUTINE" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPTHREAD_START_ROUTINE</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(*</span><span class="SyntaxType">void</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPVOID" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPWCH" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPWCH</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPWSTR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_LRESULT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">LRESULT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_PCSTR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">PCSTR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_PCWSTR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">PCWSTR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_PLONG" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">PLONG</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LONG">LONG</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHORT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHORT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHSTDAPI" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SHSTDAPI</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SIZE_T" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIZE_T</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_SRWLOCK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_UINT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_ULONG" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">ULONG</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_ULONG64" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">ULONG64</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_USHORT" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">USHORT</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAITORTIMERCALLBACK" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WAITORTIMERCALLBACK</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode">(*</span><span class="SyntaxType">void</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">)</td>
<td></td>
</tr>
<tr>
<td id="Win32_WCHAR" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WCHAR</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WORD" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WORD</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_WPARAM" class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">WPARAM</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_time_t" class="codetype"></span><span class="SyntaxCode">time_t</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td id="Win32_va_list" class="codetype"></span><span class="SyntaxCode">va_list</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_AcquireSRWLockExclusive"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">AcquireSRWLockExclusive</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L384" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AcquireSRWLockExclusive</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SRWLOCK">SRWLOCK</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_AcquireSRWLockShared"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">AcquireSRWLockShared</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L385" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AcquireSRWLockShared</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SRWLOCK">SRWLOCK</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_BeginPaint"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">BeginPaint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1033" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">BeginPaint</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpPaint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PAINTSTRUCT">PAINTSTRUCT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HDC">HDC</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_BringWindowToTop"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">BringWindowToTop</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1040" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">BringWindowToTop</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CONSOLE_SCREEN_BUFFER_INFO"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">CONSOLE_SCREEN_BUFFER_INFO</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L189" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_COORD">COORD</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwCursorPosition</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_COORD">COORD</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wAttributes</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">srWindow</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SMALL_RECT">SMALL_RECT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwMaximumWindowSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_COORD">COORD</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_COORD"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">COORD</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L169" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">x</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">y</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CREATESTRUCTA"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">CREATESTRUCTA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L292" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpCreateParams</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hInstance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hMenu</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hwndParent</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cy</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cx</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">y</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">x</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">style</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpszName</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpszClass</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwExStyle</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CREATESTRUCTW"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">CREATESTRUCTW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L308" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpCreateParams</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hInstance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hMenu</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hwndParent</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cy</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cx</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">y</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">x</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">style</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpszName</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpszClass</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwExStyle</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CancelWaitableTimer"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CancelWaitableTimer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L394" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CancelWaitableTimer</span><span class="SyntaxCode">(hTimer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ChildWindowFromPoint"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ChildWindowFromPoint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L982" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ChildWindowFromPoint</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ChildWindowFromPointEx"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ChildWindowFromPointEx</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L983" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ChildWindowFromPointEx</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">, flags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ClientToScreen"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ClientToScreen</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1039" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ClientToScreen</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpPoint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CloseClipboard"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CloseClipboard</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1001" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CloseClipboard</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CloseHandle"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CloseHandle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L398" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CloseHandle</span><span class="SyntaxCode">(handle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CoInitializeEx"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CoInitializeEx</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\ole32.swg#L7" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CoInitializeEx</span><span class="SyntaxCode">(pvReserved: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dwCoInit: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HRESULT">HRESULT</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CoUninitialize"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CoUninitialize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\ole32.swg#L8" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CoUninitialize</span><span class="SyntaxCode">()</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateConsoleScreenBuffer"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateConsoleScreenBuffer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L360" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateConsoleScreenBuffer</span><span class="SyntaxCode">(dwDesiredAccess: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, dwShareMode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpSecurityAttributes: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpScreenBufferData: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateDirectoryA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateDirectoryA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L833" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateDirectoryA</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateDirectoryW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateDirectoryW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L840" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateDirectoryW</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateEventA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateEventA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L552" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateEventA</span><span class="SyntaxCode">(lpEventAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, bInitialState: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateEventW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateEventW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L560" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateEventW</span><span class="SyntaxCode">(lpEventAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, bInitialState: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateFileA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateFileA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L694" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFileA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, dwDesiredAccess: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, dwShareMode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwCreationDisposition: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, dwFlagsAndAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, hTemplateFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateFileW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateFileW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L702" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFileW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, dwDesiredAccess: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, dwShareMode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwCreationDisposition: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, dwFlagsAndAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, hTemplateFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateIconIndirect"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateIconIndirect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1200" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateIconIndirect</span><span class="SyntaxCode">(piconinfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ICONINFO">ICONINFO</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HICON">HICON</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateMutexA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateMutexA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L568" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateMutexA</span><span class="SyntaxCode">(lpMutexAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInitialOwner: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateMutexW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateMutexW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L576" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateMutexW</span><span class="SyntaxCode">(lpMutexAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInitialOwner: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateProcessA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateProcessA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L540" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateProcessA</span><span class="SyntaxCode">(lpApplicationName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, lpCommandLine: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, lpProcessAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, lpThreadAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInheritHandles: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, dwCreationFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpEnvironment: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode">, lpCurrentDirectory: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, lpStartupInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_STARTUPINFOA">STARTUPINFOA</a></span><span class="SyntaxCode">, lpProcessInformation: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PROCESS_INFORMATION">PROCESS_INFORMATION</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateProcessW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateProcessW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L546" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateProcessW</span><span class="SyntaxCode">(lpApplicationName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, lpCommandLine: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, lpProcessAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, lpThreadAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInheritHandles: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, dwCreationFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpEnvironment: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode">, lpCurrentDirectory: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, lpStartupInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_STARTUPINFOW">STARTUPINFOW</a></span><span class="SyntaxCode">, lpProcessInformation: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PROCESS_INFORMATION">PROCESS_INFORMATION</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateThread"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateThread</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L481" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateThread</span><span class="SyntaxCode">(lpThreadAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwStackSize: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SIZE_T">SIZE_T</a></span><span class="SyntaxCode">, lpStartAddress: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPTHREAD_START_ROUTINE">LPTHREAD_START_ROUTINE</a></span><span class="SyntaxCode">, lpParameter: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dwCreationFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpThreadId: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateTimerQueueTimer"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateTimerQueueTimer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L584" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateTimerQueueTimer</span><span class="SyntaxCode">(phNewTimer: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, timerQueue: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, callback: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WAITORTIMERCALLBACK">WAITORTIMERCALLBACK</a></span><span class="SyntaxCode">, parameter: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dueTime: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, period: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, flags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ULONG">ULONG</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateWaitableTimerA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateWaitableTimerA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L392" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWaitableTimerA</span><span class="SyntaxCode">(lpTimerAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, lpTimerName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateWaitableTimerW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateWaitableTimerW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L393" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWaitableTimerW</span><span class="SyntaxCode">(lpTimerAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, lpTimerName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateWindowExA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateWindowExA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L969" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWindowExA</span><span class="SyntaxCode">(dwExStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, dwStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, </span><span class="SyntaxConstant">X</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Y</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, hWndParent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, hMenu: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HMENU">HMENU</a></span><span class="SyntaxCode">, hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HINSTANCE">HINSTANCE</a></span><span class="SyntaxCode">, lpParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_CreateWindowExW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">CreateWindowExW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L970" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWindowExW</span><span class="SyntaxCode">(dwExStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, dwStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, </span><span class="SyntaxConstant">X</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Y</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, hWndParent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, hMenu: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HMENU">HMENU</a></span><span class="SyntaxCode">, hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HINSTANCE">HINSTANCE</a></span><span class="SyntaxCode">, lpParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DebugActiveProcess"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DebugActiveProcess</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L352" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DebugActiveProcess</span><span class="SyntaxCode">(procId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DebugBreak"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DebugBreak</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L349" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DebugBreak</span><span class="SyntaxCode">()</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DefWindowProcA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DefWindowProcA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L971" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DefWindowProcA</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, uMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WPARAM">WPARAM</a></span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPARAM">LPARAM</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LRESULT">LRESULT</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DefWindowProcW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DefWindowProcW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L972" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DefWindowProcW</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, uMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WPARAM">WPARAM</a></span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPARAM">LPARAM</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LRESULT">LRESULT</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DeleteFileA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DeleteFileA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L626" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteFileA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DeleteFileW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DeleteFileW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L632" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteFileW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DeleteTimerQueueTimer"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DeleteTimerQueueTimer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L590" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteTimerQueueTimer</span><span class="SyntaxCode">(timerQueue: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, timer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, completionEvent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DestroyIcon"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DestroyIcon</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1188" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DestroyIcon</span><span class="SyntaxCode">(hIcon: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HICON">HICON</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DestroyWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DestroyWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1134" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DestroyWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DispatchMessageA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DispatchMessageA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1043" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DispatchMessageA</span><span class="SyntaxCode">(lpMsg: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LRESULT">LRESULT</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_DispatchMessageW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">DispatchMessageW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1044" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DispatchMessageW</span><span class="SyntaxCode">(lpMsg: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LRESULT">LRESULT</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_EmptyClipboard"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">EmptyClipboard</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1002" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EmptyClipboard</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_EnableWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">EnableWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1024" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnableWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, bEnable: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_EndPaint"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">EndPaint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1034" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EndPaint</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpPaint: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PAINTSTRUCT">PAINTSTRUCT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_EnumDisplayMonitors"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">EnumDisplayMonitors</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1049" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumDisplayMonitors</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HDC">HDC</a></span><span class="SyntaxCode">, lprcClip: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">, lpfnEnum: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dwData: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPARAM">LPARAM</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_EnumWindows"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">EnumWindows</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1050" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumWindows</span><span class="SyntaxCode">(lpEnumFunc: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPARAM">LPARAM</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ExitProcess"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ExitProcess</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L380" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExitProcess</span><span class="SyntaxCode">(uExitCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ExtractIconExA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ExtractIconExA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L334" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExtractIconExA</span><span class="SyntaxCode">(lpszFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, nIconIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, phiconLarge: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HICON">HICON</a></span><span class="SyntaxCode">, phiconSmall: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HICON">HICON</a></span><span class="SyntaxCode">, nIcons: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ExtractIconExW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ExtractIconExW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L343" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExtractIconExW</span><span class="SyntaxCode">(lpszFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, nIconIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, phiconLarge: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HICON">HICON</a></span><span class="SyntaxCode">, phiconSmall: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HICON">HICON</a></span><span class="SyntaxCode">, nIcons: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FILETIME"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">FILETIME</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L231" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwLowDateTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwHighDateTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FileTimeToLocalFileTime"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FileTimeToLocalFileTime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L399" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FileTimeToLocalFileTime</span><span class="SyntaxCode">(lpFileTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpLocalFileTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FileTimeToSystemTime"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FileTimeToSystemTime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L608" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FileTimeToSystemTime</span><span class="SyntaxCode">(lpFileTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpSystemTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FillRect"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FillRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1017" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FillRect</span><span class="SyntaxCode">(hDC: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HDC">HDC</a></span><span class="SyntaxCode">, lprc: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">, hbr: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HBRUSH">HBRUSH</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindClose"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindClose</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L742" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindClose</span><span class="SyntaxCode">(hFindFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindFirstFileA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindFirstFileA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L710" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstFileA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAA">WIN32_FIND_DATAA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindFirstFileW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindFirstFileW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L718" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstFileW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAW">WIN32_FIND_DATAW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindFirstVolumeA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindFirstVolumeA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L748" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstVolumeA</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPSTR">LPSTR</a></span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindFirstVolumeW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindFirstVolumeW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L756" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstVolumeW</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPWSTR">LPWSTR</a></span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindNextFileA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindNextFileA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L726" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextFileA</span><span class="SyntaxCode">(hFindFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAA">WIN32_FIND_DATAA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindNextFileW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindNextFileW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L734" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextFileW</span><span class="SyntaxCode">(hFindFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAW">WIN32_FIND_DATAW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindNextVolumeA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindNextVolumeA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L764" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextVolumeA</span><span class="SyntaxCode">(hFindVolume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPSTR">LPSTR</a></span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindNextVolumeW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindNextVolumeW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L772" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextVolumeW</span><span class="SyntaxCode">(hFindVolume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPWSTR">LPWSTR</a></span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindVolumeClose"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindVolumeClose</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L780" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindVolumeClose</span><span class="SyntaxCode">(hFindVolume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindWindowA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindWindowA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L984" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindWindowA</span><span class="SyntaxCode">(lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FindWindowW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FindWindowW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L985" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindWindowW</span><span class="SyntaxCode">(lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_FormatMessageA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">FormatMessageA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L347" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FormatMessageA</span><span class="SyntaxCode">(dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpSource: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCVOID">LPCVOID</a></span><span class="SyntaxCode">, dwMessageId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, dwLanguageId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, nSize: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Arguments</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.<a href="#Win32_va_list">va_list</a>)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GET_FILEEX_INFO_LEVELS"><span class="titletype">enum</span> <span class="titlelight">Win32.</span><span class="titlestrong">GET_FILEEX_INFO_LEVELS</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L281" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GetFileExInfoStandard</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">GetFileExMaxInfoLevel</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GET_WHEEL_DELTA_WPARAM"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GET_WHEEL_DELTA_WPARAM</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L418" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GET_WHEEL_DELTA_WPARAM</span><span class="SyntaxCode">(wParam: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GET_X_LPARAM"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GET_X_LPARAM</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L957" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GET_X_LPARAM</span><span class="SyntaxCode">(lp: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPARAM">LPARAM</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GET_Y_LPARAM"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GET_Y_LPARAM</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L958" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GET_Y_LPARAM</span><span class="SyntaxCode">(lp: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPARAM">LPARAM</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GUID"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">GUID</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L230" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Data1</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Data2</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Data3</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Data4</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">8</span><span class="SyntaxCode">] </span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetActiveWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetActiveWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L988" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetActiveWindow</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetAdaptersInfo"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetAdaptersInfo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\iphlpapi.swg#L54" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetAdaptersInfo</span><span class="SyntaxCode">(adapterInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADAPTER_INFO">IP_ADAPTER_INFO</a></span><span class="SyntaxCode">, sizePointer: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ULONG">ULONG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ULONG">ULONG</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetAsyncKeyState"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetAsyncKeyState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L962" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetAsyncKeyState</span><span class="SyntaxCode">(vKey: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHORT">SHORT</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetClientRect"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetClientRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1018" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetClientRect</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpRect: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetClipboardData"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetClipboardData</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1004" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetClipboardData</span><span class="SyntaxCode">(uFormat: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetConsoleScreenBufferInfo"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetConsoleScreenBufferInfo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L475" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetConsoleScreenBufferInfo</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpConsoleScreenBufferInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CONSOLE_SCREEN_BUFFER_INFO">CONSOLE_SCREEN_BUFFER_INFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetCurrentDirectoryA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetCurrentDirectoryA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L666" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentDirectoryA</span><span class="SyntaxCode">(nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetCurrentDirectoryW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetCurrentDirectoryW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L674" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentDirectoryW</span><span class="SyntaxCode">(nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetCurrentProcess"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetCurrentProcess</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L355" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentProcess</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetCurrentProcessId"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetCurrentProcessId</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L356" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentProcessId</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetCurrentThread"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetCurrentThread</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L382" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentThread</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetCurrentThreadId"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetCurrentThreadId</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L381" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentThreadId</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetCursorPos"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetCursorPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1015" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCursorPos</span><span class="SyntaxCode">(lpPoint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetDC"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetDC</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L994" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetDC</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HDC">HDC</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetDesktopWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetDesktopWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L986" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetDesktopWindow</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetFileAttributesA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetFileAttributesA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L638" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetFileAttributesExA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetFileAttributesExA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L654" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesExA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, fInfoLevelId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></span><span class="SyntaxCode">, lpFileInformation: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetFileAttributesExW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetFileAttributesExW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L660" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesExW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, fInfoLevelId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></span><span class="SyntaxCode">, lpFileInformation: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetFileAttributesW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetFileAttributesW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L646" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetFileSize"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetFileSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L807" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileSize</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpFileSizeHigh: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPDWORD">LPDWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetForegroundWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetForegroundWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L989" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetForegroundWindow</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetFullPathNameA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetFullPathNameA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L816" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFullPathNameA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPSTR">LPSTR</a></span><span class="SyntaxCode">, lpFilePart: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPSTR">LPSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetFullPathNameW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetFullPathNameW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L825" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFullPathNameW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCTSTR">LPCTSTR</a></span><span class="SyntaxCode">, nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPWSTR">LPWSTR</a></span><span class="SyntaxCode">, lpFilePart: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPWSTR">LPWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetIconInfo"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetIconInfo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1194" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetIconInfo</span><span class="SyntaxCode">(hIcon: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HICON">HICON</a></span><span class="SyntaxCode">, piconinfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ICONINFO">ICONINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetKeyboardState"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetKeyboardState</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L963" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetKeyboardState</span><span class="SyntaxCode">(lpKeystate: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetLastError"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetLastError</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L346" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetLastError</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetLocalTime"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetLocalTime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L375" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetLocalTime</span><span class="SyntaxCode">(lpSystemTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetMessageA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetMessageA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L973" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMessageA</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetMessageW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetMessageW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L974" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMessageW</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetModuleHandleA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetModuleHandleA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L357" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetModuleHandleA</span><span class="SyntaxCode">(lpModuleName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HMODULE">HMODULE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetModuleHandleW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetModuleHandleW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L358" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetModuleHandleW</span><span class="SyntaxCode">(lpModuleName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HMODULE">HMODULE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetMonitorInfoA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetMonitorInfoA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1158" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMonitorInfoA</span><span class="SyntaxCode">(hMonitor: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HMONITOR">HMONITOR</a></span><span class="SyntaxCode">, lpmi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MONITORINFO">MONITORINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetMonitorInfoW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetMonitorInfoW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1165" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMonitorInfoW</span><span class="SyntaxCode">(hMonitor: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HMONITOR">HMONITOR</a></span><span class="SyntaxCode">, lpmi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MONITORINFO">MONITORINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetNextWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetNextWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L991" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetNextWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, wCmd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetProcAddress"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetProcAddress</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L354" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetProcAddress</span><span class="SyntaxCode">(hModule: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HMODULE">HMODULE</a></span><span class="SyntaxCode">, lpProcName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetStdHandle"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetStdHandle</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L359" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetStdHandle</span><span class="SyntaxCode">(nStdHandle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetSystemInfo"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetSystemInfo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L366" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetSystemInfo</span><span class="SyntaxCode">(lpSystemInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEM_INFO">SYSTEM_INFO</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetSystemMetrics"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetSystemMetrics</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1010" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetSystemMetrics</span><span class="SyntaxCode">(nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetSystemTime"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetSystemTime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L376" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetSystemTime</span><span class="SyntaxCode">(lpSystemTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetTopWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetTopWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L987" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetTopWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetVolumePathNamesForVolumeNameA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetVolumePathNamesForVolumeNameA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L786" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetVolumePathNamesForVolumeNameA</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, lpszVolumePathNames: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCH">LPCH</a></span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpcchReturnLength: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetVolumePathNamesForVolumeNameW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetVolumePathNamesForVolumeNameW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L792" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetVolumePathNamesForVolumeNameW</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, lpszVolumePathNames: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPWCH">LPWCH</a></span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpcchReturnLength: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L990" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, uCmd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetWindowLongPtrA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetWindowLongPtrA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L979" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowLongPtrA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LONG">LONG</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetWindowLongPtrW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetWindowLongPtrW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L980" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowLongPtrW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LONG">LONG</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetWindowRect"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetWindowRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1019" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowRect</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpRect: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetWindowTextA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetWindowTextA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1020" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowTextA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPSTR">LPSTR</a></span><span class="SyntaxCode">, nMaxCount: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GetWindowTextW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GetWindowTextW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1021" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowTextW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPWSTR">LPWSTR</a></span><span class="SyntaxCode">, nMaxCount: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GlobalAlloc"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GlobalAlloc</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L367" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalAlloc</span><span class="SyntaxCode">(uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, dwBytes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SIZE_T">SIZE_T</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HGLOBAL">HGLOBAL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GlobalLock"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GlobalLock</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L369" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalLock</span><span class="SyntaxCode">(hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HGLOBAL">HGLOBAL</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GlobalSize"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GlobalSize</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L371" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalSize</span><span class="SyntaxCode">(hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HGLOBAL">HGLOBAL</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SIZE_T">SIZE_T</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_GlobalUnlock"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">GlobalUnlock</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L370" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalUnlock</span><span class="SyntaxCode">(hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HGLOBAL">HGLOBAL</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_HIWORD"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">HIWORD</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L149" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">HIWORD</span><span class="SyntaxCode">(l: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ICONINFO"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">ICONINFO</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L948" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">fIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">xHotspot</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">yHotspot</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hbmMask</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hbmColor</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_IP_ADAPTER_INFO"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">IP_ADAPTER_INFO</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\iphlpapi.swg#L31" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">next</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADAPTER_INFO">IP_ADAPTER_INFO</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">comboIndex</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">adapterName</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">260</span><span class="SyntaxCode">] </span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">description</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">132</span><span class="SyntaxCode">] </span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">addressLength</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">address</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">8</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BYTE">BYTE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">index</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">type</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dhcpEnabled</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">currentIpAddress</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ipAddressList</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">gatewayList</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dhcpServer</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">haveWins</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">primaryWinsServer</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">secondaryWinsServer</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">leaseObtained</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">leaseExpires</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_IP_ADDRESS_STRING"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">IP_ADDRESS_STRING</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\iphlpapi.swg#L15" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">str</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">16</span><span class="SyntaxCode">] </span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_IP_ADDR_STRING"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">IP_ADDR_STRING</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\iphlpapi.swg#L23" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">next</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ipAddress</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDRESS_STRING">IP_ADDRESS_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ipMask</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADDRESS_STRING">IP_ADDRESS_STRING</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">context</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_InitializeSRWLock"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">InitializeSRWLock</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L386" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">InitializeSRWLock</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SRWLOCK">SRWLOCK</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_InvalidateRect"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">InvalidateRect</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1036" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">InvalidateRect</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpRect: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">, bErase: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_IsClipboardFormatAvailable"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">IsClipboardFormatAvailable</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1003" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IsClipboardFormatAvailable</span><span class="SyntaxCode">(format: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_IsDebuggerPresent"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">IsDebuggerPresent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L348" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IsDebuggerPresent</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_IsWindowVisible"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">IsWindowVisible</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L992" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IsWindowVisible</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_LOWORD"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">LOWORD</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L148" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LOWORD</span><span class="SyntaxCode">(l: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_LoadCursorA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">LoadCursorA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L966" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LoadCursorA</span><span class="SyntaxCode">(hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HINSTANCE">HINSTANCE</a></span><span class="SyntaxCode">, lpCursorName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HCURSOR">HCURSOR</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_LoadCursorW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">LoadCursorW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L967" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LoadCursorW</span><span class="SyntaxCode">(hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HINSTANCE">HINSTANCE</a></span><span class="SyntaxCode">, lpCursorName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HCURSOR">HCURSOR</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_LocalFree"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">LocalFree</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L368" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LocalFree</span><span class="SyntaxCode">(hLocal: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HLOCAL">HLOCAL</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HLOCAL">HLOCAL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MAKEINTRESOURCEA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MAKEINTRESOURCEA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L414" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MAKEINTRESOURCEA</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MAKEINTRESOURCEW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MAKEINTRESOURCEW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L415" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MAKEINTRESOURCEW</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MAKELANGID"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MAKELANGID</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L135" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MAKELANGID</span><span class="SyntaxCode">(p: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WORD">WORD</a></span><span class="SyntaxCode">, s: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WORD">WORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">u16</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MINMAXINFO"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">MINMAXINFO</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L335" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">ptReserved</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ptMaxSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ptMaxPosition</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ptMinTrackSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ptMaxTrackSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MONITORINFO"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">MONITORINFO</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L821" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">rcMonitor</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">rcWork</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFlags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MSG"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">MSG</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L439" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">hwnd</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">message</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wParam</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lParam</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">time</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">pt</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lPrivate</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MapVirtualKeyA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MapVirtualKeyA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L996" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MapVirtualKeyA</span><span class="SyntaxCode">(uCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, uMapType: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MapVirtualKeyW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MapVirtualKeyW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L997" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MapVirtualKeyW</span><span class="SyntaxCode">(uCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, uMapType: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MessageBoxA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MessageBoxA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1025" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MessageBoxA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpText: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, lpCaption: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, uType: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MessageBoxW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MessageBoxW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1026" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MessageBoxW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpText: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, lpCaption: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, uType: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MonitorFromPoint"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MonitorFromPoint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1007" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MonitorFromPoint</span><span class="SyntaxCode">(pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HMONITOR">HMONITOR</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MonitorFromWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MonitorFromWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1008" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MonitorFromWindow</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HMONITOR">HMONITOR</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_MoveWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">MoveWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1140" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MoveWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, bRepaint: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_NOTIFYICONDATAA"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">NOTIFYICONDATAA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L253" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hWnd</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">uID</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">uFlags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">uCallbackMessage</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szTip</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">128</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwState</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwStateMask</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szInfo</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">256</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DUMMYUNIONNAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">{uTimeout: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, uVersion: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">}</td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szInfoTitle</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">64</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwInfoFlags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">guidItem</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_GUID">GUID</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hBalloonIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_NOTIFYICONDATAW"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">NOTIFYICONDATAW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L276" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hWnd</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">uID</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">uFlags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">uCallbackMessage</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szTip</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">128</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwState</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwStateMask</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szInfo</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">256</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">using</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DUMMYUNIONNAME</span><span class="SyntaxCode"></td>
<td class="codetype"></span><span class="SyntaxCode">{uTimeout: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, uVersion: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">}</td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szInfoTitle</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">64</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwInfoFlags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">guidItem</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_GUID">GUID</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hBalloonIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_OpenClipboard"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">OpenClipboard</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1000" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OpenClipboard</span><span class="SyntaxCode">(hWndNewOwner: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_OutputDebugStringA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">OutputDebugStringA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L350" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OutputDebugStringA</span><span class="SyntaxCode">(lpOutputString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_OutputDebugStringW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">OutputDebugStringW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L351" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OutputDebugStringW</span><span class="SyntaxCode">(lpOutputString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_PAINTSTRUCT"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">PAINTSTRUCT</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L807" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">hdc</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">fErase</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">rcPaint</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">fRestore</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">fIncUpdate</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">rgbReserved</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">32</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BYTE">BYTE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_POINT"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">POINT</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L175" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">x</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">y</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_PROCESS_INFORMATION"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">PROCESS_INFORMATION</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L331" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">hProcess</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hThread</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwProcessId</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwThreadId</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_PeekMessageA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">PeekMessageA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L975" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PeekMessageA</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wRemoveMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_PeekMessageW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">PeekMessageW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L976" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PeekMessageW</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wRemoveMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_PostMessageA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">PostMessageA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1098" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PostMessageA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, msg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WPARAM">WPARAM</a></span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPARAM">LPARAM</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_PostMessageW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">PostMessageW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1104" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PostMessageW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, msg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WPARAM">WPARAM</a></span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPARAM">LPARAM</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_PostQuitMessage"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">PostQuitMessage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L977" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PostQuitMessage</span><span class="SyntaxCode">(nExitCode: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_QueryPerformanceCounter"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">QueryPerformanceCounter</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L373" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">QueryPerformanceCounter</span><span class="SyntaxCode">(lpPerformanceCount: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LARGE_INTEGER">LARGE_INTEGER</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_QueryPerformanceFrequency"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">QueryPerformanceFrequency</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L374" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">QueryPerformanceFrequency</span><span class="SyntaxCode">(lpFrequency: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LARGE_INTEGER">LARGE_INTEGER</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_RECT"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">RECT</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L799" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">left</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">top</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">right</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">bottom</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ReadFile"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ReadFile</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L397" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReadFile</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode">, nNumberOfBytesToRead: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpNumberOfBytesRead: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPDWORD">LPDWORD</a></span><span class="SyntaxCode">, lpOverlapped: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPOVERLAPPED">LPOVERLAPPED</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_RegisterClassA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">RegisterClassA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1111" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClassA</span><span class="SyntaxCode">(lpWndClass: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WNDCLASSA">WNDCLASSA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ATOM">ATOM</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_RegisterClassW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">RegisterClassW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1120" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClassW</span><span class="SyntaxCode">(lpWndClass: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WNDCLASSW">WNDCLASSW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ATOM">ATOM</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_RegisterClipboardFormatA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">RegisterClipboardFormatA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1172" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClipboardFormatA</span><span class="SyntaxCode">(name: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_RegisterClipboardFormatW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">RegisterClipboardFormatW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1180" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClipboardFormatW</span><span class="SyntaxCode">(name: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_RegisterHotKey"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">RegisterHotKey</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1086" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterHotKey</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, id: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, fsModifiers: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, vk: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ReleaseCapture"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ReleaseCapture</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1028" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseCapture</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ReleaseDC"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ReleaseDC</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1035" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseDC</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, hDC: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HDC">HDC</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ReleaseMutex"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ReleaseMutex</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L522" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseMutex</span><span class="SyntaxCode">(hMutex: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ReleaseSRWLockExclusive"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ReleaseSRWLockExclusive</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L387" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseSRWLockExclusive</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SRWLOCK">SRWLOCK</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ReleaseSRWLockShared"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ReleaseSRWLockShared</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L388" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseSRWLockShared</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SRWLOCK">SRWLOCK</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ResetEvent"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ResetEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L528" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ResetEvent</span><span class="SyntaxCode">(hEvent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ResumeThread"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ResumeThread</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L490" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ResumeThread</span><span class="SyntaxCode">(hThread: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SECURITY_ATTRIBUTES"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">SECURITY_ATTRIBUTES</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L224" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">nLength</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpSecurityDescriptor</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">bInheritHandle</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SHFILEINFOA"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">SHFILEINFOA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L203" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">hIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">iIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwAttributes</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szDisplayName</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">260</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szTypeName</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">80</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SHFILEINFOW"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">SHFILEINFOW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L212" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">hIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">iIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwAttributes</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szDisplayName</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">260</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szTypeName</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">80</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SHGetFileInfoA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SHGetFileInfoA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L315" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetFileInfoA</span><span class="SyntaxCode">(pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, dwFileAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, psfi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHFILEINFOA">SHFILEINFOA</a></span><span class="SyntaxCode">, cbFileInfo: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD_PTR">DWORD_PTR</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SHGetFileInfoW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SHGetFileInfoW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L316" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetFileInfoW</span><span class="SyntaxCode">(pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">, dwFileAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, psfi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHFILEINFOW">SHFILEINFOW</a></span><span class="SyntaxCode">, cbFileInfo: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD_PTR">DWORD_PTR</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SHGetSpecialFolderPathA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SHGetSpecialFolderPathA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L321" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetSpecialFolderPathA</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPSTR">LPSTR</a></span><span class="SyntaxCode">, csidl: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, fCreate: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SHGetSpecialFolderPathW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SHGetSpecialFolderPathW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L327" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetSpecialFolderPathW</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPWSTR">LPWSTR</a></span><span class="SyntaxCode">, csidl: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, fCreate: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SHGetStockIconInfo"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SHGetStockIconInfo</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L351" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetStockIconInfo</span><span class="SyntaxCode">(siid: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHSTOCKICONID">SHSTOCKICONID</a></span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, psii: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHSTOCKICONINFO">SHSTOCKICONINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SHSTOCKICONID"><span class="titletype">enum</span> <span class="titlelight">Win32.</span><span class="titlestrong">SHSTOCKICONID</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L102" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DOCNOASSOC</span><span class="SyntaxCode"></td>
<td>document (blank page) no associated program. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DOCASSOC</span><span class="SyntaxCode"></td>
<td>document with an associated program. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_APPLICATION</span><span class="SyntaxCode"></td>
<td>generic application with no custom icon. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_FOLDER</span><span class="SyntaxCode"></td>
<td>folder (closed). </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_FOLDEROPEN</span><span class="SyntaxCode"></td>
<td>folder (open). </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVE525</span><span class="SyntaxCode"></td>
<td>5.25" floppy disk drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVE35</span><span class="SyntaxCode"></td>
<td>3.5" floppy disk drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVEREMOVE</span><span class="SyntaxCode"></td>
<td>removable drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVEFIXED</span><span class="SyntaxCode"></td>
<td>fixed (hard disk) drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVENET</span><span class="SyntaxCode"></td>
<td>network drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVENETDISABLED</span><span class="SyntaxCode"></td>
<td>disconnected network drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVECD</span><span class="SyntaxCode"></td>
<td>CD drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVERAM</span><span class="SyntaxCode"></td>
<td>RAM disk drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_WORLD</span><span class="SyntaxCode"></td>
<td>entire network. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_SERVER</span><span class="SyntaxCode"></td>
<td>a computer on the network. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_PRINTER</span><span class="SyntaxCode"></td>
<td>printer. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MYNETWORK</span><span class="SyntaxCode"></td>
<td>My network places. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_FIND</span><span class="SyntaxCode"></td>
<td>Find. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_HELP</span><span class="SyntaxCode"></td>
<td>Help. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_SHARE</span><span class="SyntaxCode"></td>
<td>overlay for shared items. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_LINK</span><span class="SyntaxCode"></td>
<td>overlay for shortcuts to items. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_SLOWFILE</span><span class="SyntaxCode"></td>
<td>overlay for slow items. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_RECYCLER</span><span class="SyntaxCode"></td>
<td>empty recycle bin. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_RECYCLERFULL</span><span class="SyntaxCode"></td>
<td>full recycle bin. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIACDAUDIO</span><span class="SyntaxCode"></td>
<td>Audio CD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_LOCK</span><span class="SyntaxCode"></td>
<td>Security lock. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_AUTOLIST</span><span class="SyntaxCode"></td>
<td>AutoList. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_PRINTERNET</span><span class="SyntaxCode"></td>
<td>Network printer. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_SERVERSHARE</span><span class="SyntaxCode"></td>
<td>Server share. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_PRINTERFAX</span><span class="SyntaxCode"></td>
<td>Fax printer. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_PRINTERFAXNET</span><span class="SyntaxCode"></td>
<td>Networked Fax Printer. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_PRINTERFILE</span><span class="SyntaxCode"></td>
<td>Print to File. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_STACK</span><span class="SyntaxCode"></td>
<td>Stack. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIASVCD</span><span class="SyntaxCode"></td>
<td>SVCD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_STUFFEDFOLDER</span><span class="SyntaxCode"></td>
<td>Folder containing other items. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVEUNKNOWN</span><span class="SyntaxCode"></td>
<td>Unknown drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVEDVD</span><span class="SyntaxCode"></td>
<td>DVD Drive. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIADVD</span><span class="SyntaxCode"></td>
<td>DVD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIADVDRAM</span><span class="SyntaxCode"></td>
<td>DVD-RAM Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIADVDRW</span><span class="SyntaxCode"></td>
<td>DVD-RW Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIADVDR</span><span class="SyntaxCode"></td>
<td>DVD-R Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIADVDROM</span><span class="SyntaxCode"></td>
<td>DVD-ROM Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIACDAUDIOPLUS</span><span class="SyntaxCode"></td>
<td>CD+ (Enhanced CD) Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIACDRW</span><span class="SyntaxCode"></td>
<td>CD-RW Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIACDR</span><span class="SyntaxCode"></td>
<td>CD-R Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIACDBURN</span><span class="SyntaxCode"></td>
<td>Burning CD. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIABLANKCD</span><span class="SyntaxCode"></td>
<td>Blank CD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIACDROM</span><span class="SyntaxCode"></td>
<td>CD-ROM Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_AUDIOFILES</span><span class="SyntaxCode"></td>
<td>Audio files. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_IMAGEFILES</span><span class="SyntaxCode"></td>
<td>Image files. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_VIDEOFILES</span><span class="SyntaxCode"></td>
<td>Video files. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MIXEDFILES</span><span class="SyntaxCode"></td>
<td>Mixed files. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_FOLDERBACK</span><span class="SyntaxCode"></td>
<td>Folder back. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_FOLDERFRONT</span><span class="SyntaxCode"></td>
<td>Folder front. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_SHIELD</span><span class="SyntaxCode"></td>
<td>Security shield. Use for UAC prompts only. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_WARNING</span><span class="SyntaxCode"></td>
<td>Warning. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_INFO</span><span class="SyntaxCode"></td>
<td>Informational. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_ERROR</span><span class="SyntaxCode"></td>
<td>Error. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_KEY</span><span class="SyntaxCode"></td>
<td>Key / Secure. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_SOFTWARE</span><span class="SyntaxCode"></td>
<td>Software. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_RENAME</span><span class="SyntaxCode"></td>
<td>Rename. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DELETE</span><span class="SyntaxCode"></td>
<td>Delete. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIAAUDIODVD</span><span class="SyntaxCode"></td>
<td>Audio DVD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIAMOVIEDVD</span><span class="SyntaxCode"></td>
<td>Movie DVD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIAENHANCEDCD</span><span class="SyntaxCode"></td>
<td>Enhanced CD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIAENHANCEDDVD</span><span class="SyntaxCode"></td>
<td>Enhanced DVD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIAHDDVD</span><span class="SyntaxCode"></td>
<td>HD-DVD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIABLURAY</span><span class="SyntaxCode"></td>
<td>BluRay Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIAVCD</span><span class="SyntaxCode"></td>
<td>VCD Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIADVDPLUSR</span><span class="SyntaxCode"></td>
<td>DVD+R Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIADVDPLUSRW</span><span class="SyntaxCode"></td>
<td>DVD+RW Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DESKTOPPC</span><span class="SyntaxCode"></td>
<td>desktop computer. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MOBILEPC</span><span class="SyntaxCode"></td>
<td>mobile computer (laptop/notebook). </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_USERS</span><span class="SyntaxCode"></td>
<td>users. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIASMARTMEDIA</span><span class="SyntaxCode"></td>
<td>Smart Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIACOMPACTFLASH</span><span class="SyntaxCode"></td>
<td>Compact Flash. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DEVICECELLPHONE</span><span class="SyntaxCode"></td>
<td>Cell phone. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DEVICECAMERA</span><span class="SyntaxCode"></td>
<td>Camera. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DEVICEVIDEOCAMERA</span><span class="SyntaxCode"></td>
<td>Video camera. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DEVICEAUDIOPLAYER</span><span class="SyntaxCode"></td>
<td>Audio player. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_NETWORKCONNECT</span><span class="SyntaxCode"></td>
<td>Connect to network. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_INTERNET</span><span class="SyntaxCode"></td>
<td>Internet. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_ZIPFILE</span><span class="SyntaxCode"></td>
<td>ZIP file. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_SETTINGS</span><span class="SyntaxCode"></td>
<td>Settings  107-131 are internal Vista RTM icons  132-159 for SP1 icons. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVEHDDVD</span><span class="SyntaxCode"></td>
<td>HDDVD Drive (all types). </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_DRIVEBD</span><span class="SyntaxCode"></td>
<td>BluRay Drive (all types). </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIAHDDVDROM</span><span class="SyntaxCode"></td>
<td>HDDVD-ROM Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIAHDDVDR</span><span class="SyntaxCode"></td>
<td>HDDVD-R Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIAHDDVDRAM</span><span class="SyntaxCode"></td>
<td>HDDVD-RAM Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIABDROM</span><span class="SyntaxCode"></td>
<td>BluRay ROM Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIABDR</span><span class="SyntaxCode"></td>
<td>BluRay R Media. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MEDIABDRE</span><span class="SyntaxCode"></td>
<td>BluRay RE Media (Rewriable and RAM). </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_CLUSTEREDDRIVE</span><span class="SyntaxCode"></td>
<td>Clustered disk  160+ are for Windows 7 icons. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">SIID_MAX_ICONS</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SHSTOCKICONINFO"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">SHSTOCKICONINFO</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L221" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">iSysImageIndex</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">iIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">szPath</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">260</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SMALL_RECT"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">SMALL_RECT</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L181" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">left</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">top</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">right</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">bottom</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_STARTUPINFOA"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">STARTUPINFOA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L287" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">cb</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpReserved</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpDesktop</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpTitle</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwX</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwY</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwXSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwYSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwXCountChars</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwYCountChars</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFillAttribute</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFlags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wShowWindow</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbReserved2</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpReserved2</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BYTE">BYTE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hStdInput</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hStdOutput</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hStdError</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_STARTUPINFOW"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">STARTUPINFOW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L309" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">cb</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpReserved</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpDesktop</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpTitle</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwX</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwY</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwXSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwYSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwXCountChars</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwYCountChars</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFillAttribute</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFlags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wShowWindow</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbReserved2</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpReserved2</td>
<td class="codetype"></span><span class="SyntaxCode">*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BYTE">BYTE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hStdInput</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hStdOutput</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hStdError</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SYSTEMTIME"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">SYSTEMTIME</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L212" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">wYear</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wMonth</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wDayOfWeek</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wDay</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wHour</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wMinute</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wSecond</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wMilliseconds</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SYSTEM_INFO"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">SYSTEM_INFO</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L198" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwOemId</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td>Must be updated to an union. </td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwPageSize</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpMinimumApplicationAddress</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpMaximumApplicationAddress</td>
<td class="codetype"></span><span class="SyntaxCode">^</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwActiveProcessorMask</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u64</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwNumberOfProcessors</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwProcessorType</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwAllocationGranularity</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wProcessorLevel</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wProcessorRevision</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ScreenToClient"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ScreenToClient</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1038" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ScreenToClient</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, lpPoint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetActiveWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetActiveWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1031" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetActiveWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetCapture"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetCapture</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1027" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCapture</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetClipboardData"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetClipboardData</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1005" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetClipboardData</span><span class="SyntaxCode">(uFormat: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetConsoleActiveScreenBuffer"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetConsoleActiveScreenBuffer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L361" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetConsoleActiveScreenBuffer</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetConsoleOutputCP"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetConsoleOutputCP</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L364" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetConsoleOutputCP</span><span class="SyntaxCode">(codePage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetConsoleTextAttribute"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetConsoleTextAttribute</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L469" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetConsoleTextAttribute</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, wAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WORD">WORD</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetCurrentDirectoryA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetCurrentDirectoryA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L682" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCurrentDirectoryA</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetCurrentDirectoryW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetCurrentDirectoryW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L688" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCurrentDirectoryW</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCWSTR">LPCWSTR</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetCursor"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetCursor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1029" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCursor</span><span class="SyntaxCode">(hCursor: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HCURSOR">HCURSOR</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HCURSOR">HCURSOR</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetCursorPos"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetCursorPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L964" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCursorPos</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetEvent"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetEvent</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L534" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetEvent</span><span class="SyntaxCode">(hEvent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetFilePointer"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetFilePointer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L799" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetFilePointer</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lDistanceToMove: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LONG">LONG</a></span><span class="SyntaxCode">, lpDistanceToMoveHigh: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PLONG">PLONG</a></span><span class="SyntaxCode">, dwMoveMethod: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetFileTime"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetFileTime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L620" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetFileTime</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpCreationTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpLastAccessTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpLastWriteTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetFocus"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetFocus</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1030" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetFocus</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetLayeredWindowAttributes"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetLayeredWindowAttributes</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1152" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetLayeredWindowAttributes</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, crKey: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_COLORREF">COLORREF</a></span><span class="SyntaxCode">, bAlpha: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BYTE">BYTE</a></span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetThreadPriority"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetThreadPriority</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L498" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetThreadPriority</span><span class="SyntaxCode">(hThread: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, nPriority: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetWaitableTimer"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetWaitableTimer</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L395" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWaitableTimer</span><span class="SyntaxCode">(hTimer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpDueTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LARGE_INTEGER">LARGE_INTEGER</a></span><span class="SyntaxCode">, lPeriod: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LONG">LONG</a></span><span class="SyntaxCode">, pfnCompletionRoutine: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, lpArgToCompletionRoutine: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPVOID">LPVOID</a></span><span class="SyntaxCode">, fResume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetWindowLongPtrA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetWindowLongPtrA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1046" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWindowLongPtrA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, dwNewLong: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LONG">LONG</a></span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LONG">LONG</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetWindowLongPtrW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetWindowLongPtrW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1047" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWindowLongPtrW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, dwNewLong: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LONG">LONG</a></span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LONG">LONG</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SetWindowPos"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SetWindowPos</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1146" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWindowPos</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, hWndInsertAfter: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cx: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cy: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_Shell_NotifyIconA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">Shell_NotifyIconA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L357" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Shell_NotifyIconA</span><span class="SyntaxCode">(dwMessage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_NOTIFYICONDATAA">NOTIFYICONDATAA</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_Shell_NotifyIconW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">Shell_NotifyIconW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\shell32.swg#L363" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Shell_NotifyIconW</span><span class="SyntaxCode">(dwMessage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_NOTIFYICONDATAW">NOTIFYICONDATAW</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ShowCursor"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ShowCursor</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L965" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ShowCursor</span><span class="SyntaxCode">(show: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ShowWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ShowWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1023" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ShowWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, nCmdShow: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_Sleep"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">Sleep</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L378" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Sleep</span><span class="SyntaxCode">(dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SleepEx"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SleepEx</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L379" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SleepEx</span><span class="SyntaxCode">(dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, bAltertable: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">)</code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SwitchToThread"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SwitchToThread</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L341" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SwitchToThread</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_SystemTimeToFileTime"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">SystemTimeToFileTime</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L614" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SystemTimeToFileTime</span><span class="SyntaxCode">(lpSystemTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span><span class="SyntaxCode">, lpFileTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_ToUnicode"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">ToUnicode</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L998" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ToUnicode</span><span class="SyntaxCode">(wVirtKey: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, wScanCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">, lpKeyState: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BYTE">BYTE</a></span><span class="SyntaxCode">, pwszBuff: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPWSTR">LPWSTR</a></span><span class="SyntaxCode">, cchBuff: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, wFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_UINT">UINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_TranslateMessage"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">TranslateMessage</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1042" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TranslateMessage</span><span class="SyntaxCode">(lpMsg: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_TryAcquireSRWLockExclusive"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">TryAcquireSRWLockExclusive</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L389" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TryAcquireSRWLockExclusive</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SRWLOCK">SRWLOCK</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_TryAcquireSRWLockShared"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">TryAcquireSRWLockShared</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L390" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TryAcquireSRWLockShared</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SRWLOCK">SRWLOCK</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_UnregisterHotKey"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">UnregisterHotKey</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1092" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">UnregisterHotKey</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">, id: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_UpdateWindow"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">UpdateWindow</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L1128" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">UpdateWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WIN32_FILE_ATTRIBUTE_DATA"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">WIN32_FILE_ATTRIBUTE_DATA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L237" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFileAttributes</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ftCreationTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ftLastAccessTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ftLastWriteTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">nFileSizeHigh</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">nFileSizeLow</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WIN32_FIND_DATAA"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">WIN32_FIND_DATAA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L247" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFileAttributes</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ftCreationTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ftLastAccessTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ftLastWriteTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">nFileSizeHigh</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">nFileSizeLow</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwReserved0</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwReserved1</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cFileName</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">260</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cAlternateFileName</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">14</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CHAR">CHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFileType</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwCreatorType</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wFinderFlags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WIN32_FIND_DATAW"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">WIN32_FIND_DATAW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L264" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFileAttributes</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ftCreationTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ftLastAccessTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">ftLastWriteTime</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">nFileSizeHigh</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">nFileSizeLow</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwReserved0</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwReserved1</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cFileName</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">260</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cAlternateFileName</td>
<td class="codetype"></span><span class="SyntaxCode">[</span><span class="SyntaxNumber">14</span><span class="SyntaxCode">] </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WCHAR">WCHAR</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwFileType</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">dwCreatorType</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">wFinderFlags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WINDOWPOS"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">WINDOWPOS</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L324" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">hwnd</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hwndInsertAfter</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">x</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">y</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cx</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cy</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">flags</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WNDCLASSA"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">WNDCLASSA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L264" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">style</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpfnWndProc</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbClsExtra</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbWndExtra</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hInstance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hCursor</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hbrBackground</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpszMenuName</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpszClassName</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u8</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WNDCLASSW"><span class="titletype">struct</span> <span class="titlelight">Win32.</span><span class="titlestrong">WNDCLASSW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L278" class="src">[src]</a></td>
</tr>
</table>
</p>
<table class="enumeration">
<tr>
<td class="codetype"></span><span class="SyntaxCode">style</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">u32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpfnWndProc</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbClsExtra</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">cbWndExtra</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxType">s32</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hInstance</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hIcon</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hCursor</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">hbrBackground</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpszMenuName</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
<tr>
<td class="codetype"></span><span class="SyntaxCode">lpszClassName</td>
<td class="codetype"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> ^</span><span class="SyntaxType">u16</span><span class="SyntaxCode"></td>
<td></td>
</tr>
</table>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WaitForMultipleObjects"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">WaitForMultipleObjects</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L514" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WaitForMultipleObjects</span><span class="SyntaxCode">(nCount: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpHandles: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, bWaitAll: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode">, dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WaitForSingleObject"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">WaitForSingleObject</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L505" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WaitForSingleObject</span><span class="SyntaxCode">(hHandle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WindowFromPoint"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">WindowFromPoint</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\user32.swg#L981" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WindowFromPoint</span><span class="SyntaxCode">(pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HWND">HWND</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WriteConsoleOutputCharacterA"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">WriteConsoleOutputCharacterA</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L362" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WriteConsoleOutputCharacterA</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpCharacter: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCSTR">LPCSTR</a></span><span class="SyntaxCode">, nLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, dwWriteCoord: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpNumberOfCharsWritten: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPDWORD">LPDWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WriteConsoleOutputCharacterW"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">WriteConsoleOutputCharacterW</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L363" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WriteConsoleOutputCharacterW</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpCharacter: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCTSTR">LPCTSTR</a></span><span class="SyntaxCode">, nLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, dwWriteCoord: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpNumberOfCharsWritten: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPDWORD">LPDWORD</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_WriteFile"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">WriteFile</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\kernel32.swg#L600" class="src">[src]</a></td>
</tr>
</table>
</p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WriteFile</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_HANDLE">HANDLE</a></span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPCVOID">LPCVOID</a></span><span class="SyntaxCode">, nNumberOfBytesToWrite: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_DWORD">DWORD</a></span><span class="SyntaxCode">, lpNumberOfBytesWritten: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPDWORD">LPDWORD</a></span><span class="SyntaxCode">, lpOverlapped: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_LPOVERLAPPED">LPOVERLAPPED</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_BOOL">BOOL</a></span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
<p>
<table class="item">
<tr>
<td class="item">
<span class="content" id="Win32_setError"><span class="titletype">func</span> <span class="titlelight">Win32.</span><span class="titlestrong">setError</span></span>
</td>
<td class="srcref">
<a href="https://github.com/swag-lang/swag/blob/master/bin/std/modules/win32\src\win32.swg#L4" class="src">[src]</a></td>
</tr>
</table>
</p>
<p>Fill the context error message. </p>
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setError</span><span class="SyntaxCode">(errorMessageID: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</div>
</div>
</div>
</div>
</body>
</html>
