<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<style>

    .container {
        display:        flex;
        flex-wrap:      nowrap;
        flex-direction: row;
        height:         100%;
        line-height:    1.3em;
        font-family:    Segoe UI;
    }
    .left {
        display:    block;
        overflow-y: scroll;
        width:      650px;
        height:     98vh;
    }
    .right {
        display:     block;
        overflow-y:  scroll;
        width:       100%;
        height:      98vh;
    }
    .page {
        width:  1000px;
        margin: 0px auto;
    }
    .container blockquote {
        background-color:   LightYellow;
        border-left:        6px solid Orange;
        padding:            10px;
        width:              90%;
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
        width:              94%;
        margin-left:        20px;
    }
    .SyntaxCode      { color: #7f7f7f; }
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
<h1>Module win32</h1>
<h2>Structs</h2>
<h3></h3>
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
<h2>Enums</h2>
<h3></h3>
<ul>
<li><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></li>
<li><a href="#Win32_SHSTOCKICONID">SHSTOCKICONID</a></li>
</ul>
<h2>Constants</h2>
<h3></h3>
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
<h2>Functions</h2>
<h3></h3>
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
<div class="right">
<div class="page">
<blockquote>
<b>Work in progress</b>. Generated documentation (swag doc 0.22.0)</blockquote>
<h1>Overview</h1>
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
<td id="Win32_BACKGROUND_BLUE" class="tdname">
BACKGROUND_BLUE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_BACKGROUND_GREEN" class="tdname">
BACKGROUND_GREEN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_BACKGROUND_INTENSITY" class="tdname">
BACKGROUND_INTENSITY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_BACKGROUND_MASK" class="tdname">
BACKGROUND_MASK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_BACKGROUND_RED" class="tdname">
BACKGROUND_RED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_BITMAP" class="tdname">
CF_BITMAP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_DIB" class="tdname">
CF_DIB</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_DIBV5" class="tdname">
CF_DIBV5</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_DIF" class="tdname">
CF_DIF</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_ENHMETAFILE" class="tdname">
CF_ENHMETAFILE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_HDROP" class="tdname">
CF_HDROP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_LOCALE" class="tdname">
CF_LOCALE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_METAFILEPICT" class="tdname">
CF_METAFILEPICT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_OEMTEXT" class="tdname">
CF_OEMTEXT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_PALETTE" class="tdname">
CF_PALETTE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_PENDATA" class="tdname">
CF_PENDATA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_RIFF" class="tdname">
CF_RIFF</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_SYLK" class="tdname">
CF_SYLK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_TEXT" class="tdname">
CF_TEXT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_TIFF" class="tdname">
CF_TIFF</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_UNICODETEXT" class="tdname">
CF_UNICODETEXT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CF_WAVE" class="tdname">
CF_WAVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CONSOLE_TEXTMODE_BUFFER" class="tdname">
CONSOLE_TEXTMODE_BUFFER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CREATE_ALWAYS" class="tdname">
CREATE_ALWAYS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CREATE_NEW" class="tdname">
CREATE_NEW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CREATE_SUSPENDED" class="tdname">
CREATE_SUSPENDED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CSIDL_ADMINTOOLS" class="tdname">
CSIDL_ADMINTOOLS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\Start Menu\Programs\Administrative Tools. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_ALTSTARTUP" class="tdname">
CSIDL_ALTSTARTUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>non localized startup. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_APPDATA" class="tdname">
CSIDL_APPDATA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\Application Data. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_BITBUCKET" class="tdname">
CSIDL_BITBUCKET</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;desktop&gt;\Recycle Bin. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_CDBURN_AREA" class="tdname">
CSIDL_CDBURN_AREA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>USERPROFILE\Local Settings\Application Data\Microsoft\CD Burning. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_ADMINTOOLS" class="tdname">
CSIDL_COMMON_ADMINTOOLS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\Start Menu\Programs\Administrative Tools. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_ALTSTARTUP" class="tdname">
CSIDL_COMMON_ALTSTARTUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>non localized common startup. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_APPDATA" class="tdname">
CSIDL_COMMON_APPDATA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\Application Data. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_DESKTOPDIRECTORY" class="tdname">
CSIDL_COMMON_DESKTOPDIRECTORY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\Desktop. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_DOCUMENTS" class="tdname">
CSIDL_COMMON_DOCUMENTS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\Documents. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_FAVORITES" class="tdname">
CSIDL_COMMON_FAVORITES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_MUSIC" class="tdname">
CSIDL_COMMON_MUSIC</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\My Music. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_OEM_LINKS" class="tdname">
CSIDL_COMMON_OEM_LINKS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Links to All Users OEM specific apps. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_PICTURES" class="tdname">
CSIDL_COMMON_PICTURES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\My Pictures. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_PROGRAMS" class="tdname">
CSIDL_COMMON_PROGRAMS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\Start Menu\Programs. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_STARTMENU" class="tdname">
CSIDL_COMMON_STARTMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\Start Menu. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_STARTUP" class="tdname">
CSIDL_COMMON_STARTUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\Startup. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_TEMPLATES" class="tdname">
CSIDL_COMMON_TEMPLATES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\Templates. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMMON_VIDEO" class="tdname">
CSIDL_COMMON_VIDEO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>All Users\My Video. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COMPUTERSNEARME" class="tdname">
CSIDL_COMPUTERSNEARME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Computers Near Me (computered from Workgroup membership). </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_CONNECTIONS" class="tdname">
CSIDL_CONNECTIONS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Network and Dial-up Connections. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_CONTROLS" class="tdname">
CSIDL_CONTROLS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>My Computer\Control Panel. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_COOKIES" class="tdname">
CSIDL_COOKIES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CSIDL_DESKTOP" class="tdname">
CSIDL_DESKTOP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;desktop&gt;. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_DESKTOPDIRECTORY" class="tdname">
CSIDL_DESKTOPDIRECTORY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\Desktop. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_DRIVES" class="tdname">
CSIDL_DRIVES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>My Computer. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_FAVORITES" class="tdname">
CSIDL_FAVORITES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\Favorites. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_CREATE" class="tdname">
CSIDL_FLAG_CREATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>combine with CSIDL_ value to force folder creation in SHGetFolderPath(). </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_DONT_UNEXPAND" class="tdname">
CSIDL_FLAG_DONT_UNEXPAND</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>combine with CSIDL_ value to avoid unexpanding environment variables. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_DONT_VERIFY" class="tdname">
CSIDL_FLAG_DONT_VERIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>combine with CSIDL_ value to return an unverified folder path. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_MASK" class="tdname">
CSIDL_FLAG_MASK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>mask for all possible flag values. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_NO_ALIAS" class="tdname">
CSIDL_FLAG_NO_ALIAS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>combine with CSIDL_ value to insure non-typealias versions of the pidl. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_FLAG_PER_USER_INIT" class="tdname">
CSIDL_FLAG_PER_USER_INIT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>combine with CSIDL_ value to indicate per-user init (eg. upgrade). </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_FONTS" class="tdname">
CSIDL_FONTS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>windows\fonts. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_HISTORY" class="tdname">
CSIDL_HISTORY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CSIDL_INTERNET" class="tdname">
CSIDL_INTERNET</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Internet Explorer (icon on desktop). </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_INTERNET_CACHE" class="tdname">
CSIDL_INTERNET_CACHE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CSIDL_LOCAL_APPDATA" class="tdname">
CSIDL_LOCAL_APPDATA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\Local Settings\Applicaiton Data (non roaming). </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_MYDOCUMENTS" class="tdname">
CSIDL_MYDOCUMENTS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Personal was just a silly name for My Documents. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_MYMUSIC" class="tdname">
CSIDL_MYMUSIC</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>"My Music" folder. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_MYPICTURES" class="tdname">
CSIDL_MYPICTURES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>C:\Program Files\My Pictures. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_MYVIDEO" class="tdname">
CSIDL_MYVIDEO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>"My Videos" folder. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_NETHOOD" class="tdname">
CSIDL_NETHOOD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\nethood. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_NETWORK" class="tdname">
CSIDL_NETWORK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Network Neighborhood (My Network Places). </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_PERSONAL" class="tdname">
CSIDL_PERSONAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>My Documents. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_PRINTERS" class="tdname">
CSIDL_PRINTERS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>My Computer\Printers. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_PRINTHOOD" class="tdname">
CSIDL_PRINTHOOD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\PrintHood. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_PROFILE" class="tdname">
CSIDL_PROFILE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>USERPROFILE. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAMS" class="tdname">
CSIDL_PROGRAMS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Start Menu\Programs. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES" class="tdname">
CSIDL_PROGRAM_FILES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>C:\Program Files. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILESX86" class="tdname">
CSIDL_PROGRAM_FILESX86</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>x86 C:\Program Files on RISC. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES_COMMON" class="tdname">
CSIDL_PROGRAM_FILES_COMMON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>C:\Program Files\Common. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_PROGRAM_FILES_COMMONX86" class="tdname">
CSIDL_PROGRAM_FILES_COMMONX86</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>x86 Program Files\Common on RISC. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_RECENT" class="tdname">
CSIDL_RECENT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\Recent. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_RESOURCES" class="tdname">
CSIDL_RESOURCES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Resource Direcotry. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_RESOURCES_LOCALIZED" class="tdname">
CSIDL_RESOURCES_LOCALIZED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Localized Resource Direcotry. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_SENDTO" class="tdname">
CSIDL_SENDTO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\SendTo. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_STARTMENU" class="tdname">
CSIDL_STARTMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>&lt;user name&gt;\Start Menu. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_STARTUP" class="tdname">
CSIDL_STARTUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Start Menu\Programs\Startup. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_SYSTEM" class="tdname">
CSIDL_SYSTEM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>GetSystemDirectory(). </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_SYSTEMX86" class="tdname">
CSIDL_SYSTEMX86</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>x86 system directory on RISC. </p>
</td>
</tr>
<tr>
<td id="Win32_CSIDL_TEMPLATES" class="tdname">
CSIDL_TEMPLATES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CSIDL_WINDOWS" class="tdname">
CSIDL_WINDOWS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>GetWindowsDirectory(). </p>
</td>
</tr>
<tr>
<td id="Win32_CS_BYTEALIGNCLIENT" class="tdname">
CS_BYTEALIGNCLIENT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_BYTEALIGNWINDOW" class="tdname">
CS_BYTEALIGNWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_CLASSDC" class="tdname">
CS_CLASSDC</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_DBLCLKS" class="tdname">
CS_DBLCLKS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_DROPSHADOW" class="tdname">
CS_DROPSHADOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_GLOBALCLASS" class="tdname">
CS_GLOBALCLASS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_HREDRAW" class="tdname">
CS_HREDRAW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_IME" class="tdname">
CS_IME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_NOCLOSE" class="tdname">
CS_NOCLOSE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_OWNDC" class="tdname">
CS_OWNDC</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_PARENTDC" class="tdname">
CS_PARENTDC</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_SAVEBITS" class="tdname">
CS_SAVEBITS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CS_VREDRAW" class="tdname">
CS_VREDRAW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CWP_ALL" class="tdname">
CWP_ALL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CWP_SKIPDISABLED" class="tdname">
CWP_SKIPDISABLED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CWP_SKIPINVISIBLE" class="tdname">
CWP_SKIPINVISIBLE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CWP_SKIPTRANSPARENT" class="tdname">
CWP_SKIPTRANSPARENT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_CW_USEDEFAULT" class="tdname">
CW_USEDEFAULT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_DEFAULT_MINIMUM_ENTITIES" class="tdname">
DEFAULT_MINIMUM_ENTITIES</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_DM_POINTERHITTEST" class="tdname">
DM_POINTERHITTEST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_ERROR_DEVICE_NOT_CONNECTED" class="tdname">
ERROR_DEVICE_NOT_CONNECTED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_ERROR_FILE_NOT_FOUND" class="tdname">
ERROR_FILE_NOT_FOUND</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_ERROR_IO_PENDING" class="tdname">
ERROR_IO_PENDING</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_ERROR_NO_MORE_FILES" class="tdname">
ERROR_NO_MORE_FILES</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_ERROR_SUCCESS" class="tdname">
ERROR_SUCCESS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FALSE" class="tdname">
FALSE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_ARCHIVE" class="tdname">
FILE_ATTRIBUTE_ARCHIVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_COMPRESSED" class="tdname">
FILE_ATTRIBUTE_COMPRESSED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_DEVICE" class="tdname">
FILE_ATTRIBUTE_DEVICE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_DIRECTORY" class="tdname">
FILE_ATTRIBUTE_DIRECTORY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_EA" class="tdname">
FILE_ATTRIBUTE_EA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_ENCRYPTED" class="tdname">
FILE_ATTRIBUTE_ENCRYPTED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_HIDDEN" class="tdname">
FILE_ATTRIBUTE_HIDDEN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_INTEGRITY_STREAM" class="tdname">
FILE_ATTRIBUTE_INTEGRITY_STREAM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NORMAL" class="tdname">
FILE_ATTRIBUTE_NORMAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NOT_CONTENT_INDEXED" class="tdname">
FILE_ATTRIBUTE_NOT_CONTENT_INDEXED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_NO_SCRUB_DATA" class="tdname">
FILE_ATTRIBUTE_NO_SCRUB_DATA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_OFFLINE" class="tdname">
FILE_ATTRIBUTE_OFFLINE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_PINNED" class="tdname">
FILE_ATTRIBUTE_PINNED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_READONLY" class="tdname">
FILE_ATTRIBUTE_READONLY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS" class="tdname">
FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_RECALL_ON_OPEN" class="tdname">
FILE_ATTRIBUTE_RECALL_ON_OPEN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_REPARSE_POINT" class="tdname">
FILE_ATTRIBUTE_REPARSE_POINT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_SPARSE_FILE" class="tdname">
FILE_ATTRIBUTE_SPARSE_FILE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_SYSTEM" class="tdname">
FILE_ATTRIBUTE_SYSTEM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_TEMPORARY" class="tdname">
FILE_ATTRIBUTE_TEMPORARY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_UNPINNED" class="tdname">
FILE_ATTRIBUTE_UNPINNED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_ATTRIBUTE_VIRTUAL" class="tdname">
FILE_ATTRIBUTE_VIRTUAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_BEGIN" class="tdname">
FILE_BEGIN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_CURRENT" class="tdname">
FILE_CURRENT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_END" class="tdname">
FILE_END</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_READ_ATTRIBUTES" class="tdname">
FILE_READ_ATTRIBUTES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_DELETE" class="tdname">
FILE_SHARE_DELETE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_READ" class="tdname">
FILE_SHARE_READ</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_SHARE_WRITE" class="tdname">
FILE_SHARE_WRITE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FILE_WRITE_ATTRIBUTES" class="tdname">
FILE_WRITE_ATTRIBUTES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FOREGROUND_BLUE" class="tdname">
FOREGROUND_BLUE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FOREGROUND_GREEN" class="tdname">
FOREGROUND_GREEN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FOREGROUND_INTENSITY" class="tdname">
FOREGROUND_INTENSITY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FOREGROUND_MASK" class="tdname">
FOREGROUND_MASK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FOREGROUND_RED" class="tdname">
FOREGROUND_RED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_ALLOCATE_BUFFER" class="tdname">
FORMAT_MESSAGE_ALLOCATE_BUFFER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_ARGUMENT_ARRAY" class="tdname">
FORMAT_MESSAGE_ARGUMENT_ARRAY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_HMODULE" class="tdname">
FORMAT_MESSAGE_FROM_HMODULE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_STRING" class="tdname">
FORMAT_MESSAGE_FROM_STRING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_FROM_SYSTEM" class="tdname">
FORMAT_MESSAGE_FROM_SYSTEM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_IGNORE_INSERTS" class="tdname">
FORMAT_MESSAGE_IGNORE_INSERTS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_FORMAT_MESSAGE_MAX_WIDTH_MASK" class="tdname">
FORMAT_MESSAGE_MAX_WIDTH_MASK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GENERIC_ALL" class="tdname">
GENERIC_ALL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GENERIC_EXECUTE" class="tdname">
GENERIC_EXECUTE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GENERIC_READ" class="tdname">
GENERIC_READ</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GENERIC_WRITE" class="tdname">
GENERIC_WRITE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_DDESHARE" class="tdname">
GMEM_DDESHARE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_DISCARDABLE" class="tdname">
GMEM_DISCARDABLE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_FIXED" class="tdname">
GMEM_FIXED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_INVALID_HANDLE" class="tdname">
GMEM_INVALID_HANDLE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_LOWER" class="tdname">
GMEM_LOWER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_MODIFY" class="tdname">
GMEM_MODIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_MOVEABLE" class="tdname">
GMEM_MOVEABLE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_NOCOMPACT" class="tdname">
GMEM_NOCOMPACT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_NODISCARD" class="tdname">
GMEM_NODISCARD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_NOTIFY" class="tdname">
GMEM_NOTIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_NOT_BANKED" class="tdname">
GMEM_NOT_BANKED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_SHARE" class="tdname">
GMEM_SHARE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_VALID_FLAGS" class="tdname">
GMEM_VALID_FLAGS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GMEM_ZEROINIT" class="tdname">
GMEM_ZEROINIT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GWLP_HINSTANCE" class="tdname">
GWLP_HINSTANCE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GWLP_HWNDPARENT" class="tdname">
GWLP_HWNDPARENT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GWLP_ID" class="tdname">
GWLP_ID</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GWLP_USERDATA" class="tdname">
GWLP_USERDATA</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GWLP_WNDPROC" class="tdname">
GWLP_WNDPROC</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GW_CHILD" class="tdname">
GW_CHILD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GW_ENABLEDPOPUP" class="tdname">
GW_ENABLEDPOPUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GW_HWNDFIRST" class="tdname">
GW_HWNDFIRST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GW_HWNDLAST" class="tdname">
GW_HWNDLAST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GW_HWNDNEXT" class="tdname">
GW_HWNDNEXT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GW_HWNDPREV" class="tdname">
GW_HWNDPREV</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GW_MAX" class="tdname">
GW_MAX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_GW_OWNER" class="tdname">
GW_OWNER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_HWND_BOTTOM" class="tdname">
HWND_BOTTOM</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_HWND_NOTOPMOST" class="tdname">
HWND_NOTOPMOST</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_HWND_TOP" class="tdname">
HWND_TOP</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_HWND_TOPMOST" class="tdname">
HWND_TOPMOST</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDABORT" class="tdname">
IDABORT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDCANCEL" class="tdname">
IDCANCEL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDCLOSE" class="tdname">
IDCLOSE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDCONTINUE" class="tdname">
IDCONTINUE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_APPSTARTING" class="tdname">
IDC_APPSTARTING</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_ARROW" class="tdname">
IDC_ARROW</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_CROSS" class="tdname">
IDC_CROSS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_HAND" class="tdname">
IDC_HAND</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_HELP" class="tdname">
IDC_HELP</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_IBEAM" class="tdname">
IDC_IBEAM</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_ICON" class="tdname">
IDC_ICON</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_NO" class="tdname">
IDC_NO</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_PERSON" class="tdname">
IDC_PERSON</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_PIN" class="tdname">
IDC_PIN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_SIZE" class="tdname">
IDC_SIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_SIZEALL" class="tdname">
IDC_SIZEALL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_SIZENESW" class="tdname">
IDC_SIZENESW</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_SIZENS" class="tdname">
IDC_SIZENS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_SIZENWSE" class="tdname">
IDC_SIZENWSE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_SIZEWE" class="tdname">
IDC_SIZEWE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_UPARROW" class="tdname">
IDC_UPARROW</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDC_WAIT" class="tdname">
IDC_WAIT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDHELP" class="tdname">
IDHELP</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDIGNORE" class="tdname">
IDIGNORE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDNO" class="tdname">
IDNO</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDOK" class="tdname">
IDOK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDRETRY" class="tdname">
IDRETRY</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDTIMEOUT" class="tdname">
IDTIMEOUT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDTRYAGAIN" class="tdname">
IDTRYAGAIN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_IDYES" class="tdname">
IDYES</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_INFINITE" class="tdname">
INFINITE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_INVALID_FILE_ATTRIBUTES" class="tdname">
INVALID_FILE_ATTRIBUTES</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_INVALID_FILE_SIZE" class="tdname">
INVALID_FILE_SIZE</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_INVALID_HANDLE_VALUE" class="tdname">
INVALID_HANDLE_VALUE</td>
<td class="tdtype">
Win32.HANDLE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_INVALID_SET_FILE_POINTER" class="tdname">
INVALID_SET_FILE_POINTER</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_LANG_ENGLISH" class="tdname">
LANG_ENGLISH</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_LWA_ALPHA" class="tdname">
LWA_ALPHA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_LWA_COLORKEY" class="tdname">
LWA_COLORKEY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_CHAR" class="tdname">
MAPVK_VK_TO_CHAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_VSC" class="tdname">
MAPVK_VK_TO_VSC</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAPVK_VK_TO_VSC_EX" class="tdname">
MAPVK_VK_TO_VSC_EX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAPVK_VSC_TO_VK" class="tdname">
MAPVK_VSC_TO_VK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAPVK_VSC_TO_VK_EX" class="tdname">
MAPVK_VSC_TO_VK_EX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAXIMUM_WAIT_OBJECTS" class="tdname">
MAXIMUM_WAIT_OBJECTS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_ADDRESS_LENGTH" class="tdname">
MAX_ADAPTER_ADDRESS_LENGTH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_DESCRIPTION_LENGTH" class="tdname">
MAX_ADAPTER_DESCRIPTION_LENGTH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAX_ADAPTER_NAME_LENGTH" class="tdname">
MAX_ADAPTER_NAME_LENGTH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAX_DHCPV6_DUID_LENGTH" class="tdname">
MAX_DHCPV6_DUID_LENGTH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAX_DNS_SUFFIX_STRING_LENGTH" class="tdname">
MAX_DNS_SUFFIX_STRING_LENGTH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAX_DOMAIN_NAME_LEN" class="tdname">
MAX_DOMAIN_NAME_LEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAX_HOSTNAME_LEN" class="tdname">
MAX_HOSTNAME_LEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAX_PATH" class="tdname">
MAX_PATH</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MAX_SCOPE_ID_LEN" class="tdname">
MAX_SCOPE_ID_LEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_ABORTRETRYIGNORE" class="tdname">
MB_ABORTRETRYIGNORE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_APPLMODAL" class="tdname">
MB_APPLMODAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_CANCELTRYCONTINUE" class="tdname">
MB_CANCELTRYCONTINUE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_DEFAULT_DESKTOP_ONLY" class="tdname">
MB_DEFAULT_DESKTOP_ONLY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON1" class="tdname">
MB_DEFBUTTON1</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON2" class="tdname">
MB_DEFBUTTON2</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON3" class="tdname">
MB_DEFBUTTON3</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_DEFBUTTON4" class="tdname">
MB_DEFBUTTON4</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_HELP" class="tdname">
MB_HELP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_ICONASTERISK" class="tdname">
MB_ICONASTERISK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_ICONERROR" class="tdname">
MB_ICONERROR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_ICONEXCLAMATION" class="tdname">
MB_ICONEXCLAMATION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_ICONHAND" class="tdname">
MB_ICONHAND</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_ICONINFORMATION" class="tdname">
MB_ICONINFORMATION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_ICONQUESTION" class="tdname">
MB_ICONQUESTION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_ICONSTOP" class="tdname">
MB_ICONSTOP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_ICONWARNING" class="tdname">
MB_ICONWARNING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_NOFOCUS" class="tdname">
MB_NOFOCUS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_OK" class="tdname">
MB_OK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_OKCANCEL" class="tdname">
MB_OKCANCEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_RETRYCANCEL" class="tdname">
MB_RETRYCANCEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_RIGHT" class="tdname">
MB_RIGHT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_RTLREADING" class="tdname">
MB_RTLREADING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_SETFOREGROUND" class="tdname">
MB_SETFOREGROUND</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_SYSTEMMODAL" class="tdname">
MB_SYSTEMMODAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_TASKMODAL" class="tdname">
MB_TASKMODAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_TOPMOST" class="tdname">
MB_TOPMOST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_USERICON" class="tdname">
MB_USERICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_YESNO" class="tdname">
MB_YESNO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MB_YESNOCANCEL" class="tdname">
MB_YESNOCANCEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MN_GETHMENU" class="tdname">
MN_GETHMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MOD_ALT" class="tdname">
MOD_ALT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MOD_CONTROL" class="tdname">
MOD_CONTROL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MOD_NOREPEAT" class="tdname">
MOD_NOREPEAT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MOD_SHIFT" class="tdname">
MOD_SHIFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MOD_WIN" class="tdname">
MOD_WIN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTONEAREST" class="tdname">
MONITOR_DEFAULTTONEAREST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTONULL" class="tdname">
MONITOR_DEFAULTTONULL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_MONITOR_DEFAULTTOPRIMARY" class="tdname">
MONITOR_DEFAULTTOPRIMARY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIF_GUID" class="tdname">
NIF_GUID</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIF_ICON" class="tdname">
NIF_ICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIF_INFO" class="tdname">
NIF_INFO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIF_MESSAGE" class="tdname">
NIF_MESSAGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIF_REALTIME" class="tdname">
NIF_REALTIME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIF_SHOWTIP" class="tdname">
NIF_SHOWTIP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIF_STATE" class="tdname">
NIF_STATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIF_TIP" class="tdname">
NIF_TIP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIM_ADD" class="tdname">
NIM_ADD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIM_DELETE" class="tdname">
NIM_DELETE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIM_MODIFY" class="tdname">
NIM_MODIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIM_SETFOCUS" class="tdname">
NIM_SETFOCUS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_NIM_SETVERSION" class="tdname">
NIM_SETVERSION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_OPEN_ALWAYS" class="tdname">
OPEN_ALWAYS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_OPEN_EXISTING" class="tdname">
OPEN_EXISTING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_PM_NOREMOVE" class="tdname">
PM_NOREMOVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_PM_NOYIELD" class="tdname">
PM_NOYIELD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_PM_REMOVE" class="tdname">
PM_REMOVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_ARRANGE" class="tdname">
SC_ARRANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_CLOSE" class="tdname">
SC_CLOSE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_CONTEXTHELP" class="tdname">
SC_CONTEXTHELP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_DEFAULT" class="tdname">
SC_DEFAULT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_HOTKEY" class="tdname">
SC_HOTKEY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_HSCROLL" class="tdname">
SC_HSCROLL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_KEYMENU" class="tdname">
SC_KEYMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_MAXIMIZE" class="tdname">
SC_MAXIMIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_MINIMIZE" class="tdname">
SC_MINIMIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_MONITORPOWER" class="tdname">
SC_MONITORPOWER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_MOUSEMENU" class="tdname">
SC_MOUSEMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_MOVE" class="tdname">
SC_MOVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_NEXTWINDOW" class="tdname">
SC_NEXTWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_PREVWINDOW" class="tdname">
SC_PREVWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_RESTORE" class="tdname">
SC_RESTORE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_SCREENSAVE" class="tdname">
SC_SCREENSAVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_SEPARATOR" class="tdname">
SC_SEPARATOR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_SIZE" class="tdname">
SC_SIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_TASKLIST" class="tdname">
SC_TASKLIST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SC_VSCROLL" class="tdname">
SC_VSCROLL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SHGFI_ADDOVERLAYS" class="tdname">
SHGFI_ADDOVERLAYS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>apply the appropriate overlays. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_ATTRIBUTES" class="tdname">
SHGFI_ATTRIBUTES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get attributes. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_ATTR_SPECIFIED" class="tdname">
SHGFI_ATTR_SPECIFIED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get only specified attributes. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_DISPLAYNAME" class="tdname">
SHGFI_DISPLAYNAME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get display name. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_EXETYPE" class="tdname">
SHGFI_EXETYPE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>return exe type. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_ICON" class="tdname">
SHGFI_ICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get icon. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_ICONLOCATION" class="tdname">
SHGFI_ICONLOCATION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get icon location. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_LARGEICON" class="tdname">
SHGFI_LARGEICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get large icon. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_LINKOVERLAY" class="tdname">
SHGFI_LINKOVERLAY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>put a link overlay on icon. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_OPENICON" class="tdname">
SHGFI_OPENICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get open icon. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_OVERLAYINDEX" class="tdname">
SHGFI_OVERLAYINDEX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>Get the index of the overlay. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_PIDL" class="tdname">
SHGFI_PIDL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>pszPath is a pidl. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_SELECTED" class="tdname">
SHGFI_SELECTED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>show icon in selected state. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_SHELLICONSIZE" class="tdname">
SHGFI_SHELLICONSIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get shell size icon. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_SMALLICON" class="tdname">
SHGFI_SMALLICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get small icon. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_SYSICONINDEX" class="tdname">
SHGFI_SYSICONINDEX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get system icon index. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_TYPENAME" class="tdname">
SHGFI_TYPENAME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>get type name. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGFI_USEFILEATTRIBUTES" class="tdname">
SHGFI_USEFILEATTRIBUTES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
<p>use passed dwFileAttribute. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGSI_ICON" class="tdname">
SHGSI_ICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SHGSI_ICONLOCATION" class="tdname">
SHGSI_ICONLOCATION</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
<p>you always get the icon location. </p>
</td>
</tr>
<tr>
<td id="Win32_SHGSI_LARGEICON" class="tdname">
SHGSI_LARGEICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SHGSI_LINKOVERLAY" class="tdname">
SHGSI_LINKOVERLAY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SHGSI_SELECTED" class="tdname">
SHGSI_SELECTED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SHGSI_SHELLICONSIZE" class="tdname">
SHGSI_SHELLICONSIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SHGSI_SMALLICON" class="tdname">
SHGSI_SMALLICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SHGSI_SYSICONINDEX" class="tdname">
SHGSI_SYSICONINDEX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_ARRANGE" class="tdname">
SM_ARRANGE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CARETBLINKINGENABLED" class="tdname">
SM_CARETBLINKINGENABLED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CLEANBOOT" class="tdname">
SM_CLEANBOOT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CMETRICS" class="tdname">
SM_CMETRICS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CMONITORS" class="tdname">
SM_CMONITORS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CMOUSEBUTTONS" class="tdname">
SM_CMOUSEBUTTONS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CONVERTIBLESLATEMODE" class="tdname">
SM_CONVERTIBLESLATEMODE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXBORDER" class="tdname">
SM_CXBORDER</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXCURSOR" class="tdname">
SM_CXCURSOR</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXDLGFRAME" class="tdname">
SM_CXDLGFRAME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXDOUBLECLK" class="tdname">
SM_CXDOUBLECLK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXDRAG" class="tdname">
SM_CXDRAG</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXEDGE" class="tdname">
SM_CXEDGE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXFIXEDFRAME" class="tdname">
SM_CXFIXEDFRAME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXFOCUSBORDER" class="tdname">
SM_CXFOCUSBORDER</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXFRAME" class="tdname">
SM_CXFRAME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXFULLSCREEN" class="tdname">
SM_CXFULLSCREEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXHSCROLL" class="tdname">
SM_CXHSCROLL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXHTHUMB" class="tdname">
SM_CXHTHUMB</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXICON" class="tdname">
SM_CXICON</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXICONSPACING" class="tdname">
SM_CXICONSPACING</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXMAXIMIZED" class="tdname">
SM_CXMAXIMIZED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXMAXTRACK" class="tdname">
SM_CXMAXTRACK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXMENUCHECK" class="tdname">
SM_CXMENUCHECK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXMENUSIZE" class="tdname">
SM_CXMENUSIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXMIN" class="tdname">
SM_CXMIN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXMINIMIZED" class="tdname">
SM_CXMINIMIZED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXMINSPACING" class="tdname">
SM_CXMINSPACING</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXMINTRACK" class="tdname">
SM_CXMINTRACK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXPADDEDBORDER" class="tdname">
SM_CXPADDEDBORDER</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXSCREEN" class="tdname">
SM_CXSCREEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXSIZE" class="tdname">
SM_CXSIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXSIZEFRAME" class="tdname">
SM_CXSIZEFRAME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXSMICON" class="tdname">
SM_CXSMICON</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXSMSIZE" class="tdname">
SM_CXSMSIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXVIRTUALSCREEN" class="tdname">
SM_CXVIRTUALSCREEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CXVSCROLL" class="tdname">
SM_CXVSCROLL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYBORDER" class="tdname">
SM_CYBORDER</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYCAPTION" class="tdname">
SM_CYCAPTION</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYCURSOR" class="tdname">
SM_CYCURSOR</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYDLGFRAME" class="tdname">
SM_CYDLGFRAME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYDOUBLECLK" class="tdname">
SM_CYDOUBLECLK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYDRAG" class="tdname">
SM_CYDRAG</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYEDGE" class="tdname">
SM_CYEDGE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYFIXEDFRAME" class="tdname">
SM_CYFIXEDFRAME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYFOCUSBORDER" class="tdname">
SM_CYFOCUSBORDER</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYFRAME" class="tdname">
SM_CYFRAME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYFULLSCREEN" class="tdname">
SM_CYFULLSCREEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYHSCROLL" class="tdname">
SM_CYHSCROLL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYICON" class="tdname">
SM_CYICON</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYICONSPACING" class="tdname">
SM_CYICONSPACING</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYKANJIWINDOW" class="tdname">
SM_CYKANJIWINDOW</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYMAXIMIZED" class="tdname">
SM_CYMAXIMIZED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYMAXTRACK" class="tdname">
SM_CYMAXTRACK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYMENU" class="tdname">
SM_CYMENU</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYMENUCHECK" class="tdname">
SM_CYMENUCHECK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYMENUSIZE" class="tdname">
SM_CYMENUSIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYMIN" class="tdname">
SM_CYMIN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYMINIMIZED" class="tdname">
SM_CYMINIMIZED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYMINSPACING" class="tdname">
SM_CYMINSPACING</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYMINTRACK" class="tdname">
SM_CYMINTRACK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYSCREEN" class="tdname">
SM_CYSCREEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYSIZE" class="tdname">
SM_CYSIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYSIZEFRAME" class="tdname">
SM_CYSIZEFRAME</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYSMCAPTION" class="tdname">
SM_CYSMCAPTION</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYSMICON" class="tdname">
SM_CYSMICON</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYSMSIZE" class="tdname">
SM_CYSMSIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYVIRTUALSCREEN" class="tdname">
SM_CYVIRTUALSCREEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYVSCROLL" class="tdname">
SM_CYVSCROLL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_CYVTHUMB" class="tdname">
SM_CYVTHUMB</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_DBCSENABLED" class="tdname">
SM_DBCSENABLED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_DEBUG" class="tdname">
SM_DEBUG</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_DIGITIZER" class="tdname">
SM_DIGITIZER</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_IMMENABLED" class="tdname">
SM_IMMENABLED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_MAXIMUMTOUCHES" class="tdname">
SM_MAXIMUMTOUCHES</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_MEDIACENTER" class="tdname">
SM_MEDIACENTER</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_MENUDROPALIGNMENT" class="tdname">
SM_MENUDROPALIGNMENT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_MIDEASTENABLED" class="tdname">
SM_MIDEASTENABLED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_MOUSEHORIZONTALWHEELPRESENT" class="tdname">
SM_MOUSEHORIZONTALWHEELPRESENT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_MOUSEPRESENT" class="tdname">
SM_MOUSEPRESENT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_MOUSEWHEELPRESENT" class="tdname">
SM_MOUSEWHEELPRESENT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_NETWORK" class="tdname">
SM_NETWORK</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_PENWINDOWS" class="tdname">
SM_PENWINDOWS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_REMOTECONTROL" class="tdname">
SM_REMOTECONTROL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_REMOTESESSION" class="tdname">
SM_REMOTESESSION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_RESERVED1" class="tdname">
SM_RESERVED1</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_RESERVED2" class="tdname">
SM_RESERVED2</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_RESERVED3" class="tdname">
SM_RESERVED3</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_RESERVED4" class="tdname">
SM_RESERVED4</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_SAMEDISPLAYFORMAT" class="tdname">
SM_SAMEDISPLAYFORMAT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_SECURE" class="tdname">
SM_SECURE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_SERVERR2" class="tdname">
SM_SERVERR2</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_SHOWSOUNDS" class="tdname">
SM_SHOWSOUNDS</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_SHUTTINGDOWN" class="tdname">
SM_SHUTTINGDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_SLOWMACHINE" class="tdname">
SM_SLOWMACHINE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_STARTER" class="tdname">
SM_STARTER</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_SWAPBUTTON" class="tdname">
SM_SWAPBUTTON</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_SYSTEMDOCKED" class="tdname">
SM_SYSTEMDOCKED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_TABLETPC" class="tdname">
SM_TABLETPC</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_XVIRTUALSCREEN" class="tdname">
SM_XVIRTUALSCREEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SM_YVIRTUALSCREEN" class="tdname">
SM_YVIRTUALSCREEN</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SRWLOCK_INIT" class="tdname">
SRWLOCK_INIT</td>
<td class="tdtype">
Win32.SRWLOCK</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_STD_INPUT_HANDLE" class="tdname">
STD_INPUT_HANDLE</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_STD_OUTPUT_HANDLE" class="tdname">
STD_OUTPUT_HANDLE</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SUBLANG_DEFAULT" class="tdname">
SUBLANG_DEFAULT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_ASYNCWINDOWPOS" class="tdname">
SWP_ASYNCWINDOWPOS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_DEFERERASE" class="tdname">
SWP_DEFERERASE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_DRAWFRAME" class="tdname">
SWP_DRAWFRAME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_FRAMECHANGED" class="tdname">
SWP_FRAMECHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_HIDEWINDOW" class="tdname">
SWP_HIDEWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_NOACTIVATE" class="tdname">
SWP_NOACTIVATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_NOCOPYBITS" class="tdname">
SWP_NOCOPYBITS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_NOMOVE" class="tdname">
SWP_NOMOVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_NOOWNERZORDER" class="tdname">
SWP_NOOWNERZORDER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_NOREDRAW" class="tdname">
SWP_NOREDRAW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_NOREPOSITION" class="tdname">
SWP_NOREPOSITION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_NOSENDCHANGING" class="tdname">
SWP_NOSENDCHANGING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_NOSIZE" class="tdname">
SWP_NOSIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_NOZORDER" class="tdname">
SWP_NOZORDER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SWP_SHOWWINDOW" class="tdname">
SWP_SHOWWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_FORCEMINIMIZE" class="tdname">
SW_FORCEMINIMIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_HIDE" class="tdname">
SW_HIDE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_MAX" class="tdname">
SW_MAX</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_MAXIMIZE" class="tdname">
SW_MAXIMIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_MINIMIZE" class="tdname">
SW_MINIMIZE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_NORMAL" class="tdname">
SW_NORMAL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_RESTORE" class="tdname">
SW_RESTORE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_SHOW" class="tdname">
SW_SHOW</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_SHOWDEFAULT" class="tdname">
SW_SHOWDEFAULT</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_SHOWMAXIMIZED" class="tdname">
SW_SHOWMAXIMIZED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_SHOWMINIMIZED" class="tdname">
SW_SHOWMINIMIZED</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_SHOWMINNOACTIVE" class="tdname">
SW_SHOWMINNOACTIVE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_SHOWNA" class="tdname">
SW_SHOWNA</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_SHOWNOACTIVATE" class="tdname">
SW_SHOWNOACTIVATE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_SW_SHOWNORMAL" class="tdname">
SW_SHOWNORMAL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_S_OK" class="tdname">
S_OK</td>
<td class="tdtype">
Win32.HRESULT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_ABOVE_NORMAL" class="tdname">
THREAD_PRIORITY_ABOVE_NORMAL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_BELOW_NORMAL" class="tdname">
THREAD_PRIORITY_BELOW_NORMAL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_HIGHEST" class="tdname">
THREAD_PRIORITY_HIGHEST</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_LOWEST" class="tdname">
THREAD_PRIORITY_LOWEST</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_THREAD_PRIORITY_NORMAL" class="tdname">
THREAD_PRIORITY_NORMAL</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_TRUE" class="tdname">
TRUE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_TRUNCATE_EXISTING" class="tdname">
TRUNCATE_EXISTING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_ACCEPT" class="tdname">
VK_ACCEPT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_ADD" class="tdname">
VK_ADD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_APPS" class="tdname">
VK_APPS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_ATTN" class="tdname">
VK_ATTN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_BACK" class="tdname">
VK_BACK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_BACK" class="tdname">
VK_BROWSER_BACK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_FAVORITES" class="tdname">
VK_BROWSER_FAVORITES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_FORWARD" class="tdname">
VK_BROWSER_FORWARD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_HOME" class="tdname">
VK_BROWSER_HOME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_REFRESH" class="tdname">
VK_BROWSER_REFRESH</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_SEARCH" class="tdname">
VK_BROWSER_SEARCH</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_BROWSER_STOP" class="tdname">
VK_BROWSER_STOP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_CANCEL" class="tdname">
VK_CANCEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_CAPITAL" class="tdname">
VK_CAPITAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_CLEAR" class="tdname">
VK_CLEAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_CONTROL" class="tdname">
VK_CONTROL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_CONVERT" class="tdname">
VK_CONVERT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_CRSEL" class="tdname">
VK_CRSEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_DECIMAL" class="tdname">
VK_DECIMAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_DELETE" class="tdname">
VK_DELETE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_DIVIDE" class="tdname">
VK_DIVIDE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_DOWN" class="tdname">
VK_DOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_END" class="tdname">
VK_END</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_EREOF" class="tdname">
VK_EREOF</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_ESCAPE" class="tdname">
VK_ESCAPE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_EXECUTE" class="tdname">
VK_EXECUTE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_EXSEL" class="tdname">
VK_EXSEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F1" class="tdname">
VK_F1</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F10" class="tdname">
VK_F10</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F11" class="tdname">
VK_F11</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F12" class="tdname">
VK_F12</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F13" class="tdname">
VK_F13</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F14" class="tdname">
VK_F14</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F15" class="tdname">
VK_F15</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F16" class="tdname">
VK_F16</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F17" class="tdname">
VK_F17</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F18" class="tdname">
VK_F18</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F19" class="tdname">
VK_F19</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F2" class="tdname">
VK_F2</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F20" class="tdname">
VK_F20</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F21" class="tdname">
VK_F21</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F22" class="tdname">
VK_F22</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F23" class="tdname">
VK_F23</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F24" class="tdname">
VK_F24</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F3" class="tdname">
VK_F3</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F4" class="tdname">
VK_F4</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F5" class="tdname">
VK_F5</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F6" class="tdname">
VK_F6</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F7" class="tdname">
VK_F7</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F8" class="tdname">
VK_F8</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_F9" class="tdname">
VK_F9</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_FINAL" class="tdname">
VK_FINAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_A" class="tdname">
VK_GAMEPAD_A</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_B" class="tdname">
VK_GAMEPAD_B</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_DOWN" class="tdname">
VK_GAMEPAD_DPAD_DOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_LEFT" class="tdname">
VK_GAMEPAD_DPAD_LEFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_RIGHT" class="tdname">
VK_GAMEPAD_DPAD_RIGHT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_DPAD_UP" class="tdname">
VK_GAMEPAD_DPAD_UP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_SHOULDER" class="tdname">
VK_GAMEPAD_LEFT_SHOULDER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON" class="tdname">
VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_DOWN" class="tdname">
VK_GAMEPAD_LEFT_THUMBSTICK_DOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_LEFT" class="tdname">
VK_GAMEPAD_LEFT_THUMBSTICK_LEFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT" class="tdname">
VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_THUMBSTICK_UP" class="tdname">
VK_GAMEPAD_LEFT_THUMBSTICK_UP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_LEFT_TRIGGER" class="tdname">
VK_GAMEPAD_LEFT_TRIGGER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_MENU" class="tdname">
VK_GAMEPAD_MENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_SHOULDER" class="tdname">
VK_GAMEPAD_RIGHT_SHOULDER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON" class="tdname">
VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN" class="tdname">
VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT" class="tdname">
VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT" class="tdname">
VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_THUMBSTICK_UP" class="tdname">
VK_GAMEPAD_RIGHT_THUMBSTICK_UP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_RIGHT_TRIGGER" class="tdname">
VK_GAMEPAD_RIGHT_TRIGGER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_VIEW" class="tdname">
VK_GAMEPAD_VIEW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_X" class="tdname">
VK_GAMEPAD_X</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_GAMEPAD_Y" class="tdname">
VK_GAMEPAD_Y</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_HANGEUL" class="tdname">
VK_HANGEUL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_HANGUL" class="tdname">
VK_HANGUL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_HANJA" class="tdname">
VK_HANJA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_HELP" class="tdname">
VK_HELP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_HOME" class="tdname">
VK_HOME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_ICO_00" class="tdname">
VK_ICO_00</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_ICO_CLEAR" class="tdname">
VK_ICO_CLEAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_ICO_HELP" class="tdname">
VK_ICO_HELP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_INSERT" class="tdname">
VK_INSERT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_JUNJA" class="tdname">
VK_JUNJA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_KANA" class="tdname">
VK_KANA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_KANJI" class="tdname">
VK_KANJI</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_APP1" class="tdname">
VK_LAUNCH_APP1</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_APP2" class="tdname">
VK_LAUNCH_APP2</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_MAIL" class="tdname">
VK_LAUNCH_MAIL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LAUNCH_MEDIA_SELECT" class="tdname">
VK_LAUNCH_MEDIA_SELECT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LBUTTON" class="tdname">
VK_LBUTTON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LCONTROL" class="tdname">
VK_LCONTROL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LEFT" class="tdname">
VK_LEFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LMENU" class="tdname">
VK_LMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LSHIFT" class="tdname">
VK_LSHIFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_LWIN" class="tdname">
VK_LWIN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_MBUTTON" class="tdname">
VK_MBUTTON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_NEXT_TRACK" class="tdname">
VK_MEDIA_NEXT_TRACK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_PLAY_PAUSE" class="tdname">
VK_MEDIA_PLAY_PAUSE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_PREV_TRACK" class="tdname">
VK_MEDIA_PREV_TRACK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_MEDIA_STOP" class="tdname">
VK_MEDIA_STOP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_MENU" class="tdname">
VK_MENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_MODECHANGE" class="tdname">
VK_MODECHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_MULTIPLY" class="tdname">
VK_MULTIPLY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_ACCEPT" class="tdname">
VK_NAVIGATION_ACCEPT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_CANCEL" class="tdname">
VK_NAVIGATION_CANCEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_DOWN" class="tdname">
VK_NAVIGATION_DOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_LEFT" class="tdname">
VK_NAVIGATION_LEFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_MENU" class="tdname">
VK_NAVIGATION_MENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_RIGHT" class="tdname">
VK_NAVIGATION_RIGHT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_UP" class="tdname">
VK_NAVIGATION_UP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NAVIGATION_VIEW" class="tdname">
VK_NAVIGATION_VIEW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NEXT" class="tdname">
VK_NEXT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NONAME" class="tdname">
VK_NONAME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NONCONVERT" class="tdname">
VK_NONCONVERT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMLOCK" class="tdname">
VK_NUMLOCK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD0" class="tdname">
VK_NUMPAD0</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD1" class="tdname">
VK_NUMPAD1</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD2" class="tdname">
VK_NUMPAD2</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD3" class="tdname">
VK_NUMPAD3</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD4" class="tdname">
VK_NUMPAD4</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD5" class="tdname">
VK_NUMPAD5</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD6" class="tdname">
VK_NUMPAD6</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD7" class="tdname">
VK_NUMPAD7</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD8" class="tdname">
VK_NUMPAD8</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_NUMPAD9" class="tdname">
VK_NUMPAD9</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_1" class="tdname">
VK_OEM_1</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_102" class="tdname">
VK_OEM_102</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_2" class="tdname">
VK_OEM_2</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_3" class="tdname">
VK_OEM_3</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_4" class="tdname">
VK_OEM_4</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_5" class="tdname">
VK_OEM_5</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_6" class="tdname">
VK_OEM_6</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_7" class="tdname">
VK_OEM_7</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_8" class="tdname">
VK_OEM_8</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_ATTN" class="tdname">
VK_OEM_ATTN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_AUTO" class="tdname">
VK_OEM_AUTO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_AX" class="tdname">
VK_OEM_AX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_BACKTAB" class="tdname">
VK_OEM_BACKTAB</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_CLEAR" class="tdname">
VK_OEM_CLEAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_COMMA" class="tdname">
VK_OEM_COMMA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_COPY" class="tdname">
VK_OEM_COPY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_CUSEL" class="tdname">
VK_OEM_CUSEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_ENLW" class="tdname">
VK_OEM_ENLW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_FINISH" class="tdname">
VK_OEM_FINISH</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_JISHO" class="tdname">
VK_OEM_FJ_JISHO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_LOYA" class="tdname">
VK_OEM_FJ_LOYA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_MASSHOU" class="tdname">
VK_OEM_FJ_MASSHOU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_ROYA" class="tdname">
VK_OEM_FJ_ROYA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_FJ_TOUROKU" class="tdname">
VK_OEM_FJ_TOUROKU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_JUMP" class="tdname">
VK_OEM_JUMP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_MINUS" class="tdname">
VK_OEM_MINUS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_NEC_EQUAL" class="tdname">
VK_OEM_NEC_EQUAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA1" class="tdname">
VK_OEM_PA1</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA2" class="tdname">
VK_OEM_PA2</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_PA3" class="tdname">
VK_OEM_PA3</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_PERIOD" class="tdname">
VK_OEM_PERIOD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_PLUS" class="tdname">
VK_OEM_PLUS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_RESET" class="tdname">
VK_OEM_RESET</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_OEM_WSCTRL" class="tdname">
VK_OEM_WSCTRL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_PA1" class="tdname">
VK_PA1</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_PACKET" class="tdname">
VK_PACKET</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_PAUSE" class="tdname">
VK_PAUSE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_PLAY" class="tdname">
VK_PLAY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_PRINT" class="tdname">
VK_PRINT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_PRIOR" class="tdname">
VK_PRIOR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_PROCESSKEY" class="tdname">
VK_PROCESSKEY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_RBUTTON" class="tdname">
VK_RBUTTON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_RCONTROL" class="tdname">
VK_RCONTROL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_RETURN" class="tdname">
VK_RETURN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_RIGHT" class="tdname">
VK_RIGHT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_RMENU" class="tdname">
VK_RMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_RSHIFT" class="tdname">
VK_RSHIFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_RWIN" class="tdname">
VK_RWIN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_SCROLL" class="tdname">
VK_SCROLL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_SELECT" class="tdname">
VK_SELECT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_SEPARATOR" class="tdname">
VK_SEPARATOR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_SHIFT" class="tdname">
VK_SHIFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_SLEEP" class="tdname">
VK_SLEEP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_SNAPSHOT" class="tdname">
VK_SNAPSHOT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_SPACE" class="tdname">
VK_SPACE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_SUBTRACT" class="tdname">
VK_SUBTRACT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_TAB" class="tdname">
VK_TAB</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_UP" class="tdname">
VK_UP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_DOWN" class="tdname">
VK_VOLUME_DOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_MUTE" class="tdname">
VK_VOLUME_MUTE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_VOLUME_UP" class="tdname">
VK_VOLUME_UP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_XBUTTON1" class="tdname">
VK_XBUTTON1</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_XBUTTON2" class="tdname">
VK_XBUTTON2</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_VK_ZOOM" class="tdname">
VK_ZOOM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WAIT_ABANDONED" class="tdname">
WAIT_ABANDONED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WAIT_FAILED" class="tdname">
WAIT_FAILED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WAIT_OBJECT_0" class="tdname">
WAIT_OBJECT_0</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WAIT_TIMEOUT" class="tdname">
WAIT_TIMEOUT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WA_ACTIVE" class="tdname">
WA_ACTIVE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WA_CLICKACTIVE" class="tdname">
WA_CLICKACTIVE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WA_INACTIVE" class="tdname">
WA_INACTIVE</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WHEEL_DELTA" class="tdname">
WHEEL_DELTA</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_ACTIVATE" class="tdname">
WM_ACTIVATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_ACTIVATEAPP" class="tdname">
WM_ACTIVATEAPP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_AFXFIRST" class="tdname">
WM_AFXFIRST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_AFXLAST" class="tdname">
WM_AFXLAST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_APP" class="tdname">
WM_APP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_APPCOMMAND" class="tdname">
WM_APPCOMMAND</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_ASKCBFORMATNAME" class="tdname">
WM_ASKCBFORMATNAME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CANCELJOURNAL" class="tdname">
WM_CANCELJOURNAL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CANCELMODE" class="tdname">
WM_CANCELMODE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CAPTURECHANGED" class="tdname">
WM_CAPTURECHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CHANGECBCHAIN" class="tdname">
WM_CHANGECBCHAIN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CHANGEUISTATE" class="tdname">
WM_CHANGEUISTATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CHAR" class="tdname">
WM_CHAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CHARTOITEM" class="tdname">
WM_CHARTOITEM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CHILDACTIVATE" class="tdname">
WM_CHILDACTIVATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CLEAR" class="tdname">
WM_CLEAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CLIPBOARDUPDATE" class="tdname">
WM_CLIPBOARDUPDATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CLOSE" class="tdname">
WM_CLOSE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_COMMAND" class="tdname">
WM_COMMAND</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_COMMNOTIFY" class="tdname">
WM_COMMNOTIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_COMPACTING" class="tdname">
WM_COMPACTING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_COMPAREITEM" class="tdname">
WM_COMPAREITEM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CONTEXTMENU" class="tdname">
WM_CONTEXTMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_COPY" class="tdname">
WM_COPY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_COPYDATA" class="tdname">
WM_COPYDATA</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CREATE" class="tdname">
WM_CREATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORBTN" class="tdname">
WM_CTLCOLORBTN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORDLG" class="tdname">
WM_CTLCOLORDLG</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLOREDIT" class="tdname">
WM_CTLCOLOREDIT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORLISTBOX" class="tdname">
WM_CTLCOLORLISTBOX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORMSGBOX" class="tdname">
WM_CTLCOLORMSGBOX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORSCROLLBAR" class="tdname">
WM_CTLCOLORSCROLLBAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CTLCOLORSTATIC" class="tdname">
WM_CTLCOLORSTATIC</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_CUT" class="tdname">
WM_CUT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DEADCHAR" class="tdname">
WM_DEADCHAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DELETEITEM" class="tdname">
WM_DELETEITEM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DESTROY" class="tdname">
WM_DESTROY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DESTROYCLIPBOARD" class="tdname">
WM_DESTROYCLIPBOARD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DEVICECHANGE" class="tdname">
WM_DEVICECHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DEVMODECHANGE" class="tdname">
WM_DEVMODECHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DISPLAYCHANGE" class="tdname">
WM_DISPLAYCHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED" class="tdname">
WM_DPICHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED_AFTERPARENT" class="tdname">
WM_DPICHANGED_AFTERPARENT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DPICHANGED_BEFOREPARENT" class="tdname">
WM_DPICHANGED_BEFOREPARENT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DRAWCLIPBOARD" class="tdname">
WM_DRAWCLIPBOARD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DRAWITEM" class="tdname">
WM_DRAWITEM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DROPFILES" class="tdname">
WM_DROPFILES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DWMCOLORIZATIONCOLORCHANGED" class="tdname">
WM_DWMCOLORIZATIONCOLORCHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DWMCOMPOSITIONCHANGED" class="tdname">
WM_DWMCOMPOSITIONCHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DWMNCRENDERINGCHANGED" class="tdname">
WM_DWMNCRENDERINGCHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DWMSENDICONICLIVEPREVIEWBITMAP" class="tdname">
WM_DWMSENDICONICLIVEPREVIEWBITMAP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DWMSENDICONICTHUMBNAIL" class="tdname">
WM_DWMSENDICONICTHUMBNAIL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_DWMWINDOWMAXIMIZEDCHANGE" class="tdname">
WM_DWMWINDOWMAXIMIZEDCHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_ENABLE" class="tdname">
WM_ENABLE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_ENTERIDLE" class="tdname">
WM_ENTERIDLE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_ENTERMENULOOP" class="tdname">
WM_ENTERMENULOOP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_ENTERSIZEMOVE" class="tdname">
WM_ENTERSIZEMOVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_ERASEBKGND" class="tdname">
WM_ERASEBKGND</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_EXITMENULOOP" class="tdname">
WM_EXITMENULOOP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_EXITSIZEMOVE" class="tdname">
WM_EXITSIZEMOVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_FONTCHANGE" class="tdname">
WM_FONTCHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GESTURE" class="tdname">
WM_GESTURE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GESTURENOTIFY" class="tdname">
WM_GESTURENOTIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETDLGCODE" class="tdname">
WM_GETDLGCODE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETDPISCALEDSIZE" class="tdname">
WM_GETDPISCALEDSIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETFONT" class="tdname">
WM_GETFONT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETHOTKEY" class="tdname">
WM_GETHOTKEY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETICON" class="tdname">
WM_GETICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETMINMAXINFO" class="tdname">
WM_GETMINMAXINFO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETOBJECT" class="tdname">
WM_GETOBJECT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETTEXT" class="tdname">
WM_GETTEXT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETTEXTLENGTH" class="tdname">
WM_GETTEXTLENGTH</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_GETTITLEBARINFOEX" class="tdname">
WM_GETTITLEBARINFOEX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_HANDHELDFIRST" class="tdname">
WM_HANDHELDFIRST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_HANDHELDLAST" class="tdname">
WM_HANDHELDLAST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_HELP" class="tdname">
WM_HELP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_HOTKEY" class="tdname">
WM_HOTKEY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_HSCROLL" class="tdname">
WM_HSCROLL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_HSCROLLCLIPBOARD" class="tdname">
WM_HSCROLLCLIPBOARD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_ICONERASEBKGND" class="tdname">
WM_ICONERASEBKGND</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_CHAR" class="tdname">
WM_IME_CHAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_COMPOSITION" class="tdname">
WM_IME_COMPOSITION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_COMPOSITIONFULL" class="tdname">
WM_IME_COMPOSITIONFULL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_CONTROL" class="tdname">
WM_IME_CONTROL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_ENDCOMPOSITION" class="tdname">
WM_IME_ENDCOMPOSITION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYDOWN" class="tdname">
WM_IME_KEYDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYLAST" class="tdname">
WM_IME_KEYLAST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_KEYUP" class="tdname">
WM_IME_KEYUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_NOTIFY" class="tdname">
WM_IME_NOTIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_REQUEST" class="tdname">
WM_IME_REQUEST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_SELECT" class="tdname">
WM_IME_SELECT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_SETCONTEXT" class="tdname">
WM_IME_SETCONTEXT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_IME_STARTCOMPOSITION" class="tdname">
WM_IME_STARTCOMPOSITION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_INITDIALOG" class="tdname">
WM_INITDIALOG</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_INITMENU" class="tdname">
WM_INITMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_INITMENUPOPUP" class="tdname">
WM_INITMENUPOPUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_INPUT" class="tdname">
WM_INPUT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_INPUTLANGCHANGE" class="tdname">
WM_INPUTLANGCHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_INPUTLANGCHANGEREQUEST" class="tdname">
WM_INPUTLANGCHANGEREQUEST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_INPUT_DEVICE_CHANGE" class="tdname">
WM_INPUT_DEVICE_CHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_KEYDOWN" class="tdname">
WM_KEYDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_KEYFIRST" class="tdname">
WM_KEYFIRST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_KEYLAST" class="tdname">
WM_KEYLAST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_KEYUP" class="tdname">
WM_KEYUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_KILLFOCUS" class="tdname">
WM_KILLFOCUS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONDBLCLK" class="tdname">
WM_LBUTTONDBLCLK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONDOWN" class="tdname">
WM_LBUTTONDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_LBUTTONUP" class="tdname">
WM_LBUTTONUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONDBLCLK" class="tdname">
WM_MBUTTONDBLCLK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONDOWN" class="tdname">
WM_MBUTTONDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MBUTTONUP" class="tdname">
WM_MBUTTONUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDIACTIVATE" class="tdname">
WM_MDIACTIVATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDICASCADE" class="tdname">
WM_MDICASCADE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDICREATE" class="tdname">
WM_MDICREATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDIDESTROY" class="tdname">
WM_MDIDESTROY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDIGETACTIVE" class="tdname">
WM_MDIGETACTIVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDIICONARRANGE" class="tdname">
WM_MDIICONARRANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDIMAXIMIZE" class="tdname">
WM_MDIMAXIMIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDINEXT" class="tdname">
WM_MDINEXT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDIREFRESHMENU" class="tdname">
WM_MDIREFRESHMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDIRESTORE" class="tdname">
WM_MDIRESTORE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDISETMENU" class="tdname">
WM_MDISETMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MDITILE" class="tdname">
WM_MDITILE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MEASUREITEM" class="tdname">
WM_MEASUREITEM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MENUCHAR" class="tdname">
WM_MENUCHAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MENUCOMMAND" class="tdname">
WM_MENUCOMMAND</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MENUDRAG" class="tdname">
WM_MENUDRAG</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MENUGETOBJECT" class="tdname">
WM_MENUGETOBJECT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MENURBUTTONUP" class="tdname">
WM_MENURBUTTONUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MENUSELECT" class="tdname">
WM_MENUSELECT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOUSEACTIVATE" class="tdname">
WM_MOUSEACTIVATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOUSEFIRST" class="tdname">
WM_MOUSEFIRST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOUSEHOVER" class="tdname">
WM_MOUSEHOVER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOUSEHWHEEL" class="tdname">
WM_MOUSEHWHEEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOUSELAST" class="tdname">
WM_MOUSELAST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOUSELEAVE" class="tdname">
WM_MOUSELEAVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOUSEMOVE" class="tdname">
WM_MOUSEMOVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOUSEWHEEL" class="tdname">
WM_MOUSEWHEEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOVE" class="tdname">
WM_MOVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_MOVING" class="tdname">
WM_MOVING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCACTIVATE" class="tdname">
WM_NCACTIVATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCCALCSIZE" class="tdname">
WM_NCCALCSIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCCREATE" class="tdname">
WM_NCCREATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCDESTROY" class="tdname">
WM_NCDESTROY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCHITTEST" class="tdname">
WM_NCHITTEST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONDBLCLK" class="tdname">
WM_NCLBUTTONDBLCLK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONDOWN" class="tdname">
WM_NCLBUTTONDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCLBUTTONUP" class="tdname">
WM_NCLBUTTONUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONDBLCLK" class="tdname">
WM_NCMBUTTONDBLCLK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONDOWN" class="tdname">
WM_NCMBUTTONDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCMBUTTONUP" class="tdname">
WM_NCMBUTTONUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSEHOVER" class="tdname">
WM_NCMOUSEHOVER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSELEAVE" class="tdname">
WM_NCMOUSELEAVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCMOUSEMOVE" class="tdname">
WM_NCMOUSEMOVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCPAINT" class="tdname">
WM_NCPAINT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERDOWN" class="tdname">
WM_NCPOINTERDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERUP" class="tdname">
WM_NCPOINTERUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCPOINTERUPDATE" class="tdname">
WM_NCPOINTERUPDATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONDBLCLK" class="tdname">
WM_NCRBUTTONDBLCLK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONDOWN" class="tdname">
WM_NCRBUTTONDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCRBUTTONUP" class="tdname">
WM_NCRBUTTONUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONDBLCLK" class="tdname">
WM_NCXBUTTONDBLCLK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONDOWN" class="tdname">
WM_NCXBUTTONDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NCXBUTTONUP" class="tdname">
WM_NCXBUTTONUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NEXTDLGCTL" class="tdname">
WM_NEXTDLGCTL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NEXTMENU" class="tdname">
WM_NEXTMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NOTIFY" class="tdname">
WM_NOTIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_NOTIFYFORMAT" class="tdname">
WM_NOTIFYFORMAT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PAINT" class="tdname">
WM_PAINT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PAINTCLIPBOARD" class="tdname">
WM_PAINTCLIPBOARD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PAINTICON" class="tdname">
WM_PAINTICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PALETTECHANGED" class="tdname">
WM_PALETTECHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PALETTEISCHANGING" class="tdname">
WM_PALETTEISCHANGING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PARENTNOTIFY" class="tdname">
WM_PARENTNOTIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PASTE" class="tdname">
WM_PASTE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PENWINFIRST" class="tdname">
WM_PENWINFIRST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PENWINLAST" class="tdname">
WM_PENWINLAST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERACTIVATE" class="tdname">
WM_POINTERACTIVATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERCAPTURECHANGED" class="tdname">
WM_POINTERCAPTURECHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICECHANGE" class="tdname">
WM_POINTERDEVICECHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICEINRANGE" class="tdname">
WM_POINTERDEVICEINRANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERDEVICEOUTOFRANGE" class="tdname">
WM_POINTERDEVICEOUTOFRANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERDOWN" class="tdname">
WM_POINTERDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERENTER" class="tdname">
WM_POINTERENTER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERHWHEEL" class="tdname">
WM_POINTERHWHEEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERLEAVE" class="tdname">
WM_POINTERLEAVE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDAWAY" class="tdname">
WM_POINTERROUTEDAWAY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDRELEASED" class="tdname">
WM_POINTERROUTEDRELEASED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERROUTEDTO" class="tdname">
WM_POINTERROUTEDTO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERUP" class="tdname">
WM_POINTERUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERUPDATE" class="tdname">
WM_POINTERUPDATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POINTERWHEEL" class="tdname">
WM_POINTERWHEEL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POWER" class="tdname">
WM_POWER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_POWERBROADCAST" class="tdname">
WM_POWERBROADCAST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PRINT" class="tdname">
WM_PRINT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_PRINTCLIENT" class="tdname">
WM_PRINTCLIENT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_QUERYDRAGICON" class="tdname">
WM_QUERYDRAGICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_QUERYNEWPALETTE" class="tdname">
WM_QUERYNEWPALETTE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_QUERYUISTATE" class="tdname">
WM_QUERYUISTATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_QUEUESYNC" class="tdname">
WM_QUEUESYNC</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_QUIT" class="tdname">
WM_QUIT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONDBLCLK" class="tdname">
WM_RBUTTONDBLCLK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONDOWN" class="tdname">
WM_RBUTTONDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_RBUTTONUP" class="tdname">
WM_RBUTTONUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_RENDERALLFORMATS" class="tdname">
WM_RENDERALLFORMATS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_RENDERFORMAT" class="tdname">
WM_RENDERFORMAT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SETCURSOR" class="tdname">
WM_SETCURSOR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SETFOCUS" class="tdname">
WM_SETFOCUS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SETFONT" class="tdname">
WM_SETFONT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SETHOTKEY" class="tdname">
WM_SETHOTKEY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SETICON" class="tdname">
WM_SETICON</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SETREDRAW" class="tdname">
WM_SETREDRAW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SETTEXT" class="tdname">
WM_SETTEXT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SETTINGCHANGE" class="tdname">
WM_SETTINGCHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SHOWWINDOW" class="tdname">
WM_SHOWWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SIZE" class="tdname">
WM_SIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SIZECLIPBOARD" class="tdname">
WM_SIZECLIPBOARD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SIZING" class="tdname">
WM_SIZING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SPOOLERSTATUS" class="tdname">
WM_SPOOLERSTATUS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_STYLECHANGED" class="tdname">
WM_STYLECHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_STYLECHANGING" class="tdname">
WM_STYLECHANGING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SYNCPAINT" class="tdname">
WM_SYNCPAINT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SYSCHAR" class="tdname">
WM_SYSCHAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SYSCOLORCHANGE" class="tdname">
WM_SYSCOLORCHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SYSCOMMAND" class="tdname">
WM_SYSCOMMAND</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SYSDEADCHAR" class="tdname">
WM_SYSDEADCHAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SYSKEYDOWN" class="tdname">
WM_SYSKEYDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_SYSKEYUP" class="tdname">
WM_SYSKEYUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_TABLET_FIRST" class="tdname">
WM_TABLET_FIRST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_TABLET_LAST" class="tdname">
WM_TABLET_LAST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_TCARD" class="tdname">
WM_TCARD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_THEMECHANGED" class="tdname">
WM_THEMECHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_TIMECHANGE" class="tdname">
WM_TIMECHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_TIMER" class="tdname">
WM_TIMER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_TOUCH" class="tdname">
WM_TOUCH</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_TOUCHHITTESTING" class="tdname">
WM_TOUCHHITTESTING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_UNDO" class="tdname">
WM_UNDO</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_UNICHAR" class="tdname">
WM_UNICHAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_UNINITMENUPOPUP" class="tdname">
WM_UNINITMENUPOPUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_UPDATEUISTATE" class="tdname">
WM_UPDATEUISTATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_USER" class="tdname">
WM_USER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_USERCHANGED" class="tdname">
WM_USERCHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_VKEYTOITEM" class="tdname">
WM_VKEYTOITEM</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_VSCROLL" class="tdname">
WM_VSCROLL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_VSCROLLCLIPBOARD" class="tdname">
WM_VSCROLLCLIPBOARD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_WINDOWPOSCHANGED" class="tdname">
WM_WINDOWPOSCHANGED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_WINDOWPOSCHANGING" class="tdname">
WM_WINDOWPOSCHANGING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_WININICHANGE" class="tdname">
WM_WININICHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_WTSSESSION_CHANGE" class="tdname">
WM_WTSSESSION_CHANGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONDBLCLK" class="tdname">
WM_XBUTTONDBLCLK</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONDOWN" class="tdname">
WM_XBUTTONDOWN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WM_XBUTTONUP" class="tdname">
WM_XBUTTONUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_BORDER" class="tdname">
WS_BORDER</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_CAPTION" class="tdname">
WS_CAPTION</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_CHILD" class="tdname">
WS_CHILD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_CLIPCHILDREN" class="tdname">
WS_CLIPCHILDREN</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_CLIPSIBLINGS" class="tdname">
WS_CLIPSIBLINGS</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_DISABLED" class="tdname">
WS_DISABLED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_DLGFRAME" class="tdname">
WS_DLGFRAME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_ACCEPTFILES" class="tdname">
WS_EX_ACCEPTFILES</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_APPWINDOW" class="tdname">
WS_EX_APPWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_CLIENTEDGE" class="tdname">
WS_EX_CLIENTEDGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_COMPOSITED" class="tdname">
WS_EX_COMPOSITED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_CONTEXTHELP" class="tdname">
WS_EX_CONTEXTHELP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_CONTROLPARENT" class="tdname">
WS_EX_CONTROLPARENT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_DLGMODALFRAME" class="tdname">
WS_EX_DLGMODALFRAME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_LAYERED" class="tdname">
WS_EX_LAYERED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_LAYOUTRTL" class="tdname">
WS_EX_LAYOUTRTL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_LEFT" class="tdname">
WS_EX_LEFT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_LEFTSCROLLBAR" class="tdname">
WS_EX_LEFTSCROLLBAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_LTRREADING" class="tdname">
WS_EX_LTRREADING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_MDICHILD" class="tdname">
WS_EX_MDICHILD</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_NOACTIVATE" class="tdname">
WS_EX_NOACTIVATE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_NOINHERITLAYOUT" class="tdname">
WS_EX_NOINHERITLAYOUT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_NOPARENTNOTIFY" class="tdname">
WS_EX_NOPARENTNOTIFY</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_NOREDIRECTIONBITMAP" class="tdname">
WS_EX_NOREDIRECTIONBITMAP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_OVERLAPPEDWINDOW" class="tdname">
WS_EX_OVERLAPPEDWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_PALETTEWINDOW" class="tdname">
WS_EX_PALETTEWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_RIGHT" class="tdname">
WS_EX_RIGHT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_RIGHTSCROLLBAR" class="tdname">
WS_EX_RIGHTSCROLLBAR</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_RTLREADING" class="tdname">
WS_EX_RTLREADING</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_STATICEDGE" class="tdname">
WS_EX_STATICEDGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_TOOLWINDOW" class="tdname">
WS_EX_TOOLWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_TOPMOST" class="tdname">
WS_EX_TOPMOST</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_TRANSPARENT" class="tdname">
WS_EX_TRANSPARENT</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_EX_WINDOWEDGE" class="tdname">
WS_EX_WINDOWEDGE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_GROUP" class="tdname">
WS_GROUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_HSCROLL" class="tdname">
WS_HSCROLL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_MAXIMIZE" class="tdname">
WS_MAXIMIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_MAXIMIZEBOX" class="tdname">
WS_MAXIMIZEBOX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_MINIMIZE" class="tdname">
WS_MINIMIZE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_MINIMIZEBOX" class="tdname">
WS_MINIMIZEBOX</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_OVERLAPPED" class="tdname">
WS_OVERLAPPED</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_OVERLAPPEDWINDOW" class="tdname">
WS_OVERLAPPEDWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_POPUP" class="tdname">
WS_POPUP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_POPUPWINDOW" class="tdname">
WS_POPUPWINDOW</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_SYSMENU" class="tdname">
WS_SYSMENU</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_TABSTOP" class="tdname">
WS_TABSTOP</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_THICKFRAME" class="tdname">
WS_THICKFRAME</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_VISIBLE" class="tdname">
WS_VISIBLE</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td id="Win32_WS_VSCROLL" class="tdname">
WS_VSCROLL</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AcquireSRWLockExclusive</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">AcquireSRWLockShared</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">BeginPaint</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpPaint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PAINTSTRUCT">PAINTSTRUCT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">BringWindowToTop</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
dwSize</td>
<td class="tdtype">
<a href="#Win32_COORD">Win32.COORD</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwCursorPosition</td>
<td class="tdtype">
<a href="#Win32_COORD">Win32.COORD</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wAttributes</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
srWindow</td>
<td class="tdtype">
<a href="#Win32_SMALL_RECT">Win32.SMALL_RECT</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwMaximumWindowSize</td>
<td class="tdtype">
<a href="#Win32_COORD">Win32.COORD</a></td>
<td class="enumeration">
</td>
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
<td class="tdname">
x</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
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
<td class="tdname">
lpCreateParams</td>
<td class="tdtype">
Win32.LPVOID</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hInstance</td>
<td class="tdtype">
Win32.HINSTANCE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hMenu</td>
<td class="tdtype">
Win32.HMENU</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hwndParent</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cy</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cx</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
x</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
style</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpszName</td>
<td class="tdtype">
Win32.LPCSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpszClass</td>
<td class="tdtype">
Win32.LPCSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwExStyle</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<td class="tdname">
lpCreateParams</td>
<td class="tdtype">
Win32.LPVOID</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hInstance</td>
<td class="tdtype">
Win32.HINSTANCE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hMenu</td>
<td class="tdtype">
Win32.HMENU</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hwndParent</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cy</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cx</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
x</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
style</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpszName</td>
<td class="tdtype">
Win32.LPCWSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpszClass</td>
<td class="tdtype">
Win32.LPCWSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwExStyle</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CancelWaitableTimer</span><span class="SyntaxCode">(hTimer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ChildWindowFromPoint</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ChildWindowFromPointEx</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">, flags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ClientToScreen</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpPoint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CloseClipboard</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CloseHandle</span><span class="SyntaxCode">(handle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CoInitializeEx</span><span class="SyntaxCode">(pvReserved: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dwCoInit: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HRESULT</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CoUninitialize</span><span class="SyntaxCode">()</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateConsoleScreenBuffer</span><span class="SyntaxCode">(dwDesiredAccess: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwShareMode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpSecurityAttributes: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpScreenBufferData: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateDirectoryA</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateDirectoryW</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateEventA</span><span class="SyntaxCode">(lpEventAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, bInitialState: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateEventW</span><span class="SyntaxCode">(lpEventAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, bInitialState: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFileA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, dwDesiredAccess: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwShareMode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwCreationDisposition: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwFlagsAndAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, hTemplateFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateFileW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, dwDesiredAccess: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwShareMode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpSecurityAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwCreationDisposition: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwFlagsAndAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, hTemplateFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateIconIndirect</span><span class="SyntaxCode">(piconinfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ICONINFO">ICONINFO</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateMutexA</span><span class="SyntaxCode">(lpMutexAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInitialOwner: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateMutexW</span><span class="SyntaxCode">(lpMutexAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInitialOwner: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateProcessA</span><span class="SyntaxCode">(lpApplicationName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpCommandLine: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpProcessAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, lpThreadAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInheritHandles: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, dwCreationFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpEnvironment: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, lpCurrentDirectory: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpStartupInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_STARTUPINFOA">STARTUPINFOA</a></span><span class="SyntaxCode">, lpProcessInformation: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PROCESS_INFORMATION">PROCESS_INFORMATION</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateProcessW</span><span class="SyntaxCode">(lpApplicationName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpCommandLine: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpProcessAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, lpThreadAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bInheritHandles: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, dwCreationFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpEnvironment: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, lpCurrentDirectory: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpStartupInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_STARTUPINFOW">STARTUPINFOW</a></span><span class="SyntaxCode">, lpProcessInformation: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PROCESS_INFORMATION">PROCESS_INFORMATION</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateThread</span><span class="SyntaxCode">(lpThreadAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, dwStackSize: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SIZE_T</span><span class="SyntaxCode">, lpStartAddress: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPTHREAD_START_ROUTINE</span><span class="SyntaxCode">, lpParameter: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dwCreationFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpThreadId: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateTimerQueueTimer</span><span class="SyntaxCode">(phNewTimer: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, timerQueue: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, callback: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WAITORTIMERCALLBACK</span><span class="SyntaxCode">, parameter: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dueTime: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, period: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, flags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ULONG</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWaitableTimerA</span><span class="SyntaxCode">(lpTimerAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpTimerName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWaitableTimerW</span><span class="SyntaxCode">(lpTimerAttributes: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SECURITY_ATTRIBUTES">SECURITY_ATTRIBUTES</a></span><span class="SyntaxCode">, bManualReset: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, lpTimerName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWindowExA</span><span class="SyntaxCode">(dwExStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, dwStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">X</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Y</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, hWndParent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, hMenu: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMENU</span><span class="SyntaxCode">, hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HINSTANCE</span><span class="SyntaxCode">, lpParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">CreateWindowExW</span><span class="SyntaxCode">(dwExStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, dwStyle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">X</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Y</span><span class="SyntaxCode">: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, hWndParent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, hMenu: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMENU</span><span class="SyntaxCode">, hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HINSTANCE</span><span class="SyntaxCode">, lpParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DebugActiveProcess</span><span class="SyntaxCode">(procId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DebugBreak</span><span class="SyntaxCode">()</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DefWindowProcA</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, uMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WPARAM</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LRESULT</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DefWindowProcW</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, uMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WPARAM</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LRESULT</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteFileA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteFileW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DeleteTimerQueueTimer</span><span class="SyntaxCode">(timerQueue: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, timer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, completionEvent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DestroyIcon</span><span class="SyntaxCode">(hIcon: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DestroyWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DispatchMessageA</span><span class="SyntaxCode">(lpMsg: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LRESULT</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">DispatchMessageW</span><span class="SyntaxCode">(lpMsg: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LRESULT</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EmptyClipboard</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnableWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, bEnable: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EndPaint</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpPaint: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_PAINTSTRUCT">PAINTSTRUCT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumDisplayMonitors</span><span class="SyntaxCode">(hdc: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lprcClip: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">, lpfnEnum: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, dwData: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">EnumWindows</span><span class="SyntaxCode">(lpEnumFunc: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExitProcess</span><span class="SyntaxCode">(uExitCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExtractIconExA</span><span class="SyntaxCode">(lpszFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, nIconIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, phiconLarge: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, phiconSmall: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, nIcons: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ExtractIconExW</span><span class="SyntaxCode">(lpszFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, nIconIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, phiconLarge: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, phiconSmall: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, nIcons: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
dwLowDateTime</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwHighDateTime</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FileTimeToLocalFileTime</span><span class="SyntaxCode">(lpFileTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpLocalFileTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FileTimeToSystemTime</span><span class="SyntaxCode">(lpFileTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpSystemTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FillRect</span><span class="SyntaxCode">(hDC: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">, lprc: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">, hbr: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HBRUSH</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindClose</span><span class="SyntaxCode">(hFindFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstFileA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAA">WIN32_FIND_DATAA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstFileW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAW">WIN32_FIND_DATAW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstVolumeA</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindFirstVolumeW</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextFileA</span><span class="SyntaxCode">(hFindFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAA">WIN32_FIND_DATAA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextFileW</span><span class="SyntaxCode">(hFindFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpFindFileData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WIN32_FIND_DATAW">WIN32_FIND_DATAW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextVolumeA</span><span class="SyntaxCode">(hFindVolume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindNextVolumeW</span><span class="SyntaxCode">(hFindVolume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindVolumeClose</span><span class="SyntaxCode">(hFindVolume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindWindowA</span><span class="SyntaxCode">(lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FindWindowW</span><span class="SyntaxCode">(lpClassName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpWindowName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">FormatMessageA</span><span class="SyntaxCode">(dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpSource: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCVOID</span><span class="SyntaxCode">, dwMessageId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwLanguageId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, nSize: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, </span><span class="SyntaxConstant">Arguments</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.va_list)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
GetFileExInfoStandard</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
GetFileExMaxInfoLevel</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GET_WHEEL_DELTA_WPARAM</span><span class="SyntaxCode">(wParam: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GET_X_LPARAM</span><span class="SyntaxCode">(lp: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GET_Y_LPARAM</span><span class="SyntaxCode">(lp: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">)</code>
</pre>
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
<td class="tdname">
Data1</td>
<td class="tdtype">
u32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Data2</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Data3</td>
<td class="tdtype">
u16</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
Data4</td>
<td class="tdtype">
[8] u8</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetActiveWindow</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetAdaptersInfo</span><span class="SyntaxCode">(adapterInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_IP_ADAPTER_INFO">IP_ADAPTER_INFO</a></span><span class="SyntaxCode">, sizePointer: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ULONG</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ULONG</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetAsyncKeyState</span><span class="SyntaxCode">(vKey: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SHORT</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetClientRect</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpRect: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetClipboardData</span><span class="SyntaxCode">(uFormat: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetConsoleScreenBufferInfo</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpConsoleScreenBufferInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_CONSOLE_SCREEN_BUFFER_INFO">CONSOLE_SCREEN_BUFFER_INFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentDirectoryA</span><span class="SyntaxCode">(nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentDirectoryW</span><span class="SyntaxCode">(nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentProcess</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentProcessId</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentThread</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCurrentThreadId</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetCursorPos</span><span class="SyntaxCode">(lpPoint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetDC</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetDesktopWindow</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesExA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, fInfoLevelId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></span><span class="SyntaxCode">, lpFileInformation: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesExW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, fInfoLevelId: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_GET_FILEEX_INFO_LEVELS">GET_FILEEX_INFO_LEVELS</a></span><span class="SyntaxCode">, lpFileInformation: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileAttributesW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFileSize</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpFileSizeHigh: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetForegroundWindow</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFullPathNameA</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, lpFilePart: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetFullPathNameW</span><span class="SyntaxCode">(lpFileName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCTSTR</span><span class="SyntaxCode">, nBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, lpFilePart: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetIconInfo</span><span class="SyntaxCode">(hIcon: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HICON</span><span class="SyntaxCode">, piconinfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_ICONINFO">ICONINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetKeyboardState</span><span class="SyntaxCode">(lpKeystate: *</span><span class="SyntaxType">u8</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetLastError</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetLocalTime</span><span class="SyntaxCode">(lpSystemTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMessageA</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMessageW</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetModuleHandleA</span><span class="SyntaxCode">(lpModuleName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMODULE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetModuleHandleW</span><span class="SyntaxCode">(lpModuleName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMODULE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMonitorInfoA</span><span class="SyntaxCode">(hMonitor: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMONITOR</span><span class="SyntaxCode">, lpmi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MONITORINFO">MONITORINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetMonitorInfoW</span><span class="SyntaxCode">(hMonitor: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMONITOR</span><span class="SyntaxCode">, lpmi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MONITORINFO">MONITORINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetNextWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wCmd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetProcAddress</span><span class="SyntaxCode">(hModule: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMODULE</span><span class="SyntaxCode">, lpProcName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxType">void</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetStdHandle</span><span class="SyntaxCode">(nStdHandle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetSystemInfo</span><span class="SyntaxCode">(lpSystemInfo: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEM_INFO">SYSTEM_INFO</a></span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetSystemMetrics</span><span class="SyntaxCode">(nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetSystemTime</span><span class="SyntaxCode">(lpSystemTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetTopWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetVolumePathNamesForVolumeNameA</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpszVolumePathNames: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCH</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpcchReturnLength: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetVolumePathNamesForVolumeNameW</span><span class="SyntaxCode">(lpszVolumeName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpszVolumePathNames: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWCH</span><span class="SyntaxCode">, cchBufferLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpcchReturnLength: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, uCmd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowLongPtrA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowLongPtrW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowRect</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpRect: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowTextA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, nMaxCount: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GetWindowTextW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, nMaxCount: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalAlloc</span><span class="SyntaxCode">(uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, dwBytes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SIZE_T</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLOBAL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalLock</span><span class="SyntaxCode">(hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLOBAL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalSize</span><span class="SyntaxCode">(hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLOBAL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SIZE_T</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">GlobalUnlock</span><span class="SyntaxCode">(hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HGLOBAL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">HIWORD</span><span class="SyntaxCode">(l: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
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
<td class="tdname">
fIcon</td>
<td class="tdtype">
Win32.BOOL</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
xHotspot</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
yHotspot</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hbmMask</td>
<td class="tdtype">
Win32.HBITMAP</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hbmColor</td>
<td class="tdtype">
Win32.HBITMAP</td>
<td class="enumeration">
</td>
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
<td class="tdname">
next</td>
<td class="tdtype">
*Win32.IP_ADAPTER_INFO</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
comboIndex</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
adapterName</td>
<td class="tdtype">
[260] u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
description</td>
<td class="tdtype">
[132] u8</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
addressLength</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
address</td>
<td class="tdtype">
[8] Win32.BYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
index</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
type</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dhcpEnabled</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
currentIpAddress</td>
<td class="tdtype">
*Win32.IP_ADDR_STRING</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ipAddressList</td>
<td class="tdtype">
<a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
gatewayList</td>
<td class="tdtype">
<a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dhcpServer</td>
<td class="tdtype">
<a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
haveWins</td>
<td class="tdtype">
Win32.BOOL</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
primaryWinsServer</td>
<td class="tdtype">
<a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
secondaryWinsServer</td>
<td class="tdtype">
<a href="#Win32_IP_ADDR_STRING">Win32.IP_ADDR_STRING</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
leaseObtained</td>
<td class="tdtype">
Win32.time_t</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
leaseExpires</td>
<td class="tdtype">
Win32.time_t</td>
<td class="enumeration">
</td>
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
<td class="tdname">
str</td>
<td class="tdtype">
[16] u8</td>
<td class="enumeration">
</td>
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
<td class="tdname">
next</td>
<td class="tdtype">
*Win32.IP_ADDR_STRING</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ipAddress</td>
<td class="tdtype">
<a href="#Win32_IP_ADDRESS_STRING">Win32.IP_ADDRESS_STRING</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ipMask</td>
<td class="tdtype">
<a href="#Win32_IP_ADDRESS_STRING">Win32.IP_ADDRESS_STRING</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
context</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">InitializeSRWLock</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">InvalidateRect</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpRect: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_RECT">RECT</a></span><span class="SyntaxCode">, bErase: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IsClipboardFormatAvailable</span><span class="SyntaxCode">(format: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IsDebuggerPresent</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">IsWindowVisible</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LOWORD</span><span class="SyntaxCode">(l: </span><span class="SyntaxType">u64</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LoadCursorA</span><span class="SyntaxCode">(hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HINSTANCE</span><span class="SyntaxCode">, lpCursorName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HCURSOR</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LoadCursorW</span><span class="SyntaxCode">(hInstance: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HINSTANCE</span><span class="SyntaxCode">, lpCursorName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HCURSOR</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">LocalFree</span><span class="SyntaxCode">(hLocal: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HLOCAL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HLOCAL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MAKEINTRESOURCEA</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MAKEINTRESOURCEW</span><span class="SyntaxCode">(val: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MAKELANGID</span><span class="SyntaxCode">(p: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WORD</span><span class="SyntaxCode">, s: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WORD</span><span class="SyntaxCode">)</code>
</pre>
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
<td class="tdname">
ptReserved</td>
<td class="tdtype">
<a href="#Win32_POINT">Win32.POINT</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ptMaxSize</td>
<td class="tdtype">
<a href="#Win32_POINT">Win32.POINT</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ptMaxPosition</td>
<td class="tdtype">
<a href="#Win32_POINT">Win32.POINT</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ptMinTrackSize</td>
<td class="tdtype">
<a href="#Win32_POINT">Win32.POINT</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ptMaxTrackSize</td>
<td class="tdtype">
<a href="#Win32_POINT">Win32.POINT</a></td>
<td class="enumeration">
</td>
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
<td class="tdname">
cbSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
rcMonitor</td>
<td class="tdtype">
<a href="#Win32_RECT">Win32.RECT</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
rcWork</td>
<td class="tdtype">
<a href="#Win32_RECT">Win32.RECT</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwFlags</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<td class="tdname">
hwnd</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
message</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wParam</td>
<td class="tdtype">
Win32.WPARAM</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lParam</td>
<td class="tdtype">
Win32.LPARAM</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
time</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
pt</td>
<td class="tdtype">
<a href="#Win32_POINT">Win32.POINT</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lPrivate</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MapVirtualKeyA</span><span class="SyntaxCode">(uCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, uMapType: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MapVirtualKeyW</span><span class="SyntaxCode">(uCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, uMapType: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MessageBoxA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpText: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, lpCaption: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, uType: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MessageBoxW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpText: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, lpCaption: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, uType: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MonitorFromPoint</span><span class="SyntaxCode">(pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMONITOR</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MonitorFromWindow</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HMONITOR</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">MoveWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nWidth: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, nHeight: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, bRepaint: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
cbSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hWnd</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uID</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uFlags</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uCallbackMessage</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hIcon</td>
<td class="tdtype">
Win32.HICON</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szTip</td>
<td class="tdtype">
[128] Win32.CHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwState</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwStateMask</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szInfo</td>
<td class="tdtype">
[256] Win32.CHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
<b>using</b> DUMMYUNIONNAME</td>
<td class="tdtype">
{uTimeout: Win32.UINT, uVersion: Win32.UINT}</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szInfoTitle</td>
<td class="tdtype">
[64] Win32.CHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwInfoFlags</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
guidItem</td>
<td class="tdtype">
<a href="#Win32_GUID">Win32.GUID</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hBalloonIcon</td>
<td class="tdtype">
Win32.HICON</td>
<td class="enumeration">
</td>
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
<td class="tdname">
cbSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hWnd</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uID</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uFlags</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
uCallbackMessage</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hIcon</td>
<td class="tdtype">
Win32.HICON</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szTip</td>
<td class="tdtype">
[128] Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwState</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwStateMask</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szInfo</td>
<td class="tdtype">
[256] Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
<b>using</b> DUMMYUNIONNAME</td>
<td class="tdtype">
{uTimeout: Win32.UINT, uVersion: Win32.UINT}</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szInfoTitle</td>
<td class="tdtype">
[64] Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwInfoFlags</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
guidItem</td>
<td class="tdtype">
<a href="#Win32_GUID">Win32.GUID</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hBalloonIcon</td>
<td class="tdtype">
Win32.HICON</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OpenClipboard</span><span class="SyntaxCode">(hWndNewOwner: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OutputDebugStringA</span><span class="SyntaxCode">(lpOutputString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">OutputDebugStringW</span><span class="SyntaxCode">(lpOutputString: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)</code>
</pre>
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
<td class="tdname">
hdc</td>
<td class="tdtype">
Win32.HDC</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
fErase</td>
<td class="tdtype">
Win32.BOOL</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
rcPaint</td>
<td class="tdtype">
<a href="#Win32_RECT">Win32.RECT</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
fRestore</td>
<td class="tdtype">
Win32.BOOL</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
fIncUpdate</td>
<td class="tdtype">
Win32.BOOL</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
rgbReserved</td>
<td class="tdtype">
[32] Win32.BYTE</td>
<td class="enumeration">
</td>
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
<td class="tdname">
x</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
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
<td class="tdname">
hProcess</td>
<td class="tdtype">
Win32.HANDLE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hThread</td>
<td class="tdtype">
Win32.HANDLE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwProcessId</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwThreadId</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PeekMessageA</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wRemoveMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PeekMessageW</span><span class="SyntaxCode">(lpMsg: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">, hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, wMsgFilterMin: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wMsgFilterMax: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wRemoveMsg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PostMessageA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, msg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WPARAM</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PostMessageW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, msg: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WPARAM</span><span class="SyntaxCode">, lParam: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPARAM</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">PostQuitMessage</span><span class="SyntaxCode">(nExitCode: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">QueryPerformanceCounter</span><span class="SyntaxCode">(lpPerformanceCount: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LARGE_INTEGER</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">QueryPerformanceFrequency</span><span class="SyntaxCode">(lpFrequency: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LARGE_INTEGER</span><span class="SyntaxCode">)</code>
</pre>
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
<td class="tdname">
left</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
top</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
right</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bottom</td>
<td class="tdtype">
Win32.LONG</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReadFile</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, nNumberOfBytesToRead: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpNumberOfBytesRead: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">, lpOverlapped: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPOVERLAPPED</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClassA</span><span class="SyntaxCode">(lpWndClass: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WNDCLASSA">WNDCLASSA</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ATOM</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClassW</span><span class="SyntaxCode">(lpWndClass: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_WNDCLASSW">WNDCLASSW</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">ATOM</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClipboardFormatA</span><span class="SyntaxCode">(name: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterClipboardFormatW</span><span class="SyntaxCode">(name: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">RegisterHotKey</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, id: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, fsModifiers: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, vk: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseCapture</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseDC</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, hDC: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HDC</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseMutex</span><span class="SyntaxCode">(hMutex: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseSRWLockExclusive</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ReleaseSRWLockShared</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ResetEvent</span><span class="SyntaxCode">(hEvent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ResumeThread</span><span class="SyntaxCode">(hThread: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
nLength</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpSecurityDescriptor</td>
<td class="tdtype">
Win32.LPVOID</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bInheritHandle</td>
<td class="tdtype">
Win32.BOOL</td>
<td class="enumeration">
</td>
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
<td class="tdname">
hIcon</td>
<td class="tdtype">
Win32.HICON</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
iIcon</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwAttributes</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szDisplayName</td>
<td class="tdtype">
[260] Win32.CHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szTypeName</td>
<td class="tdtype">
[80] Win32.CHAR</td>
<td class="enumeration">
</td>
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
<td class="tdname">
hIcon</td>
<td class="tdtype">
Win32.HICON</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
iIcon</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwAttributes</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szDisplayName</td>
<td class="tdtype">
[260] Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szTypeName</td>
<td class="tdtype">
[80] Win32.WCHAR</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetFileInfoA</span><span class="SyntaxCode">(pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, dwFileAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, psfi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHFILEINFOA">SHFILEINFOA</a></span><span class="SyntaxCode">, cbFileInfo: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD_PTR</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetFileInfoW</span><span class="SyntaxCode">(pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">, dwFileAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, psfi: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHFILEINFOW">SHFILEINFOW</a></span><span class="SyntaxCode">, cbFileInfo: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD_PTR</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetSpecialFolderPathA</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPSTR</span><span class="SyntaxCode">, csidl: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, fCreate: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetSpecialFolderPathW</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, pszPath: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, csidl: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, fCreate: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SHGetStockIconInfo</span><span class="SyntaxCode">(siid: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHSTOCKICONID">SHSTOCKICONID</a></span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, psii: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SHSTOCKICONINFO">SHSTOCKICONINFO</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
SIID_DOCNOASSOC</td>
<td class="enumeration">
<p>document (blank page) no associated program. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DOCASSOC</td>
<td class="enumeration">
<p>document with an associated program. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_APPLICATION</td>
<td class="enumeration">
<p>generic application with no custom icon. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_FOLDER</td>
<td class="enumeration">
<p>folder (closed). </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_FOLDEROPEN</td>
<td class="enumeration">
<p>folder (open). </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVE525</td>
<td class="enumeration">
<p>5.25" floppy disk drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVE35</td>
<td class="enumeration">
<p>3.5" floppy disk drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVEREMOVE</td>
<td class="enumeration">
<p>removable drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVEFIXED</td>
<td class="enumeration">
<p>fixed (hard disk) drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVENET</td>
<td class="enumeration">
<p>network drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVENETDISABLED</td>
<td class="enumeration">
<p>disconnected network drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVECD</td>
<td class="enumeration">
<p>CD drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVERAM</td>
<td class="enumeration">
<p>RAM disk drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_WORLD</td>
<td class="enumeration">
<p>entire network. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_SERVER</td>
<td class="enumeration">
<p>a computer on the network. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_PRINTER</td>
<td class="enumeration">
<p>printer. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MYNETWORK</td>
<td class="enumeration">
<p>My network places. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_FIND</td>
<td class="enumeration">
<p>Find. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_HELP</td>
<td class="enumeration">
<p>Help. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_SHARE</td>
<td class="enumeration">
<p>overlay for shared items. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_LINK</td>
<td class="enumeration">
<p>overlay for shortcuts to items. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_SLOWFILE</td>
<td class="enumeration">
<p>overlay for slow items. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_RECYCLER</td>
<td class="enumeration">
<p>empty recycle bin. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_RECYCLERFULL</td>
<td class="enumeration">
<p>full recycle bin. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIACDAUDIO</td>
<td class="enumeration">
<p>Audio CD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_LOCK</td>
<td class="enumeration">
<p>Security lock. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_AUTOLIST</td>
<td class="enumeration">
<p>AutoList. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_PRINTERNET</td>
<td class="enumeration">
<p>Network printer. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_SERVERSHARE</td>
<td class="enumeration">
<p>Server share. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_PRINTERFAX</td>
<td class="enumeration">
<p>Fax printer. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_PRINTERFAXNET</td>
<td class="enumeration">
<p>Networked Fax Printer. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_PRINTERFILE</td>
<td class="enumeration">
<p>Print to File. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_STACK</td>
<td class="enumeration">
<p>Stack. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIASVCD</td>
<td class="enumeration">
<p>SVCD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_STUFFEDFOLDER</td>
<td class="enumeration">
<p>Folder containing other items. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVEUNKNOWN</td>
<td class="enumeration">
<p>Unknown drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVEDVD</td>
<td class="enumeration">
<p>DVD Drive. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIADVD</td>
<td class="enumeration">
<p>DVD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIADVDRAM</td>
<td class="enumeration">
<p>DVD-RAM Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIADVDRW</td>
<td class="enumeration">
<p>DVD-RW Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIADVDR</td>
<td class="enumeration">
<p>DVD-R Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIADVDROM</td>
<td class="enumeration">
<p>DVD-ROM Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIACDAUDIOPLUS</td>
<td class="enumeration">
<p>CD+ (Enhanced CD) Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIACDRW</td>
<td class="enumeration">
<p>CD-RW Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIACDR</td>
<td class="enumeration">
<p>CD-R Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIACDBURN</td>
<td class="enumeration">
<p>Burning CD. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIABLANKCD</td>
<td class="enumeration">
<p>Blank CD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIACDROM</td>
<td class="enumeration">
<p>CD-ROM Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_AUDIOFILES</td>
<td class="enumeration">
<p>Audio files. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_IMAGEFILES</td>
<td class="enumeration">
<p>Image files. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_VIDEOFILES</td>
<td class="enumeration">
<p>Video files. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MIXEDFILES</td>
<td class="enumeration">
<p>Mixed files. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_FOLDERBACK</td>
<td class="enumeration">
<p>Folder back. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_FOLDERFRONT</td>
<td class="enumeration">
<p>Folder front. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_SHIELD</td>
<td class="enumeration">
<p>Security shield. Use for UAC prompts only. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_WARNING</td>
<td class="enumeration">
<p>Warning. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_INFO</td>
<td class="enumeration">
<p>Informational. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_ERROR</td>
<td class="enumeration">
<p>Error. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_KEY</td>
<td class="enumeration">
<p>Key / Secure. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_SOFTWARE</td>
<td class="enumeration">
<p>Software. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_RENAME</td>
<td class="enumeration">
<p>Rename. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DELETE</td>
<td class="enumeration">
<p>Delete. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIAAUDIODVD</td>
<td class="enumeration">
<p>Audio DVD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIAMOVIEDVD</td>
<td class="enumeration">
<p>Movie DVD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIAENHANCEDCD</td>
<td class="enumeration">
<p>Enhanced CD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIAENHANCEDDVD</td>
<td class="enumeration">
<p>Enhanced DVD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIAHDDVD</td>
<td class="enumeration">
<p>HD-DVD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIABLURAY</td>
<td class="enumeration">
<p>BluRay Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIAVCD</td>
<td class="enumeration">
<p>VCD Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIADVDPLUSR</td>
<td class="enumeration">
<p>DVD+R Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIADVDPLUSRW</td>
<td class="enumeration">
<p>DVD+RW Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DESKTOPPC</td>
<td class="enumeration">
<p>desktop computer. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MOBILEPC</td>
<td class="enumeration">
<p>mobile computer (laptop/notebook). </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_USERS</td>
<td class="enumeration">
<p>users. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIASMARTMEDIA</td>
<td class="enumeration">
<p>Smart Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIACOMPACTFLASH</td>
<td class="enumeration">
<p>Compact Flash. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DEVICECELLPHONE</td>
<td class="enumeration">
<p>Cell phone. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DEVICECAMERA</td>
<td class="enumeration">
<p>Camera. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DEVICEVIDEOCAMERA</td>
<td class="enumeration">
<p>Video camera. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DEVICEAUDIOPLAYER</td>
<td class="enumeration">
<p>Audio player. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_NETWORKCONNECT</td>
<td class="enumeration">
<p>Connect to network. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_INTERNET</td>
<td class="enumeration">
<p>Internet. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_ZIPFILE</td>
<td class="enumeration">
<p>ZIP file. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_SETTINGS</td>
<td class="enumeration">
<p>Settings  107-131 are internal Vista RTM icons  132-159 for SP1 icons. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVEHDDVD</td>
<td class="enumeration">
<p>HDDVD Drive (all types). </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_DRIVEBD</td>
<td class="enumeration">
<p>BluRay Drive (all types). </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIAHDDVDROM</td>
<td class="enumeration">
<p>HDDVD-ROM Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIAHDDVDR</td>
<td class="enumeration">
<p>HDDVD-R Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIAHDDVDRAM</td>
<td class="enumeration">
<p>HDDVD-RAM Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIABDROM</td>
<td class="enumeration">
<p>BluRay ROM Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIABDR</td>
<td class="enumeration">
<p>BluRay R Media. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MEDIABDRE</td>
<td class="enumeration">
<p>BluRay RE Media (Rewriable and RAM). </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_CLUSTEREDDRIVE</td>
<td class="enumeration">
<p>Clustered disk  160+ are for Windows 7 icons. </p>
</td>
</tr>
<tr>
<td class="tdname">
SIID_MAX_ICONS</td>
<td class="enumeration">
</td>
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
<td class="tdname">
cbSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hIcon</td>
<td class="tdtype">
Win32.HICON</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
iSysImageIndex</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
iIcon</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
szPath</td>
<td class="tdtype">
[260] Win32.WCHAR</td>
<td class="enumeration">
</td>
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
<td class="tdname">
left</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
top</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
right</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
bottom</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
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
<td class="tdname">
cb</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpReserved</td>
<td class="tdtype">
Win32.LPSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpDesktop</td>
<td class="tdtype">
Win32.LPSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpTitle</td>
<td class="tdtype">
Win32.LPSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwX</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwY</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwXSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwYSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwXCountChars</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwYCountChars</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwFillAttribute</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwFlags</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wShowWindow</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cbReserved2</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpReserved2</td>
<td class="tdtype">
Win32.LPBYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hStdInput</td>
<td class="tdtype">
Win32.HANDLE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hStdOutput</td>
<td class="tdtype">
Win32.HANDLE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hStdError</td>
<td class="tdtype">
Win32.HANDLE</td>
<td class="enumeration">
</td>
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
<td class="tdname">
cb</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpReserved</td>
<td class="tdtype">
Win32.LPWSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpDesktop</td>
<td class="tdtype">
Win32.LPWSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpTitle</td>
<td class="tdtype">
Win32.LPWSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwX</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwY</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwXSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwYSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwXCountChars</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwYCountChars</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwFillAttribute</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwFlags</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wShowWindow</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cbReserved2</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpReserved2</td>
<td class="tdtype">
Win32.LPBYTE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hStdInput</td>
<td class="tdtype">
Win32.HANDLE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hStdOutput</td>
<td class="tdtype">
Win32.HANDLE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hStdError</td>
<td class="tdtype">
Win32.HANDLE</td>
<td class="enumeration">
</td>
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
<td class="tdname">
wYear</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wMonth</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wDayOfWeek</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wDay</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wHour</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wMinute</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wSecond</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wMilliseconds</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
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
<td class="tdname">
dwOemId</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
<p>Must be updated to an union. </p>
</td>
</tr>
<tr>
<td class="tdname">
dwPageSize</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpMinimumApplicationAddress</td>
<td class="tdtype">
Win32.LPVOID</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpMaximumApplicationAddress</td>
<td class="tdtype">
Win32.LPVOID</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwActiveProcessorMask</td>
<td class="tdtype">
Win32.DWORD_PTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwNumberOfProcessors</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwProcessorType</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwAllocationGranularity</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wProcessorLevel</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wProcessorRevision</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ScreenToClient</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, lpPoint: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetActiveWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCapture</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetClipboardData</span><span class="SyntaxCode">(uFormat: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, hMem: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetConsoleActiveScreenBuffer</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetConsoleOutputCP</span><span class="SyntaxCode">(codePage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetConsoleTextAttribute</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, wAttributes: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">WORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCurrentDirectoryA</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCurrentDirectoryW</span><span class="SyntaxCode">(lpPathName: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCWSTR</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCursor</span><span class="SyntaxCode">(hCursor: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HCURSOR</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HCURSOR</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetCursorPos</span><span class="SyntaxCode">(x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetEvent</span><span class="SyntaxCode">(hEvent: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetFilePointer</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lDistanceToMove: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">, lpDistanceToMoveHigh: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">PLONG</span><span class="SyntaxCode">, dwMoveMethod: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetFileTime</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpCreationTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpLastAccessTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">, lpLastWriteTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetFocus</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetLayeredWindowAttributes</span><span class="SyntaxCode">(hwnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, crKey: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">COLORREF</span><span class="SyntaxCode">, bAlpha: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BYTE</span><span class="SyntaxCode">, dwFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetThreadPriority</span><span class="SyntaxCode">(hThread: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, nPriority: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWaitableTimer</span><span class="SyntaxCode">(hTimer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpDueTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LARGE_INTEGER</span><span class="SyntaxCode">, lPeriod: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">, pfnCompletionRoutine: *</span><span class="SyntaxType">void</span><span class="SyntaxCode">, lpArgToCompletionRoutine: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPVOID</span><span class="SyntaxCode">, fResume: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWindowLongPtrA</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, dwNewLong: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWindowLongPtrW</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nIndex: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, dwNewLong: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode">)-&gt;*</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LONG</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SetWindowPos</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, hWndInsertAfter: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, x: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, y: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cx: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, cy: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, uFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Shell_NotifyIconA</span><span class="SyntaxCode">(dwMessage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_NOTIFYICONDATAA">NOTIFYICONDATAA</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Shell_NotifyIconW</span><span class="SyntaxCode">(dwMessage: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpData: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_NOTIFYICONDATAW">NOTIFYICONDATAW</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ShowCursor</span><span class="SyntaxCode">(show: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ShowWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, nCmdShow: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">Sleep</span><span class="SyntaxCode">(dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SleepEx</span><span class="SyntaxCode">(dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, bAltertable: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">)</code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SwitchToThread</span><span class="SyntaxCode">()-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">SystemTimeToFileTime</span><span class="SyntaxCode">(lpSystemTime: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_SYSTEMTIME">SYSTEMTIME</a></span><span class="SyntaxCode">, lpFileTime: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_FILETIME">FILETIME</a></span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">ToUnicode</span><span class="SyntaxCode">(wVirtKey: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, wScanCode: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">, lpKeyState: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BYTE</span><span class="SyntaxCode">, pwszBuff: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPWSTR</span><span class="SyntaxCode">, cchBuff: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">, wFlags: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">UINT</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxType">s32</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TranslateMessage</span><span class="SyntaxCode">(lpMsg: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_MSG">MSG</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TryAcquireSRWLockExclusive</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">TryAcquireSRWLockShared</span><span class="SyntaxCode">(</span><span class="SyntaxConstant">SRWLock</span><span class="SyntaxCode">: *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">SRWLOCK</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">UnregisterHotKey</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">, id: </span><span class="SyntaxType">s32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">UpdateWindow</span><span class="SyntaxCode">(hWnd: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<td class="tdname">
dwFileAttributes</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ftCreationTime</td>
<td class="tdtype">
<a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ftLastAccessTime</td>
<td class="tdtype">
<a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ftLastWriteTime</td>
<td class="tdtype">
<a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
nFileSizeHigh</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
nFileSizeLow</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
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
<td class="tdname">
dwFileAttributes</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ftCreationTime</td>
<td class="tdtype">
<a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ftLastAccessTime</td>
<td class="tdtype">
<a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ftLastWriteTime</td>
<td class="tdtype">
<a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
nFileSizeHigh</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
nFileSizeLow</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwReserved0</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwReserved1</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cFileName</td>
<td class="tdtype">
[260] Win32.CHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAlternateFileName</td>
<td class="tdtype">
[14] Win32.CHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwFileType</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwCreatorType</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wFinderFlags</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
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
<td class="tdname">
dwFileAttributes</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ftCreationTime</td>
<td class="tdtype">
<a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ftLastAccessTime</td>
<td class="tdtype">
<a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
ftLastWriteTime</td>
<td class="tdtype">
<a href="#Win32_FILETIME">Win32.FILETIME</a></td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
nFileSizeHigh</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
nFileSizeLow</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwReserved0</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwReserved1</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cFileName</td>
<td class="tdtype">
[260] Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cAlternateFileName</td>
<td class="tdtype">
[14] Win32.WCHAR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwFileType</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
dwCreatorType</td>
<td class="tdtype">
Win32.DWORD</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
wFinderFlags</td>
<td class="tdtype">
Win32.WORD</td>
<td class="enumeration">
</td>
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
<td class="tdname">
hwnd</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hwndInsertAfter</td>
<td class="tdtype">
Win32.HWND</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
x</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
y</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cx</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cy</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
flags</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
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
<td class="tdname">
style</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpfnWndProc</td>
<td class="tdtype">
const *void</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cbClsExtra</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cbWndExtra</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hInstance</td>
<td class="tdtype">
Win32.HINSTANCE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hIcon</td>
<td class="tdtype">
Win32.HICON</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hCursor</td>
<td class="tdtype">
Win32.HCURSOR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hbrBackground</td>
<td class="tdtype">
Win32.HBRUSH</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpszMenuName</td>
<td class="tdtype">
Win32.LPCSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpszClassName</td>
<td class="tdtype">
Win32.LPCSTR</td>
<td class="enumeration">
</td>
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
<td class="tdname">
style</td>
<td class="tdtype">
Win32.UINT</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpfnWndProc</td>
<td class="tdtype">
const *void</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cbClsExtra</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
cbWndExtra</td>
<td class="tdtype">
s32</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hInstance</td>
<td class="tdtype">
Win32.HINSTANCE</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hIcon</td>
<td class="tdtype">
Win32.HICON</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hCursor</td>
<td class="tdtype">
Win32.HCURSOR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
hbrBackground</td>
<td class="tdtype">
Win32.HBRUSH</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpszMenuName</td>
<td class="tdtype">
Win32.LPCWSTR</td>
<td class="enumeration">
</td>
</tr>
<tr>
<td class="tdname">
lpszClassName</td>
<td class="tdtype">
Win32.LPCWSTR</td>
<td class="enumeration">
</td>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WaitForMultipleObjects</span><span class="SyntaxCode">(nCount: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpHandles: </span><span class="SyntaxKeyword">const</span><span class="SyntaxCode"> *</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, bWaitAll: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode">, dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WaitForSingleObject</span><span class="SyntaxCode">(hHandle: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, dwMilliseconds: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WindowFromPoint</span><span class="SyntaxCode">(pt: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant"><a href="#Win32_POINT">POINT</a></span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HWND</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WriteConsoleOutputCharacterA</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpCharacter: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCSTR</span><span class="SyntaxCode">, nLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwWriteCoord: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpNumberOfCharsWritten: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WriteConsoleOutputCharacterW</span><span class="SyntaxCode">(hConsoleOutput: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpCharacter: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCTSTR</span><span class="SyntaxCode">, nLength: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, dwWriteCoord: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpNumberOfCharsWritten: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxConstant">WriteFile</span><span class="SyntaxCode">(hFile: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">HANDLE</span><span class="SyntaxCode">, lpBuffer: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPCVOID</span><span class="SyntaxCode">, nNumberOfBytesToWrite: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">DWORD</span><span class="SyntaxCode">, lpNumberOfBytesWritten: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPDWORD</span><span class="SyntaxCode">, lpOverlapped: </span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">LPOVERLAPPED</span><span class="SyntaxCode">)-&gt;</span><span class="SyntaxConstant">Win32</span><span class="SyntaxCode">.</span><span class="SyntaxConstant">BOOL</span><span class="SyntaxCode"> </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
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
<pre>
<code style="white-space: break-spaces"></span><span class="SyntaxCode"></span><span class="SyntaxKeyword">func</span><span class="SyntaxCode"> </span><span class="SyntaxFunction">setError</span><span class="SyntaxCode">(errorMessageID: </span><span class="SyntaxType">u32</span><span class="SyntaxCode">) </span><span class="SyntaxKeyword">throw</span><span class="SyntaxCode"></code>
</pre>
</div>
</div>
</div>
</body>
</html>
