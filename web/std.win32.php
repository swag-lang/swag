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
        }
    .SCde { color: #7f7f7f; }
    .SCmt { color: #71a35b; }
    .SCmp { color: #7f7f7f; }
    .SFct { color: #ff6a00; }
    .SCst { color: #3173cd; }
    .SItr { color: #b4b44a; }
    .STpe { color: #3bc3a7; }
    .SKwd { color: #3186cd; }
    .SLgc { color: #b040be; }
    .SNum { color: #74a35b; }
    .SStr { color: #bb6643; }
    .SAtr { color: #7f7f7f; }
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
<td id="Win32_BACKGROUND_BLUE" class="codetype"><span class="SCde"><span class="SCst">BACKGROUND_BLUE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_GREEN" class="codetype"><span class="SCde"><span class="SCst">BACKGROUND_GREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_INTENSITY" class="codetype"><span class="SCde"><span class="SCst">BACKGROUND_INTENSITY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_MASK" class="codetype"><span class="SCde"><span class="SCst">BACKGROUND_MASK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_RED" class="codetype"><span class="SCde"><span class="SCst">BACKGROUND_RED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_BITMAP" class="codetype"><span class="SCde"><span class="SCst">CF_BITMAP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_DIB" class="codetype"><span class="SCde"><span class="SCst">CF_DIB</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_DIBV5" class="codetype"><span class="SCde"><span class="SCst">CF_DIBV5</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_DIF" class="codetype"><span class="SCde"><span class="SCst">CF_DIF</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_ENHMETAFILE" class="codetype"><span class="SCde"><span class="SCst">CF_ENHMETAFILE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_HDROP" class="codetype"><span class="SCde"><span class="SCst">CF_HDROP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_LOCALE" class="codetype"><span class="SCde"><span class="SCst">CF_LOCALE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_METAFILEPICT" class="codetype"><span class="SCde"><span class="SCst">CF_METAFILEPICT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_OEMTEXT" class="codetype"><span class="SCde"><span class="SCst">CF_OEMTEXT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_PALETTE" class="codetype"><span class="SCde"><span class="SCst">CF_PALETTE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_PENDATA" class="codetype"><span class="SCde"><span class="SCst">CF_PENDATA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_RIFF" class="codetype"><span class="SCde"><span class="SCst">CF_RIFF</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_SYLK" class="codetype"><span class="SCde"><span class="SCst">CF_SYLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_TEXT" class="codetype"><span class="SCde"><span class="SCst">CF_TEXT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_TIFF" class="codetype"><span class="SCde"><span class="SCst">CF_TIFF</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_UNICODETEXT" class="codetype"><span class="SCde"><span class="SCst">CF_UNICODETEXT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_WAVE" class="codetype"><span class="SCde"><span class="SCst">CF_WAVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CONSOLE_TEXTMODE_BUFFER" class="codetype"><span class="SCde"><span class="SCst">CONSOLE_TEXTMODE_BUFFER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CREATE_ALWAYS" class="codetype"><span class="SCde"><span class="SCst">CREATE_ALWAYS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CREATE_NEW" class="codetype"><span class="SCde"><span class="SCst">CREATE_NEW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CREATE_SUSPENDED" class="codetype"><span class="SCde"><span class="SCst">CREATE_SUSPENDED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_ADMINTOOLS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_ADMINTOOLS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\Start Menu\Programs\Administrative Tools. </td>
</tr>
<tr>
<td id="Win32_CSIDL_ALTSTARTUP" class="codetype"><span class="SCde"><span class="SCst">CSIDL_ALTSTARTUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>non localized startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_APPDATA" class="codetype"><span class="SCde"><span class="SCst">CSIDL_APPDATA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\Application Data. </td>
</tr>
<tr>
<td id="Win32_CSIDL_BITBUCKET" class="codetype"><span class="SCde"><span class="SCst">CSIDL_BITBUCKET</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;desktop&gt;\Recycle Bin. </td>
</tr>
<tr>
<td id="Win32_CSIDL_CDBURN_AREA" class="codetype"><span class="SCde"><span class="SCst">CSIDL_CDBURN_AREA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>USERPROFILE\Local Settings\Application Data\Microsoft\CD Burning. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_ADMINTOOLS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_ADMINTOOLS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\Start Menu\Programs\Administrative Tools. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_ALTSTARTUP" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_ALTSTARTUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>non localized common startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_APPDATA" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_APPDATA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\Application Data. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_DESKTOPDIRECTORY" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_DESKTOPDIRECTORY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\Desktop. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_DOCUMENTS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_DOCUMENTS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\Documents. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_FAVORITES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_FAVORITES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_MUSIC" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_MUSIC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\My Music. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_OEM_LINKS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_OEM_LINKS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Links to All Users OEM specific apps. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_PICTURES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_PICTURES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\My Pictures. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_PROGRAMS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_PROGRAMS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\Start Menu\Programs. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_STARTMENU" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_STARTMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\Start Menu. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_STARTUP" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_STARTUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\Startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_TEMPLATES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_TEMPLATES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\Templates. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_VIDEO" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMMON_VIDEO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>All Users\My Video. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMPUTERSNEARME" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COMPUTERSNEARME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Computers Near Me (computered from Workgroup membership). </td>
</tr>
<tr>
<td id="Win32_CSIDL_CONNECTIONS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_CONNECTIONS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Network and Dial-up Connections. </td>
</tr>
<tr>
<td id="Win32_CSIDL_CONTROLS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_CONTROLS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>My Computer\Control Panel. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COOKIES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_COOKIES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_DESKTOP" class="codetype"><span class="SCde"><span class="SCst">CSIDL_DESKTOP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;desktop&gt;. </td>
</tr>
<tr>
<td id="Win32_CSIDL_DESKTOPDIRECTORY" class="codetype"><span class="SCde"><span class="SCst">CSIDL_DESKTOPDIRECTORY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\Desktop. </td>
</tr>
<tr>
<td id="Win32_CSIDL_DRIVES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_DRIVES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>My Computer. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FAVORITES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_FAVORITES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\Favorites. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_CREATE" class="codetype"><span class="SCde"><span class="SCst">CSIDL_FLAG_CREATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>combine with CSIDL_ value to force folder creation in SHGetFolderPath(). </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_DONT_UNEXPAND" class="codetype"><span class="SCde"><span class="SCst">CSIDL_FLAG_DONT_UNEXPAND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>combine with CSIDL_ value to avoid unexpanding environment variables. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_DONT_VERIFY" class="codetype"><span class="SCde"><span class="SCst">CSIDL_FLAG_DONT_VERIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>combine with CSIDL_ value to return an unverified folder path. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_MASK" class="codetype"><span class="SCde"><span class="SCst">CSIDL_FLAG_MASK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>mask for all possible flag values. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_NO_ALIAS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_FLAG_NO_ALIAS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>combine with CSIDL_ value to insure non-typealias versions of the pidl. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_PER_USER_INIT" class="codetype"><span class="SCde"><span class="SCst">CSIDL_FLAG_PER_USER_INIT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>combine with CSIDL_ value to indicate per-user init (eg. upgrade). </td>
</tr>
<tr>
<td id="Win32_CSIDL_FONTS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_FONTS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>windows\fonts. </td>
</tr>
<tr>
<td id="Win32_CSIDL_HISTORY" class="codetype"><span class="SCde"><span class="SCst">CSIDL_HISTORY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_INTERNET" class="codetype"><span class="SCde"><span class="SCst">CSIDL_INTERNET</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Internet Explorer (icon on desktop). </td>
</tr>
<tr>
<td id="Win32_CSIDL_INTERNET_CACHE" class="codetype"><span class="SCde"><span class="SCst">CSIDL_INTERNET_CACHE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_LOCAL_APPDATA" class="codetype"><span class="SCde"><span class="SCst">CSIDL_LOCAL_APPDATA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\Local Settings\Applicaiton Data (non roaming). </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYDOCUMENTS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_MYDOCUMENTS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Personal was just a silly name for My Documents. </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYMUSIC" class="codetype"><span class="SCde"><span class="SCst">CSIDL_MYMUSIC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>"My Music" folder. </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYPICTURES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_MYPICTURES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>C:\Program Files\My Pictures. </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYVIDEO" class="codetype"><span class="SCde"><span class="SCst">CSIDL_MYVIDEO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>"My Videos" folder. </td>
</tr>
<tr>
<td id="Win32_CSIDL_NETHOOD" class="codetype"><span class="SCde"><span class="SCst">CSIDL_NETHOOD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\nethood. </td>
</tr>
<tr>
<td id="Win32_CSIDL_NETWORK" class="codetype"><span class="SCde"><span class="SCst">CSIDL_NETWORK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Network Neighborhood (My Network Places). </td>
</tr>
<tr>
<td id="Win32_CSIDL_PERSONAL" class="codetype"><span class="SCde"><span class="SCst">CSIDL_PERSONAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>My Documents. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PRINTERS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_PRINTERS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>My Computer\Printers. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PRINTHOOD" class="codetype"><span class="SCde"><span class="SCst">CSIDL_PRINTHOOD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\PrintHood. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROFILE" class="codetype"><span class="SCde"><span class="SCst">CSIDL_PROFILE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>USERPROFILE. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAMS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_PROGRAMS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Start Menu\Programs. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_PROGRAM_FILES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>C:\Program Files. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILESX86" class="codetype"><span class="SCde"><span class="SCst">CSIDL_PROGRAM_FILESX86</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>x86 C:\Program Files on RISC. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES_COMMON" class="codetype"><span class="SCde"><span class="SCst">CSIDL_PROGRAM_FILES_COMMON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>C:\Program Files\Common. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES_COMMONX86" class="codetype"><span class="SCde"><span class="SCst">CSIDL_PROGRAM_FILES_COMMONX86</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>x86 Program Files\Common on RISC. </td>
</tr>
<tr>
<td id="Win32_CSIDL_RECENT" class="codetype"><span class="SCde"><span class="SCst">CSIDL_RECENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\Recent. </td>
</tr>
<tr>
<td id="Win32_CSIDL_RESOURCES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_RESOURCES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Resource Direcotry. </td>
</tr>
<tr>
<td id="Win32_CSIDL_RESOURCES_LOCALIZED" class="codetype"><span class="SCde"><span class="SCst">CSIDL_RESOURCES_LOCALIZED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Localized Resource Direcotry. </td>
</tr>
<tr>
<td id="Win32_CSIDL_SENDTO" class="codetype"><span class="SCde"><span class="SCst">CSIDL_SENDTO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\SendTo. </td>
</tr>
<tr>
<td id="Win32_CSIDL_STARTMENU" class="codetype"><span class="SCde"><span class="SCst">CSIDL_STARTMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>&lt;user name&gt;\Start Menu. </td>
</tr>
<tr>
<td id="Win32_CSIDL_STARTUP" class="codetype"><span class="SCde"><span class="SCst">CSIDL_STARTUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Start Menu\Programs\Startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_SYSTEM" class="codetype"><span class="SCde"><span class="SCst">CSIDL_SYSTEM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>GetSystemDirectory(). </td>
</tr>
<tr>
<td id="Win32_CSIDL_SYSTEMX86" class="codetype"><span class="SCde"><span class="SCst">CSIDL_SYSTEMX86</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>x86 system directory on RISC. </td>
</tr>
<tr>
<td id="Win32_CSIDL_TEMPLATES" class="codetype"><span class="SCde"><span class="SCst">CSIDL_TEMPLATES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_WINDOWS" class="codetype"><span class="SCde"><span class="SCst">CSIDL_WINDOWS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>GetWindowsDirectory(). </td>
</tr>
<tr>
<td id="Win32_CS_BYTEALIGNCLIENT" class="codetype"><span class="SCde"><span class="SCst">CS_BYTEALIGNCLIENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_BYTEALIGNWINDOW" class="codetype"><span class="SCde"><span class="SCst">CS_BYTEALIGNWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_CLASSDC" class="codetype"><span class="SCde"><span class="SCst">CS_CLASSDC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_DBLCLKS" class="codetype"><span class="SCde"><span class="SCst">CS_DBLCLKS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_DROPSHADOW" class="codetype"><span class="SCde"><span class="SCst">CS_DROPSHADOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_GLOBALCLASS" class="codetype"><span class="SCde"><span class="SCst">CS_GLOBALCLASS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_HREDRAW" class="codetype"><span class="SCde"><span class="SCst">CS_HREDRAW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_IME" class="codetype"><span class="SCde"><span class="SCst">CS_IME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_NOCLOSE" class="codetype"><span class="SCde"><span class="SCst">CS_NOCLOSE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_OWNDC" class="codetype"><span class="SCde"><span class="SCst">CS_OWNDC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_PARENTDC" class="codetype"><span class="SCde"><span class="SCst">CS_PARENTDC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_SAVEBITS" class="codetype"><span class="SCde"><span class="SCst">CS_SAVEBITS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_VREDRAW" class="codetype"><span class="SCde"><span class="SCst">CS_VREDRAW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_ALL" class="codetype"><span class="SCde"><span class="SCst">CWP_ALL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_SKIPDISABLED" class="codetype"><span class="SCde"><span class="SCst">CWP_SKIPDISABLED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_SKIPINVISIBLE" class="codetype"><span class="SCde"><span class="SCst">CWP_SKIPINVISIBLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_SKIPTRANSPARENT" class="codetype"><span class="SCde"><span class="SCst">CWP_SKIPTRANSPARENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CW_USEDEFAULT" class="codetype"><span class="SCde"><span class="SCst">CW_USEDEFAULT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_DEFAULT_MINIMUM_ENTITIES" class="codetype"><span class="SCde"><span class="SCst">DEFAULT_MINIMUM_ENTITIES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_DM_POINTERHITTEST" class="codetype"><span class="SCde"><span class="SCst">DM_POINTERHITTEST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_DEVICE_NOT_CONNECTED" class="codetype"><span class="SCde"><span class="SCst">ERROR_DEVICE_NOT_CONNECTED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_FILE_NOT_FOUND" class="codetype"><span class="SCde"><span class="SCst">ERROR_FILE_NOT_FOUND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_IO_PENDING" class="codetype"><span class="SCde"><span class="SCst">ERROR_IO_PENDING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_NO_MORE_FILES" class="codetype"><span class="SCde"><span class="SCst">ERROR_NO_MORE_FILES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_SUCCESS" class="codetype"><span class="SCde"><span class="SCst">ERROR_SUCCESS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FALSE" class="codetype"><span class="SCde"><span class="SCst">FALSE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_ARCHIVE" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_ARCHIVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_COMPRESSED" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_COMPRESSED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_DEVICE" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_DEVICE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_DIRECTORY" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_DIRECTORY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_EA" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_EA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_ENCRYPTED" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_ENCRYPTED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_HIDDEN" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_HIDDEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_INTEGRITY_STREAM" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_INTEGRITY_STREAM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NORMAL" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_NORMAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NOT_CONTENT_INDEXED" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_NOT_CONTENT_INDEXED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NO_SCRUB_DATA" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_NO_SCRUB_DATA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_OFFLINE" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_OFFLINE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_PINNED" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_PINNED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_READONLY" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_READONLY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_RECALL_ON_OPEN" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_RECALL_ON_OPEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_REPARSE_POINT" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_REPARSE_POINT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_SPARSE_FILE" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_SPARSE_FILE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_SYSTEM" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_SYSTEM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_TEMPORARY" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_TEMPORARY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_UNPINNED" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_UNPINNED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_VIRTUAL" class="codetype"><span class="SCde"><span class="SCst">FILE_ATTRIBUTE_VIRTUAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_BEGIN" class="codetype"><span class="SCde"><span class="SCst">FILE_BEGIN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_CURRENT" class="codetype"><span class="SCde"><span class="SCst">FILE_CURRENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_END" class="codetype"><span class="SCde"><span class="SCst">FILE_END</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_READ_ATTRIBUTES" class="codetype"><span class="SCde"><span class="SCst">FILE_READ_ATTRIBUTES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_DELETE" class="codetype"><span class="SCde"><span class="SCst">FILE_SHARE_DELETE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_READ" class="codetype"><span class="SCde"><span class="SCst">FILE_SHARE_READ</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_WRITE" class="codetype"><span class="SCde"><span class="SCst">FILE_SHARE_WRITE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_WRITE_ATTRIBUTES" class="codetype"><span class="SCde"><span class="SCst">FILE_WRITE_ATTRIBUTES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_BLUE" class="codetype"><span class="SCde"><span class="SCst">FOREGROUND_BLUE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_GREEN" class="codetype"><span class="SCde"><span class="SCst">FOREGROUND_GREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_INTENSITY" class="codetype"><span class="SCde"><span class="SCst">FOREGROUND_INTENSITY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_MASK" class="codetype"><span class="SCde"><span class="SCst">FOREGROUND_MASK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_RED" class="codetype"><span class="SCde"><span class="SCst">FOREGROUND_RED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_ALLOCATE_BUFFER" class="codetype"><span class="SCde"><span class="SCst">FORMAT_MESSAGE_ALLOCATE_BUFFER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_ARGUMENT_ARRAY" class="codetype"><span class="SCde"><span class="SCst">FORMAT_MESSAGE_ARGUMENT_ARRAY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_HMODULE" class="codetype"><span class="SCde"><span class="SCst">FORMAT_MESSAGE_FROM_HMODULE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_STRING" class="codetype"><span class="SCde"><span class="SCst">FORMAT_MESSAGE_FROM_STRING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_SYSTEM" class="codetype"><span class="SCde"><span class="SCst">FORMAT_MESSAGE_FROM_SYSTEM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_IGNORE_INSERTS" class="codetype"><span class="SCde"><span class="SCst">FORMAT_MESSAGE_IGNORE_INSERTS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_MAX_WIDTH_MASK" class="codetype"><span class="SCde"><span class="SCst">FORMAT_MESSAGE_MAX_WIDTH_MASK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_ALL" class="codetype"><span class="SCde"><span class="SCst">GENERIC_ALL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_EXECUTE" class="codetype"><span class="SCde"><span class="SCst">GENERIC_EXECUTE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_READ" class="codetype"><span class="SCde"><span class="SCst">GENERIC_READ</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_WRITE" class="codetype"><span class="SCde"><span class="SCst">GENERIC_WRITE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_DDESHARE" class="codetype"><span class="SCde"><span class="SCst">GMEM_DDESHARE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_DISCARDABLE" class="codetype"><span class="SCde"><span class="SCst">GMEM_DISCARDABLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_FIXED" class="codetype"><span class="SCde"><span class="SCst">GMEM_FIXED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_INVALID_HANDLE" class="codetype"><span class="SCde"><span class="SCst">GMEM_INVALID_HANDLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_LOWER" class="codetype"><span class="SCde"><span class="SCst">GMEM_LOWER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_MODIFY" class="codetype"><span class="SCde"><span class="SCst">GMEM_MODIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_MOVEABLE" class="codetype"><span class="SCde"><span class="SCst">GMEM_MOVEABLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NOCOMPACT" class="codetype"><span class="SCde"><span class="SCst">GMEM_NOCOMPACT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NODISCARD" class="codetype"><span class="SCde"><span class="SCst">GMEM_NODISCARD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NOTIFY" class="codetype"><span class="SCde"><span class="SCst">GMEM_NOTIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NOT_BANKED" class="codetype"><span class="SCde"><span class="SCst">GMEM_NOT_BANKED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_SHARE" class="codetype"><span class="SCde"><span class="SCst">GMEM_SHARE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_VALID_FLAGS" class="codetype"><span class="SCde"><span class="SCst">GMEM_VALID_FLAGS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_ZEROINIT" class="codetype"><span class="SCde"><span class="SCst">GMEM_ZEROINIT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_HINSTANCE" class="codetype"><span class="SCde"><span class="SCst">GWLP_HINSTANCE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_HWNDPARENT" class="codetype"><span class="SCde"><span class="SCst">GWLP_HWNDPARENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_ID" class="codetype"><span class="SCde"><span class="SCst">GWLP_ID</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_USERDATA" class="codetype"><span class="SCde"><span class="SCst">GWLP_USERDATA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_WNDPROC" class="codetype"><span class="SCde"><span class="SCst">GWLP_WNDPROC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_CHILD" class="codetype"><span class="SCde"><span class="SCst">GW_CHILD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_ENABLEDPOPUP" class="codetype"><span class="SCde"><span class="SCst">GW_ENABLEDPOPUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDFIRST" class="codetype"><span class="SCde"><span class="SCst">GW_HWNDFIRST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDLAST" class="codetype"><span class="SCde"><span class="SCst">GW_HWNDLAST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDNEXT" class="codetype"><span class="SCde"><span class="SCst">GW_HWNDNEXT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDPREV" class="codetype"><span class="SCde"><span class="SCst">GW_HWNDPREV</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_MAX" class="codetype"><span class="SCde"><span class="SCst">GW_MAX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_OWNER" class="codetype"><span class="SCde"><span class="SCst">GW_OWNER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_BOTTOM" class="codetype"><span class="SCde"><span class="SCst">HWND_BOTTOM</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_NOTOPMOST" class="codetype"><span class="SCde"><span class="SCst">HWND_NOTOPMOST</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_TOP" class="codetype"><span class="SCde"><span class="SCst">HWND_TOP</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_TOPMOST" class="codetype"><span class="SCde"><span class="SCst">HWND_TOPMOST</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDABORT" class="codetype"><span class="SCde"><span class="SCst">IDABORT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDCANCEL" class="codetype"><span class="SCde"><span class="SCst">IDCANCEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDCLOSE" class="codetype"><span class="SCde"><span class="SCst">IDCLOSE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDCONTINUE" class="codetype"><span class="SCde"><span class="SCst">IDCONTINUE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_APPSTARTING" class="codetype"><span class="SCde"><span class="SCst">IDC_APPSTARTING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_ARROW" class="codetype"><span class="SCde"><span class="SCst">IDC_ARROW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_CROSS" class="codetype"><span class="SCde"><span class="SCst">IDC_CROSS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_HAND" class="codetype"><span class="SCde"><span class="SCst">IDC_HAND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_HELP" class="codetype"><span class="SCde"><span class="SCst">IDC_HELP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_IBEAM" class="codetype"><span class="SCde"><span class="SCst">IDC_IBEAM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_ICON" class="codetype"><span class="SCde"><span class="SCst">IDC_ICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_NO" class="codetype"><span class="SCde"><span class="SCst">IDC_NO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_PERSON" class="codetype"><span class="SCde"><span class="SCst">IDC_PERSON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_PIN" class="codetype"><span class="SCde"><span class="SCst">IDC_PIN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZE" class="codetype"><span class="SCde"><span class="SCst">IDC_SIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZEALL" class="codetype"><span class="SCde"><span class="SCst">IDC_SIZEALL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZENESW" class="codetype"><span class="SCde"><span class="SCst">IDC_SIZENESW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZENS" class="codetype"><span class="SCde"><span class="SCst">IDC_SIZENS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZENWSE" class="codetype"><span class="SCde"><span class="SCst">IDC_SIZENWSE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZEWE" class="codetype"><span class="SCde"><span class="SCst">IDC_SIZEWE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_UPARROW" class="codetype"><span class="SCde"><span class="SCst">IDC_UPARROW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_WAIT" class="codetype"><span class="SCde"><span class="SCst">IDC_WAIT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDHELP" class="codetype"><span class="SCde"><span class="SCst">IDHELP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDIGNORE" class="codetype"><span class="SCde"><span class="SCst">IDIGNORE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDNO" class="codetype"><span class="SCde"><span class="SCst">IDNO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDOK" class="codetype"><span class="SCde"><span class="SCst">IDOK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDRETRY" class="codetype"><span class="SCde"><span class="SCst">IDRETRY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDTIMEOUT" class="codetype"><span class="SCde"><span class="SCst">IDTIMEOUT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDTRYAGAIN" class="codetype"><span class="SCde"><span class="SCst">IDTRYAGAIN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IDYES" class="codetype"><span class="SCde"><span class="SCst">IDYES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_INFINITE" class="codetype"><span class="SCde"><span class="SCst">INFINITE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_FILE_ATTRIBUTES" class="codetype"><span class="SCde"><span class="SCst">INVALID_FILE_ATTRIBUTES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_FILE_SIZE" class="codetype"><span class="SCde"><span class="SCst">INVALID_FILE_SIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_HANDLE_VALUE" class="codetype"><span class="SCde"><span class="SCst">INVALID_HANDLE_VALUE</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_SET_FILE_POINTER" class="codetype"><span class="SCde"><span class="SCst">INVALID_SET_FILE_POINTER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LANG_ENGLISH" class="codetype"><span class="SCde"><span class="SCst">LANG_ENGLISH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LWA_ALPHA" class="codetype"><span class="SCde"><span class="SCst">LWA_ALPHA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LWA_COLORKEY" class="codetype"><span class="SCde"><span class="SCst">LWA_COLORKEY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_CHAR" class="codetype"><span class="SCde"><span class="SCst">MAPVK_VK_TO_CHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_VSC" class="codetype"><span class="SCde"><span class="SCst">MAPVK_VK_TO_VSC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_VSC_EX" class="codetype"><span class="SCde"><span class="SCst">MAPVK_VK_TO_VSC_EX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VSC_TO_VK" class="codetype"><span class="SCde"><span class="SCst">MAPVK_VSC_TO_VK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VSC_TO_VK_EX" class="codetype"><span class="SCde"><span class="SCst">MAPVK_VSC_TO_VK_EX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAXIMUM_WAIT_OBJECTS" class="codetype"><span class="SCde"><span class="SCst">MAXIMUM_WAIT_OBJECTS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_ADDRESS_LENGTH" class="codetype"><span class="SCde"><span class="SCst">MAX_ADAPTER_ADDRESS_LENGTH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_DESCRIPTION_LENGTH" class="codetype"><span class="SCde"><span class="SCst">MAX_ADAPTER_DESCRIPTION_LENGTH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_NAME_LENGTH" class="codetype"><span class="SCde"><span class="SCst">MAX_ADAPTER_NAME_LENGTH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_DHCPV6_DUID_LENGTH" class="codetype"><span class="SCde"><span class="SCst">MAX_DHCPV6_DUID_LENGTH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_DNS_SUFFIX_STRING_LENGTH" class="codetype"><span class="SCde"><span class="SCst">MAX_DNS_SUFFIX_STRING_LENGTH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_DOMAIN_NAME_LEN" class="codetype"><span class="SCde"><span class="SCst">MAX_DOMAIN_NAME_LEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_HOSTNAME_LEN" class="codetype"><span class="SCde"><span class="SCst">MAX_HOSTNAME_LEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_PATH" class="codetype"><span class="SCde"><span class="SCst">MAX_PATH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_SCOPE_ID_LEN" class="codetype"><span class="SCde"><span class="SCst">MAX_SCOPE_ID_LEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ABORTRETRYIGNORE" class="codetype"><span class="SCde"><span class="SCst">MB_ABORTRETRYIGNORE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_APPLMODAL" class="codetype"><span class="SCde"><span class="SCst">MB_APPLMODAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_CANCELTRYCONTINUE" class="codetype"><span class="SCde"><span class="SCst">MB_CANCELTRYCONTINUE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFAULT_DESKTOP_ONLY" class="codetype"><span class="SCde"><span class="SCst">MB_DEFAULT_DESKTOP_ONLY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON1" class="codetype"><span class="SCde"><span class="SCst">MB_DEFBUTTON1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON2" class="codetype"><span class="SCde"><span class="SCst">MB_DEFBUTTON2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON3" class="codetype"><span class="SCde"><span class="SCst">MB_DEFBUTTON3</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON4" class="codetype"><span class="SCde"><span class="SCst">MB_DEFBUTTON4</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_HELP" class="codetype"><span class="SCde"><span class="SCst">MB_HELP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONASTERISK" class="codetype"><span class="SCde"><span class="SCst">MB_ICONASTERISK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONERROR" class="codetype"><span class="SCde"><span class="SCst">MB_ICONERROR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONEXCLAMATION" class="codetype"><span class="SCde"><span class="SCst">MB_ICONEXCLAMATION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONHAND" class="codetype"><span class="SCde"><span class="SCst">MB_ICONHAND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONINFORMATION" class="codetype"><span class="SCde"><span class="SCst">MB_ICONINFORMATION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONQUESTION" class="codetype"><span class="SCde"><span class="SCst">MB_ICONQUESTION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONSTOP" class="codetype"><span class="SCde"><span class="SCst">MB_ICONSTOP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONWARNING" class="codetype"><span class="SCde"><span class="SCst">MB_ICONWARNING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_NOFOCUS" class="codetype"><span class="SCde"><span class="SCst">MB_NOFOCUS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_OK" class="codetype"><span class="SCde"><span class="SCst">MB_OK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_OKCANCEL" class="codetype"><span class="SCde"><span class="SCst">MB_OKCANCEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_RETRYCANCEL" class="codetype"><span class="SCde"><span class="SCst">MB_RETRYCANCEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_RIGHT" class="codetype"><span class="SCde"><span class="SCst">MB_RIGHT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_RTLREADING" class="codetype"><span class="SCde"><span class="SCst">MB_RTLREADING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_SETFOREGROUND" class="codetype"><span class="SCde"><span class="SCst">MB_SETFOREGROUND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_SYSTEMMODAL" class="codetype"><span class="SCde"><span class="SCst">MB_SYSTEMMODAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_TASKMODAL" class="codetype"><span class="SCde"><span class="SCst">MB_TASKMODAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_TOPMOST" class="codetype"><span class="SCde"><span class="SCst">MB_TOPMOST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_USERICON" class="codetype"><span class="SCde"><span class="SCst">MB_USERICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_YESNO" class="codetype"><span class="SCde"><span class="SCst">MB_YESNO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_YESNOCANCEL" class="codetype"><span class="SCde"><span class="SCst">MB_YESNOCANCEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MN_GETHMENU" class="codetype"><span class="SCde"><span class="SCst">MN_GETHMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_ALT" class="codetype"><span class="SCde"><span class="SCst">MOD_ALT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_CONTROL" class="codetype"><span class="SCde"><span class="SCst">MOD_CONTROL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_NOREPEAT" class="codetype"><span class="SCde"><span class="SCst">MOD_NOREPEAT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_SHIFT" class="codetype"><span class="SCde"><span class="SCst">MOD_SHIFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_WIN" class="codetype"><span class="SCde"><span class="SCst">MOD_WIN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTONEAREST" class="codetype"><span class="SCde"><span class="SCst">MONITOR_DEFAULTTONEAREST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTONULL" class="codetype"><span class="SCde"><span class="SCst">MONITOR_DEFAULTTONULL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTOPRIMARY" class="codetype"><span class="SCde"><span class="SCst">MONITOR_DEFAULTTOPRIMARY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_GUID" class="codetype"><span class="SCde"><span class="SCst">NIF_GUID</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_ICON" class="codetype"><span class="SCde"><span class="SCst">NIF_ICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_INFO" class="codetype"><span class="SCde"><span class="SCst">NIF_INFO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_MESSAGE" class="codetype"><span class="SCde"><span class="SCst">NIF_MESSAGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_REALTIME" class="codetype"><span class="SCde"><span class="SCst">NIF_REALTIME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_SHOWTIP" class="codetype"><span class="SCde"><span class="SCst">NIF_SHOWTIP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_STATE" class="codetype"><span class="SCde"><span class="SCst">NIF_STATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_TIP" class="codetype"><span class="SCde"><span class="SCst">NIF_TIP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_ADD" class="codetype"><span class="SCde"><span class="SCst">NIM_ADD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_DELETE" class="codetype"><span class="SCde"><span class="SCst">NIM_DELETE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_MODIFY" class="codetype"><span class="SCde"><span class="SCst">NIM_MODIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_SETFOCUS" class="codetype"><span class="SCde"><span class="SCst">NIM_SETFOCUS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_SETVERSION" class="codetype"><span class="SCde"><span class="SCst">NIM_SETVERSION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_OPEN_ALWAYS" class="codetype"><span class="SCde"><span class="SCst">OPEN_ALWAYS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_OPEN_EXISTING" class="codetype"><span class="SCde"><span class="SCst">OPEN_EXISTING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_PM_NOREMOVE" class="codetype"><span class="SCde"><span class="SCst">PM_NOREMOVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_PM_NOYIELD" class="codetype"><span class="SCde"><span class="SCst">PM_NOYIELD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_PM_REMOVE" class="codetype"><span class="SCde"><span class="SCst">PM_REMOVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_ARRANGE" class="codetype"><span class="SCde"><span class="SCst">SC_ARRANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_CLOSE" class="codetype"><span class="SCde"><span class="SCst">SC_CLOSE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_CONTEXTHELP" class="codetype"><span class="SCde"><span class="SCst">SC_CONTEXTHELP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_DEFAULT" class="codetype"><span class="SCde"><span class="SCst">SC_DEFAULT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_HOTKEY" class="codetype"><span class="SCde"><span class="SCst">SC_HOTKEY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_HSCROLL" class="codetype"><span class="SCde"><span class="SCst">SC_HSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_KEYMENU" class="codetype"><span class="SCde"><span class="SCst">SC_KEYMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MAXIMIZE" class="codetype"><span class="SCde"><span class="SCst">SC_MAXIMIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MINIMIZE" class="codetype"><span class="SCde"><span class="SCst">SC_MINIMIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MONITORPOWER" class="codetype"><span class="SCde"><span class="SCst">SC_MONITORPOWER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MOUSEMENU" class="codetype"><span class="SCde"><span class="SCst">SC_MOUSEMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MOVE" class="codetype"><span class="SCde"><span class="SCst">SC_MOVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_NEXTWINDOW" class="codetype"><span class="SCde"><span class="SCst">SC_NEXTWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_PREVWINDOW" class="codetype"><span class="SCde"><span class="SCst">SC_PREVWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_RESTORE" class="codetype"><span class="SCde"><span class="SCst">SC_RESTORE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_SCREENSAVE" class="codetype"><span class="SCde"><span class="SCst">SC_SCREENSAVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_SEPARATOR" class="codetype"><span class="SCde"><span class="SCst">SC_SEPARATOR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_SIZE" class="codetype"><span class="SCde"><span class="SCst">SC_SIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_TASKLIST" class="codetype"><span class="SCde"><span class="SCst">SC_TASKLIST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_VSCROLL" class="codetype"><span class="SCde"><span class="SCst">SC_VSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGFI_ADDOVERLAYS" class="codetype"><span class="SCde"><span class="SCst">SHGFI_ADDOVERLAYS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>apply the appropriate overlays. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ATTRIBUTES" class="codetype"><span class="SCde"><span class="SCst">SHGFI_ATTRIBUTES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get attributes. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ATTR_SPECIFIED" class="codetype"><span class="SCde"><span class="SCst">SHGFI_ATTR_SPECIFIED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get only specified attributes. </td>
</tr>
<tr>
<td id="Win32_SHGFI_DISPLAYNAME" class="codetype"><span class="SCde"><span class="SCst">SHGFI_DISPLAYNAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get display name. </td>
</tr>
<tr>
<td id="Win32_SHGFI_EXETYPE" class="codetype"><span class="SCde"><span class="SCst">SHGFI_EXETYPE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>return exe type. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ICON" class="codetype"><span class="SCde"><span class="SCst">SHGFI_ICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ICONLOCATION" class="codetype"><span class="SCde"><span class="SCst">SHGFI_ICONLOCATION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get icon location. </td>
</tr>
<tr>
<td id="Win32_SHGFI_LARGEICON" class="codetype"><span class="SCde"><span class="SCst">SHGFI_LARGEICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get large icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_LINKOVERLAY" class="codetype"><span class="SCde"><span class="SCst">SHGFI_LINKOVERLAY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>put a link overlay on icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_OPENICON" class="codetype"><span class="SCde"><span class="SCst">SHGFI_OPENICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get open icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_OVERLAYINDEX" class="codetype"><span class="SCde"><span class="SCst">SHGFI_OVERLAYINDEX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Get the index of the overlay. </td>
</tr>
<tr>
<td id="Win32_SHGFI_PIDL" class="codetype"><span class="SCde"><span class="SCst">SHGFI_PIDL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>pszPath is a pidl. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SELECTED" class="codetype"><span class="SCde"><span class="SCst">SHGFI_SELECTED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>show icon in selected state. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SHELLICONSIZE" class="codetype"><span class="SCde"><span class="SCst">SHGFI_SHELLICONSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get shell size icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SMALLICON" class="codetype"><span class="SCde"><span class="SCst">SHGFI_SMALLICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get small icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SYSICONINDEX" class="codetype"><span class="SCde"><span class="SCst">SHGFI_SYSICONINDEX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get system icon index. </td>
</tr>
<tr>
<td id="Win32_SHGFI_TYPENAME" class="codetype"><span class="SCde"><span class="SCst">SHGFI_TYPENAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>get type name. </td>
</tr>
<tr>
<td id="Win32_SHGFI_USEFILEATTRIBUTES" class="codetype"><span class="SCde"><span class="SCst">SHGFI_USEFILEATTRIBUTES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>use passed dwFileAttribute. </td>
</tr>
<tr>
<td id="Win32_SHGSI_ICON" class="codetype"><span class="SCde"><span class="SCst">SHGSI_ICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_ICONLOCATION" class="codetype"><span class="SCde"><span class="SCst">SHGSI_ICONLOCATION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td>you always get the icon location. </td>
</tr>
<tr>
<td id="Win32_SHGSI_LARGEICON" class="codetype"><span class="SCde"><span class="SCst">SHGSI_LARGEICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_LINKOVERLAY" class="codetype"><span class="SCde"><span class="SCst">SHGSI_LINKOVERLAY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SELECTED" class="codetype"><span class="SCde"><span class="SCst">SHGSI_SELECTED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SHELLICONSIZE" class="codetype"><span class="SCde"><span class="SCst">SHGSI_SHELLICONSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SMALLICON" class="codetype"><span class="SCde"><span class="SCst">SHGSI_SMALLICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SYSICONINDEX" class="codetype"><span class="SCde"><span class="SCst">SHGSI_SYSICONINDEX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_ARRANGE" class="codetype"><span class="SCde"><span class="SCst">SM_ARRANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CARETBLINKINGENABLED" class="codetype"><span class="SCde"><span class="SCst">SM_CARETBLINKINGENABLED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CLEANBOOT" class="codetype"><span class="SCde"><span class="SCst">SM_CLEANBOOT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CMETRICS" class="codetype"><span class="SCde"><span class="SCst">SM_CMETRICS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CMONITORS" class="codetype"><span class="SCde"><span class="SCst">SM_CMONITORS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CMOUSEBUTTONS" class="codetype"><span class="SCde"><span class="SCst">SM_CMOUSEBUTTONS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CONVERTIBLESLATEMODE" class="codetype"><span class="SCde"><span class="SCst">SM_CONVERTIBLESLATEMODE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXBORDER" class="codetype"><span class="SCde"><span class="SCst">SM_CXBORDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXCURSOR" class="codetype"><span class="SCde"><span class="SCst">SM_CXCURSOR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXDLGFRAME" class="codetype"><span class="SCde"><span class="SCst">SM_CXDLGFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXDOUBLECLK" class="codetype"><span class="SCde"><span class="SCst">SM_CXDOUBLECLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXDRAG" class="codetype"><span class="SCde"><span class="SCst">SM_CXDRAG</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXEDGE" class="codetype"><span class="SCde"><span class="SCst">SM_CXEDGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFIXEDFRAME" class="codetype"><span class="SCde"><span class="SCst">SM_CXFIXEDFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFOCUSBORDER" class="codetype"><span class="SCde"><span class="SCst">SM_CXFOCUSBORDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFRAME" class="codetype"><span class="SCde"><span class="SCst">SM_CXFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFULLSCREEN" class="codetype"><span class="SCde"><span class="SCst">SM_CXFULLSCREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXHSCROLL" class="codetype"><span class="SCde"><span class="SCst">SM_CXHSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXHTHUMB" class="codetype"><span class="SCde"><span class="SCst">SM_CXHTHUMB</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXICON" class="codetype"><span class="SCde"><span class="SCst">SM_CXICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXICONSPACING" class="codetype"><span class="SCde"><span class="SCst">SM_CXICONSPACING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMAXIMIZED" class="codetype"><span class="SCde"><span class="SCst">SM_CXMAXIMIZED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMAXTRACK" class="codetype"><span class="SCde"><span class="SCst">SM_CXMAXTRACK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMENUCHECK" class="codetype"><span class="SCde"><span class="SCst">SM_CXMENUCHECK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMENUSIZE" class="codetype"><span class="SCde"><span class="SCst">SM_CXMENUSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMIN" class="codetype"><span class="SCde"><span class="SCst">SM_CXMIN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMINIMIZED" class="codetype"><span class="SCde"><span class="SCst">SM_CXMINIMIZED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMINSPACING" class="codetype"><span class="SCde"><span class="SCst">SM_CXMINSPACING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMINTRACK" class="codetype"><span class="SCde"><span class="SCst">SM_CXMINTRACK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXPADDEDBORDER" class="codetype"><span class="SCde"><span class="SCst">SM_CXPADDEDBORDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSCREEN" class="codetype"><span class="SCde"><span class="SCst">SM_CXSCREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSIZE" class="codetype"><span class="SCde"><span class="SCst">SM_CXSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSIZEFRAME" class="codetype"><span class="SCde"><span class="SCst">SM_CXSIZEFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSMICON" class="codetype"><span class="SCde"><span class="SCst">SM_CXSMICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSMSIZE" class="codetype"><span class="SCde"><span class="SCst">SM_CXSMSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXVIRTUALSCREEN" class="codetype"><span class="SCde"><span class="SCst">SM_CXVIRTUALSCREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXVSCROLL" class="codetype"><span class="SCde"><span class="SCst">SM_CXVSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYBORDER" class="codetype"><span class="SCde"><span class="SCst">SM_CYBORDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYCAPTION" class="codetype"><span class="SCde"><span class="SCst">SM_CYCAPTION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYCURSOR" class="codetype"><span class="SCde"><span class="SCst">SM_CYCURSOR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYDLGFRAME" class="codetype"><span class="SCde"><span class="SCst">SM_CYDLGFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYDOUBLECLK" class="codetype"><span class="SCde"><span class="SCst">SM_CYDOUBLECLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYDRAG" class="codetype"><span class="SCde"><span class="SCst">SM_CYDRAG</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYEDGE" class="codetype"><span class="SCde"><span class="SCst">SM_CYEDGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFIXEDFRAME" class="codetype"><span class="SCde"><span class="SCst">SM_CYFIXEDFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFOCUSBORDER" class="codetype"><span class="SCde"><span class="SCst">SM_CYFOCUSBORDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFRAME" class="codetype"><span class="SCde"><span class="SCst">SM_CYFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFULLSCREEN" class="codetype"><span class="SCde"><span class="SCst">SM_CYFULLSCREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYHSCROLL" class="codetype"><span class="SCde"><span class="SCst">SM_CYHSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYICON" class="codetype"><span class="SCde"><span class="SCst">SM_CYICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYICONSPACING" class="codetype"><span class="SCde"><span class="SCst">SM_CYICONSPACING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYKANJIWINDOW" class="codetype"><span class="SCde"><span class="SCst">SM_CYKANJIWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMAXIMIZED" class="codetype"><span class="SCde"><span class="SCst">SM_CYMAXIMIZED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMAXTRACK" class="codetype"><span class="SCde"><span class="SCst">SM_CYMAXTRACK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMENU" class="codetype"><span class="SCde"><span class="SCst">SM_CYMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMENUCHECK" class="codetype"><span class="SCde"><span class="SCst">SM_CYMENUCHECK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMENUSIZE" class="codetype"><span class="SCde"><span class="SCst">SM_CYMENUSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMIN" class="codetype"><span class="SCde"><span class="SCst">SM_CYMIN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMINIMIZED" class="codetype"><span class="SCde"><span class="SCst">SM_CYMINIMIZED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMINSPACING" class="codetype"><span class="SCde"><span class="SCst">SM_CYMINSPACING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMINTRACK" class="codetype"><span class="SCde"><span class="SCst">SM_CYMINTRACK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSCREEN" class="codetype"><span class="SCde"><span class="SCst">SM_CYSCREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSIZE" class="codetype"><span class="SCde"><span class="SCst">SM_CYSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSIZEFRAME" class="codetype"><span class="SCde"><span class="SCst">SM_CYSIZEFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSMCAPTION" class="codetype"><span class="SCde"><span class="SCst">SM_CYSMCAPTION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSMICON" class="codetype"><span class="SCde"><span class="SCst">SM_CYSMICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSMSIZE" class="codetype"><span class="SCde"><span class="SCst">SM_CYSMSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYVIRTUALSCREEN" class="codetype"><span class="SCde"><span class="SCst">SM_CYVIRTUALSCREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYVSCROLL" class="codetype"><span class="SCde"><span class="SCst">SM_CYVSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYVTHUMB" class="codetype"><span class="SCde"><span class="SCst">SM_CYVTHUMB</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_DBCSENABLED" class="codetype"><span class="SCde"><span class="SCst">SM_DBCSENABLED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_DEBUG" class="codetype"><span class="SCde"><span class="SCst">SM_DEBUG</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_DIGITIZER" class="codetype"><span class="SCde"><span class="SCst">SM_DIGITIZER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_IMMENABLED" class="codetype"><span class="SCde"><span class="SCst">SM_IMMENABLED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MAXIMUMTOUCHES" class="codetype"><span class="SCde"><span class="SCst">SM_MAXIMUMTOUCHES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MEDIACENTER" class="codetype"><span class="SCde"><span class="SCst">SM_MEDIACENTER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MENUDROPALIGNMENT" class="codetype"><span class="SCde"><span class="SCst">SM_MENUDROPALIGNMENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MIDEASTENABLED" class="codetype"><span class="SCde"><span class="SCst">SM_MIDEASTENABLED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MOUSEHORIZONTALWHEELPRESENT" class="codetype"><span class="SCde"><span class="SCst">SM_MOUSEHORIZONTALWHEELPRESENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MOUSEPRESENT" class="codetype"><span class="SCde"><span class="SCst">SM_MOUSEPRESENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MOUSEWHEELPRESENT" class="codetype"><span class="SCde"><span class="SCst">SM_MOUSEWHEELPRESENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_NETWORK" class="codetype"><span class="SCde"><span class="SCst">SM_NETWORK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_PENWINDOWS" class="codetype"><span class="SCde"><span class="SCst">SM_PENWINDOWS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_REMOTECONTROL" class="codetype"><span class="SCde"><span class="SCst">SM_REMOTECONTROL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_REMOTESESSION" class="codetype"><span class="SCde"><span class="SCst">SM_REMOTESESSION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED1" class="codetype"><span class="SCde"><span class="SCst">SM_RESERVED1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED2" class="codetype"><span class="SCde"><span class="SCst">SM_RESERVED2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED3" class="codetype"><span class="SCde"><span class="SCst">SM_RESERVED3</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED4" class="codetype"><span class="SCde"><span class="SCst">SM_RESERVED4</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SAMEDISPLAYFORMAT" class="codetype"><span class="SCde"><span class="SCst">SM_SAMEDISPLAYFORMAT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SECURE" class="codetype"><span class="SCde"><span class="SCst">SM_SECURE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SERVERR2" class="codetype"><span class="SCde"><span class="SCst">SM_SERVERR2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SHOWSOUNDS" class="codetype"><span class="SCde"><span class="SCst">SM_SHOWSOUNDS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SHUTTINGDOWN" class="codetype"><span class="SCde"><span class="SCst">SM_SHUTTINGDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SLOWMACHINE" class="codetype"><span class="SCde"><span class="SCst">SM_SLOWMACHINE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_STARTER" class="codetype"><span class="SCde"><span class="SCst">SM_STARTER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SWAPBUTTON" class="codetype"><span class="SCde"><span class="SCst">SM_SWAPBUTTON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SYSTEMDOCKED" class="codetype"><span class="SCde"><span class="SCst">SM_SYSTEMDOCKED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_TABLETPC" class="codetype"><span class="SCde"><span class="SCst">SM_TABLETPC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_XVIRTUALSCREEN" class="codetype"><span class="SCde"><span class="SCst">SM_XVIRTUALSCREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_YVIRTUALSCREEN" class="codetype"><span class="SCde"><span class="SCst">SM_YVIRTUALSCREEN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SRWLOCK_INIT" class="codetype"><span class="SCde"><span class="SCst">SRWLOCK_INIT</span></span></td>
<td class="codetype"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_STD_INPUT_HANDLE" class="codetype"><span class="SCde"><span class="SCst">STD_INPUT_HANDLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_STD_OUTPUT_HANDLE" class="codetype"><span class="SCde"><span class="SCst">STD_OUTPUT_HANDLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SUBLANG_DEFAULT" class="codetype"><span class="SCde"><span class="SCst">SUBLANG_DEFAULT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_ASYNCWINDOWPOS" class="codetype"><span class="SCde"><span class="SCst">SWP_ASYNCWINDOWPOS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_DEFERERASE" class="codetype"><span class="SCde"><span class="SCst">SWP_DEFERERASE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_DRAWFRAME" class="codetype"><span class="SCde"><span class="SCst">SWP_DRAWFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_FRAMECHANGED" class="codetype"><span class="SCde"><span class="SCst">SWP_FRAMECHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_HIDEWINDOW" class="codetype"><span class="SCde"><span class="SCst">SWP_HIDEWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOACTIVATE" class="codetype"><span class="SCde"><span class="SCst">SWP_NOACTIVATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOCOPYBITS" class="codetype"><span class="SCde"><span class="SCst">SWP_NOCOPYBITS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOMOVE" class="codetype"><span class="SCde"><span class="SCst">SWP_NOMOVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOOWNERZORDER" class="codetype"><span class="SCde"><span class="SCst">SWP_NOOWNERZORDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOREDRAW" class="codetype"><span class="SCde"><span class="SCst">SWP_NOREDRAW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOREPOSITION" class="codetype"><span class="SCde"><span class="SCst">SWP_NOREPOSITION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOSENDCHANGING" class="codetype"><span class="SCde"><span class="SCst">SWP_NOSENDCHANGING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOSIZE" class="codetype"><span class="SCde"><span class="SCst">SWP_NOSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOZORDER" class="codetype"><span class="SCde"><span class="SCst">SWP_NOZORDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_SHOWWINDOW" class="codetype"><span class="SCde"><span class="SCst">SWP_SHOWWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_FORCEMINIMIZE" class="codetype"><span class="SCde"><span class="SCst">SW_FORCEMINIMIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_HIDE" class="codetype"><span class="SCde"><span class="SCst">SW_HIDE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_MAX" class="codetype"><span class="SCde"><span class="SCst">SW_MAX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_MAXIMIZE" class="codetype"><span class="SCde"><span class="SCst">SW_MAXIMIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_MINIMIZE" class="codetype"><span class="SCde"><span class="SCst">SW_MINIMIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_NORMAL" class="codetype"><span class="SCde"><span class="SCst">SW_NORMAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_RESTORE" class="codetype"><span class="SCde"><span class="SCst">SW_RESTORE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOW" class="codetype"><span class="SCde"><span class="SCst">SW_SHOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWDEFAULT" class="codetype"><span class="SCde"><span class="SCst">SW_SHOWDEFAULT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWMAXIMIZED" class="codetype"><span class="SCde"><span class="SCst">SW_SHOWMAXIMIZED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWMINIMIZED" class="codetype"><span class="SCde"><span class="SCst">SW_SHOWMINIMIZED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWMINNOACTIVE" class="codetype"><span class="SCde"><span class="SCst">SW_SHOWMINNOACTIVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWNA" class="codetype"><span class="SCde"><span class="SCst">SW_SHOWNA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWNOACTIVATE" class="codetype"><span class="SCde"><span class="SCst">SW_SHOWNOACTIVATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWNORMAL" class="codetype"><span class="SCde"><span class="SCst">SW_SHOWNORMAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_S_OK" class="codetype"><span class="SCde"><span class="SCst">S_OK</span></span></td>
<td class="codetype"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_ABOVE_NORMAL" class="codetype"><span class="SCde"><span class="SCst">THREAD_PRIORITY_ABOVE_NORMAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_BELOW_NORMAL" class="codetype"><span class="SCde"><span class="SCst">THREAD_PRIORITY_BELOW_NORMAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_HIGHEST" class="codetype"><span class="SCde"><span class="SCst">THREAD_PRIORITY_HIGHEST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_LOWEST" class="codetype"><span class="SCde"><span class="SCst">THREAD_PRIORITY_LOWEST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_NORMAL" class="codetype"><span class="SCde"><span class="SCst">THREAD_PRIORITY_NORMAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_TRUE" class="codetype"><span class="SCde"><span class="SCst">TRUE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_TRUNCATE_EXISTING" class="codetype"><span class="SCde"><span class="SCst">TRUNCATE_EXISTING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ACCEPT" class="codetype"><span class="SCde"><span class="SCst">VK_ACCEPT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ADD" class="codetype"><span class="SCde"><span class="SCst">VK_ADD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_APPS" class="codetype"><span class="SCde"><span class="SCst">VK_APPS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ATTN" class="codetype"><span class="SCde"><span class="SCst">VK_ATTN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BACK" class="codetype"><span class="SCde"><span class="SCst">VK_BACK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_BACK" class="codetype"><span class="SCde"><span class="SCst">VK_BROWSER_BACK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_FAVORITES" class="codetype"><span class="SCde"><span class="SCst">VK_BROWSER_FAVORITES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_FORWARD" class="codetype"><span class="SCde"><span class="SCst">VK_BROWSER_FORWARD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_HOME" class="codetype"><span class="SCde"><span class="SCst">VK_BROWSER_HOME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_REFRESH" class="codetype"><span class="SCde"><span class="SCst">VK_BROWSER_REFRESH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_SEARCH" class="codetype"><span class="SCde"><span class="SCst">VK_BROWSER_SEARCH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_STOP" class="codetype"><span class="SCde"><span class="SCst">VK_BROWSER_STOP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CANCEL" class="codetype"><span class="SCde"><span class="SCst">VK_CANCEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CAPITAL" class="codetype"><span class="SCde"><span class="SCst">VK_CAPITAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CLEAR" class="codetype"><span class="SCde"><span class="SCst">VK_CLEAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CONTROL" class="codetype"><span class="SCde"><span class="SCst">VK_CONTROL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CONVERT" class="codetype"><span class="SCde"><span class="SCst">VK_CONVERT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CRSEL" class="codetype"><span class="SCde"><span class="SCst">VK_CRSEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DECIMAL" class="codetype"><span class="SCde"><span class="SCst">VK_DECIMAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DELETE" class="codetype"><span class="SCde"><span class="SCst">VK_DELETE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DIVIDE" class="codetype"><span class="SCde"><span class="SCst">VK_DIVIDE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DOWN" class="codetype"><span class="SCde"><span class="SCst">VK_DOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_END" class="codetype"><span class="SCde"><span class="SCst">VK_END</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_EREOF" class="codetype"><span class="SCde"><span class="SCst">VK_EREOF</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ESCAPE" class="codetype"><span class="SCde"><span class="SCst">VK_ESCAPE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_EXECUTE" class="codetype"><span class="SCde"><span class="SCst">VK_EXECUTE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_EXSEL" class="codetype"><span class="SCde"><span class="SCst">VK_EXSEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F1" class="codetype"><span class="SCde"><span class="SCst">VK_F1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F10" class="codetype"><span class="SCde"><span class="SCst">VK_F10</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F11" class="codetype"><span class="SCde"><span class="SCst">VK_F11</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F12" class="codetype"><span class="SCde"><span class="SCst">VK_F12</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F13" class="codetype"><span class="SCde"><span class="SCst">VK_F13</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F14" class="codetype"><span class="SCde"><span class="SCst">VK_F14</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F15" class="codetype"><span class="SCde"><span class="SCst">VK_F15</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F16" class="codetype"><span class="SCde"><span class="SCst">VK_F16</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F17" class="codetype"><span class="SCde"><span class="SCst">VK_F17</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F18" class="codetype"><span class="SCde"><span class="SCst">VK_F18</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F19" class="codetype"><span class="SCde"><span class="SCst">VK_F19</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F2" class="codetype"><span class="SCde"><span class="SCst">VK_F2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F20" class="codetype"><span class="SCde"><span class="SCst">VK_F20</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F21" class="codetype"><span class="SCde"><span class="SCst">VK_F21</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F22" class="codetype"><span class="SCde"><span class="SCst">VK_F22</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F23" class="codetype"><span class="SCde"><span class="SCst">VK_F23</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F24" class="codetype"><span class="SCde"><span class="SCst">VK_F24</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F3" class="codetype"><span class="SCde"><span class="SCst">VK_F3</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F4" class="codetype"><span class="SCde"><span class="SCst">VK_F4</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F5" class="codetype"><span class="SCde"><span class="SCst">VK_F5</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F6" class="codetype"><span class="SCde"><span class="SCst">VK_F6</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F7" class="codetype"><span class="SCde"><span class="SCst">VK_F7</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F8" class="codetype"><span class="SCde"><span class="SCst">VK_F8</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F9" class="codetype"><span class="SCde"><span class="SCst">VK_F9</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_FINAL" class="codetype"><span class="SCde"><span class="SCst">VK_FINAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_A" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_A</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_B" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_B</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_DOWN" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_DPAD_DOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_LEFT" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_DPAD_LEFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_RIGHT" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_DPAD_RIGHT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_UP" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_DPAD_UP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_SHOULDER" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_LEFT_SHOULDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_DOWN" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_LEFT_THUMBSTICK_DOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_LEFT" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_LEFT_THUMBSTICK_LEFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_UP" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_LEFT_THUMBSTICK_UP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_TRIGGER" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_LEFT_TRIGGER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_MENU" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_MENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_SHOULDER" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_RIGHT_SHOULDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_UP" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_RIGHT_THUMBSTICK_UP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_TRIGGER" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_RIGHT_TRIGGER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_VIEW" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_VIEW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_X" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_X</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_Y" class="codetype"><span class="SCde"><span class="SCst">VK_GAMEPAD_Y</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HANGEUL" class="codetype"><span class="SCde"><span class="SCst">VK_HANGEUL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HANGUL" class="codetype"><span class="SCde"><span class="SCst">VK_HANGUL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HANJA" class="codetype"><span class="SCde"><span class="SCst">VK_HANJA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HELP" class="codetype"><span class="SCde"><span class="SCst">VK_HELP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HOME" class="codetype"><span class="SCde"><span class="SCst">VK_HOME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ICO_00" class="codetype"><span class="SCde"><span class="SCst">VK_ICO_00</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ICO_CLEAR" class="codetype"><span class="SCde"><span class="SCst">VK_ICO_CLEAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ICO_HELP" class="codetype"><span class="SCde"><span class="SCst">VK_ICO_HELP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_INSERT" class="codetype"><span class="SCde"><span class="SCst">VK_INSERT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_JUNJA" class="codetype"><span class="SCde"><span class="SCst">VK_JUNJA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_KANA" class="codetype"><span class="SCde"><span class="SCst">VK_KANA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_KANJI" class="codetype"><span class="SCde"><span class="SCst">VK_KANJI</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_APP1" class="codetype"><span class="SCde"><span class="SCst">VK_LAUNCH_APP1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_APP2" class="codetype"><span class="SCde"><span class="SCst">VK_LAUNCH_APP2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_MAIL" class="codetype"><span class="SCde"><span class="SCst">VK_LAUNCH_MAIL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_MEDIA_SELECT" class="codetype"><span class="SCde"><span class="SCst">VK_LAUNCH_MEDIA_SELECT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LBUTTON" class="codetype"><span class="SCde"><span class="SCst">VK_LBUTTON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LCONTROL" class="codetype"><span class="SCde"><span class="SCst">VK_LCONTROL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LEFT" class="codetype"><span class="SCde"><span class="SCst">VK_LEFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LMENU" class="codetype"><span class="SCde"><span class="SCst">VK_LMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LSHIFT" class="codetype"><span class="SCde"><span class="SCst">VK_LSHIFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LWIN" class="codetype"><span class="SCde"><span class="SCst">VK_LWIN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MBUTTON" class="codetype"><span class="SCde"><span class="SCst">VK_MBUTTON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_NEXT_TRACK" class="codetype"><span class="SCde"><span class="SCst">VK_MEDIA_NEXT_TRACK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_PLAY_PAUSE" class="codetype"><span class="SCde"><span class="SCst">VK_MEDIA_PLAY_PAUSE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_PREV_TRACK" class="codetype"><span class="SCde"><span class="SCst">VK_MEDIA_PREV_TRACK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_STOP" class="codetype"><span class="SCde"><span class="SCst">VK_MEDIA_STOP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MENU" class="codetype"><span class="SCde"><span class="SCst">VK_MENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MODECHANGE" class="codetype"><span class="SCde"><span class="SCst">VK_MODECHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MULTIPLY" class="codetype"><span class="SCde"><span class="SCst">VK_MULTIPLY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_ACCEPT" class="codetype"><span class="SCde"><span class="SCst">VK_NAVIGATION_ACCEPT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_CANCEL" class="codetype"><span class="SCde"><span class="SCst">VK_NAVIGATION_CANCEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_DOWN" class="codetype"><span class="SCde"><span class="SCst">VK_NAVIGATION_DOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_LEFT" class="codetype"><span class="SCde"><span class="SCst">VK_NAVIGATION_LEFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_MENU" class="codetype"><span class="SCde"><span class="SCst">VK_NAVIGATION_MENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_RIGHT" class="codetype"><span class="SCde"><span class="SCst">VK_NAVIGATION_RIGHT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_UP" class="codetype"><span class="SCde"><span class="SCst">VK_NAVIGATION_UP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_VIEW" class="codetype"><span class="SCde"><span class="SCst">VK_NAVIGATION_VIEW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NEXT" class="codetype"><span class="SCde"><span class="SCst">VK_NEXT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NONAME" class="codetype"><span class="SCde"><span class="SCst">VK_NONAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NONCONVERT" class="codetype"><span class="SCde"><span class="SCst">VK_NONCONVERT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMLOCK" class="codetype"><span class="SCde"><span class="SCst">VK_NUMLOCK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD0" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD0</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD1" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD2" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD3" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD3</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD4" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD4</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD5" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD5</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD6" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD6</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD7" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD7</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD8" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD8</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD9" class="codetype"><span class="SCde"><span class="SCst">VK_NUMPAD9</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_1" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_102" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_102</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_2" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_3" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_3</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_4" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_4</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_5" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_5</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_6" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_6</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_7" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_7</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_8" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_8</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_ATTN" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_ATTN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_AUTO" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_AUTO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_AX" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_AX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_BACKTAB" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_BACKTAB</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_CLEAR" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_CLEAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_COMMA" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_COMMA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_COPY" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_COPY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_CUSEL" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_CUSEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_ENLW" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_ENLW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FINISH" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_FINISH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_JISHO" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_FJ_JISHO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_LOYA" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_FJ_LOYA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_MASSHOU" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_FJ_MASSHOU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_ROYA" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_FJ_ROYA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_TOUROKU" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_FJ_TOUROKU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_JUMP" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_JUMP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_MINUS" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_MINUS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_NEC_EQUAL" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_NEC_EQUAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA1" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_PA1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA2" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_PA2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA3" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_PA3</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PERIOD" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_PERIOD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PLUS" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_PLUS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_RESET" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_RESET</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_WSCTRL" class="codetype"><span class="SCde"><span class="SCst">VK_OEM_WSCTRL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PA1" class="codetype"><span class="SCde"><span class="SCst">VK_PA1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PACKET" class="codetype"><span class="SCde"><span class="SCst">VK_PACKET</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PAUSE" class="codetype"><span class="SCde"><span class="SCst">VK_PAUSE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PLAY" class="codetype"><span class="SCde"><span class="SCst">VK_PLAY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PRINT" class="codetype"><span class="SCde"><span class="SCst">VK_PRINT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PRIOR" class="codetype"><span class="SCde"><span class="SCst">VK_PRIOR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PROCESSKEY" class="codetype"><span class="SCde"><span class="SCst">VK_PROCESSKEY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RBUTTON" class="codetype"><span class="SCde"><span class="SCst">VK_RBUTTON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RCONTROL" class="codetype"><span class="SCde"><span class="SCst">VK_RCONTROL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RETURN" class="codetype"><span class="SCde"><span class="SCst">VK_RETURN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RIGHT" class="codetype"><span class="SCde"><span class="SCst">VK_RIGHT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RMENU" class="codetype"><span class="SCde"><span class="SCst">VK_RMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RSHIFT" class="codetype"><span class="SCde"><span class="SCst">VK_RSHIFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RWIN" class="codetype"><span class="SCde"><span class="SCst">VK_RWIN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SCROLL" class="codetype"><span class="SCde"><span class="SCst">VK_SCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SELECT" class="codetype"><span class="SCde"><span class="SCst">VK_SELECT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SEPARATOR" class="codetype"><span class="SCde"><span class="SCst">VK_SEPARATOR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SHIFT" class="codetype"><span class="SCde"><span class="SCst">VK_SHIFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SLEEP" class="codetype"><span class="SCde"><span class="SCst">VK_SLEEP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SNAPSHOT" class="codetype"><span class="SCde"><span class="SCst">VK_SNAPSHOT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SPACE" class="codetype"><span class="SCde"><span class="SCst">VK_SPACE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SUBTRACT" class="codetype"><span class="SCde"><span class="SCst">VK_SUBTRACT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_TAB" class="codetype"><span class="SCde"><span class="SCst">VK_TAB</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_UP" class="codetype"><span class="SCde"><span class="SCst">VK_UP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_DOWN" class="codetype"><span class="SCde"><span class="SCst">VK_VOLUME_DOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_MUTE" class="codetype"><span class="SCde"><span class="SCst">VK_VOLUME_MUTE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_UP" class="codetype"><span class="SCde"><span class="SCst">VK_VOLUME_UP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_XBUTTON1" class="codetype"><span class="SCde"><span class="SCst">VK_XBUTTON1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_XBUTTON2" class="codetype"><span class="SCde"><span class="SCst">VK_XBUTTON2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ZOOM" class="codetype"><span class="SCde"><span class="SCst">VK_ZOOM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_ABANDONED" class="codetype"><span class="SCde"><span class="SCst">WAIT_ABANDONED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_FAILED" class="codetype"><span class="SCde"><span class="SCst">WAIT_FAILED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_OBJECT_0" class="codetype"><span class="SCde"><span class="SCst">WAIT_OBJECT_0</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_TIMEOUT" class="codetype"><span class="SCde"><span class="SCst">WAIT_TIMEOUT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WA_ACTIVE" class="codetype"><span class="SCde"><span class="SCst">WA_ACTIVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WA_CLICKACTIVE" class="codetype"><span class="SCde"><span class="SCst">WA_CLICKACTIVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WA_INACTIVE" class="codetype"><span class="SCde"><span class="SCst">WA_INACTIVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WHEEL_DELTA" class="codetype"><span class="SCde"><span class="SCst">WHEEL_DELTA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ACTIVATE" class="codetype"><span class="SCde"><span class="SCst">WM_ACTIVATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ACTIVATEAPP" class="codetype"><span class="SCde"><span class="SCst">WM_ACTIVATEAPP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_AFXFIRST" class="codetype"><span class="SCde"><span class="SCst">WM_AFXFIRST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_AFXLAST" class="codetype"><span class="SCde"><span class="SCst">WM_AFXLAST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_APP" class="codetype"><span class="SCde"><span class="SCst">WM_APP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_APPCOMMAND" class="codetype"><span class="SCde"><span class="SCst">WM_APPCOMMAND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ASKCBFORMATNAME" class="codetype"><span class="SCde"><span class="SCst">WM_ASKCBFORMATNAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CANCELJOURNAL" class="codetype"><span class="SCde"><span class="SCst">WM_CANCELJOURNAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CANCELMODE" class="codetype"><span class="SCde"><span class="SCst">WM_CANCELMODE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CAPTURECHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_CAPTURECHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHANGECBCHAIN" class="codetype"><span class="SCde"><span class="SCst">WM_CHANGECBCHAIN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHANGEUISTATE" class="codetype"><span class="SCde"><span class="SCst">WM_CHANGEUISTATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHAR" class="codetype"><span class="SCde"><span class="SCst">WM_CHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHARTOITEM" class="codetype"><span class="SCde"><span class="SCst">WM_CHARTOITEM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHILDACTIVATE" class="codetype"><span class="SCde"><span class="SCst">WM_CHILDACTIVATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CLEAR" class="codetype"><span class="SCde"><span class="SCst">WM_CLEAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CLIPBOARDUPDATE" class="codetype"><span class="SCde"><span class="SCst">WM_CLIPBOARDUPDATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CLOSE" class="codetype"><span class="SCde"><span class="SCst">WM_CLOSE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMMAND" class="codetype"><span class="SCde"><span class="SCst">WM_COMMAND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMMNOTIFY" class="codetype"><span class="SCde"><span class="SCst">WM_COMMNOTIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMPACTING" class="codetype"><span class="SCde"><span class="SCst">WM_COMPACTING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMPAREITEM" class="codetype"><span class="SCde"><span class="SCst">WM_COMPAREITEM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CONTEXTMENU" class="codetype"><span class="SCde"><span class="SCst">WM_CONTEXTMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COPY" class="codetype"><span class="SCde"><span class="SCst">WM_COPY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COPYDATA" class="codetype"><span class="SCde"><span class="SCst">WM_COPYDATA</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CREATE" class="codetype"><span class="SCde"><span class="SCst">WM_CREATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORBTN" class="codetype"><span class="SCde"><span class="SCst">WM_CTLCOLORBTN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORDLG" class="codetype"><span class="SCde"><span class="SCst">WM_CTLCOLORDLG</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLOREDIT" class="codetype"><span class="SCde"><span class="SCst">WM_CTLCOLOREDIT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORLISTBOX" class="codetype"><span class="SCde"><span class="SCst">WM_CTLCOLORLISTBOX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORMSGBOX" class="codetype"><span class="SCde"><span class="SCst">WM_CTLCOLORMSGBOX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORSCROLLBAR" class="codetype"><span class="SCde"><span class="SCst">WM_CTLCOLORSCROLLBAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORSTATIC" class="codetype"><span class="SCde"><span class="SCst">WM_CTLCOLORSTATIC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CUT" class="codetype"><span class="SCde"><span class="SCst">WM_CUT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DEADCHAR" class="codetype"><span class="SCde"><span class="SCst">WM_DEADCHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DELETEITEM" class="codetype"><span class="SCde"><span class="SCst">WM_DELETEITEM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DESTROY" class="codetype"><span class="SCde"><span class="SCst">WM_DESTROY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DESTROYCLIPBOARD" class="codetype"><span class="SCde"><span class="SCst">WM_DESTROYCLIPBOARD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DEVICECHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_DEVICECHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DEVMODECHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_DEVMODECHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DISPLAYCHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_DISPLAYCHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_DPICHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED_AFTERPARENT" class="codetype"><span class="SCde"><span class="SCst">WM_DPICHANGED_AFTERPARENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED_BEFOREPARENT" class="codetype"><span class="SCde"><span class="SCst">WM_DPICHANGED_BEFOREPARENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DRAWCLIPBOARD" class="codetype"><span class="SCde"><span class="SCst">WM_DRAWCLIPBOARD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DRAWITEM" class="codetype"><span class="SCde"><span class="SCst">WM_DRAWITEM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DROPFILES" class="codetype"><span class="SCde"><span class="SCst">WM_DROPFILES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMCOLORIZATIONCOLORCHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_DWMCOLORIZATIONCOLORCHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMCOMPOSITIONCHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_DWMCOMPOSITIONCHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMNCRENDERINGCHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_DWMNCRENDERINGCHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMSENDICONICLIVEPREVIEWBITMAP" class="codetype"><span class="SCde"><span class="SCst">WM_DWMSENDICONICLIVEPREVIEWBITMAP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMSENDICONICTHUMBNAIL" class="codetype"><span class="SCde"><span class="SCst">WM_DWMSENDICONICTHUMBNAIL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMWINDOWMAXIMIZEDCHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_DWMWINDOWMAXIMIZEDCHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENABLE" class="codetype"><span class="SCde"><span class="SCst">WM_ENABLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENTERIDLE" class="codetype"><span class="SCde"><span class="SCst">WM_ENTERIDLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENTERMENULOOP" class="codetype"><span class="SCde"><span class="SCst">WM_ENTERMENULOOP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENTERSIZEMOVE" class="codetype"><span class="SCde"><span class="SCst">WM_ENTERSIZEMOVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ERASEBKGND" class="codetype"><span class="SCde"><span class="SCst">WM_ERASEBKGND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_EXITMENULOOP" class="codetype"><span class="SCde"><span class="SCst">WM_EXITMENULOOP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_EXITSIZEMOVE" class="codetype"><span class="SCde"><span class="SCst">WM_EXITSIZEMOVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_FONTCHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_FONTCHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GESTURE" class="codetype"><span class="SCde"><span class="SCst">WM_GESTURE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GESTURENOTIFY" class="codetype"><span class="SCde"><span class="SCst">WM_GESTURENOTIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETDLGCODE" class="codetype"><span class="SCde"><span class="SCst">WM_GETDLGCODE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETDPISCALEDSIZE" class="codetype"><span class="SCde"><span class="SCst">WM_GETDPISCALEDSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETFONT" class="codetype"><span class="SCde"><span class="SCst">WM_GETFONT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETHOTKEY" class="codetype"><span class="SCde"><span class="SCst">WM_GETHOTKEY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETICON" class="codetype"><span class="SCde"><span class="SCst">WM_GETICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETMINMAXINFO" class="codetype"><span class="SCde"><span class="SCst">WM_GETMINMAXINFO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETOBJECT" class="codetype"><span class="SCde"><span class="SCst">WM_GETOBJECT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETTEXT" class="codetype"><span class="SCde"><span class="SCst">WM_GETTEXT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETTEXTLENGTH" class="codetype"><span class="SCde"><span class="SCst">WM_GETTEXTLENGTH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETTITLEBARINFOEX" class="codetype"><span class="SCde"><span class="SCst">WM_GETTITLEBARINFOEX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HANDHELDFIRST" class="codetype"><span class="SCde"><span class="SCst">WM_HANDHELDFIRST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HANDHELDLAST" class="codetype"><span class="SCde"><span class="SCst">WM_HANDHELDLAST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HELP" class="codetype"><span class="SCde"><span class="SCst">WM_HELP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HOTKEY" class="codetype"><span class="SCde"><span class="SCst">WM_HOTKEY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HSCROLL" class="codetype"><span class="SCde"><span class="SCst">WM_HSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HSCROLLCLIPBOARD" class="codetype"><span class="SCde"><span class="SCst">WM_HSCROLLCLIPBOARD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ICONERASEBKGND" class="codetype"><span class="SCde"><span class="SCst">WM_ICONERASEBKGND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_CHAR" class="codetype"><span class="SCde"><span class="SCst">WM_IME_CHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_COMPOSITION" class="codetype"><span class="SCde"><span class="SCst">WM_IME_COMPOSITION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_COMPOSITIONFULL" class="codetype"><span class="SCde"><span class="SCst">WM_IME_COMPOSITIONFULL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_CONTROL" class="codetype"><span class="SCde"><span class="SCst">WM_IME_CONTROL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_ENDCOMPOSITION" class="codetype"><span class="SCde"><span class="SCst">WM_IME_ENDCOMPOSITION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_IME_KEYDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYLAST" class="codetype"><span class="SCde"><span class="SCst">WM_IME_KEYLAST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYUP" class="codetype"><span class="SCde"><span class="SCst">WM_IME_KEYUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_NOTIFY" class="codetype"><span class="SCde"><span class="SCst">WM_IME_NOTIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_REQUEST" class="codetype"><span class="SCde"><span class="SCst">WM_IME_REQUEST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_SELECT" class="codetype"><span class="SCde"><span class="SCst">WM_IME_SELECT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_SETCONTEXT" class="codetype"><span class="SCde"><span class="SCst">WM_IME_SETCONTEXT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_STARTCOMPOSITION" class="codetype"><span class="SCde"><span class="SCst">WM_IME_STARTCOMPOSITION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INITDIALOG" class="codetype"><span class="SCde"><span class="SCst">WM_INITDIALOG</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INITMENU" class="codetype"><span class="SCde"><span class="SCst">WM_INITMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INITMENUPOPUP" class="codetype"><span class="SCde"><span class="SCst">WM_INITMENUPOPUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUT" class="codetype"><span class="SCde"><span class="SCst">WM_INPUT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUTLANGCHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_INPUTLANGCHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUTLANGCHANGEREQUEST" class="codetype"><span class="SCde"><span class="SCst">WM_INPUTLANGCHANGEREQUEST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUT_DEVICE_CHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_INPUT_DEVICE_CHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_KEYDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYFIRST" class="codetype"><span class="SCde"><span class="SCst">WM_KEYFIRST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYLAST" class="codetype"><span class="SCde"><span class="SCst">WM_KEYLAST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYUP" class="codetype"><span class="SCde"><span class="SCst">WM_KEYUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KILLFOCUS" class="codetype"><span class="SCde"><span class="SCst">WM_KILLFOCUS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONDBLCLK" class="codetype"><span class="SCde"><span class="SCst">WM_LBUTTONDBLCLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_LBUTTONDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONUP" class="codetype"><span class="SCde"><span class="SCst">WM_LBUTTONUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONDBLCLK" class="codetype"><span class="SCde"><span class="SCst">WM_MBUTTONDBLCLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_MBUTTONDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONUP" class="codetype"><span class="SCde"><span class="SCst">WM_MBUTTONUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIACTIVATE" class="codetype"><span class="SCde"><span class="SCst">WM_MDIACTIVATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDICASCADE" class="codetype"><span class="SCde"><span class="SCst">WM_MDICASCADE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDICREATE" class="codetype"><span class="SCde"><span class="SCst">WM_MDICREATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIDESTROY" class="codetype"><span class="SCde"><span class="SCst">WM_MDIDESTROY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIGETACTIVE" class="codetype"><span class="SCde"><span class="SCst">WM_MDIGETACTIVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIICONARRANGE" class="codetype"><span class="SCde"><span class="SCst">WM_MDIICONARRANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIMAXIMIZE" class="codetype"><span class="SCde"><span class="SCst">WM_MDIMAXIMIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDINEXT" class="codetype"><span class="SCde"><span class="SCst">WM_MDINEXT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIREFRESHMENU" class="codetype"><span class="SCde"><span class="SCst">WM_MDIREFRESHMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIRESTORE" class="codetype"><span class="SCde"><span class="SCst">WM_MDIRESTORE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDISETMENU" class="codetype"><span class="SCde"><span class="SCst">WM_MDISETMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDITILE" class="codetype"><span class="SCde"><span class="SCst">WM_MDITILE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MEASUREITEM" class="codetype"><span class="SCde"><span class="SCst">WM_MEASUREITEM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUCHAR" class="codetype"><span class="SCde"><span class="SCst">WM_MENUCHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUCOMMAND" class="codetype"><span class="SCde"><span class="SCst">WM_MENUCOMMAND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUDRAG" class="codetype"><span class="SCde"><span class="SCst">WM_MENUDRAG</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUGETOBJECT" class="codetype"><span class="SCde"><span class="SCst">WM_MENUGETOBJECT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENURBUTTONUP" class="codetype"><span class="SCde"><span class="SCst">WM_MENURBUTTONUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUSELECT" class="codetype"><span class="SCde"><span class="SCst">WM_MENUSELECT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEACTIVATE" class="codetype"><span class="SCde"><span class="SCst">WM_MOUSEACTIVATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEFIRST" class="codetype"><span class="SCde"><span class="SCst">WM_MOUSEFIRST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEHOVER" class="codetype"><span class="SCde"><span class="SCst">WM_MOUSEHOVER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEHWHEEL" class="codetype"><span class="SCde"><span class="SCst">WM_MOUSEHWHEEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSELAST" class="codetype"><span class="SCde"><span class="SCst">WM_MOUSELAST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSELEAVE" class="codetype"><span class="SCde"><span class="SCst">WM_MOUSELEAVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEMOVE" class="codetype"><span class="SCde"><span class="SCst">WM_MOUSEMOVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEWHEEL" class="codetype"><span class="SCde"><span class="SCst">WM_MOUSEWHEEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOVE" class="codetype"><span class="SCde"><span class="SCst">WM_MOVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOVING" class="codetype"><span class="SCde"><span class="SCst">WM_MOVING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCACTIVATE" class="codetype"><span class="SCde"><span class="SCst">WM_NCACTIVATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCCALCSIZE" class="codetype"><span class="SCde"><span class="SCst">WM_NCCALCSIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCCREATE" class="codetype"><span class="SCde"><span class="SCst">WM_NCCREATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCDESTROY" class="codetype"><span class="SCde"><span class="SCst">WM_NCDESTROY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCHITTEST" class="codetype"><span class="SCde"><span class="SCst">WM_NCHITTEST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONDBLCLK" class="codetype"><span class="SCde"><span class="SCst">WM_NCLBUTTONDBLCLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_NCLBUTTONDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONUP" class="codetype"><span class="SCde"><span class="SCst">WM_NCLBUTTONUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONDBLCLK" class="codetype"><span class="SCde"><span class="SCst">WM_NCMBUTTONDBLCLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_NCMBUTTONDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONUP" class="codetype"><span class="SCde"><span class="SCst">WM_NCMBUTTONUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSEHOVER" class="codetype"><span class="SCde"><span class="SCst">WM_NCMOUSEHOVER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSELEAVE" class="codetype"><span class="SCde"><span class="SCst">WM_NCMOUSELEAVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSEMOVE" class="codetype"><span class="SCde"><span class="SCst">WM_NCMOUSEMOVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPAINT" class="codetype"><span class="SCde"><span class="SCst">WM_NCPAINT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_NCPOINTERDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERUP" class="codetype"><span class="SCde"><span class="SCst">WM_NCPOINTERUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERUPDATE" class="codetype"><span class="SCde"><span class="SCst">WM_NCPOINTERUPDATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONDBLCLK" class="codetype"><span class="SCde"><span class="SCst">WM_NCRBUTTONDBLCLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_NCRBUTTONDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONUP" class="codetype"><span class="SCde"><span class="SCst">WM_NCRBUTTONUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONDBLCLK" class="codetype"><span class="SCde"><span class="SCst">WM_NCXBUTTONDBLCLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_NCXBUTTONDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONUP" class="codetype"><span class="SCde"><span class="SCst">WM_NCXBUTTONUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NEXTDLGCTL" class="codetype"><span class="SCde"><span class="SCst">WM_NEXTDLGCTL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NEXTMENU" class="codetype"><span class="SCde"><span class="SCst">WM_NEXTMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NOTIFY" class="codetype"><span class="SCde"><span class="SCst">WM_NOTIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NOTIFYFORMAT" class="codetype"><span class="SCde"><span class="SCst">WM_NOTIFYFORMAT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PAINT" class="codetype"><span class="SCde"><span class="SCst">WM_PAINT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PAINTCLIPBOARD" class="codetype"><span class="SCde"><span class="SCst">WM_PAINTCLIPBOARD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PAINTICON" class="codetype"><span class="SCde"><span class="SCst">WM_PAINTICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PALETTECHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_PALETTECHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PALETTEISCHANGING" class="codetype"><span class="SCde"><span class="SCst">WM_PALETTEISCHANGING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PARENTNOTIFY" class="codetype"><span class="SCde"><span class="SCst">WM_PARENTNOTIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PASTE" class="codetype"><span class="SCde"><span class="SCst">WM_PASTE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PENWINFIRST" class="codetype"><span class="SCde"><span class="SCst">WM_PENWINFIRST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PENWINLAST" class="codetype"><span class="SCde"><span class="SCst">WM_PENWINLAST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERACTIVATE" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERACTIVATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERCAPTURECHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERCAPTURECHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICECHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERDEVICECHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICEINRANGE" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERDEVICEINRANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICEOUTOFRANGE" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERDEVICEOUTOFRANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERENTER" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERENTER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERHWHEEL" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERHWHEEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERLEAVE" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERLEAVE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDAWAY" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERROUTEDAWAY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDRELEASED" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERROUTEDRELEASED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDTO" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERROUTEDTO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERUP" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERUPDATE" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERUPDATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERWHEEL" class="codetype"><span class="SCde"><span class="SCst">WM_POINTERWHEEL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POWER" class="codetype"><span class="SCde"><span class="SCst">WM_POWER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POWERBROADCAST" class="codetype"><span class="SCde"><span class="SCst">WM_POWERBROADCAST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PRINT" class="codetype"><span class="SCde"><span class="SCst">WM_PRINT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PRINTCLIENT" class="codetype"><span class="SCde"><span class="SCst">WM_PRINTCLIENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUERYDRAGICON" class="codetype"><span class="SCde"><span class="SCst">WM_QUERYDRAGICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUERYNEWPALETTE" class="codetype"><span class="SCde"><span class="SCst">WM_QUERYNEWPALETTE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUERYUISTATE" class="codetype"><span class="SCde"><span class="SCst">WM_QUERYUISTATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUEUESYNC" class="codetype"><span class="SCde"><span class="SCst">WM_QUEUESYNC</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUIT" class="codetype"><span class="SCde"><span class="SCst">WM_QUIT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONDBLCLK" class="codetype"><span class="SCde"><span class="SCst">WM_RBUTTONDBLCLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_RBUTTONDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONUP" class="codetype"><span class="SCde"><span class="SCst">WM_RBUTTONUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RENDERALLFORMATS" class="codetype"><span class="SCde"><span class="SCst">WM_RENDERALLFORMATS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RENDERFORMAT" class="codetype"><span class="SCde"><span class="SCst">WM_RENDERFORMAT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETCURSOR" class="codetype"><span class="SCde"><span class="SCst">WM_SETCURSOR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETFOCUS" class="codetype"><span class="SCde"><span class="SCst">WM_SETFOCUS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETFONT" class="codetype"><span class="SCde"><span class="SCst">WM_SETFONT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETHOTKEY" class="codetype"><span class="SCde"><span class="SCst">WM_SETHOTKEY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETICON" class="codetype"><span class="SCde"><span class="SCst">WM_SETICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETREDRAW" class="codetype"><span class="SCde"><span class="SCst">WM_SETREDRAW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETTEXT" class="codetype"><span class="SCde"><span class="SCst">WM_SETTEXT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETTINGCHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_SETTINGCHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SHOWWINDOW" class="codetype"><span class="SCde"><span class="SCst">WM_SHOWWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SIZE" class="codetype"><span class="SCde"><span class="SCst">WM_SIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SIZECLIPBOARD" class="codetype"><span class="SCde"><span class="SCst">WM_SIZECLIPBOARD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SIZING" class="codetype"><span class="SCde"><span class="SCst">WM_SIZING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SPOOLERSTATUS" class="codetype"><span class="SCde"><span class="SCst">WM_SPOOLERSTATUS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_STYLECHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_STYLECHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_STYLECHANGING" class="codetype"><span class="SCde"><span class="SCst">WM_STYLECHANGING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYNCPAINT" class="codetype"><span class="SCde"><span class="SCst">WM_SYNCPAINT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSCHAR" class="codetype"><span class="SCde"><span class="SCst">WM_SYSCHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSCOLORCHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_SYSCOLORCHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSCOMMAND" class="codetype"><span class="SCde"><span class="SCst">WM_SYSCOMMAND</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSDEADCHAR" class="codetype"><span class="SCde"><span class="SCst">WM_SYSDEADCHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSKEYDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_SYSKEYDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSKEYUP" class="codetype"><span class="SCde"><span class="SCst">WM_SYSKEYUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TABLET_FIRST" class="codetype"><span class="SCde"><span class="SCst">WM_TABLET_FIRST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TABLET_LAST" class="codetype"><span class="SCde"><span class="SCst">WM_TABLET_LAST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TCARD" class="codetype"><span class="SCde"><span class="SCst">WM_TCARD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_THEMECHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_THEMECHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TIMECHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_TIMECHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TIMER" class="codetype"><span class="SCde"><span class="SCst">WM_TIMER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TOUCH" class="codetype"><span class="SCde"><span class="SCst">WM_TOUCH</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TOUCHHITTESTING" class="codetype"><span class="SCde"><span class="SCst">WM_TOUCHHITTESTING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UNDO" class="codetype"><span class="SCde"><span class="SCst">WM_UNDO</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UNICHAR" class="codetype"><span class="SCde"><span class="SCst">WM_UNICHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UNINITMENUPOPUP" class="codetype"><span class="SCde"><span class="SCst">WM_UNINITMENUPOPUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UPDATEUISTATE" class="codetype"><span class="SCde"><span class="SCst">WM_UPDATEUISTATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_USER" class="codetype"><span class="SCde"><span class="SCst">WM_USER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_USERCHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_USERCHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_VKEYTOITEM" class="codetype"><span class="SCde"><span class="SCst">WM_VKEYTOITEM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_VSCROLL" class="codetype"><span class="SCde"><span class="SCst">WM_VSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_VSCROLLCLIPBOARD" class="codetype"><span class="SCde"><span class="SCst">WM_VSCROLLCLIPBOARD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WINDOWPOSCHANGED" class="codetype"><span class="SCde"><span class="SCst">WM_WINDOWPOSCHANGED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WINDOWPOSCHANGING" class="codetype"><span class="SCde"><span class="SCst">WM_WINDOWPOSCHANGING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WININICHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_WININICHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WTSSESSION_CHANGE" class="codetype"><span class="SCde"><span class="SCst">WM_WTSSESSION_CHANGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONDBLCLK" class="codetype"><span class="SCde"><span class="SCst">WM_XBUTTONDBLCLK</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONDOWN" class="codetype"><span class="SCde"><span class="SCst">WM_XBUTTONDOWN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONUP" class="codetype"><span class="SCde"><span class="SCst">WM_XBUTTONUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_BORDER" class="codetype"><span class="SCde"><span class="SCst">WS_BORDER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CAPTION" class="codetype"><span class="SCde"><span class="SCst">WS_CAPTION</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CHILD" class="codetype"><span class="SCde"><span class="SCst">WS_CHILD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CLIPCHILDREN" class="codetype"><span class="SCde"><span class="SCst">WS_CLIPCHILDREN</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CLIPSIBLINGS" class="codetype"><span class="SCde"><span class="SCst">WS_CLIPSIBLINGS</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_DISABLED" class="codetype"><span class="SCde"><span class="SCst">WS_DISABLED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_DLGFRAME" class="codetype"><span class="SCde"><span class="SCst">WS_DLGFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_ACCEPTFILES" class="codetype"><span class="SCde"><span class="SCst">WS_EX_ACCEPTFILES</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_APPWINDOW" class="codetype"><span class="SCde"><span class="SCst">WS_EX_APPWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_CLIENTEDGE" class="codetype"><span class="SCde"><span class="SCst">WS_EX_CLIENTEDGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_COMPOSITED" class="codetype"><span class="SCde"><span class="SCst">WS_EX_COMPOSITED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_CONTEXTHELP" class="codetype"><span class="SCde"><span class="SCst">WS_EX_CONTEXTHELP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_CONTROLPARENT" class="codetype"><span class="SCde"><span class="SCst">WS_EX_CONTROLPARENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_DLGMODALFRAME" class="codetype"><span class="SCde"><span class="SCst">WS_EX_DLGMODALFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LAYERED" class="codetype"><span class="SCde"><span class="SCst">WS_EX_LAYERED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LAYOUTRTL" class="codetype"><span class="SCde"><span class="SCst">WS_EX_LAYOUTRTL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LEFT" class="codetype"><span class="SCde"><span class="SCst">WS_EX_LEFT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LEFTSCROLLBAR" class="codetype"><span class="SCde"><span class="SCst">WS_EX_LEFTSCROLLBAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LTRREADING" class="codetype"><span class="SCde"><span class="SCst">WS_EX_LTRREADING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_MDICHILD" class="codetype"><span class="SCde"><span class="SCst">WS_EX_MDICHILD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOACTIVATE" class="codetype"><span class="SCde"><span class="SCst">WS_EX_NOACTIVATE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOINHERITLAYOUT" class="codetype"><span class="SCde"><span class="SCst">WS_EX_NOINHERITLAYOUT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOPARENTNOTIFY" class="codetype"><span class="SCde"><span class="SCst">WS_EX_NOPARENTNOTIFY</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOREDIRECTIONBITMAP" class="codetype"><span class="SCde"><span class="SCst">WS_EX_NOREDIRECTIONBITMAP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_OVERLAPPEDWINDOW" class="codetype"><span class="SCde"><span class="SCst">WS_EX_OVERLAPPEDWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_PALETTEWINDOW" class="codetype"><span class="SCde"><span class="SCst">WS_EX_PALETTEWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_RIGHT" class="codetype"><span class="SCde"><span class="SCst">WS_EX_RIGHT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_RIGHTSCROLLBAR" class="codetype"><span class="SCde"><span class="SCst">WS_EX_RIGHTSCROLLBAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_RTLREADING" class="codetype"><span class="SCde"><span class="SCst">WS_EX_RTLREADING</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_STATICEDGE" class="codetype"><span class="SCde"><span class="SCst">WS_EX_STATICEDGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_TOOLWINDOW" class="codetype"><span class="SCde"><span class="SCst">WS_EX_TOOLWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_TOPMOST" class="codetype"><span class="SCde"><span class="SCst">WS_EX_TOPMOST</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_TRANSPARENT" class="codetype"><span class="SCde"><span class="SCst">WS_EX_TRANSPARENT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_WINDOWEDGE" class="codetype"><span class="SCde"><span class="SCst">WS_EX_WINDOWEDGE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_GROUP" class="codetype"><span class="SCde"><span class="SCst">WS_GROUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_HSCROLL" class="codetype"><span class="SCde"><span class="SCst">WS_HSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MAXIMIZE" class="codetype"><span class="SCde"><span class="SCst">WS_MAXIMIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MAXIMIZEBOX" class="codetype"><span class="SCde"><span class="SCst">WS_MAXIMIZEBOX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MINIMIZE" class="codetype"><span class="SCde"><span class="SCst">WS_MINIMIZE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MINIMIZEBOX" class="codetype"><span class="SCde"><span class="SCst">WS_MINIMIZEBOX</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_OVERLAPPED" class="codetype"><span class="SCde"><span class="SCst">WS_OVERLAPPED</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_OVERLAPPEDWINDOW" class="codetype"><span class="SCde"><span class="SCst">WS_OVERLAPPEDWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_POPUP" class="codetype"><span class="SCde"><span class="SCst">WS_POPUP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_POPUPWINDOW" class="codetype"><span class="SCde"><span class="SCst">WS_POPUPWINDOW</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_SYSMENU" class="codetype"><span class="SCde"><span class="SCst">WS_SYSMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_TABSTOP" class="codetype"><span class="SCde"><span class="SCst">WS_TABSTOP</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_THICKFRAME" class="codetype"><span class="SCde"><span class="SCst">WS_THICKFRAME</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_VISIBLE" class="codetype"><span class="SCde"><span class="SCst">WS_VISIBLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_VSCROLL" class="codetype"><span class="SCde"><span class="SCst">WS_VSCROLL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<td id="Win32_ATOM" class="codetype"><span class="SCde"><span class="SCst">ATOM</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_BOOL" class="codetype"><span class="SCde"><span class="SCst">BOOL</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_BYTE" class="codetype"><span class="SCde"><span class="SCst">BYTE</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_CHAR" class="codetype"><span class="SCde"><span class="SCst">CHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_COLORREF" class="codetype"><span class="SCde"><span class="SCst">COLORREF</span></span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_DWORD" class="codetype"><span class="SCde"><span class="SCst">DWORD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_DWORD64" class="codetype"><span class="SCde"><span class="SCst">DWORD64</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_DWORD_PTR" class="codetype"><span class="SCde"><span class="SCst">DWORD_PTR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HANDLE" class="codetype"><span class="SCde"><span class="SCst">HANDLE</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HBITMAP" class="codetype"><span class="SCde"><span class="SCst">HBITMAP</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HBRUSH" class="codetype"><span class="SCde"><span class="SCst">HBRUSH</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HCURSOR" class="codetype"><span class="SCde"><span class="SCst">HCURSOR</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HDC" class="codetype"><span class="SCde"><span class="SCst">HDC</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HGLOBAL" class="codetype"><span class="SCde"><span class="SCst">HGLOBAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HICON" class="codetype"><span class="SCde"><span class="SCst">HICON</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HINSTANCE" class="codetype"><span class="SCde"><span class="SCst">HINSTANCE</span></span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HLOCAL" class="codetype"><span class="SCde"><span class="SCst">HLOCAL</span></span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HMENU" class="codetype"><span class="SCde"><span class="SCst">HMENU</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HMODULE" class="codetype"><span class="SCde"><span class="SCst">HMODULE</span></span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HMONITOR" class="codetype"><span class="SCde"><span class="SCst">HMONITOR</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HRESULT" class="codetype"><span class="SCde"><span class="SCst">HRESULT</span></span></td>
<td class="codetype"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND" class="codetype"><span class="SCde"><span class="SCst">HWND</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_IP_MASK_STRING" class="codetype"><span class="SCde"><span class="SCst">IP_MASK_STRING</span></span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDRESS_STRING">IP_ADDRESS_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LARGE_INTEGER" class="codetype"><span class="SCde"><span class="SCst">LARGE_INTEGER</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LONG" class="codetype"><span class="SCde"><span class="SCst">LONG</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPARAM" class="codetype"><span class="SCde"><span class="SCst">LPARAM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPBYTE" class="codetype"><span class="SCde"><span class="SCst">LPBYTE</span></span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BYTE">BYTE</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCH" class="codetype"><span class="SCde"><span class="SCst">LPCH</span></span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCSTR" class="codetype"><span class="SCde"><span class="SCst">LPCSTR</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCTSTR" class="codetype"><span class="SCde"><span class="SCst">LPCTSTR</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCVOID" class="codetype"><span class="SCde"><span class="SCst">LPCVOID</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPCWSTR" class="codetype"><span class="SCde"><span class="SCst">LPCWSTR</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPDWORD" class="codetype"><span class="SCde"><span class="SCst">LPDWORD</span></span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPOVERLAPPED" class="codetype"><span class="SCde"><span class="SCst">LPOVERLAPPED</span></span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPSTR" class="codetype"><span class="SCde"><span class="SCst">LPSTR</span></span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPTHREAD_START_ROUTINE" class="codetype"><span class="SCde"><span class="SCst">LPTHREAD_START_ROUTINE</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="STpe">void</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPVOID" class="codetype"><span class="SCde"><span class="SCst">LPVOID</span></span></td>
<td class="codetype"><span class="SCde">^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPWCH" class="codetype"><span class="SCde"><span class="SCst">LPWCH</span></span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LPWSTR" class="codetype"><span class="SCde"><span class="SCst">LPWSTR</span></span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_LRESULT" class="codetype"><span class="SCde"><span class="SCst">LRESULT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_PCSTR" class="codetype"><span class="SCde"><span class="SCst">PCSTR</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_PCWSTR" class="codetype"><span class="SCde"><span class="SCst">PCWSTR</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_PLONG" class="codetype"><span class="SCde"><span class="SCst">PLONG</span></span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LONG">LONG</a></span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHORT" class="codetype"><span class="SCde"><span class="SCst">SHORT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s16</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SHSTDAPI" class="codetype"><span class="SCde"><span class="SCst">SHSTDAPI</span></span></td>
<td class="codetype"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SIZE_T" class="codetype"><span class="SCde"><span class="SCst">SIZE_T</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_SRWLOCK" class="codetype"><span class="SCde"><span class="SCst">SRWLOCK</span></span></td>
<td class="codetype"><span class="SCde">*<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_UINT" class="codetype"><span class="SCde"><span class="SCst">UINT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_ULONG" class="codetype"><span class="SCde"><span class="SCst">ULONG</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_ULONG64" class="codetype"><span class="SCde"><span class="SCst">ULONG64</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_USHORT" class="codetype"><span class="SCde"><span class="SCst">USHORT</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WAITORTIMERCALLBACK" class="codetype"><span class="SCde"><span class="SCst">WAITORTIMERCALLBACK</span></span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">func</span>(*<span class="STpe">void</span>, <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>)</span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WCHAR" class="codetype"><span class="SCde"><span class="SCst">WCHAR</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WORD" class="codetype"><span class="SCde"><span class="SCst">WORD</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_WPARAM" class="codetype"><span class="SCde"><span class="SCst">WPARAM</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_time_t" class="codetype"><span class="SCde">time_t</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td id="Win32_va_list" class="codetype"><span class="SCde">va_list</span></td>
<td class="codetype"><span class="SCde">^<span class="STpe">u8</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">AcquireSRWLockExclusive</span>(<span class="SCst">SRWLock</span>: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SRWLOCK">SRWLOCK</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">AcquireSRWLockShared</span>(<span class="SCst">SRWLock</span>: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SRWLOCK">SRWLOCK</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">BeginPaint</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpPaint: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_PAINTSTRUCT">PAINTSTRUCT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HDC">HDC</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">BringWindowToTop</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<td class="codetype"><span class="SCde">dwSize</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_COORD">COORD</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwCursorPosition</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_COORD">COORD</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wAttributes</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">srWindow</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SMALL_RECT">SMALL_RECT</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwMaximumWindowSize</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_COORD">COORD</a></span></span></td>
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
<td class="codetype"><span class="SCde">x</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">y</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
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
<td class="codetype"><span class="SCde">lpCreateParams</span></td>
<td class="codetype"><span class="SCde">^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hInstance</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hMenu</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hwndParent</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cy</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cx</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">y</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">x</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">style</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpszName</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpszClass</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwExStyle</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<td class="codetype"><span class="SCde">lpCreateParams</span></td>
<td class="codetype"><span class="SCde">^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hInstance</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hMenu</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hwndParent</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cy</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cx</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">y</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">x</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">style</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpszName</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpszClass</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwExStyle</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CancelWaitableTimer</span>(hTimer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ChildWindowFromPoint</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, pt: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ChildWindowFromPointEx</span>(hwnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, pt: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span>, flags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ClientToScreen</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpPoint: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CloseClipboard</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CloseHandle</span>(handle: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CoInitializeEx</span>(pvReserved: *<span class="STpe">void</span>, dwCoInit: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HRESULT">HRESULT</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CoUninitialize</span>()</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateConsoleScreenBuffer</span>(dwDesiredAccess: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, dwShareMode: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpSecurityAttributes: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, dwFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpScreenBufferData: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateDirectoryA</span>(lpPathName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, lpSecurityAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateDirectoryW</span>(lpPathName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, lpSecurityAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateEventA</span>(lpEventAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, bManualReset: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, bInitialState: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, lpName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateEventW</span>(lpEventAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, bManualReset: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, bInitialState: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, lpName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateFileA</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, dwDesiredAccess: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, dwShareMode: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpSecurityAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, dwCreationDisposition: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, dwFlagsAndAttributes: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, hTemplateFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateFileW</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, dwDesiredAccess: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, dwShareMode: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpSecurityAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, dwCreationDisposition: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, dwFlagsAndAttributes: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, hTemplateFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateIconIndirect</span>(piconinfo: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_ICONINFO">ICONINFO</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HICON">HICON</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateMutexA</span>(lpMutexAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, bInitialOwner: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, lpName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateMutexW</span>(lpMutexAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, bInitialOwner: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, lpName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateProcessA</span>(lpApplicationName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, lpCommandLine: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, lpProcessAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, lpThreadAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, bInheritHandles: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, dwCreationFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpEnvironment: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span>, lpCurrentDirectory: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, lpStartupInfo: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_STARTUPINFOA">STARTUPINFOA</a></span>, lpProcessInformation: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_PROCESS_INFORMATION">PROCESS_INFORMATION</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateProcessW</span>(lpApplicationName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, lpCommandLine: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, lpProcessAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, lpThreadAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, bInheritHandles: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, dwCreationFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpEnvironment: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span>, lpCurrentDirectory: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, lpStartupInfo: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_STARTUPINFOW">STARTUPINFOW</a></span>, lpProcessInformation: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_PROCESS_INFORMATION">PROCESS_INFORMATION</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateThread</span>(lpThreadAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, dwStackSize: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SIZE_T">SIZE_T</a></span>, lpStartAddress: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPTHREAD_START_ROUTINE">LPTHREAD_START_ROUTINE</a></span>, lpParameter: *<span class="STpe">void</span>, dwCreationFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpThreadId: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateTimerQueueTimer</span>(phNewTimer: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, timerQueue: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, callback: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WAITORTIMERCALLBACK">WAITORTIMERCALLBACK</a></span>, parameter: *<span class="STpe">void</span>, dueTime: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, period: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, flags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_ULONG">ULONG</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateWaitableTimerA</span>(lpTimerAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, bManualReset: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, lpTimerName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateWaitableTimerW</span>(lpTimerAttributes: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span>, bManualReset: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, lpTimerName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateWindowExA</span>(dwExStyle: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpClassName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, lpWindowName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, dwStyle: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, <span class="SCst">X</span>: <span class="STpe">s32</span>, <span class="SCst">Y</span>: <span class="STpe">s32</span>, nWidth: <span class="STpe">s32</span>, nHeight: <span class="STpe">s32</span>, hWndParent: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, hMenu: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HMENU">HMENU</a></span>, hInstance: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HINSTANCE">HINSTANCE</a></span>, lpParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">CreateWindowExW</span>(dwExStyle: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpClassName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, lpWindowName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, dwStyle: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, <span class="SCst">X</span>: <span class="STpe">s32</span>, <span class="SCst">Y</span>: <span class="STpe">s32</span>, nWidth: <span class="STpe">s32</span>, nHeight: <span class="STpe">s32</span>, hWndParent: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, hMenu: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HMENU">HMENU</a></span>, hInstance: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HINSTANCE">HINSTANCE</a></span>, lpParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DebugActiveProcess</span>(procId: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DebugBreak</span>()</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DefWindowProcA</span>(hwnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, uMsg: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WPARAM">WPARAM</a></span>, lParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPARAM">LPARAM</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LRESULT">LRESULT</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DefWindowProcW</span>(hwnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, uMsg: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WPARAM">WPARAM</a></span>, lParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPARAM">LPARAM</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LRESULT">LRESULT</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DeleteFileA</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DeleteFileW</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DeleteTimerQueueTimer</span>(timerQueue: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, timer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, completionEvent: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DestroyIcon</span>(hIcon: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HICON">HICON</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DestroyWindow</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DispatchMessageA</span>(lpMsg: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_MSG">MSG</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LRESULT">LRESULT</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">DispatchMessageW</span>(lpMsg: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_MSG">MSG</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LRESULT">LRESULT</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">EmptyClipboard</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">EnableWindow</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, bEnable: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">EndPaint</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpPaint: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_PAINTSTRUCT">PAINTSTRUCT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">EnumDisplayMonitors</span>(hdc: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HDC">HDC</a></span>, lprcClip: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_RECT">RECT</a></span>, lpfnEnum: <span class="SKwd">const</span> *<span class="STpe">void</span>, dwData: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPARAM">LPARAM</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">EnumWindows</span>(lpEnumFunc: <span class="SKwd">const</span> *<span class="STpe">void</span>, lParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPARAM">LPARAM</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ExitProcess</span>(uExitCode: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ExtractIconExA</span>(lpszFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, nIconIndex: <span class="STpe">s32</span>, phiconLarge: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HICON">HICON</a></span>, phiconSmall: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HICON">HICON</a></span>, nIcons: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ExtractIconExW</span>(lpszFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, nIconIndex: <span class="STpe">s32</span>, phiconLarge: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HICON">HICON</a></span>, phiconSmall: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HICON">HICON</a></span>, nIcons: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span> <span class="SKwd">throw</span></span></code>
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
<td class="codetype"><span class="SCde">dwLowDateTime</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwHighDateTime</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FileTimeToLocalFileTime</span>(lpFileTime: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span>, lpLocalFileTime: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FileTimeToSystemTime</span>(lpFileTime: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span>, lpSystemTime: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FillRect</span>(hDC: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HDC">HDC</a></span>, lprc: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_RECT">RECT</a></span>, hbr: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HBRUSH">HBRUSH</a></span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindClose</span>(hFindFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindFirstFileA</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, lpFindFileData: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WIN32_FIND_DATAA">WIN32_FIND_DATAA</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindFirstFileW</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, lpFindFileData: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WIN32_FIND_DATAW">WIN32_FIND_DATAW</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindFirstVolumeA</span>(lpszVolumeName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPSTR">LPSTR</a></span>, cchBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindFirstVolumeW</span>(lpszVolumeName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPWSTR">LPWSTR</a></span>, cchBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindNextFileA</span>(hFindFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpFindFileData: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WIN32_FIND_DATAA">WIN32_FIND_DATAA</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindNextFileW</span>(hFindFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpFindFileData: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WIN32_FIND_DATAW">WIN32_FIND_DATAW</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindNextVolumeA</span>(hFindVolume: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpszVolumeName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPSTR">LPSTR</a></span>, cchBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindNextVolumeW</span>(hFindVolume: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpszVolumeName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPWSTR">LPWSTR</a></span>, cchBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindVolumeClose</span>(hFindVolume: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindWindowA</span>(lpClassName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, lpWindowName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FindWindowW</span>(lpClassName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, lpWindowName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">FormatMessageA</span>(dwFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpSource: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCVOID">LPCVOID</a></span>, dwMessageId: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, dwLanguageId: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpBuffer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, nSize: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, <span class="SCst">Arguments</span>: *<span class="SCst">Win32</span>.<a href="#Win32_va_list">va_list</a>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span></span></code>
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
<td class="codetype"><span class="SCde"><span class="SCst">GetFileExInfoStandard</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">GetFileExMaxInfoLevel</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GET_WHEEL_DELTA_WPARAM</span>(wParam: <span class="STpe">u64</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GET_X_LPARAM</span>(lp: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPARAM">LPARAM</a></span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GET_Y_LPARAM</span>(lp: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPARAM">LPARAM</a></span>)-&gt;<span class="STpe">s32</span></span></code>
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
<td class="codetype"><span class="SCde"><span class="SCst">Data1</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">Data2</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">Data3</span></span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">Data4</span></span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">8</span>] <span class="STpe">u8</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetActiveWindow</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetAdaptersInfo</span>(adapterInfo: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADAPTER_INFO">IP_ADAPTER_INFO</a></span>, sizePointer: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_ULONG">ULONG</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_ULONG">ULONG</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetAsyncKeyState</span>(vKey: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SHORT">SHORT</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetClientRect</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpRect: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_RECT">RECT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetClipboardData</span>(uFormat: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetConsoleScreenBufferInfo</span>(hConsoleOutput: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpConsoleScreenBufferInfo: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CONSOLE_SCREEN_BUFFER_INFO">CONSOLE_SCREEN_BUFFER_INFO</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetCurrentDirectoryA</span>(nBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpBuffer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetCurrentDirectoryW</span>(nBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpBuffer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetCurrentProcess</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetCurrentProcessId</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetCurrentThread</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetCurrentThreadId</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetCursorPos</span>(lpPoint: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetDC</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HDC">HDC</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetDesktopWindow</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetFileAttributesA</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetFileAttributesExA</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, fInfoLevelId: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></span>, lpFileInformation: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetFileAttributesExW</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, fInfoLevelId: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></span>, lpFileInformation: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetFileAttributesW</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetFileSize</span>(hFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpFileSizeHigh: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPDWORD">LPDWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetForegroundWindow</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetFullPathNameA</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, nBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpBuffer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPSTR">LPSTR</a></span>, lpFilePart: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPSTR">LPSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetFullPathNameW</span>(lpFileName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCTSTR">LPCTSTR</a></span>, nBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpBuffer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPWSTR">LPWSTR</a></span>, lpFilePart: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPWSTR">LPWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetIconInfo</span>(hIcon: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HICON">HICON</a></span>, piconinfo: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_ICONINFO">ICONINFO</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetKeyboardState</span>(lpKeystate: *<span class="STpe">u8</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetLastError</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetLocalTime</span>(lpSystemTime: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetMessageA</span>(lpMsg: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_MSG">MSG</a></span>, hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, wMsgFilterMin: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wMsgFilterMax: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetMessageW</span>(lpMsg: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_MSG">MSG</a></span>, hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, wMsgFilterMin: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wMsgFilterMax: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetModuleHandleA</span>(lpModuleName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HMODULE">HMODULE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetModuleHandleW</span>(lpModuleName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HMODULE">HMODULE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetMonitorInfoA</span>(hMonitor: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HMONITOR">HMONITOR</a></span>, lpmi: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_MONITORINFO">MONITORINFO</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetMonitorInfoW</span>(hMonitor: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HMONITOR">HMONITOR</a></span>, lpmi: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_MONITORINFO">MONITORINFO</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetNextWindow</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, wCmd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetProcAddress</span>(hModule: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HMODULE">HMODULE</a></span>, lpProcName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SKwd">const</span> *<span class="STpe">void</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetStdHandle</span>(nStdHandle: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetSystemInfo</span>(lpSystemInfo: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SYSTEM_INFO">SYSTEM_INFO</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetSystemMetrics</span>(nIndex: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetSystemTime</span>(lpSystemTime: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetTopWindow</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetVolumePathNamesForVolumeNameA</span>(lpszVolumeName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, lpszVolumePathNames: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCH">LPCH</a></span>, cchBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpcchReturnLength: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetVolumePathNamesForVolumeNameW</span>(lpszVolumeName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, lpszVolumePathNames: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPWCH">LPWCH</a></span>, cchBufferLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpcchReturnLength: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetWindow</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, uCmd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetWindowLongPtrA</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, nIndex: <span class="STpe">s32</span>)-&gt;*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LONG">LONG</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetWindowLongPtrW</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, nIndex: <span class="STpe">s32</span>)-&gt;*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LONG">LONG</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetWindowRect</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpRect: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_RECT">RECT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetWindowTextA</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpString: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPSTR">LPSTR</a></span>, nMaxCount: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GetWindowTextW</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpString: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPWSTR">LPWSTR</a></span>, nMaxCount: <span class="STpe">s32</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GlobalAlloc</span>(uFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, dwBytes: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SIZE_T">SIZE_T</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HGLOBAL">HGLOBAL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GlobalLock</span>(hMem: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HGLOBAL">HGLOBAL</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GlobalSize</span>(hMem: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HGLOBAL">HGLOBAL</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SIZE_T">SIZE_T</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">GlobalUnlock</span>(hMem: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HGLOBAL">HGLOBAL</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">HIWORD</span>(l: <span class="STpe">u64</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<td class="codetype"><span class="SCde">fIcon</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">xHotspot</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">yHotspot</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hbmMask</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hbmColor</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
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
<td class="codetype"><span class="SCde">next</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADAPTER_INFO">IP_ADAPTER_INFO</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">comboIndex</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">adapterName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">260</span>] <span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">description</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">132</span>] <span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">addressLength</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">address</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">8</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BYTE">BYTE</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">index</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">type</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dhcpEnabled</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">currentIpAddress</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ipAddressList</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">gatewayList</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dhcpServer</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">haveWins</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">primaryWinsServer</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">secondaryWinsServer</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">leaseObtained</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">leaseExpires</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
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
<td class="codetype"><span class="SCde">str</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">16</span>] <span class="STpe">u8</span></span></td>
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
<td class="codetype"><span class="SCde">next</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDR_STRING">IP_ADDR_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ipAddress</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDRESS_STRING">IP_ADDRESS_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ipMask</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_IP_ADDRESS_STRING">IP_ADDRESS_STRING</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">context</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">InitializeSRWLock</span>(<span class="SCst">SRWLock</span>: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SRWLOCK">SRWLOCK</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">InvalidateRect</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpRect: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_RECT">RECT</a></span>, bErase: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">IsClipboardFormatAvailable</span>(format: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">IsDebuggerPresent</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">IsWindowVisible</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">LOWORD</span>(l: <span class="STpe">u64</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">LoadCursorA</span>(hInstance: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HINSTANCE">HINSTANCE</a></span>, lpCursorName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HCURSOR">HCURSOR</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">LoadCursorW</span>(hInstance: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HINSTANCE">HINSTANCE</a></span>, lpCursorName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HCURSOR">HCURSOR</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">LocalFree</span>(hLocal: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HLOCAL">HLOCAL</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HLOCAL">HLOCAL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MAKEINTRESOURCEA</span>(val: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MAKEINTRESOURCEW</span>(val: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MAKELANGID</span>(p: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WORD">WORD</a></span>, s: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WORD">WORD</a></span>)-&gt;<span class="STpe">u16</span></span></code>
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
<td class="codetype"><span class="SCde">ptReserved</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ptMaxSize</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ptMaxPosition</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ptMinTrackSize</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ptMaxTrackSize</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span></span></td>
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
<td class="codetype"><span class="SCde">cbSize</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rcMonitor</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_RECT">RECT</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rcWork</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_RECT">RECT</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwFlags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<td class="codetype"><span class="SCde">hwnd</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">message</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wParam</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lParam</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">time</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">pt</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lPrivate</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MapVirtualKeyA</span>(uCode: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, uMapType: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MapVirtualKeyW</span>(uCode: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, uMapType: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MessageBoxA</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpText: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, lpCaption: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, uType: <span class="STpe">u32</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MessageBoxW</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpText: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, lpCaption: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, uType: <span class="STpe">u32</span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MonitorFromPoint</span>(pt: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span>, dwFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HMONITOR">HMONITOR</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MonitorFromWindow</span>(hwnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, dwFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HMONITOR">HMONITOR</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">MoveWindow</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, nWidth: <span class="STpe">s32</span>, nHeight: <span class="STpe">s32</span>, bRepaint: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>) <span class="SKwd">throw</span></span></code>
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
<td class="codetype"><span class="SCde">cbSize</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hWnd</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">uID</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">uFlags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">uCallbackMessage</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hIcon</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szTip</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">128</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwState</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwStateMask</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szInfo</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">256</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SKwd">using</span> <span class="SCst">DUMMYUNIONNAME</span></span></td>
<td class="codetype"><span class="SCde">{uTimeout: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, uVersion: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>}</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szInfoTitle</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">64</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwInfoFlags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">guidItem</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_GUID">GUID</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hBalloonIcon</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
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
<td class="codetype"><span class="SCde">cbSize</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hWnd</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">uID</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">uFlags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">uCallbackMessage</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hIcon</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szTip</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">128</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwState</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwStateMask</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szInfo</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">256</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SKwd">using</span> <span class="SCst">DUMMYUNIONNAME</span></span></td>
<td class="codetype"><span class="SCde">{uTimeout: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, uVersion: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>}</span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szInfoTitle</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">64</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwInfoFlags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">guidItem</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_GUID">GUID</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hBalloonIcon</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">OpenClipboard</span>(hWndNewOwner: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">OutputDebugStringA</span>(lpOutputString: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">OutputDebugStringW</span>(lpOutputString: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)</span></code>
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
<td class="codetype"><span class="SCde">hdc</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">fErase</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rcPaint</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_RECT">RECT</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">fRestore</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">fIncUpdate</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">rgbReserved</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">32</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BYTE">BYTE</a></span></span></td>
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
<td class="codetype"><span class="SCde">x</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">y</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
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
<td class="codetype"><span class="SCde">hProcess</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hThread</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwProcessId</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwThreadId</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">PeekMessageA</span>(lpMsg: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_MSG">MSG</a></span>, hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, wMsgFilterMin: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wMsgFilterMax: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wRemoveMsg: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">PeekMessageW</span>(lpMsg: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_MSG">MSG</a></span>, hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, wMsgFilterMin: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wMsgFilterMax: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wRemoveMsg: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">PostMessageA</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, msg: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WPARAM">WPARAM</a></span>, lParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPARAM">LPARAM</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">PostMessageW</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, msg: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WPARAM">WPARAM</a></span>, lParam: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPARAM">LPARAM</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">PostQuitMessage</span>(nExitCode: <span class="STpe">s32</span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">QueryPerformanceCounter</span>(lpPerformanceCount: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LARGE_INTEGER">LARGE_INTEGER</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">QueryPerformanceFrequency</span>(lpFrequency: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LARGE_INTEGER">LARGE_INTEGER</a></span>)</span></code>
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
<td class="codetype"><span class="SCde">left</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">top</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">right</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bottom</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ReadFile</span>(hFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpBuffer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span>, nNumberOfBytesToRead: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpNumberOfBytesRead: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPDWORD">LPDWORD</a></span>, lpOverlapped: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPOVERLAPPED">LPOVERLAPPED</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">RegisterClassA</span>(lpWndClass: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WNDCLASSA">WNDCLASSA</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_ATOM">ATOM</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">RegisterClassW</span>(lpWndClass: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WNDCLASSW">WNDCLASSW</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_ATOM">ATOM</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">RegisterClipboardFormatA</span>(name: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">RegisterClipboardFormatW</span>(name: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">RegisterHotKey</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, id: <span class="STpe">s32</span>, fsModifiers: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, vk: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ReleaseCapture</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ReleaseDC</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, hDC: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HDC">HDC</a></span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ReleaseMutex</span>(hMutex: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ReleaseSRWLockExclusive</span>(<span class="SCst">SRWLock</span>: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SRWLOCK">SRWLOCK</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ReleaseSRWLockShared</span>(<span class="SCst">SRWLock</span>: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SRWLOCK">SRWLOCK</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ResetEvent</span>(hEvent: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ResumeThread</span>(hThread: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<td class="codetype"><span class="SCde">nLength</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpSecurityDescriptor</span></td>
<td class="codetype"><span class="SCde">^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bInheritHandle</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
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
<td class="codetype"><span class="SCde">hIcon</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">iIcon</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwAttributes</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szDisplayName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">260</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szTypeName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">80</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
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
<td class="codetype"><span class="SCde">hIcon</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">iIcon</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwAttributes</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szDisplayName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">260</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szTypeName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">80</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SHGetFileInfoA</span>(pszPath: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, dwFileAttributes: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, psfi: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SHFILEINFOA">SHFILEINFOA</a></span>, cbFileInfo: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, uFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD_PTR">DWORD_PTR</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SHGetFileInfoW</span>(pszPath: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>, dwFileAttributes: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, psfi: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SHFILEINFOW">SHFILEINFOW</a></span>, cbFileInfo: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, uFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD_PTR">DWORD_PTR</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SHGetSpecialFolderPathA</span>(hwnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, pszPath: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPSTR">LPSTR</a></span>, csidl: <span class="STpe">s32</span>, fCreate: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SHGetSpecialFolderPathW</span>(hwnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, pszPath: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPWSTR">LPWSTR</a></span>, csidl: <span class="STpe">s32</span>, fCreate: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SHGetStockIconInfo</span>(siid: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SHSTOCKICONID">SHSTOCKICONID</a></span>, uFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, psii: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SHSTOCKICONINFO">SHSTOCKICONINFO</a></span>) <span class="SKwd">throw</span></span></code>
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
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DOCNOASSOC</span></span></td>
<td>document (blank page) no associated program. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DOCASSOC</span></span></td>
<td>document with an associated program. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_APPLICATION</span></span></td>
<td>generic application with no custom icon. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_FOLDER</span></span></td>
<td>folder (closed). </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_FOLDEROPEN</span></span></td>
<td>folder (open). </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVE525</span></span></td>
<td>5.25" floppy disk drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVE35</span></span></td>
<td>3.5" floppy disk drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVEREMOVE</span></span></td>
<td>removable drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVEFIXED</span></span></td>
<td>fixed (hard disk) drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVENET</span></span></td>
<td>network drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVENETDISABLED</span></span></td>
<td>disconnected network drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVECD</span></span></td>
<td>CD drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVERAM</span></span></td>
<td>RAM disk drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_WORLD</span></span></td>
<td>entire network. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_SERVER</span></span></td>
<td>a computer on the network. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_PRINTER</span></span></td>
<td>printer. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MYNETWORK</span></span></td>
<td>My network places. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_FIND</span></span></td>
<td>Find. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_HELP</span></span></td>
<td>Help. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_SHARE</span></span></td>
<td>overlay for shared items. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_LINK</span></span></td>
<td>overlay for shortcuts to items. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_SLOWFILE</span></span></td>
<td>overlay for slow items. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_RECYCLER</span></span></td>
<td>empty recycle bin. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_RECYCLERFULL</span></span></td>
<td>full recycle bin. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIACDAUDIO</span></span></td>
<td>Audio CD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_LOCK</span></span></td>
<td>Security lock. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_AUTOLIST</span></span></td>
<td>AutoList. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_PRINTERNET</span></span></td>
<td>Network printer. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_SERVERSHARE</span></span></td>
<td>Server share. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_PRINTERFAX</span></span></td>
<td>Fax printer. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_PRINTERFAXNET</span></span></td>
<td>Networked Fax Printer. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_PRINTERFILE</span></span></td>
<td>Print to File. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_STACK</span></span></td>
<td>Stack. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIASVCD</span></span></td>
<td>SVCD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_STUFFEDFOLDER</span></span></td>
<td>Folder containing other items. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVEUNKNOWN</span></span></td>
<td>Unknown drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVEDVD</span></span></td>
<td>DVD Drive. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIADVD</span></span></td>
<td>DVD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIADVDRAM</span></span></td>
<td>DVD-RAM Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIADVDRW</span></span></td>
<td>DVD-RW Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIADVDR</span></span></td>
<td>DVD-R Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIADVDROM</span></span></td>
<td>DVD-ROM Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIACDAUDIOPLUS</span></span></td>
<td>CD+ (Enhanced CD) Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIACDRW</span></span></td>
<td>CD-RW Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIACDR</span></span></td>
<td>CD-R Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIACDBURN</span></span></td>
<td>Burning CD. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIABLANKCD</span></span></td>
<td>Blank CD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIACDROM</span></span></td>
<td>CD-ROM Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_AUDIOFILES</span></span></td>
<td>Audio files. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_IMAGEFILES</span></span></td>
<td>Image files. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_VIDEOFILES</span></span></td>
<td>Video files. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MIXEDFILES</span></span></td>
<td>Mixed files. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_FOLDERBACK</span></span></td>
<td>Folder back. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_FOLDERFRONT</span></span></td>
<td>Folder front. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_SHIELD</span></span></td>
<td>Security shield. Use for UAC prompts only. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_WARNING</span></span></td>
<td>Warning. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_INFO</span></span></td>
<td>Informational. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_ERROR</span></span></td>
<td>Error. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_KEY</span></span></td>
<td>Key / Secure. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_SOFTWARE</span></span></td>
<td>Software. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_RENAME</span></span></td>
<td>Rename. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DELETE</span></span></td>
<td>Delete. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIAAUDIODVD</span></span></td>
<td>Audio DVD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIAMOVIEDVD</span></span></td>
<td>Movie DVD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIAENHANCEDCD</span></span></td>
<td>Enhanced CD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIAENHANCEDDVD</span></span></td>
<td>Enhanced DVD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIAHDDVD</span></span></td>
<td>HD-DVD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIABLURAY</span></span></td>
<td>BluRay Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIAVCD</span></span></td>
<td>VCD Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIADVDPLUSR</span></span></td>
<td>DVD+R Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIADVDPLUSRW</span></span></td>
<td>DVD+RW Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DESKTOPPC</span></span></td>
<td>desktop computer. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MOBILEPC</span></span></td>
<td>mobile computer (laptop/notebook). </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_USERS</span></span></td>
<td>users. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIASMARTMEDIA</span></span></td>
<td>Smart Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIACOMPACTFLASH</span></span></td>
<td>Compact Flash. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DEVICECELLPHONE</span></span></td>
<td>Cell phone. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DEVICECAMERA</span></span></td>
<td>Camera. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DEVICEVIDEOCAMERA</span></span></td>
<td>Video camera. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DEVICEAUDIOPLAYER</span></span></td>
<td>Audio player. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_NETWORKCONNECT</span></span></td>
<td>Connect to network. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_INTERNET</span></span></td>
<td>Internet. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_ZIPFILE</span></span></td>
<td>ZIP file. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_SETTINGS</span></span></td>
<td>Settings  107-131 are internal Vista RTM icons  132-159 for SP1 icons. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVEHDDVD</span></span></td>
<td>HDDVD Drive (all types). </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_DRIVEBD</span></span></td>
<td>BluRay Drive (all types). </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIAHDDVDROM</span></span></td>
<td>HDDVD-ROM Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIAHDDVDR</span></span></td>
<td>HDDVD-R Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIAHDDVDRAM</span></span></td>
<td>HDDVD-RAM Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIABDROM</span></span></td>
<td>BluRay ROM Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIABDR</span></span></td>
<td>BluRay R Media. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MEDIABDRE</span></span></td>
<td>BluRay RE Media (Rewriable and RAM). </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_CLUSTEREDDRIVE</span></span></td>
<td>Clustered disk  160+ are for Windows 7 icons. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde"><span class="SCst">SIID_MAX_ICONS</span></span></td>
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
<td class="codetype"><span class="SCde">cbSize</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hIcon</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">iSysImageIndex</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">iIcon</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">szPath</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">260</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
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
<td class="codetype"><span class="SCde">left</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">top</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">right</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">bottom</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
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
<td class="codetype"><span class="SCde">cb</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpReserved</span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpDesktop</span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpTitle</span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwX</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwY</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwXSize</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwYSize</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwXCountChars</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwYCountChars</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwFillAttribute</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwFlags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wShowWindow</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cbReserved2</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpReserved2</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BYTE">BYTE</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hStdInput</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hStdOutput</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hStdError</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
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
<td class="codetype"><span class="SCde">cb</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpReserved</span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpDesktop</span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpTitle</span></td>
<td class="codetype"><span class="SCde">^<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwX</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwY</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwXSize</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwYSize</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwXCountChars</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwYCountChars</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwFillAttribute</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwFlags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wShowWindow</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cbReserved2</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpReserved2</span></td>
<td class="codetype"><span class="SCde">*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BYTE">BYTE</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hStdInput</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hStdOutput</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hStdError</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">void</span></span></td>
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
<td class="codetype"><span class="SCde">wYear</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wMonth</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wDayOfWeek</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wDay</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wHour</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wMinute</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wSecond</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wMilliseconds</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
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
<td class="codetype"><span class="SCde">dwOemId</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td>Must be updated to an union. </td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwPageSize</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpMinimumApplicationAddress</span></td>
<td class="codetype"><span class="SCde">^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpMaximumApplicationAddress</span></td>
<td class="codetype"><span class="SCde">^<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwActiveProcessorMask</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u64</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwNumberOfProcessors</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwProcessorType</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwAllocationGranularity</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wProcessorLevel</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wProcessorRevision</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ScreenToClient</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, lpPoint: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetActiveWindow</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetCapture</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetClipboardData</span>(uFormat: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, hMem: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetConsoleActiveScreenBuffer</span>(hConsoleOutput: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetConsoleOutputCP</span>(codePage: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetConsoleTextAttribute</span>(hConsoleOutput: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, wAttributes: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WORD">WORD</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetCurrentDirectoryA</span>(lpPathName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetCurrentDirectoryW</span>(lpPathName: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCWSTR">LPCWSTR</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetCursor</span>(hCursor: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HCURSOR">HCURSOR</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HCURSOR">HCURSOR</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetCursorPos</span>(x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetEvent</span>(hEvent: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetFilePointer</span>(hFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lDistanceToMove: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LONG">LONG</a></span>, lpDistanceToMoveHigh: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_PLONG">PLONG</a></span>, dwMoveMethod: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetFileTime</span>(hFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpCreationTime: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span>, lpLastAccessTime: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span>, lpLastWriteTime: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetFocus</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetLayeredWindowAttributes</span>(hwnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, crKey: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_COLORREF">COLORREF</a></span>, bAlpha: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BYTE">BYTE</a></span>, dwFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetThreadPriority</span>(hThread: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, nPriority: <span class="STpe">s32</span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetWaitableTimer</span>(hTimer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpDueTime: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LARGE_INTEGER">LARGE_INTEGER</a></span>, lPeriod: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LONG">LONG</a></span>, pfnCompletionRoutine: *<span class="STpe">void</span>, lpArgToCompletionRoutine: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPVOID">LPVOID</a></span>, fResume: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetWindowLongPtrA</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, nIndex: <span class="STpe">s32</span>, dwNewLong: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LONG">LONG</a></span>)-&gt;*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LONG">LONG</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetWindowLongPtrW</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, nIndex: <span class="STpe">s32</span>, dwNewLong: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LONG">LONG</a></span>)-&gt;*<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LONG">LONG</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SetWindowPos</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, hWndInsertAfter: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, x: <span class="STpe">s32</span>, y: <span class="STpe">s32</span>, cx: <span class="STpe">s32</span>, cy: <span class="STpe">s32</span>, uFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">Shell_NotifyIconA</span>(dwMessage: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpData: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_NOTIFYICONDATAA">NOTIFYICONDATAA</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">Shell_NotifyIconW</span>(dwMessage: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpData: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_NOTIFYICONDATAW">NOTIFYICONDATAW</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ShowCursor</span>(show: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ShowWindow</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, nCmdShow: <span class="STpe">s32</span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">Sleep</span>(dwMilliseconds: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SleepEx</span>(dwMilliseconds: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, bAltertable: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>)</span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SwitchToThread</span>()-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">SystemTimeToFileTime</span>(lpSystemTime: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span>, lpFileTime: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">ToUnicode</span>(wVirtKey: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, wScanCode: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>, lpKeyState: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BYTE">BYTE</a></span>, pwszBuff: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPWSTR">LPWSTR</a></span>, cchBuff: <span class="STpe">s32</span>, wFlags: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_UINT">UINT</a></span>)-&gt;<span class="STpe">s32</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">TranslateMessage</span>(lpMsg: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_MSG">MSG</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">TryAcquireSRWLockExclusive</span>(<span class="SCst">SRWLock</span>: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SRWLOCK">SRWLOCK</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">TryAcquireSRWLockShared</span>(<span class="SCst">SRWLock</span>: *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_SRWLOCK">SRWLOCK</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">UnregisterHotKey</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>, id: <span class="STpe">s32</span>) <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">UpdateWindow</span>(hWnd: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span>) <span class="SKwd">throw</span></span></code>
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
<td class="codetype"><span class="SCde">dwFileAttributes</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ftCreationTime</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ftLastAccessTime</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ftLastWriteTime</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nFileSizeHigh</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nFileSizeLow</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<td class="codetype"><span class="SCde">dwFileAttributes</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ftCreationTime</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ftLastAccessTime</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ftLastWriteTime</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nFileSizeHigh</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nFileSizeLow</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwReserved0</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwReserved1</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cFileName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">260</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAlternateFileName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">14</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_CHAR">CHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwFileType</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwCreatorType</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wFinderFlags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
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
<td class="codetype"><span class="SCde">dwFileAttributes</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ftCreationTime</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ftLastAccessTime</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">ftLastWriteTime</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_FILETIME">FILETIME</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nFileSizeHigh</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">nFileSizeLow</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwReserved0</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwReserved1</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cFileName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">260</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cAlternateFileName</span></td>
<td class="codetype"><span class="SCde">[<span class="SNum">14</span>] <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_WCHAR">WCHAR</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwFileType</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">dwCreatorType</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">wFinderFlags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u16</span></span></td>
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
<td class="codetype"><span class="SCde">hwnd</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hwndInsertAfter</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">x</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">y</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cx</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cy</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">flags</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
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
<td class="codetype"><span class="SCde">style</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpfnWndProc</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cbClsExtra</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cbWndExtra</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hInstance</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hIcon</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hCursor</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hbrBackground</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpszMenuName</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u8</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpszClassName</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u8</span></span></td>
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
<td class="codetype"><span class="SCde">style</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">u32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpfnWndProc</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cbClsExtra</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">cbWndExtra</span></td>
<td class="codetype"><span class="SCde"><span class="STpe">s32</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hInstance</span></td>
<td class="codetype"><span class="SCde"><span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hIcon</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hCursor</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">hbrBackground</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> *<span class="STpe">void</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpszMenuName</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u16</span></span></td>
<td></td>
</tr>
<tr>
<td class="codetype"><span class="SCde">lpszClassName</span></td>
<td class="codetype"><span class="SCde"><span class="SKwd">const</span> ^<span class="STpe">u16</span></span></td>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">WaitForMultipleObjects</span>(nCount: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpHandles: <span class="SKwd">const</span> *<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, bWaitAll: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span>, dwMilliseconds: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">WaitForSingleObject</span>(hHandle: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, dwMilliseconds: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">WindowFromPoint</span>(pt: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_POINT">POINT</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HWND">HWND</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">WriteConsoleOutputCharacterA</span>(hConsoleOutput: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpCharacter: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCSTR">LPCSTR</a></span>, nLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, dwWriteCoord: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpNumberOfCharsWritten: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPDWORD">LPDWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">WriteConsoleOutputCharacterW</span>(hConsoleOutput: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpCharacter: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCTSTR">LPCTSTR</a></span>, nLength: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, dwWriteCoord: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpNumberOfCharsWritten: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPDWORD">LPDWORD</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SCst">WriteFile</span>(hFile: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_HANDLE">HANDLE</a></span>, lpBuffer: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPCVOID">LPCVOID</a></span>, nNumberOfBytesToWrite: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_DWORD">DWORD</a></span>, lpNumberOfBytesWritten: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPDWORD">LPDWORD</a></span>, lpOverlapped: <span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_LPOVERLAPPED">LPOVERLAPPED</a></span>)-&gt;<span class="SCst">Win32</span>.<span class="SCst"><a href="#Win32_BOOL">BOOL</a></span> <span class="SKwd">throw</span></span></code>
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
<div class="precode"><code><span class="SCde"><span class="SKwd">func</span> <span class="SFct">setError</span>(errorMessageID: <span class="STpe">u32</span>) <span class="SKwd">throw</span></span></code>
</div>
</div>
</div>
</div>
</body>
</html>
