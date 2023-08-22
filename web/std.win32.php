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
<td id="Win32_BACKGROUND_BLUE">BACKGROUND_BLUE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_GREEN">BACKGROUND_GREEN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_INTENSITY">BACKGROUND_INTENSITY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_MASK">BACKGROUND_MASK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_BACKGROUND_RED">BACKGROUND_RED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_BITMAP">CF_BITMAP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_DIB">CF_DIB</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_DIBV5">CF_DIBV5</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_DIF">CF_DIF</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_ENHMETAFILE">CF_ENHMETAFILE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_HDROP">CF_HDROP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_LOCALE">CF_LOCALE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_METAFILEPICT">CF_METAFILEPICT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_OEMTEXT">CF_OEMTEXT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_PALETTE">CF_PALETTE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_PENDATA">CF_PENDATA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_RIFF">CF_RIFF</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_SYLK">CF_SYLK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_TEXT">CF_TEXT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_TIFF">CF_TIFF</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_UNICODETEXT">CF_UNICODETEXT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CF_WAVE">CF_WAVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CONSOLE_TEXTMODE_BUFFER">CONSOLE_TEXTMODE_BUFFER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CREATE_ALWAYS">CREATE_ALWAYS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CREATE_NEW">CREATE_NEW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CREATE_SUSPENDED">CREATE_SUSPENDED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_ADMINTOOLS">CSIDL_ADMINTOOLS</td>
<td>u32</td>
<td>&lt;user name&gt;\Start Menu\Programs\Administrative Tools. </td>
</tr>
<tr>
<td id="Win32_CSIDL_ALTSTARTUP">CSIDL_ALTSTARTUP</td>
<td>u32</td>
<td>non localized startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_APPDATA">CSIDL_APPDATA</td>
<td>u32</td>
<td>&lt;user name&gt;\Application Data. </td>
</tr>
<tr>
<td id="Win32_CSIDL_BITBUCKET">CSIDL_BITBUCKET</td>
<td>u32</td>
<td>&lt;desktop&gt;\Recycle Bin. </td>
</tr>
<tr>
<td id="Win32_CSIDL_CDBURN_AREA">CSIDL_CDBURN_AREA</td>
<td>u32</td>
<td>USERPROFILE\Local Settings\Application Data\Microsoft\CD Burning. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_ADMINTOOLS">CSIDL_COMMON_ADMINTOOLS</td>
<td>u32</td>
<td>All Users\Start Menu\Programs\Administrative Tools. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_ALTSTARTUP">CSIDL_COMMON_ALTSTARTUP</td>
<td>u32</td>
<td>non localized common startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_APPDATA">CSIDL_COMMON_APPDATA</td>
<td>u32</td>
<td>All Users\Application Data. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_DESKTOPDIRECTORY">CSIDL_COMMON_DESKTOPDIRECTORY</td>
<td>u32</td>
<td>All Users\Desktop. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_DOCUMENTS">CSIDL_COMMON_DOCUMENTS</td>
<td>u32</td>
<td>All Users\Documents. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_FAVORITES">CSIDL_COMMON_FAVORITES</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_MUSIC">CSIDL_COMMON_MUSIC</td>
<td>u32</td>
<td>All Users\My Music. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_OEM_LINKS">CSIDL_COMMON_OEM_LINKS</td>
<td>u32</td>
<td>Links to All Users OEM specific apps. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_PICTURES">CSIDL_COMMON_PICTURES</td>
<td>u32</td>
<td>All Users\My Pictures. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_PROGRAMS">CSIDL_COMMON_PROGRAMS</td>
<td>u32</td>
<td>All Users\Start Menu\Programs. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_STARTMENU">CSIDL_COMMON_STARTMENU</td>
<td>u32</td>
<td>All Users\Start Menu. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_STARTUP">CSIDL_COMMON_STARTUP</td>
<td>u32</td>
<td>All Users\Startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_TEMPLATES">CSIDL_COMMON_TEMPLATES</td>
<td>u32</td>
<td>All Users\Templates. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_VIDEO">CSIDL_COMMON_VIDEO</td>
<td>u32</td>
<td>All Users\My Video. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COMPUTERSNEARME">CSIDL_COMPUTERSNEARME</td>
<td>u32</td>
<td>Computers Near Me (computered from Workgroup membership). </td>
</tr>
<tr>
<td id="Win32_CSIDL_CONNECTIONS">CSIDL_CONNECTIONS</td>
<td>u32</td>
<td>Network and Dial-up Connections. </td>
</tr>
<tr>
<td id="Win32_CSIDL_CONTROLS">CSIDL_CONTROLS</td>
<td>u32</td>
<td>My Computer\Control Panel. </td>
</tr>
<tr>
<td id="Win32_CSIDL_COOKIES">CSIDL_COOKIES</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_DESKTOP">CSIDL_DESKTOP</td>
<td>u32</td>
<td>&lt;desktop&gt;. </td>
</tr>
<tr>
<td id="Win32_CSIDL_DESKTOPDIRECTORY">CSIDL_DESKTOPDIRECTORY</td>
<td>u32</td>
<td>&lt;user name&gt;\Desktop. </td>
</tr>
<tr>
<td id="Win32_CSIDL_DRIVES">CSIDL_DRIVES</td>
<td>u32</td>
<td>My Computer. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FAVORITES">CSIDL_FAVORITES</td>
<td>u32</td>
<td>&lt;user name&gt;\Favorites. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_CREATE">CSIDL_FLAG_CREATE</td>
<td>u32</td>
<td>combine with CSIDL_ value to force folder creation in SHGetFolderPath(). </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_DONT_UNEXPAND">CSIDL_FLAG_DONT_UNEXPAND</td>
<td>u32</td>
<td>combine with CSIDL_ value to avoid unexpanding environment variables. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_DONT_VERIFY">CSIDL_FLAG_DONT_VERIFY</td>
<td>u32</td>
<td>combine with CSIDL_ value to return an unverified folder path. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_MASK">CSIDL_FLAG_MASK</td>
<td>u32</td>
<td>mask for all possible flag values. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_NO_ALIAS">CSIDL_FLAG_NO_ALIAS</td>
<td>u32</td>
<td>combine with CSIDL_ value to insure non-typealias versions of the pidl. </td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_PER_USER_INIT">CSIDL_FLAG_PER_USER_INIT</td>
<td>u32</td>
<td>combine with CSIDL_ value to indicate per-user init (eg. upgrade). </td>
</tr>
<tr>
<td id="Win32_CSIDL_FONTS">CSIDL_FONTS</td>
<td>u32</td>
<td>windows\fonts. </td>
</tr>
<tr>
<td id="Win32_CSIDL_HISTORY">CSIDL_HISTORY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_INTERNET">CSIDL_INTERNET</td>
<td>u32</td>
<td>Internet Explorer (icon on desktop). </td>
</tr>
<tr>
<td id="Win32_CSIDL_INTERNET_CACHE">CSIDL_INTERNET_CACHE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_LOCAL_APPDATA">CSIDL_LOCAL_APPDATA</td>
<td>u32</td>
<td>&lt;user name&gt;\Local Settings\Applicaiton Data (non roaming). </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYDOCUMENTS">CSIDL_MYDOCUMENTS</td>
<td>u32</td>
<td>Personal was just a silly name for My Documents. </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYMUSIC">CSIDL_MYMUSIC</td>
<td>u32</td>
<td>"My Music" folder. </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYPICTURES">CSIDL_MYPICTURES</td>
<td>u32</td>
<td>C:\Program Files\My Pictures. </td>
</tr>
<tr>
<td id="Win32_CSIDL_MYVIDEO">CSIDL_MYVIDEO</td>
<td>u32</td>
<td>"My Videos" folder. </td>
</tr>
<tr>
<td id="Win32_CSIDL_NETHOOD">CSIDL_NETHOOD</td>
<td>u32</td>
<td>&lt;user name&gt;\nethood. </td>
</tr>
<tr>
<td id="Win32_CSIDL_NETWORK">CSIDL_NETWORK</td>
<td>u32</td>
<td>Network Neighborhood (My Network Places). </td>
</tr>
<tr>
<td id="Win32_CSIDL_PERSONAL">CSIDL_PERSONAL</td>
<td>u32</td>
<td>My Documents. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PRINTERS">CSIDL_PRINTERS</td>
<td>u32</td>
<td>My Computer\Printers. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PRINTHOOD">CSIDL_PRINTHOOD</td>
<td>u32</td>
<td>&lt;user name&gt;\PrintHood. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROFILE">CSIDL_PROFILE</td>
<td>u32</td>
<td>USERPROFILE. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAMS">CSIDL_PROGRAMS</td>
<td>u32</td>
<td>Start Menu\Programs. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES">CSIDL_PROGRAM_FILES</td>
<td>u32</td>
<td>C:\Program Files. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILESX86">CSIDL_PROGRAM_FILESX86</td>
<td>u32</td>
<td>x86 C:\Program Files on RISC. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES_COMMON">CSIDL_PROGRAM_FILES_COMMON</td>
<td>u32</td>
<td>C:\Program Files\Common. </td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES_COMMONX86">CSIDL_PROGRAM_FILES_COMMONX86</td>
<td>u32</td>
<td>x86 Program Files\Common on RISC. </td>
</tr>
<tr>
<td id="Win32_CSIDL_RECENT">CSIDL_RECENT</td>
<td>u32</td>
<td>&lt;user name&gt;\Recent. </td>
</tr>
<tr>
<td id="Win32_CSIDL_RESOURCES">CSIDL_RESOURCES</td>
<td>u32</td>
<td>Resource Direcotry. </td>
</tr>
<tr>
<td id="Win32_CSIDL_RESOURCES_LOCALIZED">CSIDL_RESOURCES_LOCALIZED</td>
<td>u32</td>
<td>Localized Resource Direcotry. </td>
</tr>
<tr>
<td id="Win32_CSIDL_SENDTO">CSIDL_SENDTO</td>
<td>u32</td>
<td>&lt;user name&gt;\SendTo. </td>
</tr>
<tr>
<td id="Win32_CSIDL_STARTMENU">CSIDL_STARTMENU</td>
<td>u32</td>
<td>&lt;user name&gt;\Start Menu. </td>
</tr>
<tr>
<td id="Win32_CSIDL_STARTUP">CSIDL_STARTUP</td>
<td>u32</td>
<td>Start Menu\Programs\Startup. </td>
</tr>
<tr>
<td id="Win32_CSIDL_SYSTEM">CSIDL_SYSTEM</td>
<td>u32</td>
<td>GetSystemDirectory(). </td>
</tr>
<tr>
<td id="Win32_CSIDL_SYSTEMX86">CSIDL_SYSTEMX86</td>
<td>u32</td>
<td>x86 system directory on RISC. </td>
</tr>
<tr>
<td id="Win32_CSIDL_TEMPLATES">CSIDL_TEMPLATES</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CSIDL_WINDOWS">CSIDL_WINDOWS</td>
<td>u32</td>
<td>GetWindowsDirectory(). </td>
</tr>
<tr>
<td id="Win32_CS_BYTEALIGNCLIENT">CS_BYTEALIGNCLIENT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_BYTEALIGNWINDOW">CS_BYTEALIGNWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_CLASSDC">CS_CLASSDC</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_DBLCLKS">CS_DBLCLKS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_DROPSHADOW">CS_DROPSHADOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_GLOBALCLASS">CS_GLOBALCLASS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_HREDRAW">CS_HREDRAW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_IME">CS_IME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_NOCLOSE">CS_NOCLOSE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_OWNDC">CS_OWNDC</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_PARENTDC">CS_PARENTDC</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_SAVEBITS">CS_SAVEBITS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CS_VREDRAW">CS_VREDRAW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_ALL">CWP_ALL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_SKIPDISABLED">CWP_SKIPDISABLED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_SKIPINVISIBLE">CWP_SKIPINVISIBLE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CWP_SKIPTRANSPARENT">CWP_SKIPTRANSPARENT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_CW_USEDEFAULT">CW_USEDEFAULT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_DEFAULT_MINIMUM_ENTITIES">DEFAULT_MINIMUM_ENTITIES</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_DM_POINTERHITTEST">DM_POINTERHITTEST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_DEVICE_NOT_CONNECTED">ERROR_DEVICE_NOT_CONNECTED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_FILE_NOT_FOUND">ERROR_FILE_NOT_FOUND</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_IO_PENDING">ERROR_IO_PENDING</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_NO_MORE_FILES">ERROR_NO_MORE_FILES</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_ERROR_SUCCESS">ERROR_SUCCESS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FALSE">FALSE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_ARCHIVE">FILE_ATTRIBUTE_ARCHIVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_COMPRESSED">FILE_ATTRIBUTE_COMPRESSED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_DEVICE">FILE_ATTRIBUTE_DEVICE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_DIRECTORY">FILE_ATTRIBUTE_DIRECTORY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_EA">FILE_ATTRIBUTE_EA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_ENCRYPTED">FILE_ATTRIBUTE_ENCRYPTED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_HIDDEN">FILE_ATTRIBUTE_HIDDEN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_INTEGRITY_STREAM">FILE_ATTRIBUTE_INTEGRITY_STREAM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NORMAL">FILE_ATTRIBUTE_NORMAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NOT_CONTENT_INDEXED">FILE_ATTRIBUTE_NOT_CONTENT_INDEXED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NO_SCRUB_DATA">FILE_ATTRIBUTE_NO_SCRUB_DATA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_OFFLINE">FILE_ATTRIBUTE_OFFLINE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_PINNED">FILE_ATTRIBUTE_PINNED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_READONLY">FILE_ATTRIBUTE_READONLY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS">FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_RECALL_ON_OPEN">FILE_ATTRIBUTE_RECALL_ON_OPEN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_REPARSE_POINT">FILE_ATTRIBUTE_REPARSE_POINT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_SPARSE_FILE">FILE_ATTRIBUTE_SPARSE_FILE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_SYSTEM">FILE_ATTRIBUTE_SYSTEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_TEMPORARY">FILE_ATTRIBUTE_TEMPORARY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_UNPINNED">FILE_ATTRIBUTE_UNPINNED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_VIRTUAL">FILE_ATTRIBUTE_VIRTUAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_BEGIN">FILE_BEGIN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_CURRENT">FILE_CURRENT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_END">FILE_END</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_READ_ATTRIBUTES">FILE_READ_ATTRIBUTES</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_DELETE">FILE_SHARE_DELETE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_READ">FILE_SHARE_READ</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_WRITE">FILE_SHARE_WRITE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FILE_WRITE_ATTRIBUTES">FILE_WRITE_ATTRIBUTES</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_BLUE">FOREGROUND_BLUE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_GREEN">FOREGROUND_GREEN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_INTENSITY">FOREGROUND_INTENSITY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_MASK">FOREGROUND_MASK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FOREGROUND_RED">FOREGROUND_RED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_ALLOCATE_BUFFER">FORMAT_MESSAGE_ALLOCATE_BUFFER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_ARGUMENT_ARRAY">FORMAT_MESSAGE_ARGUMENT_ARRAY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_HMODULE">FORMAT_MESSAGE_FROM_HMODULE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_STRING">FORMAT_MESSAGE_FROM_STRING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_SYSTEM">FORMAT_MESSAGE_FROM_SYSTEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_IGNORE_INSERTS">FORMAT_MESSAGE_IGNORE_INSERTS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_MAX_WIDTH_MASK">FORMAT_MESSAGE_MAX_WIDTH_MASK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_ALL">GENERIC_ALL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_EXECUTE">GENERIC_EXECUTE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_READ">GENERIC_READ</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GENERIC_WRITE">GENERIC_WRITE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_DDESHARE">GMEM_DDESHARE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_DISCARDABLE">GMEM_DISCARDABLE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_FIXED">GMEM_FIXED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_INVALID_HANDLE">GMEM_INVALID_HANDLE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_LOWER">GMEM_LOWER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_MODIFY">GMEM_MODIFY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_MOVEABLE">GMEM_MOVEABLE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NOCOMPACT">GMEM_NOCOMPACT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NODISCARD">GMEM_NODISCARD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NOTIFY">GMEM_NOTIFY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_NOT_BANKED">GMEM_NOT_BANKED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_SHARE">GMEM_SHARE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_VALID_FLAGS">GMEM_VALID_FLAGS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GMEM_ZEROINIT">GMEM_ZEROINIT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_HINSTANCE">GWLP_HINSTANCE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_HWNDPARENT">GWLP_HWNDPARENT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_ID">GWLP_ID</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_USERDATA">GWLP_USERDATA</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GWLP_WNDPROC">GWLP_WNDPROC</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_CHILD">GW_CHILD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_ENABLEDPOPUP">GW_ENABLEDPOPUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDFIRST">GW_HWNDFIRST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDLAST">GW_HWNDLAST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDNEXT">GW_HWNDNEXT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_HWNDPREV">GW_HWNDPREV</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_MAX">GW_MAX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_GW_OWNER">GW_OWNER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_BOTTOM">HWND_BOTTOM</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_NOTOPMOST">HWND_NOTOPMOST</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_TOP">HWND_TOP</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td id="Win32_HWND_TOPMOST">HWND_TOPMOST</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDABORT">IDABORT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDCANCEL">IDCANCEL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDCLOSE">IDCLOSE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDCONTINUE">IDCONTINUE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_APPSTARTING">IDC_APPSTARTING</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_ARROW">IDC_ARROW</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_CROSS">IDC_CROSS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_HAND">IDC_HAND</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_HELP">IDC_HELP</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_IBEAM">IDC_IBEAM</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_ICON">IDC_ICON</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_NO">IDC_NO</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_PERSON">IDC_PERSON</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_PIN">IDC_PIN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZE">IDC_SIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZEALL">IDC_SIZEALL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZENESW">IDC_SIZENESW</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZENS">IDC_SIZENS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZENWSE">IDC_SIZENWSE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_SIZEWE">IDC_SIZEWE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_UPARROW">IDC_UPARROW</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDC_WAIT">IDC_WAIT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDHELP">IDHELP</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDIGNORE">IDIGNORE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDNO">IDNO</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDOK">IDOK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDRETRY">IDRETRY</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDTIMEOUT">IDTIMEOUT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDTRYAGAIN">IDTRYAGAIN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_IDYES">IDYES</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_INFINITE">INFINITE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_FILE_ATTRIBUTES">INVALID_FILE_ATTRIBUTES</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_FILE_SIZE">INVALID_FILE_SIZE</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_HANDLE_VALUE">INVALID_HANDLE_VALUE</td>
<td>Win32.HANDLE</td>
<td></td>
</tr>
<tr>
<td id="Win32_INVALID_SET_FILE_POINTER">INVALID_SET_FILE_POINTER</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td id="Win32_LANG_ENGLISH">LANG_ENGLISH</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_LWA_ALPHA">LWA_ALPHA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_LWA_COLORKEY">LWA_COLORKEY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_CHAR">MAPVK_VK_TO_CHAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_VSC">MAPVK_VK_TO_VSC</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_VSC_EX">MAPVK_VK_TO_VSC_EX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VSC_TO_VK">MAPVK_VSC_TO_VK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAPVK_VSC_TO_VK_EX">MAPVK_VSC_TO_VK_EX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAXIMUM_WAIT_OBJECTS">MAXIMUM_WAIT_OBJECTS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_ADDRESS_LENGTH">MAX_ADAPTER_ADDRESS_LENGTH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_DESCRIPTION_LENGTH">MAX_ADAPTER_DESCRIPTION_LENGTH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_NAME_LENGTH">MAX_ADAPTER_NAME_LENGTH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_DHCPV6_DUID_LENGTH">MAX_DHCPV6_DUID_LENGTH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_DNS_SUFFIX_STRING_LENGTH">MAX_DNS_SUFFIX_STRING_LENGTH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_DOMAIN_NAME_LEN">MAX_DOMAIN_NAME_LEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_HOSTNAME_LEN">MAX_HOSTNAME_LEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_PATH">MAX_PATH</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MAX_SCOPE_ID_LEN">MAX_SCOPE_ID_LEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ABORTRETRYIGNORE">MB_ABORTRETRYIGNORE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_APPLMODAL">MB_APPLMODAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_CANCELTRYCONTINUE">MB_CANCELTRYCONTINUE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFAULT_DESKTOP_ONLY">MB_DEFAULT_DESKTOP_ONLY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON1">MB_DEFBUTTON1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON2">MB_DEFBUTTON2</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON3">MB_DEFBUTTON3</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON4">MB_DEFBUTTON4</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_HELP">MB_HELP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONASTERISK">MB_ICONASTERISK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONERROR">MB_ICONERROR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONEXCLAMATION">MB_ICONEXCLAMATION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONHAND">MB_ICONHAND</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONINFORMATION">MB_ICONINFORMATION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONQUESTION">MB_ICONQUESTION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONSTOP">MB_ICONSTOP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_ICONWARNING">MB_ICONWARNING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_NOFOCUS">MB_NOFOCUS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_OK">MB_OK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_OKCANCEL">MB_OKCANCEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_RETRYCANCEL">MB_RETRYCANCEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_RIGHT">MB_RIGHT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_RTLREADING">MB_RTLREADING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_SETFOREGROUND">MB_SETFOREGROUND</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_SYSTEMMODAL">MB_SYSTEMMODAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_TASKMODAL">MB_TASKMODAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_TOPMOST">MB_TOPMOST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_USERICON">MB_USERICON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_YESNO">MB_YESNO</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MB_YESNOCANCEL">MB_YESNOCANCEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MN_GETHMENU">MN_GETHMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_ALT">MOD_ALT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_CONTROL">MOD_CONTROL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_NOREPEAT">MOD_NOREPEAT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_SHIFT">MOD_SHIFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MOD_WIN">MOD_WIN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTONEAREST">MONITOR_DEFAULTTONEAREST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTONULL">MONITOR_DEFAULTTONULL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTOPRIMARY">MONITOR_DEFAULTTOPRIMARY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_GUID">NIF_GUID</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_ICON">NIF_ICON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_INFO">NIF_INFO</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_MESSAGE">NIF_MESSAGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_REALTIME">NIF_REALTIME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_SHOWTIP">NIF_SHOWTIP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_STATE">NIF_STATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIF_TIP">NIF_TIP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_ADD">NIM_ADD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_DELETE">NIM_DELETE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_MODIFY">NIM_MODIFY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_SETFOCUS">NIM_SETFOCUS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_NIM_SETVERSION">NIM_SETVERSION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_OPEN_ALWAYS">OPEN_ALWAYS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_OPEN_EXISTING">OPEN_EXISTING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_PM_NOREMOVE">PM_NOREMOVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_PM_NOYIELD">PM_NOYIELD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_PM_REMOVE">PM_REMOVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_ARRANGE">SC_ARRANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_CLOSE">SC_CLOSE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_CONTEXTHELP">SC_CONTEXTHELP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_DEFAULT">SC_DEFAULT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_HOTKEY">SC_HOTKEY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_HSCROLL">SC_HSCROLL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_KEYMENU">SC_KEYMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MAXIMIZE">SC_MAXIMIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MINIMIZE">SC_MINIMIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MONITORPOWER">SC_MONITORPOWER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MOUSEMENU">SC_MOUSEMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_MOVE">SC_MOVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_NEXTWINDOW">SC_NEXTWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_PREVWINDOW">SC_PREVWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_RESTORE">SC_RESTORE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_SCREENSAVE">SC_SCREENSAVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_SEPARATOR">SC_SEPARATOR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_SIZE">SC_SIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_TASKLIST">SC_TASKLIST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SC_VSCROLL">SC_VSCROLL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGFI_ADDOVERLAYS">SHGFI_ADDOVERLAYS</td>
<td>u32</td>
<td>apply the appropriate overlays. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ATTRIBUTES">SHGFI_ATTRIBUTES</td>
<td>u32</td>
<td>get attributes. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ATTR_SPECIFIED">SHGFI_ATTR_SPECIFIED</td>
<td>u32</td>
<td>get only specified attributes. </td>
</tr>
<tr>
<td id="Win32_SHGFI_DISPLAYNAME">SHGFI_DISPLAYNAME</td>
<td>u32</td>
<td>get display name. </td>
</tr>
<tr>
<td id="Win32_SHGFI_EXETYPE">SHGFI_EXETYPE</td>
<td>u32</td>
<td>return exe type. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ICON">SHGFI_ICON</td>
<td>u32</td>
<td>get icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_ICONLOCATION">SHGFI_ICONLOCATION</td>
<td>u32</td>
<td>get icon location. </td>
</tr>
<tr>
<td id="Win32_SHGFI_LARGEICON">SHGFI_LARGEICON</td>
<td>u32</td>
<td>get large icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_LINKOVERLAY">SHGFI_LINKOVERLAY</td>
<td>u32</td>
<td>put a link overlay on icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_OPENICON">SHGFI_OPENICON</td>
<td>u32</td>
<td>get open icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_OVERLAYINDEX">SHGFI_OVERLAYINDEX</td>
<td>u32</td>
<td>Get the index of the overlay. </td>
</tr>
<tr>
<td id="Win32_SHGFI_PIDL">SHGFI_PIDL</td>
<td>u32</td>
<td>pszPath is a pidl. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SELECTED">SHGFI_SELECTED</td>
<td>u32</td>
<td>show icon in selected state. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SHELLICONSIZE">SHGFI_SHELLICONSIZE</td>
<td>u32</td>
<td>get shell size icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SMALLICON">SHGFI_SMALLICON</td>
<td>u32</td>
<td>get small icon. </td>
</tr>
<tr>
<td id="Win32_SHGFI_SYSICONINDEX">SHGFI_SYSICONINDEX</td>
<td>u32</td>
<td>get system icon index. </td>
</tr>
<tr>
<td id="Win32_SHGFI_TYPENAME">SHGFI_TYPENAME</td>
<td>u32</td>
<td>get type name. </td>
</tr>
<tr>
<td id="Win32_SHGFI_USEFILEATTRIBUTES">SHGFI_USEFILEATTRIBUTES</td>
<td>u32</td>
<td>use passed dwFileAttribute. </td>
</tr>
<tr>
<td id="Win32_SHGSI_ICON">SHGSI_ICON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_ICONLOCATION">SHGSI_ICONLOCATION</td>
<td>s32</td>
<td>you always get the icon location. </td>
</tr>
<tr>
<td id="Win32_SHGSI_LARGEICON">SHGSI_LARGEICON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_LINKOVERLAY">SHGSI_LINKOVERLAY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SELECTED">SHGSI_SELECTED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SHELLICONSIZE">SHGSI_SHELLICONSIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SMALLICON">SHGSI_SMALLICON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SHGSI_SYSICONINDEX">SHGSI_SYSICONINDEX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_ARRANGE">SM_ARRANGE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CARETBLINKINGENABLED">SM_CARETBLINKINGENABLED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CLEANBOOT">SM_CLEANBOOT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CMETRICS">SM_CMETRICS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CMONITORS">SM_CMONITORS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CMOUSEBUTTONS">SM_CMOUSEBUTTONS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CONVERTIBLESLATEMODE">SM_CONVERTIBLESLATEMODE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXBORDER">SM_CXBORDER</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXCURSOR">SM_CXCURSOR</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXDLGFRAME">SM_CXDLGFRAME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXDOUBLECLK">SM_CXDOUBLECLK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXDRAG">SM_CXDRAG</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXEDGE">SM_CXEDGE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFIXEDFRAME">SM_CXFIXEDFRAME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFOCUSBORDER">SM_CXFOCUSBORDER</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFRAME">SM_CXFRAME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXFULLSCREEN">SM_CXFULLSCREEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXHSCROLL">SM_CXHSCROLL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXHTHUMB">SM_CXHTHUMB</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXICON">SM_CXICON</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXICONSPACING">SM_CXICONSPACING</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMAXIMIZED">SM_CXMAXIMIZED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMAXTRACK">SM_CXMAXTRACK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMENUCHECK">SM_CXMENUCHECK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMENUSIZE">SM_CXMENUSIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMIN">SM_CXMIN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMINIMIZED">SM_CXMINIMIZED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMINSPACING">SM_CXMINSPACING</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXMINTRACK">SM_CXMINTRACK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXPADDEDBORDER">SM_CXPADDEDBORDER</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSCREEN">SM_CXSCREEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSIZE">SM_CXSIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSIZEFRAME">SM_CXSIZEFRAME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSMICON">SM_CXSMICON</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXSMSIZE">SM_CXSMSIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXVIRTUALSCREEN">SM_CXVIRTUALSCREEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CXVSCROLL">SM_CXVSCROLL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYBORDER">SM_CYBORDER</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYCAPTION">SM_CYCAPTION</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYCURSOR">SM_CYCURSOR</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYDLGFRAME">SM_CYDLGFRAME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYDOUBLECLK">SM_CYDOUBLECLK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYDRAG">SM_CYDRAG</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYEDGE">SM_CYEDGE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFIXEDFRAME">SM_CYFIXEDFRAME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFOCUSBORDER">SM_CYFOCUSBORDER</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFRAME">SM_CYFRAME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYFULLSCREEN">SM_CYFULLSCREEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYHSCROLL">SM_CYHSCROLL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYICON">SM_CYICON</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYICONSPACING">SM_CYICONSPACING</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYKANJIWINDOW">SM_CYKANJIWINDOW</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMAXIMIZED">SM_CYMAXIMIZED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMAXTRACK">SM_CYMAXTRACK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMENU">SM_CYMENU</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMENUCHECK">SM_CYMENUCHECK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMENUSIZE">SM_CYMENUSIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMIN">SM_CYMIN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMINIMIZED">SM_CYMINIMIZED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMINSPACING">SM_CYMINSPACING</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYMINTRACK">SM_CYMINTRACK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSCREEN">SM_CYSCREEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSIZE">SM_CYSIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSIZEFRAME">SM_CYSIZEFRAME</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSMCAPTION">SM_CYSMCAPTION</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSMICON">SM_CYSMICON</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYSMSIZE">SM_CYSMSIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYVIRTUALSCREEN">SM_CYVIRTUALSCREEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYVSCROLL">SM_CYVSCROLL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_CYVTHUMB">SM_CYVTHUMB</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_DBCSENABLED">SM_DBCSENABLED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_DEBUG">SM_DEBUG</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_DIGITIZER">SM_DIGITIZER</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_IMMENABLED">SM_IMMENABLED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MAXIMUMTOUCHES">SM_MAXIMUMTOUCHES</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MEDIACENTER">SM_MEDIACENTER</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MENUDROPALIGNMENT">SM_MENUDROPALIGNMENT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MIDEASTENABLED">SM_MIDEASTENABLED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MOUSEHORIZONTALWHEELPRESENT">SM_MOUSEHORIZONTALWHEELPRESENT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MOUSEPRESENT">SM_MOUSEPRESENT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_MOUSEWHEELPRESENT">SM_MOUSEWHEELPRESENT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_NETWORK">SM_NETWORK</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_PENWINDOWS">SM_PENWINDOWS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_REMOTECONTROL">SM_REMOTECONTROL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_REMOTESESSION">SM_REMOTESESSION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED1">SM_RESERVED1</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED2">SM_RESERVED2</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED3">SM_RESERVED3</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_RESERVED4">SM_RESERVED4</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SAMEDISPLAYFORMAT">SM_SAMEDISPLAYFORMAT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SECURE">SM_SECURE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SERVERR2">SM_SERVERR2</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SHOWSOUNDS">SM_SHOWSOUNDS</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SHUTTINGDOWN">SM_SHUTTINGDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SLOWMACHINE">SM_SLOWMACHINE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_STARTER">SM_STARTER</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SWAPBUTTON">SM_SWAPBUTTON</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_SYSTEMDOCKED">SM_SYSTEMDOCKED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_TABLETPC">SM_TABLETPC</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_XVIRTUALSCREEN">SM_XVIRTUALSCREEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SM_YVIRTUALSCREEN">SM_YVIRTUALSCREEN</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SRWLOCK_INIT">SRWLOCK_INIT</td>
<td>Win32.SRWLOCK</td>
<td></td>
</tr>
<tr>
<td id="Win32_STD_INPUT_HANDLE">STD_INPUT_HANDLE</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td id="Win32_STD_OUTPUT_HANDLE">STD_OUTPUT_HANDLE</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td id="Win32_SUBLANG_DEFAULT">SUBLANG_DEFAULT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_ASYNCWINDOWPOS">SWP_ASYNCWINDOWPOS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_DEFERERASE">SWP_DEFERERASE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_DRAWFRAME">SWP_DRAWFRAME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_FRAMECHANGED">SWP_FRAMECHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_HIDEWINDOW">SWP_HIDEWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOACTIVATE">SWP_NOACTIVATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOCOPYBITS">SWP_NOCOPYBITS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOMOVE">SWP_NOMOVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOOWNERZORDER">SWP_NOOWNERZORDER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOREDRAW">SWP_NOREDRAW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOREPOSITION">SWP_NOREPOSITION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOSENDCHANGING">SWP_NOSENDCHANGING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOSIZE">SWP_NOSIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_NOZORDER">SWP_NOZORDER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SWP_SHOWWINDOW">SWP_SHOWWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_FORCEMINIMIZE">SW_FORCEMINIMIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_HIDE">SW_HIDE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_MAX">SW_MAX</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_MAXIMIZE">SW_MAXIMIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_MINIMIZE">SW_MINIMIZE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_NORMAL">SW_NORMAL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_RESTORE">SW_RESTORE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOW">SW_SHOW</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWDEFAULT">SW_SHOWDEFAULT</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWMAXIMIZED">SW_SHOWMAXIMIZED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWMINIMIZED">SW_SHOWMINIMIZED</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWMINNOACTIVE">SW_SHOWMINNOACTIVE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWNA">SW_SHOWNA</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWNOACTIVATE">SW_SHOWNOACTIVATE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_SW_SHOWNORMAL">SW_SHOWNORMAL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_S_OK">S_OK</td>
<td>Win32.HRESULT</td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_ABOVE_NORMAL">THREAD_PRIORITY_ABOVE_NORMAL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_BELOW_NORMAL">THREAD_PRIORITY_BELOW_NORMAL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_HIGHEST">THREAD_PRIORITY_HIGHEST</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_LOWEST">THREAD_PRIORITY_LOWEST</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_NORMAL">THREAD_PRIORITY_NORMAL</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_TRUE">TRUE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_TRUNCATE_EXISTING">TRUNCATE_EXISTING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ACCEPT">VK_ACCEPT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ADD">VK_ADD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_APPS">VK_APPS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ATTN">VK_ATTN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BACK">VK_BACK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_BACK">VK_BROWSER_BACK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_FAVORITES">VK_BROWSER_FAVORITES</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_FORWARD">VK_BROWSER_FORWARD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_HOME">VK_BROWSER_HOME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_REFRESH">VK_BROWSER_REFRESH</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_SEARCH">VK_BROWSER_SEARCH</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_STOP">VK_BROWSER_STOP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CANCEL">VK_CANCEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CAPITAL">VK_CAPITAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CLEAR">VK_CLEAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CONTROL">VK_CONTROL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CONVERT">VK_CONVERT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_CRSEL">VK_CRSEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DECIMAL">VK_DECIMAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DELETE">VK_DELETE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DIVIDE">VK_DIVIDE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_DOWN">VK_DOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_END">VK_END</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_EREOF">VK_EREOF</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ESCAPE">VK_ESCAPE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_EXECUTE">VK_EXECUTE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_EXSEL">VK_EXSEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F1">VK_F1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F10">VK_F10</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F11">VK_F11</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F12">VK_F12</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F13">VK_F13</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F14">VK_F14</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F15">VK_F15</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F16">VK_F16</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F17">VK_F17</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F18">VK_F18</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F19">VK_F19</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F2">VK_F2</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F20">VK_F20</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F21">VK_F21</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F22">VK_F22</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F23">VK_F23</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F24">VK_F24</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F3">VK_F3</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F4">VK_F4</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F5">VK_F5</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F6">VK_F6</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F7">VK_F7</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F8">VK_F8</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_F9">VK_F9</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_FINAL">VK_FINAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_A">VK_GAMEPAD_A</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_B">VK_GAMEPAD_B</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_DOWN">VK_GAMEPAD_DPAD_DOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_LEFT">VK_GAMEPAD_DPAD_LEFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_RIGHT">VK_GAMEPAD_DPAD_RIGHT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_UP">VK_GAMEPAD_DPAD_UP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_SHOULDER">VK_GAMEPAD_LEFT_SHOULDER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON">VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_DOWN">VK_GAMEPAD_LEFT_THUMBSTICK_DOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_LEFT">VK_GAMEPAD_LEFT_THUMBSTICK_LEFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT">VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_UP">VK_GAMEPAD_LEFT_THUMBSTICK_UP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_TRIGGER">VK_GAMEPAD_LEFT_TRIGGER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_MENU">VK_GAMEPAD_MENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_SHOULDER">VK_GAMEPAD_RIGHT_SHOULDER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON">VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN">VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT">VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT">VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_UP">VK_GAMEPAD_RIGHT_THUMBSTICK_UP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_TRIGGER">VK_GAMEPAD_RIGHT_TRIGGER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_VIEW">VK_GAMEPAD_VIEW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_X">VK_GAMEPAD_X</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_Y">VK_GAMEPAD_Y</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HANGEUL">VK_HANGEUL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HANGUL">VK_HANGUL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HANJA">VK_HANJA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HELP">VK_HELP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_HOME">VK_HOME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ICO_00">VK_ICO_00</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ICO_CLEAR">VK_ICO_CLEAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ICO_HELP">VK_ICO_HELP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_INSERT">VK_INSERT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_JUNJA">VK_JUNJA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_KANA">VK_KANA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_KANJI">VK_KANJI</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_APP1">VK_LAUNCH_APP1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_APP2">VK_LAUNCH_APP2</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_MAIL">VK_LAUNCH_MAIL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_MEDIA_SELECT">VK_LAUNCH_MEDIA_SELECT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LBUTTON">VK_LBUTTON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LCONTROL">VK_LCONTROL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LEFT">VK_LEFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LMENU">VK_LMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LSHIFT">VK_LSHIFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_LWIN">VK_LWIN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MBUTTON">VK_MBUTTON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_NEXT_TRACK">VK_MEDIA_NEXT_TRACK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_PLAY_PAUSE">VK_MEDIA_PLAY_PAUSE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_PREV_TRACK">VK_MEDIA_PREV_TRACK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_STOP">VK_MEDIA_STOP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MENU">VK_MENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MODECHANGE">VK_MODECHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_MULTIPLY">VK_MULTIPLY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_ACCEPT">VK_NAVIGATION_ACCEPT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_CANCEL">VK_NAVIGATION_CANCEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_DOWN">VK_NAVIGATION_DOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_LEFT">VK_NAVIGATION_LEFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_MENU">VK_NAVIGATION_MENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_RIGHT">VK_NAVIGATION_RIGHT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_UP">VK_NAVIGATION_UP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_VIEW">VK_NAVIGATION_VIEW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NEXT">VK_NEXT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NONAME">VK_NONAME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NONCONVERT">VK_NONCONVERT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMLOCK">VK_NUMLOCK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD0">VK_NUMPAD0</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD1">VK_NUMPAD1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD2">VK_NUMPAD2</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD3">VK_NUMPAD3</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD4">VK_NUMPAD4</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD5">VK_NUMPAD5</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD6">VK_NUMPAD6</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD7">VK_NUMPAD7</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD8">VK_NUMPAD8</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD9">VK_NUMPAD9</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_1">VK_OEM_1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_102">VK_OEM_102</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_2">VK_OEM_2</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_3">VK_OEM_3</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_4">VK_OEM_4</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_5">VK_OEM_5</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_6">VK_OEM_6</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_7">VK_OEM_7</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_8">VK_OEM_8</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_ATTN">VK_OEM_ATTN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_AUTO">VK_OEM_AUTO</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_AX">VK_OEM_AX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_BACKTAB">VK_OEM_BACKTAB</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_CLEAR">VK_OEM_CLEAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_COMMA">VK_OEM_COMMA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_COPY">VK_OEM_COPY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_CUSEL">VK_OEM_CUSEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_ENLW">VK_OEM_ENLW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FINISH">VK_OEM_FINISH</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_JISHO">VK_OEM_FJ_JISHO</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_LOYA">VK_OEM_FJ_LOYA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_MASSHOU">VK_OEM_FJ_MASSHOU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_ROYA">VK_OEM_FJ_ROYA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_TOUROKU">VK_OEM_FJ_TOUROKU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_JUMP">VK_OEM_JUMP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_MINUS">VK_OEM_MINUS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_NEC_EQUAL">VK_OEM_NEC_EQUAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA1">VK_OEM_PA1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA2">VK_OEM_PA2</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA3">VK_OEM_PA3</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PERIOD">VK_OEM_PERIOD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_PLUS">VK_OEM_PLUS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_RESET">VK_OEM_RESET</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_OEM_WSCTRL">VK_OEM_WSCTRL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PA1">VK_PA1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PACKET">VK_PACKET</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PAUSE">VK_PAUSE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PLAY">VK_PLAY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PRINT">VK_PRINT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PRIOR">VK_PRIOR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_PROCESSKEY">VK_PROCESSKEY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RBUTTON">VK_RBUTTON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RCONTROL">VK_RCONTROL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RETURN">VK_RETURN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RIGHT">VK_RIGHT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RMENU">VK_RMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RSHIFT">VK_RSHIFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_RWIN">VK_RWIN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SCROLL">VK_SCROLL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SELECT">VK_SELECT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SEPARATOR">VK_SEPARATOR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SHIFT">VK_SHIFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SLEEP">VK_SLEEP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SNAPSHOT">VK_SNAPSHOT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SPACE">VK_SPACE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_SUBTRACT">VK_SUBTRACT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_TAB">VK_TAB</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_UP">VK_UP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_DOWN">VK_VOLUME_DOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_MUTE">VK_VOLUME_MUTE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_UP">VK_VOLUME_UP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_XBUTTON1">VK_XBUTTON1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_XBUTTON2">VK_XBUTTON2</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_VK_ZOOM">VK_ZOOM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_ABANDONED">WAIT_ABANDONED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_FAILED">WAIT_FAILED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_OBJECT_0">WAIT_OBJECT_0</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WAIT_TIMEOUT">WAIT_TIMEOUT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WA_ACTIVE">WA_ACTIVE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WA_CLICKACTIVE">WA_CLICKACTIVE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WA_INACTIVE">WA_INACTIVE</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WHEEL_DELTA">WHEEL_DELTA</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ACTIVATE">WM_ACTIVATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ACTIVATEAPP">WM_ACTIVATEAPP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_AFXFIRST">WM_AFXFIRST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_AFXLAST">WM_AFXLAST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_APP">WM_APP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_APPCOMMAND">WM_APPCOMMAND</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ASKCBFORMATNAME">WM_ASKCBFORMATNAME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CANCELJOURNAL">WM_CANCELJOURNAL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CANCELMODE">WM_CANCELMODE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CAPTURECHANGED">WM_CAPTURECHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHANGECBCHAIN">WM_CHANGECBCHAIN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHANGEUISTATE">WM_CHANGEUISTATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHAR">WM_CHAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHARTOITEM">WM_CHARTOITEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CHILDACTIVATE">WM_CHILDACTIVATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CLEAR">WM_CLEAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CLIPBOARDUPDATE">WM_CLIPBOARDUPDATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CLOSE">WM_CLOSE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMMAND">WM_COMMAND</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMMNOTIFY">WM_COMMNOTIFY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMPACTING">WM_COMPACTING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COMPAREITEM">WM_COMPAREITEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CONTEXTMENU">WM_CONTEXTMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COPY">WM_COPY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_COPYDATA">WM_COPYDATA</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CREATE">WM_CREATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORBTN">WM_CTLCOLORBTN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORDLG">WM_CTLCOLORDLG</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLOREDIT">WM_CTLCOLOREDIT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORLISTBOX">WM_CTLCOLORLISTBOX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORMSGBOX">WM_CTLCOLORMSGBOX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORSCROLLBAR">WM_CTLCOLORSCROLLBAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORSTATIC">WM_CTLCOLORSTATIC</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_CUT">WM_CUT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DEADCHAR">WM_DEADCHAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DELETEITEM">WM_DELETEITEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DESTROY">WM_DESTROY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DESTROYCLIPBOARD">WM_DESTROYCLIPBOARD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DEVICECHANGE">WM_DEVICECHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DEVMODECHANGE">WM_DEVMODECHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DISPLAYCHANGE">WM_DISPLAYCHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED">WM_DPICHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED_AFTERPARENT">WM_DPICHANGED_AFTERPARENT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED_BEFOREPARENT">WM_DPICHANGED_BEFOREPARENT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DRAWCLIPBOARD">WM_DRAWCLIPBOARD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DRAWITEM">WM_DRAWITEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DROPFILES">WM_DROPFILES</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMCOLORIZATIONCOLORCHANGED">WM_DWMCOLORIZATIONCOLORCHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMCOMPOSITIONCHANGED">WM_DWMCOMPOSITIONCHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMNCRENDERINGCHANGED">WM_DWMNCRENDERINGCHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMSENDICONICLIVEPREVIEWBITMAP">WM_DWMSENDICONICLIVEPREVIEWBITMAP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMSENDICONICTHUMBNAIL">WM_DWMSENDICONICTHUMBNAIL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_DWMWINDOWMAXIMIZEDCHANGE">WM_DWMWINDOWMAXIMIZEDCHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENABLE">WM_ENABLE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENTERIDLE">WM_ENTERIDLE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENTERMENULOOP">WM_ENTERMENULOOP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ENTERSIZEMOVE">WM_ENTERSIZEMOVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ERASEBKGND">WM_ERASEBKGND</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_EXITMENULOOP">WM_EXITMENULOOP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_EXITSIZEMOVE">WM_EXITSIZEMOVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_FONTCHANGE">WM_FONTCHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GESTURE">WM_GESTURE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GESTURENOTIFY">WM_GESTURENOTIFY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETDLGCODE">WM_GETDLGCODE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETDPISCALEDSIZE">WM_GETDPISCALEDSIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETFONT">WM_GETFONT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETHOTKEY">WM_GETHOTKEY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETICON">WM_GETICON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETMINMAXINFO">WM_GETMINMAXINFO</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETOBJECT">WM_GETOBJECT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETTEXT">WM_GETTEXT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETTEXTLENGTH">WM_GETTEXTLENGTH</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_GETTITLEBARINFOEX">WM_GETTITLEBARINFOEX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HANDHELDFIRST">WM_HANDHELDFIRST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HANDHELDLAST">WM_HANDHELDLAST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HELP">WM_HELP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HOTKEY">WM_HOTKEY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HSCROLL">WM_HSCROLL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_HSCROLLCLIPBOARD">WM_HSCROLLCLIPBOARD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_ICONERASEBKGND">WM_ICONERASEBKGND</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_CHAR">WM_IME_CHAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_COMPOSITION">WM_IME_COMPOSITION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_COMPOSITIONFULL">WM_IME_COMPOSITIONFULL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_CONTROL">WM_IME_CONTROL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_ENDCOMPOSITION">WM_IME_ENDCOMPOSITION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYDOWN">WM_IME_KEYDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYLAST">WM_IME_KEYLAST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYUP">WM_IME_KEYUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_NOTIFY">WM_IME_NOTIFY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_REQUEST">WM_IME_REQUEST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_SELECT">WM_IME_SELECT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_SETCONTEXT">WM_IME_SETCONTEXT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_IME_STARTCOMPOSITION">WM_IME_STARTCOMPOSITION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INITDIALOG">WM_INITDIALOG</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INITMENU">WM_INITMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INITMENUPOPUP">WM_INITMENUPOPUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUT">WM_INPUT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUTLANGCHANGE">WM_INPUTLANGCHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUTLANGCHANGEREQUEST">WM_INPUTLANGCHANGEREQUEST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_INPUT_DEVICE_CHANGE">WM_INPUT_DEVICE_CHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYDOWN">WM_KEYDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYFIRST">WM_KEYFIRST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYLAST">WM_KEYLAST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KEYUP">WM_KEYUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_KILLFOCUS">WM_KILLFOCUS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONDBLCLK">WM_LBUTTONDBLCLK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONDOWN">WM_LBUTTONDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONUP">WM_LBUTTONUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONDBLCLK">WM_MBUTTONDBLCLK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONDOWN">WM_MBUTTONDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONUP">WM_MBUTTONUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIACTIVATE">WM_MDIACTIVATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDICASCADE">WM_MDICASCADE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDICREATE">WM_MDICREATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIDESTROY">WM_MDIDESTROY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIGETACTIVE">WM_MDIGETACTIVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIICONARRANGE">WM_MDIICONARRANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIMAXIMIZE">WM_MDIMAXIMIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDINEXT">WM_MDINEXT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIREFRESHMENU">WM_MDIREFRESHMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDIRESTORE">WM_MDIRESTORE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDISETMENU">WM_MDISETMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MDITILE">WM_MDITILE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MEASUREITEM">WM_MEASUREITEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUCHAR">WM_MENUCHAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUCOMMAND">WM_MENUCOMMAND</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUDRAG">WM_MENUDRAG</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUGETOBJECT">WM_MENUGETOBJECT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENURBUTTONUP">WM_MENURBUTTONUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MENUSELECT">WM_MENUSELECT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEACTIVATE">WM_MOUSEACTIVATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEFIRST">WM_MOUSEFIRST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEHOVER">WM_MOUSEHOVER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEHWHEEL">WM_MOUSEHWHEEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSELAST">WM_MOUSELAST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSELEAVE">WM_MOUSELEAVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEMOVE">WM_MOUSEMOVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOUSEWHEEL">WM_MOUSEWHEEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOVE">WM_MOVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_MOVING">WM_MOVING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCACTIVATE">WM_NCACTIVATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCCALCSIZE">WM_NCCALCSIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCCREATE">WM_NCCREATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCDESTROY">WM_NCDESTROY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCHITTEST">WM_NCHITTEST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONDBLCLK">WM_NCLBUTTONDBLCLK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONDOWN">WM_NCLBUTTONDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONUP">WM_NCLBUTTONUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONDBLCLK">WM_NCMBUTTONDBLCLK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONDOWN">WM_NCMBUTTONDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONUP">WM_NCMBUTTONUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSEHOVER">WM_NCMOUSEHOVER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSELEAVE">WM_NCMOUSELEAVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSEMOVE">WM_NCMOUSEMOVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPAINT">WM_NCPAINT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERDOWN">WM_NCPOINTERDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERUP">WM_NCPOINTERUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERUPDATE">WM_NCPOINTERUPDATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONDBLCLK">WM_NCRBUTTONDBLCLK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONDOWN">WM_NCRBUTTONDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONUP">WM_NCRBUTTONUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONDBLCLK">WM_NCXBUTTONDBLCLK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONDOWN">WM_NCXBUTTONDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONUP">WM_NCXBUTTONUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NEXTDLGCTL">WM_NEXTDLGCTL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NEXTMENU">WM_NEXTMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NOTIFY">WM_NOTIFY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_NOTIFYFORMAT">WM_NOTIFYFORMAT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PAINT">WM_PAINT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PAINTCLIPBOARD">WM_PAINTCLIPBOARD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PAINTICON">WM_PAINTICON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PALETTECHANGED">WM_PALETTECHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PALETTEISCHANGING">WM_PALETTEISCHANGING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PARENTNOTIFY">WM_PARENTNOTIFY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PASTE">WM_PASTE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PENWINFIRST">WM_PENWINFIRST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PENWINLAST">WM_PENWINLAST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERACTIVATE">WM_POINTERACTIVATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERCAPTURECHANGED">WM_POINTERCAPTURECHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICECHANGE">WM_POINTERDEVICECHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICEINRANGE">WM_POINTERDEVICEINRANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICEOUTOFRANGE">WM_POINTERDEVICEOUTOFRANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERDOWN">WM_POINTERDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERENTER">WM_POINTERENTER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERHWHEEL">WM_POINTERHWHEEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERLEAVE">WM_POINTERLEAVE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDAWAY">WM_POINTERROUTEDAWAY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDRELEASED">WM_POINTERROUTEDRELEASED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDTO">WM_POINTERROUTEDTO</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERUP">WM_POINTERUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERUPDATE">WM_POINTERUPDATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POINTERWHEEL">WM_POINTERWHEEL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POWER">WM_POWER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_POWERBROADCAST">WM_POWERBROADCAST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PRINT">WM_PRINT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_PRINTCLIENT">WM_PRINTCLIENT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUERYDRAGICON">WM_QUERYDRAGICON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUERYNEWPALETTE">WM_QUERYNEWPALETTE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUERYUISTATE">WM_QUERYUISTATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUEUESYNC">WM_QUEUESYNC</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_QUIT">WM_QUIT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONDBLCLK">WM_RBUTTONDBLCLK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONDOWN">WM_RBUTTONDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONUP">WM_RBUTTONUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RENDERALLFORMATS">WM_RENDERALLFORMATS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_RENDERFORMAT">WM_RENDERFORMAT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETCURSOR">WM_SETCURSOR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETFOCUS">WM_SETFOCUS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETFONT">WM_SETFONT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETHOTKEY">WM_SETHOTKEY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETICON">WM_SETICON</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETREDRAW">WM_SETREDRAW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETTEXT">WM_SETTEXT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SETTINGCHANGE">WM_SETTINGCHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SHOWWINDOW">WM_SHOWWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SIZE">WM_SIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SIZECLIPBOARD">WM_SIZECLIPBOARD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SIZING">WM_SIZING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SPOOLERSTATUS">WM_SPOOLERSTATUS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_STYLECHANGED">WM_STYLECHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_STYLECHANGING">WM_STYLECHANGING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYNCPAINT">WM_SYNCPAINT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSCHAR">WM_SYSCHAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSCOLORCHANGE">WM_SYSCOLORCHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSCOMMAND">WM_SYSCOMMAND</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSDEADCHAR">WM_SYSDEADCHAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSKEYDOWN">WM_SYSKEYDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_SYSKEYUP">WM_SYSKEYUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TABLET_FIRST">WM_TABLET_FIRST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TABLET_LAST">WM_TABLET_LAST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TCARD">WM_TCARD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_THEMECHANGED">WM_THEMECHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TIMECHANGE">WM_TIMECHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TIMER">WM_TIMER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TOUCH">WM_TOUCH</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_TOUCHHITTESTING">WM_TOUCHHITTESTING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UNDO">WM_UNDO</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UNICHAR">WM_UNICHAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UNINITMENUPOPUP">WM_UNINITMENUPOPUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_UPDATEUISTATE">WM_UPDATEUISTATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_USER">WM_USER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_USERCHANGED">WM_USERCHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_VKEYTOITEM">WM_VKEYTOITEM</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_VSCROLL">WM_VSCROLL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_VSCROLLCLIPBOARD">WM_VSCROLLCLIPBOARD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WINDOWPOSCHANGED">WM_WINDOWPOSCHANGED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WINDOWPOSCHANGING">WM_WINDOWPOSCHANGING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WININICHANGE">WM_WININICHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_WTSSESSION_CHANGE">WM_WTSSESSION_CHANGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONDBLCLK">WM_XBUTTONDBLCLK</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONDOWN">WM_XBUTTONDOWN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONUP">WM_XBUTTONUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_BORDER">WS_BORDER</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CAPTION">WS_CAPTION</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CHILD">WS_CHILD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CLIPCHILDREN">WS_CLIPCHILDREN</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_CLIPSIBLINGS">WS_CLIPSIBLINGS</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_DISABLED">WS_DISABLED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_DLGFRAME">WS_DLGFRAME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_ACCEPTFILES">WS_EX_ACCEPTFILES</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_APPWINDOW">WS_EX_APPWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_CLIENTEDGE">WS_EX_CLIENTEDGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_COMPOSITED">WS_EX_COMPOSITED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_CONTEXTHELP">WS_EX_CONTEXTHELP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_CONTROLPARENT">WS_EX_CONTROLPARENT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_DLGMODALFRAME">WS_EX_DLGMODALFRAME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LAYERED">WS_EX_LAYERED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LAYOUTRTL">WS_EX_LAYOUTRTL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LEFT">WS_EX_LEFT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LEFTSCROLLBAR">WS_EX_LEFTSCROLLBAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_LTRREADING">WS_EX_LTRREADING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_MDICHILD">WS_EX_MDICHILD</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOACTIVATE">WS_EX_NOACTIVATE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOINHERITLAYOUT">WS_EX_NOINHERITLAYOUT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOPARENTNOTIFY">WS_EX_NOPARENTNOTIFY</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_NOREDIRECTIONBITMAP">WS_EX_NOREDIRECTIONBITMAP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_OVERLAPPEDWINDOW">WS_EX_OVERLAPPEDWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_PALETTEWINDOW">WS_EX_PALETTEWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_RIGHT">WS_EX_RIGHT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_RIGHTSCROLLBAR">WS_EX_RIGHTSCROLLBAR</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_RTLREADING">WS_EX_RTLREADING</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_STATICEDGE">WS_EX_STATICEDGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_TOOLWINDOW">WS_EX_TOOLWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_TOPMOST">WS_EX_TOPMOST</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_TRANSPARENT">WS_EX_TRANSPARENT</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_EX_WINDOWEDGE">WS_EX_WINDOWEDGE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_GROUP">WS_GROUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_HSCROLL">WS_HSCROLL</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MAXIMIZE">WS_MAXIMIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MAXIMIZEBOX">WS_MAXIMIZEBOX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MINIMIZE">WS_MINIMIZE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_MINIMIZEBOX">WS_MINIMIZEBOX</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_OVERLAPPED">WS_OVERLAPPED</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_OVERLAPPEDWINDOW">WS_OVERLAPPEDWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_POPUP">WS_POPUP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_POPUPWINDOW">WS_POPUPWINDOW</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_SYSMENU">WS_SYSMENU</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_TABSTOP">WS_TABSTOP</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_THICKFRAME">WS_THICKFRAME</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_VISIBLE">WS_VISIBLE</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td id="Win32_WS_VSCROLL">WS_VSCROLL</td>
<td>u32</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AcquireSRWLockExclusive</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AcquireSRWLockShared</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">BeginPaint</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpPaint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PAINTSTRUCT">PAINTSTRUCT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">BringWindowToTop</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<td>dwSize</td>
<td><a href="#Win32_COORD">Win32.COORD</a></td>
<td></td>
</tr>
<tr>
<td>dwCursorPosition</td>
<td><a href="#Win32_COORD">Win32.COORD</a></td>
<td></td>
</tr>
<tr>
<td>wAttributes</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>srWindow</td>
<td><a href="#Win32_SMALL_RECT">Win32.SMALL_RECT</a></td>
<td></td>
</tr>
<tr>
<td>dwMaximumWindowSize</td>
<td><a href="#Win32_COORD">Win32.COORD</a></td>
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
<td>x</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>y</td>
<td>Win32.WORD</td>
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
<td>lpCreateParams</td>
<td>Win32.LPVOID</td>
<td></td>
</tr>
<tr>
<td>hInstance</td>
<td>Win32.HINSTANCE</td>
<td></td>
</tr>
<tr>
<td>hMenu</td>
<td>Win32.HMENU</td>
<td></td>
</tr>
<tr>
<td>hwndParent</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td>cy</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>cx</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>y</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>x</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>style</td>
<td>Win32.LONG</td>
<td></td>
</tr>
<tr>
<td>lpszName</td>
<td>Win32.LPCSTR</td>
<td></td>
</tr>
<tr>
<td>lpszClass</td>
<td>Win32.LPCSTR</td>
<td></td>
</tr>
<tr>
<td>dwExStyle</td>
<td>Win32.DWORD</td>
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
<td>lpCreateParams</td>
<td>Win32.LPVOID</td>
<td></td>
</tr>
<tr>
<td>hInstance</td>
<td>Win32.HINSTANCE</td>
<td></td>
</tr>
<tr>
<td>hMenu</td>
<td>Win32.HMENU</td>
<td></td>
</tr>
<tr>
<td>hwndParent</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td>cy</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>cx</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>y</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>x</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>style</td>
<td>Win32.LONG</td>
<td></td>
</tr>
<tr>
<td>lpszName</td>
<td>Win32.LPCWSTR</td>
<td></td>
</tr>
<tr>
<td>lpszClass</td>
<td>Win32.LPCWSTR</td>
<td></td>
</tr>
<tr>
<td>dwExStyle</td>
<td>Win32.DWORD</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CancelWaitableTimer</span><span class="SyntaxCode">(hTimer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ChildWindowFromPoint</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ChildWindowFromPointEx</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">, flags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ClientToScreen</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpPoint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CloseClipboard</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CloseHandle</span><span class="SyntaxCode">(handle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CoInitializeEx</span><span class="SyntaxCode">(pvReserved: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dwCoInit: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HRESULT</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateConsoleScreenBuffer</span><span class="SyntaxCode">(dwDesiredAccess: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwShareMode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpSecurityAttributes: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpScreenBufferData: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateDirectoryA</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateDirectoryW</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateEventA</span><span class="SyntaxCode">(lpEventAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, bInitialState: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateEventW</span><span class="SyntaxCode">(lpEventAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, bInitialState: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFileA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, dwDesiredAccess: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwShareMode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwCreationDisposition: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwFlagsAndAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, hTemplateFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFileW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, dwDesiredAccess: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwShareMode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwCreationDisposition: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwFlagsAndAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, hTemplateFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateIconIndirect</span><span class="SyntaxCode">(piconinfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ICONINFO">ICONINFO</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateMutexA</span><span class="SyntaxCode">(lpMutexAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInitialOwner: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateMutexW</span><span class="SyntaxCode">(lpMutexAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInitialOwner: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateProcessA</span><span class="SyntaxCode">(lpApplicationName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpCommandLine: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpProcessAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, lpThreadAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInheritHandles: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, dwCreationFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpEnvironment: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, lpCurrentDirectory: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpStartupInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_STARTUPINFOA">STARTUPINFOA</a></span><span class="SyntaxCode">, lpProcessInformation: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PROCESS_INFORMATION">PROCESS_INFORMATION</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateProcessW</span><span class="SyntaxCode">(lpApplicationName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpCommandLine: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpProcessAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, lpThreadAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInheritHandles: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, dwCreationFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpEnvironment: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, lpCurrentDirectory: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpStartupInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_STARTUPINFOW">STARTUPINFOW</a></span><span class="SyntaxCode">, lpProcessInformation: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PROCESS_INFORMATION">PROCESS_INFORMATION</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateThread</span><span class="SyntaxCode">(lpThreadAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwStackSize: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SIZE_T</span><span class="SyntaxCode">, lpStartAddress: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPTHREAD_START_ROUTINE</span><span class="SyntaxCode">, lpParameter: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dwCreationFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpThreadId: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateTimerQueueTimer</span><span class="SyntaxCode">(phNewTimer: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, timerQueue: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, callback: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WAITORTIMERCALLBACK</span><span class="SyntaxCode">, parameter: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dueTime: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, period: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, flags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ULONG</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWaitableTimerA</span><span class="SyntaxCode">(lpTimerAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpTimerName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWaitableTimerW</span><span class="SyntaxCode">(lpTimerAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpTimerName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWindowExA</span><span class="SyntaxCode">(dwExStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, dwStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">X</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Y</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, hWndParent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, hMenu: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMENU</span><span class="SyntaxCode">, hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HINSTANCE</span><span class="SyntaxCode">, lpParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWindowExW</span><span class="SyntaxCode">(dwExStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, dwStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">X</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Y</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, hWndParent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, hMenu: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMENU</span><span class="SyntaxCode">, hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HINSTANCE</span><span class="SyntaxCode">, lpParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DebugActiveProcess</span><span class="SyntaxCode">(procId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DefWindowProcA</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, uMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WPARAM</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LRESULT</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DefWindowProcW</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, uMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WPARAM</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LRESULT</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteFileA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteFileW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteTimerQueueTimer</span><span class="SyntaxCode">(timerQueue: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, timer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, completionEvent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DestroyIcon</span><span class="SyntaxCode">(hIcon: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DestroyWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DispatchMessageA</span><span class="SyntaxCode">(lpMsg: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LRESULT</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DispatchMessageW</span><span class="SyntaxCode">(lpMsg: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LRESULT</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EmptyClipboard</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnableWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, bEnable: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EndPaint</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpPaint: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PAINTSTRUCT">PAINTSTRUCT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumDisplayMonitors</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lprcClip: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">, lpfnEnum: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dwData: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumWindows</span><span class="SyntaxCode">(lpEnumFunc: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExitProcess</span><span class="SyntaxCode">(uExitCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExtractIconExA</span><span class="SyntaxCode">(lpszFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, nIconIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, phiconLarge: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, phiconSmall: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, nIcons: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExtractIconExW</span><span class="SyntaxCode">(lpszFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, nIconIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, phiconLarge: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, phiconSmall: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, nIcons: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td>dwLowDateTime</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwHighDateTime</td>
<td>Win32.DWORD</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FileTimeToLocalFileTime</span><span class="SyntaxCode">(lpFileTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpLocalFileTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FillRect</span><span class="SyntaxCode">(hDC: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lprc: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">, hbr: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBRUSH</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindClose</span><span class="SyntaxCode">(hFindFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstFileA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAA">WIN32_FIND_DATAA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstFileW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAW">WIN32_FIND_DATAW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstVolumeA</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstVolumeW</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextFileA</span><span class="SyntaxCode">(hFindFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAA">WIN32_FIND_DATAA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextFileW</span><span class="SyntaxCode">(hFindFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAW">WIN32_FIND_DATAW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextVolumeA</span><span class="SyntaxCode">(hFindVolume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextVolumeW</span><span class="SyntaxCode">(hFindVolume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindVolumeClose</span><span class="SyntaxCode">(hFindVolume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindWindowA</span><span class="SyntaxCode">(lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindWindowW</span><span class="SyntaxCode">(lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FormatMessageA</span><span class="SyntaxCode">(dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpSource: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCVOID</span><span class="SyntaxCode">, dwMessageId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwLanguageId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, nSize: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Arguments</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.va_list)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
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
<td>GetFileExInfoStandard</td>
<td></td>
</tr>
<tr>
<td>GetFileExMaxInfoLevel</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GET_WHEEL_DELTA_WPARAM</span><span class="SyntaxCode">(wParam: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GET_X_LPARAM</span><span class="SyntaxCode">(lp: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GET_Y_LPARAM</span><span class="SyntaxCode">(lp: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)</code>
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
<td>Data1</td>
<td>u32</td>
<td></td>
</tr>
<tr>
<td>Data2</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>Data3</td>
<td>u16</td>
<td></td>
</tr>
<tr>
<td>Data4</td>
<td>[8] u8</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetActiveWindow</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetAdaptersInfo</span><span class="SyntaxCode">(adapterInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADAPTER_INFO">IP_ADAPTER_INFO</a></span><span class="SyntaxCode">, sizePointer: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ULONG</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ULONG</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetAsyncKeyState</span><span class="SyntaxCode">(vKey: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SHORT</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetClientRect</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpRect: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetClipboardData</span><span class="SyntaxCode">(uFormat: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetConsoleScreenBufferInfo</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpConsoleScreenBufferInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CONSOLE_SCREEN_BUFFER_INFO">CONSOLE_SCREEN_BUFFER_INFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentDirectoryA</span><span class="SyntaxCode">(nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentDirectoryW</span><span class="SyntaxCode">(nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentProcess</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentProcessId</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentThread</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentThreadId</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCursorPos</span><span class="SyntaxCode">(lpPoint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetDC</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetDesktopWindow</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesExA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, fInfoLevelId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></span><span class="SyntaxCode">, lpFileInformation: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesExW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, fInfoLevelId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></span><span class="SyntaxCode">, lpFileInformation: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileSize</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpFileSizeHigh: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetForegroundWindow</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFullPathNameA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, lpFilePart: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFullPathNameW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCTSTR</span><span class="SyntaxCode">, nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, lpFilePart: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetIconInfo</span><span class="SyntaxCode">(hIcon: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, piconinfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ICONINFO">ICONINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetKeyboardState</span><span class="SyntaxCode">(lpKeystate: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetLastError</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMessageA</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMessageW</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetModuleHandleA</span><span class="SyntaxCode">(lpModuleName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMODULE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetModuleHandleW</span><span class="SyntaxCode">(lpModuleName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMODULE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMonitorInfoA</span><span class="SyntaxCode">(hMonitor: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMONITOR</span><span class="SyntaxCode">, lpmi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MONITORINFO">MONITORINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMonitorInfoW</span><span class="SyntaxCode">(hMonitor: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMONITOR</span><span class="SyntaxCode">, lpmi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MONITORINFO">MONITORINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetNextWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wCmd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetProcAddress</span><span class="SyntaxCode">(hModule: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMODULE</span><span class="SyntaxCode">, lpProcName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetStdHandle</span><span class="SyntaxCode">(nStdHandle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetTopWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetVolumePathNamesForVolumeNameA</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpszVolumePathNames: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCH</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpcchReturnLength: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetVolumePathNamesForVolumeNameW</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpszVolumePathNames: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWCH</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpcchReturnLength: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, uCmd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowLongPtrA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowLongPtrW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowRect</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpRect: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowTextA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, nMaxCount: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowTextW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, nMaxCount: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalAlloc</span><span class="SyntaxCode">(uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, dwBytes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SIZE_T</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLOBAL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalLock</span><span class="SyntaxCode">(hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLOBAL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalSize</span><span class="SyntaxCode">(hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLOBAL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SIZE_T</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalUnlock</span><span class="SyntaxCode">(hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLOBAL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">HIWORD</span><span class="SyntaxCode">(l: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
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
<td>fIcon</td>
<td>Win32.BOOL</td>
<td></td>
</tr>
<tr>
<td>xHotspot</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>yHotspot</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>hbmMask</td>
<td>Win32.HBITMAP</td>
<td></td>
</tr>
<tr>
<td>hbmColor</td>
<td>Win32.HBITMAP</td>
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
<td>next</td>
<td>*Win32.IP_ADAPTER_INFO</td>
<td></td>
</tr>
<tr>
<td>comboIndex</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>adapterName</td>
<td>[260] u8</td>
<td></td>
</tr>
<tr>
<td>description</td>
<td>[132] u8</td>
<td></td>
</tr>
<tr>
<td>addressLength</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>address</td>
<td>[8] Win32.BYTE</td>
<td></td>
</tr>
<tr>
<td>index</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>type</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>dhcpEnabled</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>currentIpAddress</td>
<td>*Win32.IP_ADDR_STRING</td>
<td></td>
</tr>
<tr>
<td>ipAddressList</td>
<td><a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td></td>
</tr>
<tr>
<td>gatewayList</td>
<td><a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td></td>
</tr>
<tr>
<td>dhcpServer</td>
<td><a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td></td>
</tr>
<tr>
<td>haveWins</td>
<td>Win32.BOOL</td>
<td></td>
</tr>
<tr>
<td>primaryWinsServer</td>
<td><a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td></td>
</tr>
<tr>
<td>secondaryWinsServer</td>
<td><a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td></td>
</tr>
<tr>
<td>leaseObtained</td>
<td>Win32.time_t</td>
<td></td>
</tr>
<tr>
<td>leaseExpires</td>
<td>Win32.time_t</td>
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
<td>str</td>
<td>[16] u8</td>
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
<td>next</td>
<td>*Win32.IP_ADDR_STRING</td>
<td></td>
</tr>
<tr>
<td>ipAddress</td>
<td><a href="#Win32_IP_ADDRESS_STRING">Win32.IP_ADDRESS_STRING</a></td>
<td></td>
</tr>
<tr>
<td>ipMask</td>
<td><a href="#Win32_IP_ADDRESS_STRING">Win32.IP_ADDRESS_STRING</a></td>
<td></td>
</tr>
<tr>
<td>context</td>
<td>Win32.DWORD</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">InitializeSRWLock</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">InvalidateRect</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpRect: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">, bErase: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IsClipboardFormatAvailable</span><span class="SyntaxCode">(format: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IsDebuggerPresent</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IsWindowVisible</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LOWORD</span><span class="SyntaxCode">(l: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LoadCursorA</span><span class="SyntaxCode">(hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HINSTANCE</span><span class="SyntaxCode">, lpCursorName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HCURSOR</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LoadCursorW</span><span class="SyntaxCode">(hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HINSTANCE</span><span class="SyntaxCode">, lpCursorName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HCURSOR</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LocalFree</span><span class="SyntaxCode">(hLocal: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HLOCAL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HLOCAL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MAKEINTRESOURCEA</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MAKEINTRESOURCEW</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MAKELANGID</span><span class="SyntaxCode">(p: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WORD</span><span class="SyntaxCode">, s: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WORD</span><span class="SyntaxCode">)</code>
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
<td>ptReserved</td>
<td><a href="#Win32_POINT">Win32.POINT</a></td>
<td></td>
</tr>
<tr>
<td>ptMaxSize</td>
<td><a href="#Win32_POINT">Win32.POINT</a></td>
<td></td>
</tr>
<tr>
<td>ptMaxPosition</td>
<td><a href="#Win32_POINT">Win32.POINT</a></td>
<td></td>
</tr>
<tr>
<td>ptMinTrackSize</td>
<td><a href="#Win32_POINT">Win32.POINT</a></td>
<td></td>
</tr>
<tr>
<td>ptMaxTrackSize</td>
<td><a href="#Win32_POINT">Win32.POINT</a></td>
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
<td>cbSize</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>rcMonitor</td>
<td><a href="#Win32_RECT">Win32.RECT</a></td>
<td></td>
</tr>
<tr>
<td>rcWork</td>
<td><a href="#Win32_RECT">Win32.RECT</a></td>
<td></td>
</tr>
<tr>
<td>dwFlags</td>
<td>Win32.DWORD</td>
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
<td>hwnd</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td>message</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>wParam</td>
<td>Win32.WPARAM</td>
<td></td>
</tr>
<tr>
<td>lParam</td>
<td>Win32.LPARAM</td>
<td></td>
</tr>
<tr>
<td>time</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>pt</td>
<td><a href="#Win32_POINT">Win32.POINT</a></td>
<td></td>
</tr>
<tr>
<td>lPrivate</td>
<td>Win32.DWORD</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MapVirtualKeyA</span><span class="SyntaxCode">(uCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, uMapType: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MapVirtualKeyW</span><span class="SyntaxCode">(uCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, uMapType: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MessageBoxA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpText: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpCaption: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, uType: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MessageBoxW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpText: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpCaption: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, uType: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MonitorFromPoint</span><span class="SyntaxCode">(pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMONITOR</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MonitorFromWindow</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMONITOR</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MoveWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, bRepaint: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td>cbSize</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>hWnd</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td>uID</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>uFlags</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>uCallbackMessage</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>hIcon</td>
<td>Win32.HICON</td>
<td></td>
</tr>
<tr>
<td>szTip</td>
<td>[128] Win32.CHAR</td>
<td></td>
</tr>
<tr>
<td>dwState</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwStateMask</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>szInfo</td>
<td>[256] Win32.CHAR</td>
<td></td>
</tr>
<tr>
<td><b>using</b> DUMMYUNIONNAME</td>
<td>{uTimeout: Win32.UINT, uVersion: Win32.UINT}</td>
<td></td>
</tr>
<tr>
<td>szInfoTitle</td>
<td>[64] Win32.CHAR</td>
<td></td>
</tr>
<tr>
<td>dwInfoFlags</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>guidItem</td>
<td><a href="#Win32_GUID">Win32.GUID</a></td>
<td></td>
</tr>
<tr>
<td>hBalloonIcon</td>
<td>Win32.HICON</td>
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
<td>cbSize</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>hWnd</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td>uID</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>uFlags</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>uCallbackMessage</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>hIcon</td>
<td>Win32.HICON</td>
<td></td>
</tr>
<tr>
<td>szTip</td>
<td>[128] Win32.WCHAR</td>
<td></td>
</tr>
<tr>
<td>dwState</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwStateMask</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>szInfo</td>
<td>[256] Win32.WCHAR</td>
<td></td>
</tr>
<tr>
<td><b>using</b> DUMMYUNIONNAME</td>
<td>{uTimeout: Win32.UINT, uVersion: Win32.UINT}</td>
<td></td>
</tr>
<tr>
<td>szInfoTitle</td>
<td>[64] Win32.WCHAR</td>
<td></td>
</tr>
<tr>
<td>dwInfoFlags</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>guidItem</td>
<td><a href="#Win32_GUID">Win32.GUID</a></td>
<td></td>
</tr>
<tr>
<td>hBalloonIcon</td>
<td>Win32.HICON</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OpenClipboard</span><span class="SyntaxCode">(hWndNewOwner: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OutputDebugStringA</span><span class="SyntaxCode">(lpOutputString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OutputDebugStringW</span><span class="SyntaxCode">(lpOutputString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)</code>
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
<td>hdc</td>
<td>Win32.HDC</td>
<td></td>
</tr>
<tr>
<td>fErase</td>
<td>Win32.BOOL</td>
<td></td>
</tr>
<tr>
<td>rcPaint</td>
<td><a href="#Win32_RECT">Win32.RECT</a></td>
<td></td>
</tr>
<tr>
<td>fRestore</td>
<td>Win32.BOOL</td>
<td></td>
</tr>
<tr>
<td>fIncUpdate</td>
<td>Win32.BOOL</td>
<td></td>
</tr>
<tr>
<td>rgbReserved</td>
<td>[32] Win32.BYTE</td>
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
<td>x</td>
<td>Win32.LONG</td>
<td></td>
</tr>
<tr>
<td>y</td>
<td>Win32.LONG</td>
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
<td>hProcess</td>
<td>Win32.HANDLE</td>
<td></td>
</tr>
<tr>
<td>hThread</td>
<td>Win32.HANDLE</td>
<td></td>
</tr>
<tr>
<td>dwProcessId</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwThreadId</td>
<td>Win32.DWORD</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PeekMessageA</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wRemoveMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PeekMessageW</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wRemoveMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PostMessageA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, msg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WPARAM</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PostMessageW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, msg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WPARAM</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">QueryPerformanceCounter</span><span class="SyntaxCode">(lpPerformanceCount: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LARGE_INTEGER</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">QueryPerformanceFrequency</span><span class="SyntaxCode">(lpFrequency: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LARGE_INTEGER</span><span class="SyntaxCode">)</code>
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
<td>left</td>
<td>Win32.LONG</td>
<td></td>
</tr>
<tr>
<td>top</td>
<td>Win32.LONG</td>
<td></td>
</tr>
<tr>
<td>right</td>
<td>Win32.LONG</td>
<td></td>
</tr>
<tr>
<td>bottom</td>
<td>Win32.LONG</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReadFile</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, nNumberOfBytesToRead: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpNumberOfBytesRead: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">, lpOverlapped: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPOVERLAPPED</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClassA</span><span class="SyntaxCode">(lpWndClass: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WNDCLASSA">WNDCLASSA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ATOM</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClassW</span><span class="SyntaxCode">(lpWndClass: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WNDCLASSW">WNDCLASSW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ATOM</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClipboardFormatA</span><span class="SyntaxCode">(name: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClipboardFormatW</span><span class="SyntaxCode">(name: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterHotKey</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, id: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, fsModifiers: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, vk: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseCapture</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseDC</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, hDC: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseMutex</span><span class="SyntaxCode">(hMutex: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseSRWLockExclusive</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseSRWLockShared</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ResetEvent</span><span class="SyntaxCode">(hEvent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ResumeThread</span><span class="SyntaxCode">(hThread: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td>nLength</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>lpSecurityDescriptor</td>
<td>Win32.LPVOID</td>
<td></td>
</tr>
<tr>
<td>bInheritHandle</td>
<td>Win32.BOOL</td>
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
<td>hIcon</td>
<td>Win32.HICON</td>
<td></td>
</tr>
<tr>
<td>iIcon</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>dwAttributes</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>szDisplayName</td>
<td>[260] Win32.CHAR</td>
<td></td>
</tr>
<tr>
<td>szTypeName</td>
<td>[80] Win32.CHAR</td>
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
<td>hIcon</td>
<td>Win32.HICON</td>
<td></td>
</tr>
<tr>
<td>iIcon</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>dwAttributes</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>szDisplayName</td>
<td>[260] Win32.WCHAR</td>
<td></td>
</tr>
<tr>
<td>szTypeName</td>
<td>[80] Win32.WCHAR</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetFileInfoA</span><span class="SyntaxCode">(pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, dwFileAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, psfi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHFILEINFOA">SHFILEINFOA</a></span><span class="SyntaxCode">, cbFileInfo: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD_PTR</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetFileInfoW</span><span class="SyntaxCode">(pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, dwFileAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, psfi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHFILEINFOW">SHFILEINFOW</a></span><span class="SyntaxCode">, cbFileInfo: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD_PTR</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetSpecialFolderPathA</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, csidl: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, fCreate: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetSpecialFolderPathW</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, csidl: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, fCreate: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetStockIconInfo</span><span class="SyntaxCode">(siid: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHSTOCKICONID">SHSTOCKICONID</a></span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, psii: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHSTOCKICONINFO">SHSTOCKICONINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td>SIID_DOCNOASSOC</td>
<td>document (blank page) no associated program. </td>
</tr>
<tr>
<td>SIID_DOCASSOC</td>
<td>document with an associated program. </td>
</tr>
<tr>
<td>SIID_APPLICATION</td>
<td>generic application with no custom icon. </td>
</tr>
<tr>
<td>SIID_FOLDER</td>
<td>folder (closed). </td>
</tr>
<tr>
<td>SIID_FOLDEROPEN</td>
<td>folder (open). </td>
</tr>
<tr>
<td>SIID_DRIVE525</td>
<td>5.25" floppy disk drive. </td>
</tr>
<tr>
<td>SIID_DRIVE35</td>
<td>3.5" floppy disk drive. </td>
</tr>
<tr>
<td>SIID_DRIVEREMOVE</td>
<td>removable drive. </td>
</tr>
<tr>
<td>SIID_DRIVEFIXED</td>
<td>fixed (hard disk) drive. </td>
</tr>
<tr>
<td>SIID_DRIVENET</td>
<td>network drive. </td>
</tr>
<tr>
<td>SIID_DRIVENETDISABLED</td>
<td>disconnected network drive. </td>
</tr>
<tr>
<td>SIID_DRIVECD</td>
<td>CD drive. </td>
</tr>
<tr>
<td>SIID_DRIVERAM</td>
<td>RAM disk drive. </td>
</tr>
<tr>
<td>SIID_WORLD</td>
<td>entire network. </td>
</tr>
<tr>
<td>SIID_SERVER</td>
<td>a computer on the network. </td>
</tr>
<tr>
<td>SIID_PRINTER</td>
<td>printer. </td>
</tr>
<tr>
<td>SIID_MYNETWORK</td>
<td>My network places. </td>
</tr>
<tr>
<td>SIID_FIND</td>
<td>Find. </td>
</tr>
<tr>
<td>SIID_HELP</td>
<td>Help. </td>
</tr>
<tr>
<td>SIID_SHARE</td>
<td>overlay for shared items. </td>
</tr>
<tr>
<td>SIID_LINK</td>
<td>overlay for shortcuts to items. </td>
</tr>
<tr>
<td>SIID_SLOWFILE</td>
<td>overlay for slow items. </td>
</tr>
<tr>
<td>SIID_RECYCLER</td>
<td>empty recycle bin. </td>
</tr>
<tr>
<td>SIID_RECYCLERFULL</td>
<td>full recycle bin. </td>
</tr>
<tr>
<td>SIID_MEDIACDAUDIO</td>
<td>Audio CD Media. </td>
</tr>
<tr>
<td>SIID_LOCK</td>
<td>Security lock. </td>
</tr>
<tr>
<td>SIID_AUTOLIST</td>
<td>AutoList. </td>
</tr>
<tr>
<td>SIID_PRINTERNET</td>
<td>Network printer. </td>
</tr>
<tr>
<td>SIID_SERVERSHARE</td>
<td>Server share. </td>
</tr>
<tr>
<td>SIID_PRINTERFAX</td>
<td>Fax printer. </td>
</tr>
<tr>
<td>SIID_PRINTERFAXNET</td>
<td>Networked Fax Printer. </td>
</tr>
<tr>
<td>SIID_PRINTERFILE</td>
<td>Print to File. </td>
</tr>
<tr>
<td>SIID_STACK</td>
<td>Stack. </td>
</tr>
<tr>
<td>SIID_MEDIASVCD</td>
<td>SVCD Media. </td>
</tr>
<tr>
<td>SIID_STUFFEDFOLDER</td>
<td>Folder containing other items. </td>
</tr>
<tr>
<td>SIID_DRIVEUNKNOWN</td>
<td>Unknown drive. </td>
</tr>
<tr>
<td>SIID_DRIVEDVD</td>
<td>DVD Drive. </td>
</tr>
<tr>
<td>SIID_MEDIADVD</td>
<td>DVD Media. </td>
</tr>
<tr>
<td>SIID_MEDIADVDRAM</td>
<td>DVD-RAM Media. </td>
</tr>
<tr>
<td>SIID_MEDIADVDRW</td>
<td>DVD-RW Media. </td>
</tr>
<tr>
<td>SIID_MEDIADVDR</td>
<td>DVD-R Media. </td>
</tr>
<tr>
<td>SIID_MEDIADVDROM</td>
<td>DVD-ROM Media. </td>
</tr>
<tr>
<td>SIID_MEDIACDAUDIOPLUS</td>
<td>CD+ (Enhanced CD) Media. </td>
</tr>
<tr>
<td>SIID_MEDIACDRW</td>
<td>CD-RW Media. </td>
</tr>
<tr>
<td>SIID_MEDIACDR</td>
<td>CD-R Media. </td>
</tr>
<tr>
<td>SIID_MEDIACDBURN</td>
<td>Burning CD. </td>
</tr>
<tr>
<td>SIID_MEDIABLANKCD</td>
<td>Blank CD Media. </td>
</tr>
<tr>
<td>SIID_MEDIACDROM</td>
<td>CD-ROM Media. </td>
</tr>
<tr>
<td>SIID_AUDIOFILES</td>
<td>Audio files. </td>
</tr>
<tr>
<td>SIID_IMAGEFILES</td>
<td>Image files. </td>
</tr>
<tr>
<td>SIID_VIDEOFILES</td>
<td>Video files. </td>
</tr>
<tr>
<td>SIID_MIXEDFILES</td>
<td>Mixed files. </td>
</tr>
<tr>
<td>SIID_FOLDERBACK</td>
<td>Folder back. </td>
</tr>
<tr>
<td>SIID_FOLDERFRONT</td>
<td>Folder front. </td>
</tr>
<tr>
<td>SIID_SHIELD</td>
<td>Security shield. Use for UAC prompts only. </td>
</tr>
<tr>
<td>SIID_WARNING</td>
<td>Warning. </td>
</tr>
<tr>
<td>SIID_INFO</td>
<td>Informational. </td>
</tr>
<tr>
<td>SIID_ERROR</td>
<td>Error. </td>
</tr>
<tr>
<td>SIID_KEY</td>
<td>Key / Secure. </td>
</tr>
<tr>
<td>SIID_SOFTWARE</td>
<td>Software. </td>
</tr>
<tr>
<td>SIID_RENAME</td>
<td>Rename. </td>
</tr>
<tr>
<td>SIID_DELETE</td>
<td>Delete. </td>
</tr>
<tr>
<td>SIID_MEDIAAUDIODVD</td>
<td>Audio DVD Media. </td>
</tr>
<tr>
<td>SIID_MEDIAMOVIEDVD</td>
<td>Movie DVD Media. </td>
</tr>
<tr>
<td>SIID_MEDIAENHANCEDCD</td>
<td>Enhanced CD Media. </td>
</tr>
<tr>
<td>SIID_MEDIAENHANCEDDVD</td>
<td>Enhanced DVD Media. </td>
</tr>
<tr>
<td>SIID_MEDIAHDDVD</td>
<td>HD-DVD Media. </td>
</tr>
<tr>
<td>SIID_MEDIABLURAY</td>
<td>BluRay Media. </td>
</tr>
<tr>
<td>SIID_MEDIAVCD</td>
<td>VCD Media. </td>
</tr>
<tr>
<td>SIID_MEDIADVDPLUSR</td>
<td>DVD+R Media. </td>
</tr>
<tr>
<td>SIID_MEDIADVDPLUSRW</td>
<td>DVD+RW Media. </td>
</tr>
<tr>
<td>SIID_DESKTOPPC</td>
<td>desktop computer. </td>
</tr>
<tr>
<td>SIID_MOBILEPC</td>
<td>mobile computer (laptop/notebook). </td>
</tr>
<tr>
<td>SIID_USERS</td>
<td>users. </td>
</tr>
<tr>
<td>SIID_MEDIASMARTMEDIA</td>
<td>Smart Media. </td>
</tr>
<tr>
<td>SIID_MEDIACOMPACTFLASH</td>
<td>Compact Flash. </td>
</tr>
<tr>
<td>SIID_DEVICECELLPHONE</td>
<td>Cell phone. </td>
</tr>
<tr>
<td>SIID_DEVICECAMERA</td>
<td>Camera. </td>
</tr>
<tr>
<td>SIID_DEVICEVIDEOCAMERA</td>
<td>Video camera. </td>
</tr>
<tr>
<td>SIID_DEVICEAUDIOPLAYER</td>
<td>Audio player. </td>
</tr>
<tr>
<td>SIID_NETWORKCONNECT</td>
<td>Connect to network. </td>
</tr>
<tr>
<td>SIID_INTERNET</td>
<td>Internet. </td>
</tr>
<tr>
<td>SIID_ZIPFILE</td>
<td>ZIP file. </td>
</tr>
<tr>
<td>SIID_SETTINGS</td>
<td>Settings  107-131 are internal Vista RTM icons  132-159 for SP1 icons. </td>
</tr>
<tr>
<td>SIID_DRIVEHDDVD</td>
<td>HDDVD Drive (all types). </td>
</tr>
<tr>
<td>SIID_DRIVEBD</td>
<td>BluRay Drive (all types). </td>
</tr>
<tr>
<td>SIID_MEDIAHDDVDROM</td>
<td>HDDVD-ROM Media. </td>
</tr>
<tr>
<td>SIID_MEDIAHDDVDR</td>
<td>HDDVD-R Media. </td>
</tr>
<tr>
<td>SIID_MEDIAHDDVDRAM</td>
<td>HDDVD-RAM Media. </td>
</tr>
<tr>
<td>SIID_MEDIABDROM</td>
<td>BluRay ROM Media. </td>
</tr>
<tr>
<td>SIID_MEDIABDR</td>
<td>BluRay R Media. </td>
</tr>
<tr>
<td>SIID_MEDIABDRE</td>
<td>BluRay RE Media (Rewriable and RAM). </td>
</tr>
<tr>
<td>SIID_CLUSTEREDDRIVE</td>
<td>Clustered disk  160+ are for Windows 7 icons. </td>
</tr>
<tr>
<td>SIID_MAX_ICONS</td>
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
<td>cbSize</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>hIcon</td>
<td>Win32.HICON</td>
<td></td>
</tr>
<tr>
<td>iSysImageIndex</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>iIcon</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>szPath</td>
<td>[260] Win32.WCHAR</td>
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
<td>left</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>top</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>right</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>bottom</td>
<td>Win32.WORD</td>
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
<td>cb</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>lpReserved</td>
<td>Win32.LPSTR</td>
<td></td>
</tr>
<tr>
<td>lpDesktop</td>
<td>Win32.LPSTR</td>
<td></td>
</tr>
<tr>
<td>lpTitle</td>
<td>Win32.LPSTR</td>
<td></td>
</tr>
<tr>
<td>dwX</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwY</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwXSize</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwYSize</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwXCountChars</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwYCountChars</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwFillAttribute</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwFlags</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>wShowWindow</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>cbReserved2</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>lpReserved2</td>
<td>Win32.LPBYTE</td>
<td></td>
</tr>
<tr>
<td>hStdInput</td>
<td>Win32.HANDLE</td>
<td></td>
</tr>
<tr>
<td>hStdOutput</td>
<td>Win32.HANDLE</td>
<td></td>
</tr>
<tr>
<td>hStdError</td>
<td>Win32.HANDLE</td>
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
<td>cb</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>lpReserved</td>
<td>Win32.LPWSTR</td>
<td></td>
</tr>
<tr>
<td>lpDesktop</td>
<td>Win32.LPWSTR</td>
<td></td>
</tr>
<tr>
<td>lpTitle</td>
<td>Win32.LPWSTR</td>
<td></td>
</tr>
<tr>
<td>dwX</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwY</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwXSize</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwYSize</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwXCountChars</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwYCountChars</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwFillAttribute</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwFlags</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>wShowWindow</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>cbReserved2</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>lpReserved2</td>
<td>Win32.LPBYTE</td>
<td></td>
</tr>
<tr>
<td>hStdInput</td>
<td>Win32.HANDLE</td>
<td></td>
</tr>
<tr>
<td>hStdOutput</td>
<td>Win32.HANDLE</td>
<td></td>
</tr>
<tr>
<td>hStdError</td>
<td>Win32.HANDLE</td>
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
<td>wYear</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>wMonth</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>wDayOfWeek</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>wDay</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>wHour</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>wMinute</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>wSecond</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>wMilliseconds</td>
<td>Win32.WORD</td>
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
<td>dwOemId</td>
<td>Win32.DWORD</td>
<td>Must be updated to an union. </td>
</tr>
<tr>
<td>dwPageSize</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>lpMinimumApplicationAddress</td>
<td>Win32.LPVOID</td>
<td></td>
</tr>
<tr>
<td>lpMaximumApplicationAddress</td>
<td>Win32.LPVOID</td>
<td></td>
</tr>
<tr>
<td>dwActiveProcessorMask</td>
<td>Win32.DWORD_PTR</td>
<td></td>
</tr>
<tr>
<td>dwNumberOfProcessors</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwProcessorType</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwAllocationGranularity</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>wProcessorLevel</td>
<td>Win32.WORD</td>
<td></td>
</tr>
<tr>
<td>wProcessorRevision</td>
<td>Win32.WORD</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ScreenToClient</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpPoint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetActiveWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCapture</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetClipboardData</span><span class="SyntaxCode">(uFormat: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetConsoleActiveScreenBuffer</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetConsoleOutputCP</span><span class="SyntaxCode">(codePage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetConsoleTextAttribute</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, wAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCurrentDirectoryA</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCurrentDirectoryW</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCursor</span><span class="SyntaxCode">(hCursor: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HCURSOR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HCURSOR</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetEvent</span><span class="SyntaxCode">(hEvent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetFilePointer</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lDistanceToMove: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">, lpDistanceToMoveHigh: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">PLONG</span><span class="SyntaxCode">, dwMoveMethod: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetFileTime</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpCreationTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpLastAccessTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpLastWriteTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetFocus</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetLayeredWindowAttributes</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, crKey: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">, bAlpha: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BYTE</span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetThreadPriority</span><span class="SyntaxCode">(hThread: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, nPriority: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWaitableTimer</span><span class="SyntaxCode">(hTimer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpDueTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LARGE_INTEGER</span><span class="SyntaxCode">, lPeriod: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">, pfnCompletionRoutine: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, lpArgToCompletionRoutine: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, fResume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWindowLongPtrA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, dwNewLong: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWindowLongPtrW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, dwNewLong: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWindowPos</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, hWndInsertAfter: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cx: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cy: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Shell_NotifyIconA</span><span class="SyntaxCode">(dwMessage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_NOTIFYICONDATAA">NOTIFYICONDATAA</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Shell_NotifyIconW</span><span class="SyntaxCode">(dwMessage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_NOTIFYICONDATAW">NOTIFYICONDATAW</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ShowCursor</span><span class="SyntaxCode">(show: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ShowWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nCmdShow: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Sleep</span><span class="SyntaxCode">(dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SleepEx</span><span class="SyntaxCode">(dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, bAltertable: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)</code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SwitchToThread</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ToUnicode</span><span class="SyntaxCode">(wVirtKey: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wScanCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, lpKeyState: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BYTE</span><span class="SyntaxCode">, pwszBuff: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, cchBuff: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, wFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TranslateMessage</span><span class="SyntaxCode">(lpMsg: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TryAcquireSRWLockExclusive</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TryAcquireSRWLockShared</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">UnregisterHotKey</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, id: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">UpdateWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<td>dwFileAttributes</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>ftCreationTime</td>
<td><a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td></td>
</tr>
<tr>
<td>ftLastAccessTime</td>
<td><a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td></td>
</tr>
<tr>
<td>ftLastWriteTime</td>
<td><a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td></td>
</tr>
<tr>
<td>nFileSizeHigh</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>nFileSizeLow</td>
<td>Win32.DWORD</td>
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
<td>dwFileAttributes</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>ftCreationTime</td>
<td><a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td></td>
</tr>
<tr>
<td>ftLastAccessTime</td>
<td><a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td></td>
</tr>
<tr>
<td>ftLastWriteTime</td>
<td><a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td></td>
</tr>
<tr>
<td>nFileSizeHigh</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>nFileSizeLow</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwReserved0</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwReserved1</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>cFileName</td>
<td>[260] Win32.CHAR</td>
<td></td>
</tr>
<tr>
<td>cAlternateFileName</td>
<td>[14] Win32.CHAR</td>
<td></td>
</tr>
<tr>
<td>dwFileType</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwCreatorType</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>wFinderFlags</td>
<td>Win32.WORD</td>
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
<td>dwFileAttributes</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>ftCreationTime</td>
<td><a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td></td>
</tr>
<tr>
<td>ftLastAccessTime</td>
<td><a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td></td>
</tr>
<tr>
<td>ftLastWriteTime</td>
<td><a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td></td>
</tr>
<tr>
<td>nFileSizeHigh</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>nFileSizeLow</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwReserved0</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwReserved1</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>cFileName</td>
<td>[260] Win32.WCHAR</td>
<td></td>
</tr>
<tr>
<td>cAlternateFileName</td>
<td>[14] Win32.WCHAR</td>
<td></td>
</tr>
<tr>
<td>dwFileType</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>dwCreatorType</td>
<td>Win32.DWORD</td>
<td></td>
</tr>
<tr>
<td>wFinderFlags</td>
<td>Win32.WORD</td>
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
<td>hwnd</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td>hwndInsertAfter</td>
<td>Win32.HWND</td>
<td></td>
</tr>
<tr>
<td>x</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>y</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>cx</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>cy</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>flags</td>
<td>Win32.UINT</td>
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
<td>style</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>lpfnWndProc</td>
<td>const *void</td>
<td></td>
</tr>
<tr>
<td>cbClsExtra</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>cbWndExtra</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>hInstance</td>
<td>Win32.HINSTANCE</td>
<td></td>
</tr>
<tr>
<td>hIcon</td>
<td>Win32.HICON</td>
<td></td>
</tr>
<tr>
<td>hCursor</td>
<td>Win32.HCURSOR</td>
<td></td>
</tr>
<tr>
<td>hbrBackground</td>
<td>Win32.HBRUSH</td>
<td></td>
</tr>
<tr>
<td>lpszMenuName</td>
<td>Win32.LPCSTR</td>
<td></td>
</tr>
<tr>
<td>lpszClassName</td>
<td>Win32.LPCSTR</td>
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
<td>style</td>
<td>Win32.UINT</td>
<td></td>
</tr>
<tr>
<td>lpfnWndProc</td>
<td>const *void</td>
<td></td>
</tr>
<tr>
<td>cbClsExtra</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>cbWndExtra</td>
<td>s32</td>
<td></td>
</tr>
<tr>
<td>hInstance</td>
<td>Win32.HINSTANCE</td>
<td></td>
</tr>
<tr>
<td>hIcon</td>
<td>Win32.HICON</td>
<td></td>
</tr>
<tr>
<td>hCursor</td>
<td>Win32.HCURSOR</td>
<td></td>
</tr>
<tr>
<td>hbrBackground</td>
<td>Win32.HBRUSH</td>
<td></td>
</tr>
<tr>
<td>lpszMenuName</td>
<td>Win32.LPCWSTR</td>
<td></td>
</tr>
<tr>
<td>lpszClassName</td>
<td>Win32.LPCWSTR</td>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WaitForMultipleObjects</span><span class="SyntaxCode">(nCount: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpHandles: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, bWaitAll: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WaitForSingleObject</span><span class="SyntaxCode">(hHandle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WindowFromPoint</span><span class="SyntaxCode">(pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WriteConsoleOutputCharacterA</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpCharacter: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, nLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwWriteCoord: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpNumberOfCharsWritten: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WriteConsoleOutputCharacterW</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpCharacter: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCTSTR</span><span class="SyntaxCode">, nLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwWriteCoord: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpNumberOfCharsWritten: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
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
<div class="precode"><code></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WriteFile</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCVOID</span><span class="SyntaxCode">, nNumberOfBytesToWrite: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpNumberOfBytesWritten: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">, lpOverlapped: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPOVERLAPPED</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
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
